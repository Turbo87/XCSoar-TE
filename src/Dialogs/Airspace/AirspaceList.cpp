/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2013 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "Airspace.hpp"
#include "Dialogs/XML.hpp"
#include "Airspace/AirspaceSorter.hpp"
#include "Math/FastMath.h"
#include "Form/Form.hpp"
#include "Form/Button.hpp"
#include "Form/List.hpp"
#include "Form/Util.hpp"
#include "Form/Edit.hpp"
#include "Form/DataField/String.hpp"
#include "Form/DataField/Enum.hpp"
#include "Form/DataField/Listener.hpp"
#include "Form/DataField/Prefix.hpp"
#include "Engine/Airspace/Airspaces.hpp"
#include "Engine/Airspace/AbstractAirspace.hpp"
#include "Renderer/AirspaceListRenderer.hpp"
#include "Look/MapLook.hpp"
#include "Screen/Canvas.hpp"
#include "Screen/Layout.hpp"
#include "Screen/Busy.hpp"
#include "Screen/Key.h"
#include "Compiler.h"
#include "Util/Macros.hpp"
#include "Units/Units.hpp"
#include "Formatter/AngleFormatter.hpp"
#include "UIGlobals.hpp"
#include "Interface.hpp"
#include "Blackboard/ScopeGPSListener.hpp"
#include "Language/Language.hpp"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

class AirspaceListDialog : public ListItemRenderer, public ListCursorHandler {
public:
  /* virtual methods from ListItemRenderer */
  virtual void OnPaintItem(Canvas &canvas, const PixelRect rc,
                           unsigned idx) override;

  /* virtual methods from ListCursorHandler */
  virtual bool CanActivateItem(unsigned index) const override {
    return true;
  }

  virtual void OnActivateItem(unsigned index) override;
};

/**
 * Special enum integer value for "filter disabled".
 */
static constexpr unsigned WILDCARD = 0x7fff;

static const Airspaces *airspaces;
static ProtectedAirspaceWarningManager *airspace_warnings;

static GeoPoint location;

static WndForm *dialog;
static WndProperty *name_control;
static WndProperty *distance_control;
static WndProperty *direction_control;
static WndProperty *type_control;
static ListControl *airspace_list_control;

static Angle last_heading;

static constexpr StaticEnumChoice type_filter_list[] = {
  { WILDCARD, _T("*") },
  { OTHER, _T("Other") },
  { RESTRICT, _T("Restricted areas") },
  { PROHIBITED, _T("Prohibited areas") },
  { DANGER, _T("Danger areas") },
  { CLASSA, _T("Class A") },
  { CLASSB, _T("Class B") },
  { CLASSC, _T("Class C") },
  { CLASSD, _T("Class D") },
  { NOGLIDER, _T("No gliders") },
  { CTR, _T("CTR") },
  { WAVE, _T("Wave") },
  { CLASSE, _T("Class E") },
  { CLASSF, _T("Class F") },
  { TMZ, _T("TMZ") },
  { MATZ, _T("MATZ") },
  { 0 }
};

static AirspaceSelectInfoVector airspace_list;

struct AirspaceListDialogState
{
  fixed distance;
  unsigned direction;
  unsigned type;

  AirspaceListDialogState()
    :distance(fixed(-1)), direction(WILDCARD), type(WILDCARD) {}
};

static AirspaceListDialogState dialog_state;

void
AirspaceListDialog::OnActivateItem(unsigned i)
{
  if (airspace_list.empty()) {
    assert(i == 0);
    return;
  }

  assert(i < airspace_list.size());

  dlgAirspaceDetails(airspace_list[i].GetAirspace(), airspace_warnings);
}

