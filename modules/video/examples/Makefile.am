@SET_MAKE@

AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I ../../../m4

OCCBUILD = @OCCBUILD@

EXTRA_DIST = \
	fastvid.occ \
	vidgrab.occ

if BUILD_VIDEO_EXAMPLES
examples = \
	fastvid \
	vidgrab
endif

all-local: $(examples)

install-exec-local: $(examples)
	$(OCCBUILD) --install-examples video --prefix "$(prefix)" --destdir "$(DESTDIR)" $(examples)

clean-local:
	-$(OCCBUILD) --clean $(examples)

SUFFIXES = .occ


# Depend on toolchain so targets rebuild when compiler/runtime changes
TOOLCHAIN_DEPS = \
	$(KROC_BUILD_ROOT)/compiler/occ21/occ21 \
	$(KROC_BUILD_ROOT)/tools/tranx86/tranx86 \
	$(KROC_BUILD_ROOT)/runtime/ccsp/libccsp.a

$(examples): $(TOOLCHAIN_DEPS)

.occ:
	$(OCCBUILD) --program $<
