Razor
=====
Razor is an occam/occam-pi language platform, comprised of an occam compiler, native-code translator and supporting run-time system. It began as a fork of the Kent Retargetable occam Compiler and is being modernised for contemporary 64-bit systems.

Razor adds:
* Real 64-bit compiler supporting aarch64 and x64 architectures on both macOS and Linux
* Modernised legacy code in the occ21 compiler (no more K&R!)
* A new directory layout that is easier to navigate and more consistent with other projects
* Build system fixes and migration of Python 2.x to 3.x
* Additional compiler code generation tests (cgtests) to prevent regressions
* Docs to prompt AI coding tools for additional feature development

## Prerequisites
To compile and use Razor, you will need to have the following already installed on your machine:

* the bash shell
* the GNU toolchain -- i.e. GCC 2.95.2 or later, binutils 2.0 or later, GNU awk, GNU make
* pkg-config
* the C development headers
* Python 2.4 or later
* xsltproc

Some occam-pi modules can optionally make use of other native libraries if available:

* libpng
* SDL and SDL_Sound
* OpenGL
* MySQL
* Player

To check out and update Razor from the Git repository, you will also need:

* Git
* automake 1.8 or later, autoconf 2.52 or later (but not autoconf 2.64, which is buggy)

On Debian-based systems, we suggest installing the following packages:

```bash
apt-get install bash gcc binutils gawk make automake autoconf pkg-config \
libc6-dev libsdl12-compat-dev libsdl-sound1.2-dev libgl1-mesa-dev \
libmysqlclient15-dev libpng12-dev libxmu-dev libxi-dev \
libplayercore2-dev libplayerc2-dev libltdl3-dev \
perl python xsltproc git
```

## Getting the Razor source
There are two supported ways of obtaining a Razor source tree:

### From Git
Clone the Razor repository:

```bash
git clone https://github.com/jimmoores/razor.git razor
```

Once you've got a Razor distribution this way, you can use `git pull` in the
`razor` directory at any time to update the source tree.

### From a tarball

Release tarballs, when published, extract to a `razor-VERSION` directory.


## Installation (for end users)

For most users who've downloaded a Razor source tree, the easiest way to compile and install Razor is to use the `build` script that's provided.

`build` supports several options; to list all of them, run:

```bash
./build --help
```

A typical invocation will look like:

```bash
./build --prefix=/usr/local/razor
```

It's often convenient to use an installation directory somewhere inside your home directory (e.g. `$HOME/razor`) because then you can build and install Razor as your regular user without needing to fiddle with permissions.

