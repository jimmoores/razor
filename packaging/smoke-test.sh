#!/bin/sh
# Post-install smoke test for Razor/KRoC packages.

set -eu

die()
{
	printf 'smoke-test: %s\n' "$*" >&2
	exit 1
}

note()
{
	printf 'smoke-test: %s\n' "$*" >&2
}

run()
{
	note "+ $*"
	"$@"
}

check_cmd()
{
	command -v "$1" >/dev/null 2>&1 || die "missing command: $1"
}

check_tool()
{
	name=$1
	shift
	note "checking $name"
	"$@" >/dev/null 2>&1 || die "$name check failed: $*"
}

find_cif_examples()
{
	if test "${KROC_CIF_EXAMPLES_DIR:-}" != ""; then
		printf '%s\n' "$KROC_CIF_EXAMPLES_DIR"
		return
	fi

	prefix=${KROC_PREFIX:-/usr}
	for dir in \
		"$prefix/lib/kroc/examples/cif" \
		"$prefix/lib/kroc/"*examples/cif \
		"/usr/local/lib/kroc/examples/cif" \
		"/usr/local/lib/kroc/"*examples/cif
	do
		if test -x "$dir/cift1"; then
			printf '%s\n' "$dir"
			return
		fi
	done

	return 1
}

run_commstime()
{
	prog=$1

	timeout_prog=${KROC_TIMEOUT:-}
	if test "$timeout_prog" = ""; then
		if command -v timeout >/dev/null 2>&1; then
			timeout_prog=timeout
		elif command -v gtimeout >/dev/null 2>&1; then
			timeout_prog=gtimeout
		fi
	fi

	if test "$timeout_prog" != ""; then
		note "+ $timeout_prog -k 1 5 $prog"
		set +e
		"$timeout_prog" -k 1 5 "$prog"
		status=$?
		set -e
		case $status in
			0|124)
				return 0
				;;
			*)
				die "cif-commstime failed with status $status"
				;;
		esac
	fi

	note "+ $prog (manual 5 second timeout)"
	"$prog" &
	pid=$!
	sleep 5
	kill -TERM "$pid" 2>/dev/null || true
	sleep 1
	kill -KILL "$pid" 2>/dev/null || true
	wait "$pid" 2>/dev/null || true
}

check_cmd kroc
check_cmd occbuild
check_cmd occ21
check_cmd tranx86

check_tool "kroc" kroc --version
check_tool "occbuild" occbuild --help
check_tool "occ21" occ21 -i
check_tool "tranx86" tranx86 --version

tmpbase=${TMPDIR:-/tmp}
workdir=$(mktemp -d "$tmpbase/kroc-smoke.XXXXXX")
trap 'rm -rf "$workdir"' EXIT HUP INT TERM

cd "$workdir"
cat >hello.occ <<'EOF'
#INCLUDE "course.module"
PROC hello (CHAN BYTE out!)
  out.string ("KRoC smoke test*n", 0, out!)
:
EOF

run occbuild --program hello.occ
run ./hello >hello.out
grep 'KRoC smoke test' hello.out >/dev/null 2>&1 || die "hello program did not print expected output"

if test "${KROC_RUN_CIF:-1}" != "0"; then
	cif_dir=$(find_cif_examples) || die "CIF examples not found; set KROC_CIF_EXAMPLES_DIR or KROC_RUN_CIF=0"
	test -x "$cif_dir/cift1" || die "missing $cif_dir/cift1"
	test -x "$cif_dir/cift15" || die "missing $cif_dir/cift15"
	test -x "$cif_dir/cif-commstime" || die "missing $cif_dir/cif-commstime"

	run "$cif_dir/cift1"
	run "$cif_dir/cift15"
	run_commstime "$cif_dir/cif-commstime"
fi

note "ok"
