# Razor packaging plan

This document tracks the work needed to make Razor installable through
standard package managers on current 64-bit platforms.

The target set below was checked on 2026-06-27. Re-check it before each release,
because Fedora, Ubuntu interim releases, macOS, and Windows support windows move
quickly.

## Package-manager goal

Users should be able to install the native Razor toolchain with the platform
package manager they already use:

| Platform family | Primary user command | First channel | Long-term channel |
| --- | --- | --- | --- |
| Debian | `apt install razor` | signed upstream APT repo | Debian archive |
| Ubuntu | `apt install razor` | Launchpad PPA or signed upstream APT repo | Ubuntu archive, normally via Debian sync |
| Fedora | `dnf install razor` | Fedora COPR | Fedora archive |
| Rocky Linux | `dnf install razor` | COPR or signed upstream RPM repo | EPEL where dependencies allow |
| RHEL | `dnf install razor` | COPR or signed upstream RPM repo | EPEL where dependencies allow |
| macOS | `brew install razor` | upstream Homebrew tap | Homebrew core if accepted |
| Windows | `winget install <publisher>.razor` | GitHub-hosted signed installer plus winget manifest | Windows Package Manager community repository |

Official distribution archives are the right long-term destination, but they are
slow to enter and slower to update. The practical first milestone is signed
upstream repositories/taps/manifests that use the same package metadata and
smoke tests expected by the official channels.

## Version and architecture matrix

Architecture names differ by packaging ecosystem:

| User term | Debian/Ubuntu | RPM/Fedora/RHEL | macOS/Homebrew | Windows/winget |
| --- | --- | --- | --- | --- |
| x64 | `amd64` | `x86_64` | `x86_64` | `x64` |
| aarch64 | `arm64` | `aarch64` | `arm64` | `arm64` |

Initial native-build targets:

| Platform | Versions | Architectures | Notes |
| --- | --- | --- | --- |
| Debian | 13 `trixie`, 12 `bookworm` | `amd64`, `arm64` | Debian 13 is stable; Debian 12 is oldstable. |
| Ubuntu | 26.04 LTS, 24.04 LTS | `amd64`, `arm64` | Prefer the two latest LTS releases for a compiler toolchain. The literal interim target on 2026-06-27 is 25.10, but it reaches EOL in July 2026 and should not drive the first durable package promise. |
| Fedora | 44, 43 | `x86_64`, `aarch64` | Fedora normally supports the current and previous releases. |
| Rocky Linux | 10, 9 | `x86_64`, `aarch64` | Rocky 10 inherits the newer x86-64 CPU baseline from the EL10 family. |
| RHEL | 10, 9 | `x86_64`, `aarch64` | Build against the oldest supported minor in each major stream where possible. |
| macOS | 26 Tahoe, 15 Sequoia | `arm64`, `x86_64` | Homebrew currently treats both Apple Silicon and Intel as supported, but Intel support is expected to degrade after 2026. |
| Windows | 11, 10 | `x64`, `arm64` | `winget` supports modern Windows 10, Windows 11, and Windows Server 2025. |

Sources checked for this matrix:

- Debian releases: https://www.debian.org/releases/
- Ubuntu release list: https://ubuntu.com/project/docs/release-team/list-of-releases/
- Fedora lifecycle: https://docs.fedoraproject.org/en-US/releases/lifecycle/
- Rocky Linux version guide: https://wiki.rockylinux.org/rocky/version/
- RHEL release dates and lifecycle: https://access.redhat.com/articles/red-hat-enterprise-linux-release-dates
- Homebrew support tiers: https://docs.brew.sh/Support-Tiers
- Windows Package Manager manifests: https://learn.microsoft.com/en-us/windows/package-manager/package/manifest

## Package shape

Start with a single `razor` package per platform, then split once the install
surface is stable:

| Package | Contents |
| --- | --- |
| `razor` | user-facing tools: `razor`, `occbuild`, `occ21`, `tranx86`, `ilibr`, `mkoccdeps`, `occamdoc`, setup helpers |
| `razor-libs` or `razor-runtime` | CCSP runtime, librazorif, installed occam libraries |
| `razor-devel` / `libccsp-dev` | headers, static libraries, `pkg-config` metadata, m4 macros |
| `razor-doc` | manuals and generated documentation |
| `razor-examples` | installed examples and teaching material |

The first package can be monolithic if that gets users a working compiler
sooner. Debian/Fedora review will likely ask for a cleaner split, especially for
headers, libraries, examples, and optional modules with large native
dependencies.