Alternatively, you can install Razor directly into a system prefix such as `/usr/local` (the default if you don't specify `--prefix`), which generally means you won't need to source `razor-setup.sh` to set up your environment (assuming you have `/usr/local/lib` listed in `/etc/ld.so.conf`).

### Build script options

| Option | Description |
|--------|-------------|
| `--prefix=DIRECTORY` | Select directory to install into (e.g. `/usr/local`) |
| `--with-toolchain=tvm` | Build the Transterpreter toolchain instead of the native Razor toolchain |
| `--enable-pony` | Enable pony networking support for cluster/networked channels |

### The Transterpreter

If the machine you want to run occam-pi programs on doesn't have an IA32 (x86) or AMD64 (x86-64) processor, you'll need to use the Transterpreter, an interpretive implementation of the occam-pi virtual machine:

```bash
./build --with-toolchain=tvm --prefix=/usr/local/razor
```


## Installation (for developers and packagers)

Razor uses GNU automake. If you've downloaded a tarball release, you can install it in the same way as any other automake package:

```bash
./configure --prefix=...
make
make install
```

If you've downloaded Razor from Git, there won't be a `configure` script in the tree yet; you need to generate it first:

```bash
autoreconf -vfi
```

After you have configured Razor once, typing `make` will usually regenerate the automake files if necessary -- unless a new directory has been added to the Razor source tree, in which case another `autoreconf -vfi` will be necessary.

### Configure options

The `configure` script accepts the standard GNU autotools options plus these Razor-specific ones:

| Option | Description |
|--------|-------------|
| `--prefix=DIR` | Installation prefix (default: `/usr/local`) |
| `--with-toolchain=ENV` | Select occam toolchain: native (default), `tvm`, or `tock` |
| `--with-wrapper=WRAPPER` | Transterpreter wrapper to use: `posix` (default) or `none` (TVM toolchain only) |
| `--enable-pony` | Enable pony networking support |

Razor's installation process supports `DESTDIR`, so it should be straightforward to create OS distribution packages.


## Make targets

The following `make` targets are available after running `configure`:

### Building

| Target | Description |
|--------|-------------|
| `make` | Build the entire Razor system (compiler, translator, runtime, and all enabled modules). Displays a summary of enabled/disabled modules on completion. |
| `make -C compiler/occ21` | Build only the occ21 occam compiler |
| `make -C translator/tranx86` | Build only the tranx86 native-code translator |
| `make -C runtime/ccsp` | Build only the CCSP runtime system |

### Testing

| Target | Description |
|--------|-------------|
| `make check` | Run the Razor test suite (code generation tests under `tests/cgtests/`) |
| `make -C tests/cgtests check` | Run just the code generation tests |

### Installing

| Target | Description |
|--------|-------------|
| `make install` | Install Razor into the configured `--prefix` directory |

### Cleaning

| Target | Description |
|--------|-------------|
| `make clean` | Remove built object files, libraries, and generated occam artifacts (`.tce`, `.lib`, `.lbb`, `.module`) |
| `make distclean` | Remove all files generated by `configure` and `make`, restoring the tree to its distributed state |
| `make rebuild-libs` | Clean all occam object files and libraries (but not tools or runtime), then rebuild all modules. Useful after modifying `tranx86` when the occ21 output (`.tce` files) is unchanged. |


## Configuration

Before using Razor, remember to source the relevant setup file:

* `. PREFIX/bin/razor-setup.sh` for Bourne-style shells such as bash

* `source PREFIX/bin/razor-setup.csh` for C-style shells such as csh/tcsh

It may be convenient to include this command in your shell's startup file (e.g. `~/.bash_profile`, `~/.cshrc`, or `/etc/profile`).

### Verifying the installation

To check that your new Razor installation works, put this into a file called `hello.occ`:

```occam
#INCLUDE "course.module"
PROC hello (CHAN BYTE out!)
  out.string ("Hello, world!*n", 0, out!)
:
```

and run `occbuild --program hello.occ` to compile it; this should give you a binary called `hello`.


## Directories

  bin/

    User-facing entry points (the native driver `razor`, `occbuild`, `ilibr`,
    and related setup scripts).

  compiler/

    The occam compiler (`occ21`).

  translator/

    The native-code translator (`tranx86`).

  runtime/

    Low-level runtime support code (CCSP kernel, libkrocif,
    POSIX wrappers).

  tvm/

    The Transterpreter virtual machine (a distinct runtime for
    microcontrollers), including `libtvm` and platform wrappers.

  stdlib/

    Core language libraries that ship with every Razor installation -
    part of the language's ABI. Each module has `libsrc/` (source) and
    typically `examples/`.

  contrib/

    Optional, platform-specific modules (SDL, OpenGL, CUDA, robotics,
    terminal UI, networked processes, etc.). May not build on all
    platforms or may require external libraries.

  build-tools/

    Internal tools used to build Razor itself or to build occam programs
    (`occamdoc`, `mkoccdeps`, `plinker`, `slinker`, `tinyswig`,
    `tenctool`, `schemescanner`) - not needed to use Razor once installed.

  platform/

    OS/hardware-specific loader and updater tools (LEGO Mindstorms,
    Windows stub/updater, macOS updater).

  ide/

    Editor integrations (the jEdit plugin `occplug`).

  examples/

    Teaching material (`course/`) and standalone demo programs (`demos/`).

  tests/

    Functional test programs for occam-pi systems - `cgtests/`,
    `corner-cases/`, `trivial/`, `benchmarks/` (cross-language
    comparisons), plus `hereticc/` and `occbench/`.

  docs/

    Various items of documentation - `essentially-razor.txt` is a good
    place to start; `docs/reference/` holds archived Inmos / upstream
    PDFs.

  packaging/

    Distribution packaging scripts (Debian, RPM, macOS, Windows).

  licenses/

    Copies of relevant software licenses (see Licensing below).

Note: most directories contain further README files.


## Licensing

Razor is free software. In general, tools are made available under the
GNU General Public License (v2 or later), and libraries are made
available under the GNU Lesser General Public License (v2 or later).
Information about the GNU licenses can be found at:

http://www.fsf.org/licensing/


## Reporting problems

Known bugs are given in the top-level BUGS file. If Razor fails to
compile or work correctly on your system, please open an issue with a
brief description of the problem and a copy of the "typescript" file
generated by "build".
