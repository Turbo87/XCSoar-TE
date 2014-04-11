/* Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2014 The XCSoar Project
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

#ifndef FLATELLIPSE_HPP
#define FLATELLIPSE_HPP

#include "FlatPoint.hpp"
#include "FlatLine.hpp"
#include "Compiler.h"

/**
 * 2-d ellipse in real-valued projected coordinates, with methods for
 * intersection tests etc.  The ellipse itself need not be axis-aligned.
 */
class FlatEllipse
{
  FlatPoint f1, f2, ap;
  FlatPoint p;
  fixed a;
  fixed b;
  Angle theta;

  Angle theta_initial;

public:
  /** 
   * Constructor.
   * 
   * @param _f1 Focus A
   * @param _f2 Focus B
   * @param _ap Any point on the ellipse
   * 
   * @return Initialised object
   */
  FlatEllipse(const FlatPoint &_f1, const FlatPoint &_f2, const FlatPoint &_ap);

  /**
   * Dummy constructor for zero-sized ellipse
   *
   * @return Initialised object
   */
  FlatEllipse()
    :f1(fixed(0), fixed(0)),
     f2(fixed(0), fixed(0)),
     ap(fixed(0), fixed(0)),
     p(fixed(0), fixed(0)),
     a(fixed(1)), b(fixed(1)),
     theta(Angle::Zero()),
     theta_initial(Angle::Zero()) {}

  /**
   * Parametric representation of ellipse
   *
   * @param t Parameter [0,1]
   *
   * @return Location on ellipse
   */
  gcc_pure
  FlatPoint Parametric(const fixed t) const;

  /**
   * Find intersection of line from focus 1 to p, through the ellipse
   *
   * @param p Reference point
   * @param i1 Intersection point 1 if found
   * @param i2 Intersection point 2 if found
   *
   * @return True if line intersects
   */
  bool IntersectExtended(const FlatPoint &p, FlatPoint &i1, FlatPoint &i2) const;

private:
  gcc_pure
  fixed ab() const;

  gcc_pure
  fixed ba() const;

  bool Intersect(const FlatLine &line, FlatPoint &i1, FlatPoint &i2) const;
};

#endif
