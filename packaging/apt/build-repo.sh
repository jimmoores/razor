#!/bin/sh
# Build a simple APT repository from staged Razor .deb artifacts.

set -eu

usage()
{
	cat >&2 <<'EOF'
Usage: packaging/apt/build-repo.sh INCOMING_DIR OUTPUT_REPO_DIR

INCOMING_DIR must contain packages under:
  INCOMING_DIR/<suite>/<arch>/*.deb

Example:
  artifacts/apt/incoming/bookworm/amd64/razor_..._amd64.deb
  artifacts/apt/incoming/trixie/amd64/razor_..._amd64.deb
  artifacts/apt/incoming/trixie/arm64/razor_..._arm64.deb

Set RAZOR_APT_SIGNING_KEY to sign Release metadata with gpg.
EOF
	exit 2
}

require_cmd()
{
	command -v "$1" >/dev/null 2>&1 || {
		printf 'build-repo.sh: missing required command: %s\n' "$1" >&2
		exit 1
	}
}

absolute_dir()
{
	path=$1
	if test -d "$path"; then
		(cd "$path" && pwd)
	else
		parent=$(dirname "$path")
		base=$(basename "$path")
		mkdir -p "$parent"
		printf '%s/%s\n' "$(cd "$parent" && pwd)" "$base"
	fi
}

test "$#" -eq 2 || usage

incoming=$(absolute_dir "$1")
repo=$(absolute_dir "$2")

test -d "$incoming" || {
	printf 'build-repo.sh: incoming directory does not exist: %s\n' "$incoming" >&2
	exit 1
}

require_cmd apt-ftparchive
require_cmd awk
require_cmd cp
require_cmd date
require_cmd dpkg-scanpackages
require_cmd find
require_cmd gzip
require_cmd mkdir
require_cmd mktemp
require_cmd rm
require_cmd sha256sum
require_cmd sort
require_cmd wc
require_cmd xargs

if test "${RAZOR_APT_SIGNING_KEY:-}" != ""; then
	require_cmd gpg
fi

tmp=$(mktemp -d "${TMPDIR:-/tmp}/razor-apt-repo.XXXXXX")
trap 'rm -rf "$tmp"' EXIT HUP INT TERM

suite_arches=$tmp/suite-arches
: >"$suite_arches"

rm -rf "$repo/dists" "$repo/pool"
mkdir -p "$repo"

find "$incoming" -mindepth 2 -maxdepth 2 -type d | sort | while IFS= read -r archdir; do
	rel=${archdir#"$incoming"/}
	suite=${rel%%/*}
	arch=${rel##*/}

	deb_count=$(find "$archdir" -maxdepth 1 -type f -name '*.deb' ! -name '._*' | wc -l | awk '{print $1}')
	test "$deb_count" != "0" || continue

	case $arch in
		amd64|arm64)
			;;
		*)
			printf 'build-repo.sh: warning: indexing unexpected architecture directory: %s\n' "$arch" >&2
			;;
	esac

	pool_dir=$repo/pool/$suite/main/r/razor
	mkdir -p "$pool_dir"
	find "$archdir" -maxdepth 1 -type f -name '*.deb' ! -name '._*' -exec cp -p {} "$pool_dir/" \;
	printf '%s %s\n' "$suite" "$arch" >>"$suite_arches"
done

test -s "$suite_arches" || {
	printf 'build-repo.sh: no .deb artifacts found below %s\n' "$incoming" >&2
	exit 1
}

sort -u "$suite_arches" -o "$suite_arches"

while IFS=' ' read -r suite arch; do
	binary_dir=$repo/dists/$suite/main/binary-$arch
	mkdir -p "$binary_dir"
	(
		cd "$repo"
		dpkg-scanpackages --arch "$arch" "pool/$suite/main/r/razor" /dev/null >"dists/$suite/main/binary-$arch/Packages"
	)
	gzip -n -9 -c "$binary_dir/Packages" >"$binary_dir/Packages.gz"
done <"$suite_arches"

awk '{print $1}' "$suite_arches" | sort -u | while IFS= read -r suite; do
	archs=$(awk -v suite="$suite" '$1 == suite {print $2}' "$suite_arches" | sort -u | xargs)
	(
		cd "$repo"
		apt-ftparchive \
			-o "APT::FTPArchive::Release::Origin=Razor" \
			-o "APT::FTPArchive::Release::Label=Razor" \
			-o "APT::FTPArchive::Release::Suite=$suite" \
			-o "APT::FTPArchive::Release::Codename=$suite" \
			-o "APT::FTPArchive::Release::Architectures=$archs" \
			-o "APT::FTPArchive::Release::Components=main" \
			-o "APT::FTPArchive::Release::Description=Razor upstream package repository" \
			release "dists/$suite" >"dists/$suite/Release"

		if test "${RAZOR_APT_SIGNING_KEY:-}" != ""; then
			gpg --batch --yes --local-user "$RAZOR_APT_SIGNING_KEY" \
				--clearsign --digest-algo SHA256 \
				-o "dists/$suite/InRelease" "dists/$suite/Release"
			gpg --batch --yes --local-user "$RAZOR_APT_SIGNING_KEY" \
				--armor --detach-sign \
				-o "dists/$suite/Release.gpg" "dists/$suite/Release"
		else
			rm -f "dists/$suite/InRelease" "dists/$suite/Release.gpg"
		fi
	)
done

{
	printf 'Razor APT repository\n'
	printf 'Generated: %s\n' "$(date -u '+%Y-%m-%dT%H:%M:%SZ')"
	printf 'Incoming: %s\n' "$incoming"
	printf 'Repository: %s\n\n' "$repo"
	printf 'Suites and architectures:\n'
	awk '{print "  " $1 " " $2}' "$suite_arches"
	printf '\nArtifacts:\n'
	(
		cd "$repo"
		find pool -type f -name '*.deb' | sort | while IFS= read -r file; do
			set -- $(sha256sum "$file")
			printf '  %s  %s\n' "$1" "$file"
		done
	)
} >"$repo/MANIFEST.txt"

printf 'APT repository written to %s\n' "$repo"
if test "${RAZOR_APT_SIGNING_KEY:-}" = ""; then
	printf 'Repository is unsigned; set RAZOR_APT_SIGNING_KEY to emit InRelease and Release.gpg.\n' >&2
fi
