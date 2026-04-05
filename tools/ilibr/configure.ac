AC_INIT([ilibr],[1.1],[kroc-bugs@kent.ac.uk])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

dnl Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AC_DEFINE(HOST_OS_IS_UNIX,1,[define if host OS is UNIX])

dnl Checks for header files.
m4_warn([obsolete],
[The preprocessor macro 'STDC_HEADERS' is obsolete.
  Except in unusual embedded environments, you can safely include all
  C89 headers unconditionally.])dnl
# Autoupdate added the next two lines to ensure that your configure
# script's behavior did not change.  They are probably safe to remove.
AC_PROG_EGREP

AC_CHECK_HEADERS(string.h stdlib.h string.h strings.h unistd.h)

AC_CONFIG_FILES([Makefile tcofflib/Makefile])
AC_OUTPUT
