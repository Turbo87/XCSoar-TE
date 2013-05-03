XCSoar-TE README
=============

XCSoar-TE is a tactical glide computer for Android, Linux, Mac OS X,
Windows and Windows CE/Mobile. It was forked from XCSoar.

This file is aimed at developers.  Users should read the manual which
can be found on the XCSoar home page (<http://www.xcsoar.org/>).


Getting the source
------------------

The XCSoar-TE source code is managed with git (<http://git-scm.com/>).  It
can be downloaded with the following command:

    git clone git://github.com/XCSoar/XCSoar-TE/

To update your repository, type:

    git pull

For more information, please refer to the git documentation.


Compiling from source
---------------------

To compile from source, you need:

- GNU make
- GNU compiler collection (gcc), version 4.6.2 or later
  or clang/LLVM 3.1 (with `make CLANG=y`)
- GNU gettext
- rsvg (<http://librsvg.sourceforge.net/>)
- ImageMagick 6.4 (<http://www.imagemagick.org/>)
- xsltproc (<http://xmlsoft.org/XSLT/xsltproc2.html>)
- Perl and XML::Parser

To cross-compile to (desktop) Windows, you need the mingw-w64 version of gcc:

- <http://mingw-w64.sourceforge.net/>

For Pocket PC / Windows CE / Windows Mobile, you need mingw32ce:

- <http://max.kellermann.name/download/xcsoar/devel/cegcc/>

For Linux/Unix, you need:

- Boost (<http://www.boost.org/>)
- zlib (<http://www.zlib.net/>)
- CURL (<http://curl.haxx.se/>)
- SDL (<http://www.libsdl.org/>)
- SDL_ttf (<http://www.libsdl.org/projects/SDL_ttf/>)
- libpng (<http://www.libpng.org/>)
- libjpeg (<http://libjpeg.sourceforge.net/>)
- OpenGL
- to run XCSoar-TE, you need one of the following fonts (Debian package):
  DejaVu (`ttf-dejavu`), Droid (`ttf-droid`), Freefont (`ttf-freefont)

On Debian, you may install these using the following command:

    apt-get install make g++ \
        libboost-dev zlib1g-dev \
        libsdl1.2-dev libsdl-ttf2.0-dev \
        libpng-dev libjpeg-dev \
        libcurl4-openssl-dev \
        libxml-parser-perl \
        librsvg2-bin xsltproc \
        imagemagick gettext \
        ttf-dejavu

For Android, you need:

- Android SDK level 16 (<http://developer.android.com/sdk/>)
- Android NDK r8e (<http://developer.android.com/sdk/ndk/>)
- Ogg Vorbis (<http://www.vorbis.com/>)
- Apache Ant (<http://ant.apache.org/>)

For Mac OS X, you need:

- GCC 4.6.2 or newer (<http://hpc.sourceforge.net/>, or homebrew, or Macports)
- Boost (<http://www.boost.org/>)
- zlib (<http://www.zlib.net/>)
- CURL (<http://curl.haxx.se/>)
- SDL (<http://www.libsdl.org/>)
- SDL_ttf (<http://www.libsdl.org/projects/SDL_ttf/>)
- libpng (<http://www.libpng.org/>)
- libjpeg (<http://libjpeg.sourceforge.net/>)
- libicns (<http://icns.sourceforge.net/>)

To compile, run:

    make TARGET=<target>

To compile on a Unix-like system (Linux, Mac), you may run the
unofficial configure script to detect the build toolchain (optional
and unsupported, not recommended):

    ./build/contrib/configure
    make

Supported targets:

- PC: desktop Windows (32 bit)

- WIN64: desktop Windows (64 bit)

- PPC2000, PPC2003: Pocket PC (for PPC2002 devices, use PPC2000 target)

- WM5: Windows Mobile 5.0 and later

- WM5X, PPC2003X: same as above with code optimised for XScale CPUs

- ALTAIR: Triadis Altair

- UNIX: Linux and other UNIX-like operating systems, including Mac OS X

- UNIX32, UNIX64: like UNIX, but explicitly choose 32 or 64 bit build

- OPT: alias for UNIX with optimisation and no debugging

- PI: cross-compiling for the Raspberry Pi

- KOBO: cross-compiling for Kobo Mini and others (experimental)

- ANDROID: Android with ARMv6 CPU

- ANDROID7: Android with ARMv7 CPU

- ANDROID7NEON: Android with ARMv7 CPU and NEON extension

- ANDROID86: Android with x86 CPU

- ANDROIDMIPS: Android with MIPS CPU

- ANDROIDFAT: "fat" Android package for all supported CPUs

- CYGWIN: desktop Windows with Cygwin (experimental)

- WINE: WineLib (experimental)


Submitting patches
------------------

Subscribe to the XCSoar developer mailing list:

- <https://lists.sourceforge.net/lists/listinfo/xcsoar-devel>
- xcsoar-devel@lists.sourceforge.net

Use this mailing list to submit patches or "git pull" requests.

- patches should be self-contained
- patches should be self-documenting  
  (add a good description on what is changed, and why you are changing it)
- write one patch for one change