## Release gates

A package build is releasable for a platform/architecture only after all of the
following pass on that exact platform/architecture:

1. Source tarball builds without using `.git` metadata:
   `autoreconf -vfi`, `./configure --prefix=/usr --with-toolchain=razor`,
   `make`, `make DESTDIR="$pkgdir" install`.
2. `DESTDIR` install has no references to the build directory or staging
   directory in installed scripts, pkg-config files, module files, or setup
   files.
3. Package manager lint passes:
   `lintian` for Debian/Ubuntu, `rpmlint` for RPM, `brew audit` for Homebrew,
   and `winget validate` for Windows manifests.
4. Post-install smoke tests pass from a clean login shell:
   `razor --version`, `occbuild --help`, `occ21 --help`, `tranx86 --version`.
5. A minimal occam program compiles and runs from outside the source tree.
6. CIF/runtime smoke tests pass, at minimum `cift1`, `cift15`, and
   `timeout -k 1 5 cif-commstime`.
7. Removal and upgrade work cleanly:
   install previous package, upgrade to current package, compile the smoke test,
   remove the package, and verify no owned files are left behind.

Native builders are preferred over cross-builds because the runtime needs to run
smoke tests. Cross-builds are acceptable only when followed by native execution
on the same OS and CPU family.

## Build infrastructure

Use the distro-native build tools so the package metadata stays close to what
official repositories expect:

| Family | Build tool | Notes |
| --- | --- | --- |
| Debian/Ubuntu | `sbuild` or `pbuilder` | Keep `debian/` metadata modern: `debhelper-compat (= 13)`, `Architecture: any`, `Rules-Requires-Root: no`. |
| Fedora/Rocky/RHEL | `mock` | Build one source RPM against Fedora 44/43 and EL 10/9 targets. |
| macOS | Homebrew bottle workflow | Keep the formula in an upstream tap until Homebrew core is realistic. |
| Windows | native GitHub Actions or self-hosted Windows builders | Decide first whether the deliverable is an MSI, MSIX, or portable zip plus launcher scripts. |

Recommended CI stages:

1. `dist`: create and verify a release tarball with `make distcheck` where
   available.
2. `linux-deb`: build `.deb` packages in `sbuild` for Debian and Ubuntu targets.
3. `linux-rpm`: build `.rpm` packages in `mock` for Fedora and EL targets.
4. `macos-brew`: build and bottle the Homebrew formula on `arm64` and `x86_64`.
5. `windows-winget`: build signed Windows installers for `x64` and `arm64`, then
   validate winget manifests.
6. `publish`: only after all package artifacts pass smoke tests.

## Smoke test script

`packaging/smoke-test.sh` is the shared post-install test for package jobs. Run
it after installing the package into a clean test environment:

```sh
packaging/smoke-test.sh
```

Useful environment overrides:

| Variable | Purpose |
| --- | --- |
| `razor_PREFIX` | Prefix to search for installed razor examples, default `/usr`. |
| `razor_CIF_EXAMPLES_DIR` | Exact directory containing `cift1`, `cift15`, and `cif-commstime`. |
| `razor_RUN_CIF=0` | Skip CIF runtime examples. This is for bring-up only, not release gating. |
| `razor_TIMEOUT` | Timeout command to use for `cif-commstime`, for example `gtimeout` on macOS. |

## First repository changes

The existing packaging files are historical and should not be shipped as-is:

- `packaging/debian/control` is pinned to `Architecture: i386`, uses old
  debhelper style, and references obsolete dependency names.
- `packaging/rpm/tvm.spec` is a Fedora 11-era TVM package rather than a native
  razor package.
- `packaging/osx` and `packaging/windows` predate the current 64-bit port and
  should be treated as references for intent, not as release automation.

The first implementation pass should add or modernize:

1. `debian/` packaging for the native razor toolchain, probably derived from
   `packaging/debian/` but updated for Debian 13/12 and Ubuntu 26.04/24.04.
2. `packaging/rpm/razor.spec` for Fedora and EL builds.
3. `packaging/homebrew/razor.rb` for the upstream tap.
4. `packaging/windows/winget/` manifests plus a documented Windows installer
   build.
5. Package CI that runs `packaging/smoke-test.sh` consistently across package
   formats.

Do the Debian and RPM package metadata first. They exercise the autotools
install surface most directly and will reveal path, dependency, and test issues
that macOS and Windows packaging would otherwise rediscover later.
