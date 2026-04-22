## Process this file with automake to produce Makefile.in

@SET_MAKE@

AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I ../../../m4

OCCBUILD = @OCCBUILD@

EXTRA_DIST = \
	args.occ \
	getopt.occ \
	getoptlong.occ \
	lsdemo.occ \
	pingdemo.occ \
	sockdaytime.occ \
	sockstreamcli.occ \
	sockstreamserv.occ \
	socktcpcli.occ \
	socktcpserv.occ \
	sockudpcli.occ \
	sockudpserv.occ \
	test-getopt.sh

if BUILD_BSCLIB_EXAMPLES
examples = \
	args \
	getopt \
	getoptlong \
	lsdemo \
	sockdaytime \
	sockstreamcli \
	sockstreamserv \
	socktcpcli \
	socktcpserv \
	sockudpcli \
	sockudpserv

if OCCBUILD_KROC
examples += \
	pingdemo
endif
endif

all-local: $(examples)

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
