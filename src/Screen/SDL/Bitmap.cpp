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

#include "Screen/Bitmap.hpp"
#include "Screen/Debug.hpp"
#include "ResourceLoader.hpp"
#include "OS/ConvertPathName.hpp"

#include "Screen/SDL/Format.hpp"

#ifdef ENABLE_OPENGL
#include "Screen/OpenGL/Texture.hpp"
#include "Screen/OpenGL/Debug.hpp"
#endif

#include <SDL_endian.h>
#include <SDL_image.h>

#ifdef WIN32
  #include <windows.h>
#endif

#include <assert.h>

bool
Bitmap::Load(SDL_Surface *_surface, Type type)
{
  assert(IsScreenInitialized());
  assert(_surface != NULL);

#ifdef ENABLE_OPENGL
  assert(texture == NULL);
  assert(pthread_equal(pthread_self(), OpenGL::thread));

  texture = new GLTexture(_surface);
  size.cx = _surface->w;
  size.cy = _surface->h;
  SDL_FreeSurface(_surface);

  return true;
#else
  assert(surface == NULL);

  switch (type) {
  case Type::STANDARD:
    surface = ConvertToDisplayFormat(_surface);
    break;

  case Type::MONO:
    // XXX convert?
    surface = _surface;

    assert(surface->format->palette != NULL &&
           surface->format->palette->ncolors == 2);
    break;
  }

  return true;
#endif
}

#ifndef USE_LIBPNG

bool
Bitmap::Load(unsigned id, Type type)
{
  assert(IsScreenInitialized());

  Reset();

  ResourceLoader::Data data = ResourceLoader::Load(id);
  if (data.first == NULL)
    return false;

#ifdef WIN32
  const BITMAPINFO *info = (const BITMAPINFO *)data.first;
  if (data.second < sizeof(*info))
    return false;

  int pitch = (((info->bmiHeader.biWidth * info->bmiHeader.biBitCount + 7) / 8 - 1) | 3) + 1;
  int data_size = pitch * info->bmiHeader.biHeight;

  /* duplicate the BMP file and re-insert the BITMAPFILEHEADER which
     is not included in this .EXE file */
  size_t size = data.second;
  BITMAPFILEHEADER *header = (BITMAPFILEHEADER *)malloc(sizeof(*header) + size);
  if (header == NULL)
    /* out of memory */
    return false;

  /* byte order?  this constant is correct according to MSDN */
  header->bfType = 0x4D42;
  header->bfSize = sizeof(*header) + size;
  header->bfReserved1 = 0;
  header->bfReserved2 = 0;
  header->bfOffBits = sizeof(BITMAPFILEHEADER) + size - data_size;
  memcpy(header + 1, data.first, data.second);

  const void *bmp_data = header;
  size_t bmp_size = sizeof(*header) + size;
#else
  const void *bmp_data = data.first;
  size_t bmp_size = data.second;
#endif

  SDL_RWops *rw = SDL_RWFromConstMem(bmp_data, bmp_size);

#ifdef WIN32
  SDL_Surface *original = ::SDL_LoadBMP_RW(rw, 1);
  free(header);
#else
  SDL_Surface *original = ::IMG_LoadPNG_RW(rw);
  SDL_RWclose(rw);
#endif

  if (original == NULL)
    return false;

  Load(original, type);

  return true;
}

#endif /* !USE_LIBPNG */

bool
Bitmap::LoadStretch(unsigned id, unsigned zoom)
{
  assert(zoom > 0);

  // XXX
  return Load(id);
}

#ifndef USE_LIBPNG

bool
Bitmap::LoadFile(const TCHAR *path)
{
  NarrowPathName narrow_path(path);
  SDL_Surface *original = ::IMG_Load(narrow_path);
  return original != NULL && Load(original);
}

#endif /* !USE_LIBPNG */

#ifndef ENABLE_OPENGL

void
Bitmap::Reset()
{
  assert(!IsDefined() || IsScreenInitialized());

  if (surface != NULL) {
    SDL_FreeSurface(surface);
    surface = NULL;
  }
}

const PixelSize
Bitmap::GetSize() const
{
  assert(IsDefined());

  return { surface->w, surface->h };
}

#endif /* !OpenGL */
