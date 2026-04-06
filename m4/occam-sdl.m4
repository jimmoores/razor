# Configure paths for SDL
# Modernized to prefer sdl12-compat via pkg-config

dnl OCCAM_PATH_SDL([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for SDL, and define SDL_CFLAGS and SDL_LIBS
dnl
AC_DEFUN([OCCAM_PATH_SDL],
[dnl 
AC_REQUIRE([AC_CANONICAL_TARGET])
AC_REQUIRE([OCCAM_MACOS_OPENGL])
AC_REQUIRE([PKG_PROG_PKG_CONFIG])

dnl Get the cflags and libraries from the sdl-config script
dnl
AC_ARG_WITH(sdl-prefix,[  --with-sdl-prefix=PFX   Prefix where SDL is installed (optional)],
            sdl_prefix="$withval", sdl_prefix="")
AC_ARG_WITH(sdl-exec-prefix,[  --with-sdl-exec-prefix=PFX Exec prefix where SDL is installed (optional)],
            sdl_exec_prefix="$withval", sdl_exec_prefix="")
AC_ARG_ENABLE(sdltest, [  --disable-sdltest       Do not try to compile and run a test SDL program],
		    , enable_sdltest=yes)

  no_sdl=""
  case "$target_os" in
    darwin*)
      enable_sdltest=no
      ;; 
    none)
      no_sdl="yes"
      ;;
  esac

  min_sdl_version=ifelse([$1], ,0.11.0,$1)

  if test "x$no_sdl" = x ; then
    dnl First try pkg-config for sdl12_compat (SDL2 backend)
    AC_MSG_CHECKING([for sdl12_compat via pkg-config])
    if test -n "$PKG_CONFIG" && $PKG_CONFIG --exists sdl12_compat; then
      AC_MSG_RESULT([yes])
      SDL_CFLAGS=`$PKG_CONFIG --cflags sdl12_compat`
      SDL_LIBS=`$PKG_CONFIG --libs sdl12_compat`
      SDL_LIBS="$SDL_LIBS $OCCAM_MACOS_OPENGL_LDFLAGS"
      no_sdl=""
    else
      AC_MSG_RESULT([no])
      dnl Fallback to standard sdl-config
      if test x$sdl_exec_prefix != x ; then
        sdl_args="$sdl_args --exec-prefix=$sdl_exec_prefix"
        if test x${SDL_CONFIG+set} != xset ; then
          SDL_CONFIG=$sdl_exec_prefix/bin/sdl-config
        fi
      fi
      if test x$sdl_prefix != x ; then
        sdl_args="$sdl_args --prefix=$sdl_prefix"
        if test x${SDL_CONFIG+set} != xset ; then
          SDL_CONFIG=$sdl_prefix/bin/sdl-config
        fi
      fi

      if test "x$prefix" != xNONE; then
        PATH="$prefix/bin:$prefix/usr/bin:$PATH"
      fi
      AC_PATH_PROG(SDL_CONFIG, sdl-config, no, [$PATH])
      AC_MSG_CHECKING(for SDL - version >= $min_sdl_version)
      if test "$SDL_CONFIG" = "no" ; then
        no_sdl=yes
      else
        SDL_CFLAGS=`$SDL_CONFIG $sdlconf_args --cflags`
        SDL_LIBS=`$SDL_CONFIG $sdlconf_args --libs`
        SDL_LIBS="$SDL_LIBS $OCCAM_MACOS_OPENGL_LDFLAGS"
        no_sdl=""
      fi
    fi
  fi

  if test "x$no_sdl" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     SDL_CFLAGS=""
     SDL_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(SDL_CFLAGS)
  AC_SUBST(SDL_LIBS)
])
