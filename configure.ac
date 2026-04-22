# Top-level configure.ac for the KRoC tree.

AC_INIT([kroc],[1.6.1-pre0],[kroc-bugs@kent.ac.uk])
AC_PREREQ([2.69])
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_PROG_CC

OCCAM_TOOLCHAIN
KROC_BUILD_ROOT="`pwd`"
KROC_SRC_ROOT="`(cd $srcdir && pwd)`"
OCCAM_OCCBUILD

if test "x$OCCBUILD_API_VERSION" != x3; then
  AC_MSG_ERROR([this configure script has been built with an out-of-date version of `occbuild.m4'; try clearing your `ACLOCAL' environment variable.])
fi

OCCAM_CONFIG_SUBDIRS(bin/ilibr)
OCCAM_CONFIG_SUBDIRS(bin)
OCCAM_CONFIG_SUBDIRS(build-tools/mkoccdeps)
if test $OCCBUILD_TOOLCHAIN != tock; then
  OCCAM_CONFIG_SUBDIRS(compiler/occ21)
fi
OCCAM_CONFIG_SUBDIRS(build-tools/occamdoc)

if test $OCCBUILD_TOOLCHAIN = kroc -o $OCCBUILD_TOOLCHAIN = tock; then
  OCCAM_CONFIG_TARGET_SUBDIRS(runtime/ccsp)
fi

if test $OCCBUILD_TOOLCHAIN = kroc; then
  OCCAM_CONFIG_SUBDIRS(translator/tranx86)

  OCCAM_CONFIG_TARGET_SUBDIRS(runtime/libkrocif)
fi

if test $OCCBUILD_TOOLCHAIN = tvm; then
  OCCAM_CONFIG_SUBDIRS(build-tools/plinker)
  OCCAM_CONFIG_SUBDIRS(build-tools/tenctool)
  OCCAM_CONFIG_SUBDIRS(contrib/skroc)
  OCCAM_CONFIG_SUBDIRS(build-tools/slinker)
  OCCAM_CONFIG_SUBDIRS(build-tools/tinyswig)

  OCCAM_CONFIG_TARGET_SUBDIRS(tvm/libtvm)

  AC_ARG_WITH([wrapper],
              AS_HELP_STRING([--with-wrapper=...],[Transterpreter wrapper to use (posix, none)]),
              [wrapper=$withval], [wrapper=posix])
  if test "x$wrapper" = "xposix"; then
    OCCAM_CONFIG_TARGET_SUBDIRS(tvm/posix)
  fi
fi

# The modules must be built in dependency order.
OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/inmoslibs/libsrc)
#OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/nocclibs/libsrc)
OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/bsclib/libsrc)
OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/cif/libsrc)

if test "x$KROC_RMOX" = "x"; then
  OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/dynproc/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(examples/course/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/pony/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/udc/libsrc)
fi
OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/random/libsrc)
OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/time/libsrc)
OCCAM_CONFIG_TARGET_SUBDIRS(contrib/raster/libsrc)
if test "x$KROC_RMOX" = "x"; then
  OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/useful/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/fmtoutlib/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/cdx/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/trap/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/ttyutil/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/moa/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/occSDL/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/sdlraster/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/occade/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/occGL/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/ocuda/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/button/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/video/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/graphics3d/libsrc)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/player/libsrc)

  OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/bsclib/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/button/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/cdx/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/cif/examples)
  # OCCAM_CONFIG_TARGET_SUBDIRS(examples/course/answers)
  OCCAM_CONFIG_TARGET_SUBDIRS(examples/course/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(examples/course/exercises)
  OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/dynproc/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/graphics3d/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/moa/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/occGL/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/occSDL/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/occade/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/ocuda/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/player/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/player/exercises)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/pony/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/random/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/raster/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/sdlraster/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/trap/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/ttyutil/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/udc/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(stdlib/useful/examples)
  OCCAM_CONFIG_TARGET_SUBDIRS(contrib/video/examples)

  OCCAM_CONFIG_TARGET_SUBDIRS(examples/demos/bar)
  OCCAM_CONFIG_TARGET_SUBDIRS(examples/demos/game)
  OCCAM_CONFIG_TARGET_SUBDIRS(examples/demos/robots)
  OCCAM_CONFIG_TARGET_SUBDIRS(examples/demos/shootout)
  OCCAM_CONFIG_TARGET_SUBDIRS(examples/demos/ttygames)

  OCCAM_CONFIG_TARGET_SUBDIRS(tests/cgtests)
  OCCAM_CONFIG_TARGET_SUBDIRS(tests/hereticc)
  OCCAM_CONFIG_TARGET_SUBDIRS(tests/occbench)
fi

OCCAM_CONFIG_SUBDIRS(docs)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
OCCAM_OUTPUT_SUBDIRS
