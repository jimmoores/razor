@SET_MAKE@

AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I ../../../m4

OCCBUILD = @OCCBUILD@

EXTRA_DIST = \
	mysqlclient.occ \
	simple.occ \
	test1.occ \
	test2.occ

if BUILD_MOA_EXAMPLES
examples = \
	mysqlclient \
	simple \
	test1 \
	test2
endif

all-local: $(examples)

install-exec-local: $(examples)
	$(OCCBUILD) --install-examples moa --prefix "$(prefix)" --destdir "$(DESTDIR)" $(examples)

clean-local:
	-$(OCCBUILD) --clean $(examples)

SUFFIXES = .occ


# Depend on toolchain so targets rebuild when compiler/runtime changes
TOOLCHAIN_DEPS = \
	$(KROC_BUILD_ROOT)/compiler/occ21/occ21 \
	$(KROC_BUILD_ROOT)/translator/tranx86/tranx86 \
	$(KROC_BUILD_ROOT)/runtime/ccsp/libccsp.a

$(examples): $(TOOLCHAIN_DEPS)

.occ:
	$(OCCBUILD) --program $<
