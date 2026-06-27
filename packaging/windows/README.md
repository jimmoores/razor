# Windows packaging

The old scripts in this directory are historical KRoC application-bundle
builders. Keep them as reference material for now, but do not treat them as the
current Razor packaging path.

The practical Windows path has two layers:

1. Build native MSYS2 packages for `ucrt64`/`clang64` (`x64`) and `clangarm64`
   (`arm64`) using `packaging/windows/msys2/PKGBUILD.in`.
2. Publish signed MSI/WiX installers and render
   `packaging/windows/winget/JimMoores.Razor.yaml.in` for submission to the
   Windows Package Manager community repository.

MSYS2 is the bring-up target because Razor already uses autotools and POSIX-like
tooling. A winget package should come after the Windows install layout, signing,
and upgrade behavior are stable.

Useful MSYS2 validation commands after rendering `PKGBUILD.in`:

```sh
MINGW_ARCH=ucrt64 makepkg-mingw --cleanbuild --syncdeps --force
MINGW_ARCH=clangarm64 makepkg-mingw --cleanbuild --syncdeps --force
```

Useful winget validation command after rendering a real manifest:

```powershell
winget validate .\JimMoores.Razor.yaml
```

References:

- MSYS2 package guidelines: https://www.msys2.org/dev/pkgbuild/
- Windows Package Manager manifests: https://learn.microsoft.com/en-us/windows/package-manager/package/manifest
