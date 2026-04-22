## Process this file with automake to produce Makefile.in

@SET_MAKE@

AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I ../../../m4

OCCBUILD = @OCCBUILD@

EXTRA_DIST = \
	clockproto.inc \
	crashlib.occ \
	crashproto.inc \
	primes.occ \
	sieve.occ \
	sort_pump.occ \
	suspendable.occ \
	testcrash.occ \
	testdyn.occ \
	testdyn2.occ \
	testdyn3.occ \
	testlib.occ \
	testpump.occ \
	testsuspend.occ

if BUILD_DYNPROC_EXAMPLES
examples = \
	primes \
	testdyn \
	testdyn2 \
	testdyn3 \
	testpump \
	testsuspend \
	testcrash
if BUILD_SHARED
dynprocs = \
	sieve.so \
	testlib.so \
	sort_pump.so \
	suspendable.so \
	crashlib.so
endif
endif

all-local: $(examples) $(dynprocs)

clean-local:
	-$(OCCBUILD) --clean $(examples)
	rm -f $(dynprocs)

SUFFIXES = .occ .tce .so


# Depend on toolchain so targets rebuild when compiler/runtime changes
TOOLCHAIN_DEPS = \
	$(KROC_BUILD_ROOT)/compiler/occ21/occ21 \
	$(KROC_BUILD_ROOT)/translator/tranx86/tranx86 \
	$(KROC_BUILD_ROOT)/runtime/ccsp/libccsp.a

$(examples) $(dynprocs): $(TOOLCHAIN_DEPS)

.occ:
	$(OCCBUILD) --program $<

.occ.so:
	$(OCCBUILD) --object $<
	$(OCCBUILD) --library $*.lib $*.tce
	mv liboccam_$*.so $@
	-$(OCCBUILD) --clean $*.tce $*.lib
