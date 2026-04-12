## Process this file with automake to produce Makefile.in

@SET_MAKE@
AM_CPPFLAGS = -I@srcdir@/../include

noinst_LIBRARIES=libkernel.a

AUTOMAKE_OPTIONS=foreign

DEFS=@DEFS@ @EXTRA_DEFS@ -Wall -fPIC

libkernel_a_SOURCES = bsyscalls.c deadlock.c dynproc.c entry.c sched.c stats.c
if CCSP_AARCH64
libkernel_a_SOURCES += aarch64_cif.S
endif
if CCSP_X64
libkernel_a_SOURCES += x64_cif.S
endif
if CCSP_I386
libkernel_a_SOURCES += i386_cif.S
endif

