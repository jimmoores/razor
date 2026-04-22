## Process this file with automake to produce Makefile.in

@SET_MAKE@

AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I ../../../m4

OCCBUILD = @OCCBUILD@ --kroc-opts "-e"

EXTRA_DIST = \
	buffered.occ \
	commstime.occ \
	simple.occ \
	socktest.occ \
	softalt.occ \
	softalt2.occ

if BUILD_UDC_EXAMPLES
examples = \
	buffered \
	commstime \
	simple \
	socktest \
	softalt \
	softalt2
endif

all-local: $(examples)

clean-local:
	-$(OCCBUILD) --clean $(examples)

SUFFIXES = .occ .tce


# Depend on toolchain so targets rebuild when compiler/runtime changes
TOOLCHAIN_DEPS = \
	$(KROC_BUILD_ROOT)/compiler/occ21/occ21 \
	$(KROC_BUILD_ROOT)/tools/tranx86/tranx86 \
	$(KROC_BUILD_ROOT)/runtime/ccsp/libccsp.a

$(examples): $(TOOLCHAIN_DEPS)

.occ:
	$(OCCBUILD) --program $<
