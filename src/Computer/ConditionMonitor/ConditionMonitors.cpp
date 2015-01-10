/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2015 The XCSoar Project
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

#include "ConditionMonitors.hpp"
#include "ConditionMonitorAATTime.hpp"
#include "ConditionMonitorFinalGlide.hpp"
#include "ConditionMonitorGlideTerrain.hpp"
#include "ConditionMonitorLandableReachable.hpp"
#include "ConditionMonitorStartRules.hpp"
#include "ConditionMonitorSunset.hpp"
#include "ConditionMonitorWind.hpp"

static ConditionMonitorWind cm_wind;
static ConditionMonitorFinalGlide cm_finalglide;
static ConditionMonitorSunset cm_sunset;
static ConditionMonitorAATTime cm_aattime;
static ConditionMonitorStartRules cm_startrules;
static ConditionMonitorGlideTerrain cm_glideterrain;
static ConditionMonitorLandableReachable cm_landablereachable;

void
ConditionMonitorsUpdate(const NMEAInfo &basic, const DerivedInfo &calculated,
                        const ComputerSettings &settings)
{
  cm_wind.Update(basic, calculated, settings);
  cm_finalglide.Update(basic, calculated, settings);
  cm_sunset.Update(basic, calculated, settings);
  cm_aattime.Update(basic, calculated, settings);
  cm_startrules.Update(basic, calculated, settings);
  cm_glideterrain.Update(basic, calculated, settings);
  cm_landablereachable.Update(basic, calculated, settings);
}
