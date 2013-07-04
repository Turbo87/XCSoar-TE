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

#ifndef XCSOAR_SCREEN_MEMORY_OPTIMISED_HPP
#define XCSOAR_SCREEN_MEMORY_OPTIMISED_HPP

#include "PixelOperations.hpp"

#ifdef __ARM_NEON__
#include "NEON.hpp"
#endif

/**
 * This class hosts two base classes: one that is optimised (e.g. via
 * SIMD) and one that is portable (but slow).  The optimised one will
 * be used as much as possible, and for the odd remainder, we use the
 * portable version.
 */
template<typename Optimised, unsigned N, typename Portable>
class SelectOptimisedPixelOperations
  : protected Optimised, protected Portable {
public:
  typedef typename Portable::PixelTraits PixelTraits;
  typedef typename PixelTraits::color_type color_type;
  typedef typename PixelTraits::rpointer_type rpointer_type;
  typedef typename PixelTraits::const_rpointer_type const_rpointer_type;

  static constexpr unsigned PORTABLE_MASK = N - 1;
  static constexpr unsigned OPTIMISED_MASK = ~PORTABLE_MASK;

  constexpr SelectOptimisedPixelOperations(const SelectOptimisedPixelOperations<Optimised, N, Portable> &other)
    :Optimised((const Optimised &)other), Portable((const Portable &)other) {}

  template<typename... Args>
  explicit constexpr SelectOptimisedPixelOperations(Args... args)
    :Optimised(args...), Portable(args...) {}

  gcc_flatten gcc_nonnull_all
  void FillPixels(rpointer_type p, unsigned n, color_type c) const {
    const unsigned no = n & OPTIMISED_MASK;
    const unsigned np = n & PORTABLE_MASK;

    Optimised::FillPixels(p, no, c);
    Portable::FillPixels(PixelTraits::Next(p, no), np, c);
  }

  gcc_flatten gcc_nonnull_all
  void CopyPixels(rpointer_type p, const_rpointer_type q, unsigned n) const {
    const unsigned no = n & OPTIMISED_MASK;
    const unsigned np = n & PORTABLE_MASK;

    Optimised::CopyPixels(p, q, no);
    Portable::CopyPixels(PixelTraits::Next(p, no),
                         PixelTraits::Next(q, no), np);
  }
};

template<typename PixelTraits>
class AlphaPixelOperations
  : public PortableAlphaPixelOperations<PixelTraits> {
public:
  explicit constexpr AlphaPixelOperations(const uint8_t alpha)
    :PortableAlphaPixelOperations<PixelTraits>(alpha) {}
};

#ifdef __ARM_NEON__

template<>
class AlphaPixelOperations<GreyscalePixelTraits>
  : public SelectOptimisedPixelOperations<NEONAlphaPixelOperations, 16,
                                          PortableAlphaPixelOperations<GreyscalePixelTraits>> {
  using Base = SelectOptimisedPixelOperations<NEONAlphaPixelOperations, 16,
                                              PortableAlphaPixelOperations<GreyscalePixelTraits>>;

public:
  explicit constexpr AlphaPixelOperations(const uint8_t alpha)
    :SelectOptimisedPixelOperations(alpha) {}
};

#endif

#endif
