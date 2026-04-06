dnl Process this file with autoconf to produce a configure script.

AC_INIT
AC_CONFIG_SRCDIR([ioport.inc])
AC_PREREQ([2.69])

dnl for automake 1.1 
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE(ioport, 1.0)

dnl Checks for programs.
AC_PROG_CC
OCCAM_OCCBUILD

dnl Checks for occam modules.
OCCAM_INCLUDE([file.module], ,
              [AC_MSG_ERROR(Required modules not found)])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