static void
UpdateList()
{
  AirspaceFilterData data;
  data.Clear();

  if (dialog_state.type != WILDCARD)
    data.cls = (AirspaceClass)dialog_state.type;

  const TCHAR *name_filter = name_control->GetDataField()->GetAsString();
  if (!StringIsEmpty(name_filter))
    data.name_prefix = name_filter;

  if (dialog_state.direction != WILDCARD) {
    data.direction = dialog_state.direction == 0
      ? CommonInterface::Basic().attitude.heading
      : Angle::Degrees(dialog_state.direction);
  }

  if (positive(dialog_state.distance))
    data.distance = dialog_state.distance;

  airspace_list = FilterAirspaces(*airspaces,
                                  CommonInterface::Basic().location,
                                  data);

  airspace_list_control->SetLength(std::max((size_t)1, airspace_list.size()));
  airspace_list_control->Invalidate();
}

static void
FilterMode(bool direction)
{
  if (direction) {
    dialog_state.distance = fixed(-1);
    dialog_state.direction = WILDCARD;

    DataFieldEnum *df = (DataFieldEnum *)distance_control->GetDataField();
    df->Set(WILDCARD);
    distance_control->RefreshDisplay();

    df = (DataFieldEnum *)direction_control->GetDataField();
    df->Set(WILDCARD);
    direction_control->RefreshDisplay();
  } else {
    DataFieldString *df = (DataFieldString *)name_control->GetDataField();
    df->Set(_T(""));
    name_control->RefreshDisplay();
  }
}

class AirspaceFilterListener: public DataFieldListener
{
private:
  /* virtual methods from DataFieldListener */
  virtual void OnModified(DataField &df) override;
};

void
AirspaceFilterListener::OnModified(DataField &df)
{
  if (&df == distance_control->GetDataField()) {
    DataFieldEnum &dfe = (DataFieldEnum &)df;
    dialog_state.distance = dfe.GetValue() != WILDCARD
      ? Units::ToSysDistance(fixed(dfe.GetValue()))
      : fixed(-1);

  } else if (&df == direction_control->GetDataField()) {
    DataFieldEnum &dfe = (DataFieldEnum &)df;
    dialog_state.direction = dfe.GetValue();

  } else if (&df == type_control->GetDataField()) {
    DataFieldEnum &dfe = (DataFieldEnum &)df;
    dialog_state.type = dfe.GetValue();
  }

  FilterMode(&df == name_control->GetDataField());
  UpdateList();
}

void
AirspaceListDialog::OnPaintItem(Canvas &canvas, const PixelRect rc,
                                unsigned i)
{
  if (airspace_list.empty()) {
    assert(i == 0);

    canvas.DrawText(rc.left + Layout::FastScale(2),
                    rc.top + Layout::FastScale(2), _("No Match!"));
    return;
  }

  assert(i < airspace_list.size());

  const AbstractAirspace &airspace = airspace_list[i].GetAirspace();

  AirspaceListRenderer::Draw(
      canvas, rc, airspace,
      airspace_list[i].GetVector(location, airspaces->GetProjection()),
      UIGlobals::GetDialogLook(), UIGlobals::GetMapLook().airspace,
      CommonInterface::GetMapSettings().airspace);
}

gcc_pure
static const TCHAR *
GetHeadingString(TCHAR *buffer)
{
  TCHAR heading[32];
  FormatBearing(heading, ARRAY_SIZE(heading),
                CommonInterface::Basic().attitude.heading);

  _stprintf(buffer, _T("%s (%s)"), _("Heading"), heading);
  return buffer;
}

static void
OnGPSUpdate(const MoreData &basic)
{
  if (dialog_state.direction == 0 && !CommonInterface::Calculated().circling) {
    const Angle heading = basic.attitude.heading;
    Angle a = last_heading - heading;
    if (a.AsDelta().AbsoluteDegrees() >= fixed(10)) {
      last_heading = heading;

      UpdateList();

      DataFieldEnum &df = *(DataFieldEnum *)direction_control->GetDataField();
      TCHAR buffer[64];
      df.replaceEnumText(0, GetHeadingString(buffer));
      direction_control->RefreshDisplay();
    }
  }
}

#ifdef GNAV

static bool
FormKeyDown(unsigned key_code)
{
  WndProperty* wp;
  unsigned new_index = dialog_state.type;

  wp = ((WndProperty *)dialog->FindByName(_T("prpFltType")));

  switch(key_code) {
  case KEY_APP1:
    new_index = WILDCARD;
    break;
  case KEY_APP2:
    new_index = RESTRICT;
    break;
  case KEY_APP3:
    new_index = PROHIBITED;
    break;

  default:
    return false;
  }

  if (dialog_state.type != new_index){
    wp->GetDataField()->SetAsInteger(new_index);
    wp->RefreshDisplay();
  }

  return true;
}

