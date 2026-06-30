# Razor APT repository staging

This directory contains the helper used to assemble verified `.deb` artifacts
into a simple upstream APT repository.

Generated packages and repository metadata are staged under `artifacts/apt/`,
which is intentionally ignored by git.

## Incoming artifact layout

Place built packages under:

```text
artifacts/apt/incoming/<suite>/<arch>/*.deb
```

For the current Debian targets:

```text
artifacts/apt/incoming/bookworm/amd64/
artifacts/apt/incoming/trixie/amd64/
artifacts/apt/incoming/trixie/arm64/
```

`razor-dbgsym` packages may be present beside the main `razor` package; they
will be indexed with the rest of the `.deb` files.

## Build repository metadata

Run this on a Debian/Ubuntu machine with `dpkg-scanpackages` and
`apt-ftparchive` installed:

```sh
packaging/apt/build-repo.sh \
  artifacts/apt/incoming \
  artifacts/apt/repository
```

The output repository has the usual APT shape:

```text
artifacts/apt/repository/
  dists/<suite>/Release
  dists/<suite>/main/binary-<arch>/Packages.gz
  pool/<suite>/main/r/razor/*.deb
```

To sign the repository, set `RAZOR_APT_SIGNING_KEY` to a GPG key ID, fingerprint,
or email address available to `gpg`:

```sh
RAZOR_APT_SIGNING_KEY=release@example.org \
  packaging/apt/build-repo.sh artifacts/apt/incoming artifacts/apt/repository
```

That emits both `InRelease` and `Release.gpg`. Without a signing key the helper
creates an unsigned staging repo; use `[trusted=yes]` only for local testing.

## Local test source

For a local unsigned staging repo:

```text
deb [trusted=yes] file:/absolute/path/to/artifacts/apt/repository trixie main
```

For public distribution, publish the repository over HTTPS and configure users
with a `Signed-By` keyring instead of `trusted=yes`.
