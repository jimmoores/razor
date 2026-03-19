## Process this file with automake to produce Makefile.in

@SET_MAKE@

AUTOMAKE_OPTIONS = foreign subdir-objects no-dependencies
ACLOCAL_AMFLAGS = -I ../../m4

KROC=@KROC@
ILIBR=@ILIBR@

DEFS = -I$(srcdir)/include @DEFS@ @KROC_CCSP_CFLAGS@ @KROC_CCSP_CINCPATH@ -fPIC
EXTRALDFLAGS = @KROC_CCSP_LDFLAGS@ @KROC_CCSP_LIBPATH@

SUBDIRS = include

noinst_LIBRARIES = libkrocif.a
if BUILD_SHARED
shared_libs = @LIBKROCIF@
endif

libkrocif_a_SOURCES = \
	io/io.c \
	io/input.occ \
	io/output.occ \
	rts/cif.c \
	rts/occam_entry.c \
	rts/rtsmain.c \
	rts/tty.c \
	rts/userproc.c \
	support/spinterf.c \
	support/spunix.c

all-local: libkrocif.a $(shared_libs) rts/rtsmain.o

if BUILD_SHARED
@LIBKROCIF@: $(libkrocif_a_OBJECTS)
	$(CC) @SLINKFLAGS@ $(CFLAGS) $(EXTRALDFLAGS) -Wl,-Bsymbolic -lccsp -o @LIBKROCIF@ $(libkrocif_a_OBJECTS)
endif

install-exec-local: libkrocif.a $(shared_libs) rts/rtsmain.o
	$(mkinstalldirs) "$(DESTDIR)$(libdir)"
	$(INSTALL_DATA) libkrocif.a "$(DESTDIR)$(libdir)/libkrocif.a"
	$(INSTALL_DATA) rts/rtsmain.o "$(DESTDIR)$(libdir)/rtsmain.o"
if BUILD_SHARED
	$(INSTALL_DATA) @LIBKROCIF@ "$(DESTDIR)$(libdir)/@LIBKROCIF@"
else
	rm -f "$(DESTDIR)$(libdir)/@LIBKROCIF@"
endif

CLEANFILES = libkrocif.a $(shared_libs) *~ io/*.o rts/*.o support/*.o

SUFFIXES=.occ

.occ.o:
	$(KROC) $(KROCOPTS) -c -v -o $@ $<