#endif /* GNAV */

static void
FillDistanceEnum(DataFieldEnum &df)
{
  df.AddChoice(0, _T("*"));

  static constexpr unsigned distances[] = {
    25, 50, 75, 100, 150, 250, 500, 1000
  };

  TCHAR buffer[64];
  const TCHAR *unit = Units::GetDistanceName();
  for (unsigned i = 0; i < ARRAY_SIZE(distances); ++i) {
    _stprintf(buffer, _T("%u %s"), distances[i], unit);
    df.AddChoice(distances[i], buffer);
  }

  df.Set(0u);
}

static void
FillDirectionEnum(DataFieldEnum &df)
{
  TCHAR buffer[64];

  df.AddChoice(WILDCARD, _T("*"));
  df.AddChoice(0, GetHeadingString(buffer));

  static constexpr unsigned directions[] = {
    360, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330
  };

  for (unsigned i = 0; i < ARRAY_SIZE(directions); ++i) {
    FormatBearing(buffer, ARRAY_SIZE(buffer), directions[i]);
    df.AddChoice(directions[i], buffer);
  }

  df.Set(WILDCARD);
}

static void
PrepareAirspaceSelectDialog()
{
  gcc_unused ScopeBusyIndicator busy;

  dialog = LoadDialog(nullptr, UIGlobals::GetMainWindow(),
                  Layout::landscape ? _T("IDR_XML_AIRSPACESELECT_L") :
                                      _T("IDR_XML_AIRSPACESELECT"));
  assert(dialog != NULL);

#ifdef GNAV
  dialog->SetKeyDownFunction(FormKeyDown);
#endif

  const DialogLook &dialog_look = UIGlobals::GetDialogLook();

  airspace_list_control = (ListControl*)dialog->FindByName(_T("frmAirspaceList"));
  assert(airspace_list_control != NULL);
  airspace_list_control->SetItemHeight(AirspaceListRenderer::GetHeight(dialog_look));

  name_control = (WndProperty*)dialog->FindByName(_T("prpFltName"));
  assert(name_control != NULL);
  name_control->SetDataField(new PrefixDataField());

  distance_control = (WndProperty*)dialog->FindByName(_T("prpFltDistance"));
  assert(distance_control != NULL);
  FillDistanceEnum(*(DataFieldEnum *)distance_control->GetDataField());
  distance_control->RefreshDisplay();

  direction_control = (WndProperty*)dialog->FindByName(_T("prpFltDirection"));
  assert(direction_control != NULL);
  FillDirectionEnum(*(DataFieldEnum *)direction_control->GetDataField());
  direction_control->RefreshDisplay();

  type_control = (WndProperty*)dialog->FindByName(_T("prpFltType"));
  assert(type_control != NULL);
  LoadFormProperty(*dialog, _T("prpFltType"), type_filter_list, WILDCARD);
}

void
ShowAirspaceListDialog(const Airspaces &_airspaces,
                       ProtectedAirspaceWarningManager *_airspace_warnings)
{
  airspace_warnings = _airspace_warnings;
  airspaces = &_airspaces;
  location = CommonInterface::Basic().location;

  PrepareAirspaceSelectDialog();

  AirspaceListDialog dialog2;
  airspace_list_control->SetItemRenderer(&dialog2);
  airspace_list_control->SetCursorHandler(&dialog2);

  AirspaceFilterListener listener;
  name_control->GetDataField()->SetListener(&listener);
  distance_control->GetDataField()->SetListener(&listener);
  direction_control->GetDataField()->SetListener(&listener);
  type_control->GetDataField()->SetListener(&listener);

  UpdateList();

  const ScopeGPSListener l(CommonInterface::GetLiveBlackboard(), OnGPSUpdate);

  dialog->ShowModal();
  delete dialog;
}

