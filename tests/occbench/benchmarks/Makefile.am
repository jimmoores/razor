@SET_MAKE@

AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I ../../m4

# Disable usage checking
OCCBUILD = @OCCBUILD@ --occ21-opts '-NU'

EXTRA_DIST = \
	fft2.occ \
	fft2-noresched.occ \
	fft2-base.occ \
	fft3.occ \
	mandelbrot.occ

if BUILD_BENCHMARKS
benchmarks = \
	fft2 \
	fft2-noresched \
	fft3 \
	mandelbrot
endif

all-local: $(benchmarks)

install-exec-local: $(benchmarks)
	$(OCCBUILD) --install-examples occbench --prefix "$(prefix)" --destdir "$(DESTDIR)" $(benchmarks)

clean-local:
	-$(OCCBUILD) --clean $(benchmarks)

SUFFIXES = .occ


# Depend on toolchain so targets rebuild when compiler/runtime changes
TOOLCHAIN_DEPS = \
	$(KROC_BUILD_ROOT)/compiler/occ21/occ21 \
	$(KROC_BUILD_ROOT)/tools/tranx86/tranx86 \
	$(KROC_BUILD_ROOT)/runtime/ccsp/libccsp.a

$(benchmarks): $(TOOLCHAIN_DEPS)

.occ:
	$(OCCBUILD) --program $<
