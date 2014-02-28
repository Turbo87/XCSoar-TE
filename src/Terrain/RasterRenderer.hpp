/*
Copyright_License {

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

#ifndef XCSOAR_RASTER_RENDERER_HPP
#define XCSOAR_RASTER_RENDERER_HPP

#include "Terrain/HeightMatrix.hpp"
#include "Screen/RawBitmap.hpp"
#include "Math/fixed.hpp"
#include "Util/NonCopyable.hpp"

#ifdef ENABLE_OPENGL
#include "Geo/GeoBounds.hpp"
#endif

#define NUM_COLOR_RAMP_LEVELS 13

class Angle;
class Canvas;
class RasterMap;
class WindowProjection;
struct ColorRamp;

class RasterRenderer : private NonCopyable {
  /** screen dimensions in coarse pixels */
  unsigned quantisation_pixels;

#ifdef ENABLE_OPENGL
  /**
   * The value of #quantisation_pixels that was used in the last
   * ScanMap() call.
   */
  unsigned last_quantisation_pixels;
#endif

  /**
   * Step size used for slope calculations.  Slope shading is disabled
   * when this attribute is 0.
   */
  unsigned quantisation_effective;

#ifdef ENABLE_OPENGL
  /**
   * The area that was rendered previously into the #HeightMatrix and
   * the #RawBitmap.  This attribute is used to decide whether the
   * texture has to be redrawn.
   */
  GeoBounds bounds;
#endif

  HeightMatrix height_matrix;
  RawBitmap *image;

  unsigned char *contour_column_base;

  fixed pixel_size;

  BGRColor color_table[256 * 128];

public:
  RasterRenderer();
  ~RasterRenderer();

  const HeightMatrix &GetHeightMatrix() const {
    return height_matrix;
  }

  unsigned GetWidth() const {
    return height_matrix.GetWidth();
  }

  unsigned GetHeight() const {
    return height_matrix.GetHeight();
  }

#ifdef ENABLE_OPENGL
  void Invalidate() {
    bounds.SetInvalid();
  }

  /**
   * Calculate a new #quantisation_pixels value.
   *
   * @return true if the new #quantisation_pixels value is smaller
   * than the previous one (redraw needed)
   */
  bool UpdateQuantisation();

  const GeoBounds &GetBounds() const {
    return bounds;
  }

  const GLTexture &BindAndGetTexture() const {
    return image->BindAndGetTexture();
  }
#endif

  /**
   * Generate the color table.
   */
  void ColorTable(const ColorRamp *color_ramp, bool do_water,
                  unsigned height_scale, int interp_levels);

  /**
   * Scan the map and fill the height matrix.
   */
  void ScanMap(const RasterMap &map, const WindowProjection &projection);

  /**
   * Convert the height matrix into the image.
   */
  void GenerateImage(bool do_shading,
                     unsigned height_scale, int contrast, int brightness,
                     const Angle sunazimuth,
                     bool do_contour);

  const RawBitmap &GetImage() const {
    return *image;
  }

protected:
  /**
   * Convert the height matrix into the image, without shading.
   */
  void GenerateUnshadedImage(unsigned height_scale,
                             const unsigned contour_height_scale);

  /**
   * Convert the height matrix into the image, with slope shading.
   */
  void GenerateSlopeImage(unsigned height_scale, int contrast,
                          const int sx, const int sy, const int sz,
                          const unsigned contour_height_scale);

  /**
   * Convert the height matrix into the image, with slope shading.
   */
  void GenerateSlopeImage(unsigned height_scale,
                          int contrast, int brightness,
                          const Angle sunazimuth,
                          const unsigned contour_height_scale);

private:

  void ContourStart(const unsigned contour_height_scale);
};

#endif
