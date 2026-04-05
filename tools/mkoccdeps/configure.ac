AC_INIT([mkoccdeps],[0.9],[kroc-bugs@kent.ac.uk])
AC_CANONICAL_TARGET

AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

dnl Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET

dnl Checks for libraries.

dnl Checks for header files.
m4_warn([obsolete],
[The preprocessor macro 'STDC_HEADERS' is obsolete.
  Except in unusual embedded environments, you can safely include all
  C89 headers unconditionally.])dnl
# Autoupdate added the next two lines to ensure that your configure
# script's behavior did not change.  They are probably safe to remove.
AC_PROG_EGREP

AC_CHECK_HEADERS(unistd.h stdlib.h string.h malloc.h stdarg.h)

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

