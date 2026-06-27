# Homebrew packaging

This directory contains the upstream-tap starting point for installing Razor
with Homebrew on macOS `arm64` and `x86_64`.

`razor.rb.in` is a release template rather than a checked-in tap formula. Render
it during a release after producing an immutable source archive and SHA-256
checksum:

```sh
sed \
  -e "s|@RAZOR_VERSION@|1.6.1-pre0|g" \
  -e "s|@RAZOR_TARBALL_URL@|https://github.com/jimmoores/razor/releases/download/v1.6.1-pre0/razor-1.6.1-pre0.tar.gz|g" \
  -e "s|@RAZOR_TARBALL_SHA256@|...|g" \
  packaging/homebrew/razor.rb.in > Formula/razor.rb
```

For the first public channel, keep this formula in an upstream tap such as
`jimmoores/homebrew-razor`. Homebrew core can come later, after the package has
stable release cadence, enough users, and passing bottle automation.

Useful validation commands in the tap:

```sh
brew install --build-from-source ./Formula/razor.rb
brew test razor
brew audit --strict --online razor
```

The formula follows the standard Homebrew formula structure documented at
https://docs.brew.sh/Formula-Cookbook.
