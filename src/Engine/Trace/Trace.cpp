/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2011 The XCSoar Project
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

#include "Trace.hpp"
#include "Navigation/Aircraft.hpp"
#include <algorithm>

Trace::Trace(const unsigned _no_thin_time, const unsigned max_time,
             const unsigned max_points)
  :m_max_time(max_time),
   no_thin_time(_no_thin_time),
   m_max_points(max_points),
   m_opt_points((3*max_points)/4)
{
  m_last_point.time = null_time;
  assert(max_points >= 4);
}

void
Trace::clear()
{
  chronological_list.clear();
  m_last_point.time = null_time;
  m_average_delta_distance = 0;
  m_average_delta_time = 0;
  delta_list.clear();
}

void
Trace::append(const AIRCRAFT_STATE& state)
{
  if (empty()) {
    // first point determines origin for flat projection
    task_projection.reset(state.get_location());
    task_projection.update_fast();
    m_last_point.time = null_time;
  } else if (!chronological_list.empty() &&
             state.Time < fixed(m_last_point.time)) {
    // gone back in time, must reset. (shouldn't get here!)
    assert(1);
    clear();
    return;
  }

  TracePoint tp(state);
  tp.project(task_projection);

  // only add one item per two seconds
  if ((tp.time - m_last_point.time) < 2)
    return;

  tp.project(task_projection);
  m_last_point = tp;

  delta_list.append(tp, chronological_list);
}

unsigned
Trace::get_min_time() const
{
  if (m_last_point.time == null_time ||
      m_max_time == null_time)
    return 0;

  return std::max(0, (int)m_last_point.time - (int)m_max_time);
}

bool
Trace::optimise_if_old()
{
  if (chronological_list.size() >= m_max_points) {
    // first remove points outside max time range
    bool updated = delta_list.erase_earlier_than(get_min_time(),
                                                 chronological_list);

    if (chronological_list.size() >= m_opt_points)
      // if still too big, remove points based on line simplification
      updated |= delta_list.erase_delta(m_opt_points, chronological_list,
                                        no_thin_time);

    if (!updated)
      return false;

  } else if (chronological_list.size() * 2 == m_max_points) {
    // half size, appropriate time to remove old points
    if (!delta_list.erase_earlier_than(get_min_time(), chronological_list))
      return false;

  } else
    return false;

  m_average_delta_distance = delta_list.calc_average_delta_distance(no_thin_time);
  m_average_delta_time = delta_list.calc_average_delta_time(no_thin_time);

  return true;
}

unsigned
Trace::size() const
{
  return chronological_list.size();
}

bool
Trace::empty() const
{
  return delta_list.empty();
}

bool
Trace::is_null(const TracePoint& tp)
{
  return tp.time == null_time;
}

void
Trace::get_trace_edges(TracePointVector &v) const
{
  v.clear();

  if (chronological_list.size() >= 2) {
    v.reserve(2);
    v.push_back(chronological_list.front().point);
    v.push_back(chronological_list.back().point);
  }
}
