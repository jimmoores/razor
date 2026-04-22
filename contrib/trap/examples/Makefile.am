## Process this file with automake to produce Makefile.in

@SET_MAKE@

AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I ../../../m4

OCCBUILD = @OCCBUILD@

EXTRA_DIST = \
	groupops.occ \
	traplatency.occ \
	traptest.occ

if BUILD_TRAP_EXAMPLES
examples = \
	groupops \
	traplatency \
	traptest
endif

all-local: $(examples)

install-exec-local: $(examples)
	$(OCCBUILD) --install-examples trap --prefix "$(prefix)" --destdir "$(DESTDIR)" $(examples)

clean-local:
	-$(OCCBUILD) --clean $(examples)

SUFFIXES = .occ .tce


# Depend on toolchain so targets rebuild when compiler/runtime changes
TOOLCHAIN_DEPS = \
	$(KROC_BUILD_ROOT)/compiler/occ21/occ21 \
	$(KROC_BUILD_ROOT)/translator/tranx86/tranx86 \
	$(KROC_BUILD_ROOT)/runtime/ccsp/libccsp.a

$(examples): $(TOOLCHAIN_DEPS)

.occ:
	$(OCCBUILD) --program $<
