## process this file with automake to produce Makefile.in

@SET_MAKE@

AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I ../m4

SUBDIRS = sc lib api

EXTRA_DIST = \
	README

htmldocdir = $(datadir)/razor/doc/html
dist_htmldoc_DATA = \
	kroc-faq.html \
	occ21-extensions.html \
	udo.html

txtdocdir = $(datadir)/razor/doc/txt
dist_txtdoc_DATA = \
	AUTHORS \
	OLD-AUTHORS \
	about-razor.txt \
	essentially-razor.txt \
	hlps.txt \
	hostio.txt \
	o2cl.txt \
	pragma.txt

psdocdir = $(datadir)/razor/doc/ps
dist_psdoc_DATA = \
	blocking-native.ps \
	bsclib.ps \
	native-call.ps \
	o2cl.ps \
	udc.ps \
	udo.ps

msdocdir = $(datadir)/razor/doc/ms
dist_msdoc_DATA = \
	o2cl.ms

pythondocdir = $(datadir)/razor/doc/python
dist_pythondoc_DATA = \
	moinmoinwiki-nocc-langdef.py \
	moinmoinwiki-occampi.py
