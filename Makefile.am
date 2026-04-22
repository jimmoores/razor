# Top-level Makefile.am for the KRoC tree.

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = @occam_subdirs@

EXTRA_DIST = \
	.gitignore \
	BUGS \
	CHANGELOG \
	README \
	examples/demos/README \
	modules/button/README \
	modules/cif/README \
	examples/course/README \
	examples/course/etc/README \
	examples/course/etc/reading.list \
	modules/dynproc/README \
	modules/fmtoutlib/README \
	modules/moa/README \
	modules/nocclibs/README \
	modules/occGL/README \
	modules/occSDL/README \
	modules/pony/README \
	modules/sdlraster/README \
	modules/udc/README

aclocaldir = $(prefix)/share/aclocal
dist_aclocal_DATA = \
	m4/kroc.m4 \
	m4/occam-mysql.m4 \
	m4/occam-opengl.m4 \
	m4/occam-pkg.m4 \
	m4/occam-sdl.m4 \
	m4/occam-cuda.m4 \
	m4/occbuild.m4 \
	m4/tvm.m4

all-local: display-message

.PHONY: display-message
display-message:
	@modules_enabled=; \
	modules_disabled=; \
	touch in-tree-modules; \
	while read file path deps; do \
	  name="`echo $$file | sed 's,.module,,'`"; \
	  if test "$$path" = "-"; then \
	    modules_disabled="$$modules_disabled $$name"; \
	  else \
	    modules_enabled="$$modules_enabled $$name"; \
	  fi; \
	done <in-tree-modules; \
	num_enabled="`echo $$modules_enabled | wc -w`"; \
	num_disabled="`echo $$modules_disabled | wc -w`"; \
	num_total="`echo $$modules_enabled $$modules_disabled | wc -w`"; \
	printf "\nKRoC has now been built.\n\n"; \
	printf "Modules enabled ($$num_enabled/$$num_total):\n $$modules_enabled\n\n"; \
	printf "Modules disabled ($$num_disabled/$$num_total):\n $$modules_disabled\n\n"; \
	printf "Please report problems with KRoC to <$(PACKAGE_BUGREPORT)>.\n\n"

DISTCLEANFILES = in-tree-modules

# Thorough cleanup of all generated occam build artifacts across the tree.
# The per-directory clean-local targets use occbuild --clean which can
# silently fail (e.g. on permission errors).  This catches anything missed.
# Uses git ls-files to avoid deleting tracked source files (.lbb, .module).
clean-local:
	cd $(srcdir) && git ls-files --others --ignored --exclude-standard \
		-- '*.tce' '*.lib' '*.lbb' '*.module' 'liboccam_*.a' \
		'liboccam_*.so' 'liboccam_*.dylib' | xargs rm -f 2>/dev/null; true

# Rebuild all occam libraries from their .tce files after tranx86 changes.
# Use this when tranx86 has been modified but occ21 output (.tce) is unchanged.
# This avoids stale .o files compiled by an older version of tranx86.
.PHONY: rebuild-libs
rebuild-libs:
	@echo "Cleaning all occam object files and libraries..."
	@cd $(srcdir) && git ls-files --others --ignored --exclude-standard \
		-- '*.o' '*.lib' '*.lbb' '*.module' 'liboccam_*.a' \
		'liboccam_*.so' 'liboccam_*.dylib' 'forall-*' | \
		grep -v 'tools/' | grep -v 'runtime/' | xargs rm -f 2>/dev/null; true
	@echo "Rebuilding all modules..."
	$(MAKE) all
