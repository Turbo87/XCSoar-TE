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

#ifndef XCSOAR_PAGES_HPP
#define XCSOAR_PAGES_HPP

struct PageLayout;

namespace Pages
{
  /**
   * Opens the given page.
   * @param page The page to open
   */
  void Open(unsigned page);
  /**
   * Opens the next page.
   */
  void Next();
  /**
   * Opens the previous page.
   */
  void Prev();

  /**
   * Opens the given layout.
   * Attention! Internally the previous page is still selected.
   * @param layout The layout to open
   */
  void OpenLayout(const PageLayout &layout);

  unsigned NextIndex();
  unsigned PrevIndex();

  void Update();
};

#endif
