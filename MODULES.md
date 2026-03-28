# KRoC Modules Reference

This document describes each module in the `modules/` directory, its purpose,
build status, dependencies, and how to enable optional modules.

## Build Summary

Of the 28 modules, 33 of 50 sub-modules build by default on AArch64 macOS.
The 17 disabled sub-modules are primarily graphical (requiring SDL 1.2),
hardware-specific (V4L, CUDA), or opt-in legacy features.

## Core Libraries (always enabled)

### inmoslibs — Inmos Standard Libraries
The foundational occam libraries ported from the original Inmos transputer
toolset. Provides mathematical functions, string handling, type conversion,
host I/O, and stream processing.

- **Provides**: forall.module, convert.module, ioconv.module, hostio.module,
  hostsp.module, splib.module, maths.module, snglmath.module, dblmath.module,
  stream.module, string.module
- **Dependencies**: None
- **Platforms**: All

### course — Teaching Library
Basic utilities and abstractions for teaching occam-pi, including shared
screen output, formatted I/O, and demonstration programs.

- **Provides**: course.module, shared_screen.module, ss.module
- **Dependencies**: None
- **Platforms**: All

### cif — C Interface to CCSP
C interface library for running occam processes from C code. Essential for
the runtime's process management and the CIF example programs.

- **Provides**: cif.module
- **Dependencies**: None
- **Platforms**: KRoC toolchain only (not TVM or Tock)

### time — Time Utilities
Timer measurement and delay utilities.

- **Provides**: time.module
- **Dependencies**: None
- **Platforms**: All

## Standard Libraries (enabled by default)

### bsclib — Basic Services C Library
File I/O, process management, socket networking, and HTTP client utilities.

- **Provides**: file.module, proc.module, sock.module, http.module
- **Dependencies**: POSIX headers
- **Platforms**: All (proclib disabled on MinGW)
- **Optional**: cspdrv.module (requires `--enable-cspdrvlib`)

### fmtoutlib — Formatted Output
Formatted string output utilities (sprintf-like functions for occam).

- **Provides**: fmtout.module
- **Dependencies**: convert.module (from inmoslibs)
- **Platforms**: All

### random — Random Number Generation
Pseudo-random number generator library.

- **Provides**: random.module
- **Dependencies**: maths.module (from inmoslibs)
- **Platforms**: All

### raster — 2D Raster Graphics
2D graphics primitives, text rendering, and image I/O (PPM, PNG).

- **Provides**: raster.module, rastergraphics.module, rastertext.module,
  rasterio.module
- **Dependencies**: course.module, file.module (for rasterio);
  libpng (optional, for PNG support)
- **Platforms**: All

### ttyutil — Terminal Utilities
VT220/ANSI terminal control for cursor movement, colours, and screen
management.

- **Provides**: ttyutil.module
- **Dependencies**: course.module
- **Platforms**: All

### useful — General Utilities
Mathematical, I/O, debugging, and formatting utility procedures.

- **Provides**: useful.module
- **Dependencies**: course.module, maths.module, time.module;
  file.module (optional, for I/O utilities);
  Python 2.2+ (build-time, for code generation)
- **Platforms**: All (trace mechanism disabled on AVR)

### trap — Process Trapping Framework
Named process synchronisation and distributed process registry.

- **Provides**: selector.module, trap.module
- **Dependencies**: file.module, sock.module, useful.module, cif.module
- **Platforms**: All

### ioport — I/O Port Interface
Hardware I/O port definitions for embedded and robotic applications.

- **Provides**: ioport.module
- **Dependencies**: file.module
- **Platforms**: All

### pioneer — Pioneer Robot Control
Interface for controlling Pioneer mobile robots via serial I/O.

- **Provides**: pioneer.module
- **Dependencies**: ioport.module
- **Platforms**: All

## Conditionally Enabled (depends on external libraries)

### occSDL — SDL Binding
SWIG-generated occam bindings to the SDL (Simple DirectMedia Layer) library
for window management, input events, and audio.

- **Provides**: occSDL.module, occSDLsound.module (if SDL_sound available)
- **Dependencies**: **SDL 1.2.0+** (sdl-config), SWIG (build-time);
  SDL_sound (optional, for audio decoding)
- **Platforms**: All (Cocoa helper on macOS)
- **Status on AArch64 macOS**: **Disabled** — requires SDL 1.2 which is
  not available; SDL 2 is installed but the API is incompatible

### sdlraster — SDL Raster Display
Bridges the raster graphics library to SDL surfaces for on-screen display.

- **Provides**: sdlraster.module, miniraster.module
- **Dependencies**: occSDL.module, raster.module
- **Platforms**: All (requires occSDL)
- **Status on AArch64 macOS**: **Disabled** — depends on occSDL

### occGL — OpenGL Binding
SWIG-generated occam bindings to OpenGL (GL and GLU).

- **Provides**: occGL.module
- **Dependencies**: **OpenGL** development headers and libraries (GL/gl.h, GL/glu.h)
- **Platforms**: All (requires OpenGL)

### button — OpenGL Button Rendering
Pre-rendered text buttons for OpenGL interfaces.

- **Provides**: button.module
- **Dependencies**: course.module, occSDL.module, occGL.module
- **Platforms**: All (requires occSDL and occGL)

### occade — Arcade Game Framework
Interactive game development framework with sprite management.

- **Provides**: occade.module, miniraster.module
- **Dependencies**: file.module, course.module, occSDL.module, raster.module,
  sdlraster.module, rasterio.module
- **Platforms**: All (requires occSDL chain)

### graphics3d — 3D Raster Graphics
3D rendering utilities including chess piece models.

- **Provides**: graphics3d.module
- **Dependencies**: course.module, maths.module, raster.module, sdlraster.module
- **Platforms**: All (requires sdlraster)

### cdx — Network Distribution
Network barriers and distributed computing primitives.

- **Provides**: cdx.module, netbar.module
- **Dependencies**: sock.module, **SDL 1.2+**
- **Platforms**: All (requires SDL)

### video — Video4Linux Frame Grabber
USB camera and video device capture via Video4Linux.

- **Provides**: video.module
- **Dependencies**: **Linux V4L headers** (linux/videodev2.h),
  **libv4lconvert**; raster.module
- **Platforms**: **Linux only**

### moa — MySQL Occam API
Occam interface to MySQL databases.

- **Provides**: moa.module
- **Dependencies**: **MySQL client library** (libmysqlclient); cif.module
- **Platforms**: All (requires MySQL)
- **To enable**: Install MySQL development package and rebuild

### player — Player/Stage Robot Simulator
Occam bindings to the Player middleware for robot simulation.

- **Provides**: occplayer.module, player.module
- **Dependencies**: **Player client library 2.1.0+**, **Python 2.2+**,
  SWIG (build-time); useful.module, occSDL.module, occGL.module,
  course.module, maths.module
- **Platforms**: All (requires Player/Stage)

### ocuda — CUDA GPU Bindings
Occam interface to NVIDIA CUDA for GPU computation.

- **Provides**: ocuda.module
- **Dependencies**: **NVIDIA CUDA 4.2+** (compute capability sm_13+)
- **Platforms**: Systems with NVIDIA GPUs

## Opt-In Modules (disabled by default)

### dynproc — Dynamic Process Loading
Runtime loading and execution of dynamically compiled occam processes.

- **Provides**: dynproc.module
- **Dependencies**: None
- **Platforms**: KRoC toolchain only
- **To enable**: Set `KROC_CCSP_ENABLE_DYNPROC=yes` before configure
- **Note**: Shared library (.so) support disabled on macOS (uses .dylib)

### pony — TCP/IP Networking
High-performance TCP/IP stack with vectored I/O by Mario Schweigler.

- **Provides**: pony.module
- **Dependencies**: POSIX headers; course.module, sock.module, file.module,
  proc.module, cif.module
- **Platforms**: KRoC toolchain only
- **To enable**: Set `KROC_CCSP_ENABLE_PONY=yes` before configure

### udc — User-Defined Channels (Legacy)
Emulation of soft-channel I/O. Preserved for legacy code compatibility.

- **Provides**: udc.module
- **Dependencies**: POSIX headers
- **Platforms**: All
- **To enable**: Pass `--enable-udc` to the udc configure script

## Dependency Graph

```
inmoslibs (core: forall, maths, convert, string, hostio, stream)
├── course
│   ├── ttyutil
│   ├── useful ← time, maths, [file]
│   │   └── trap ← file, sock, cif
│   ├── fmtoutlib ← convert
│   ├── random ← maths
│   └── raster ← [file], [libpng]
│       └── sdlraster ← occSDL
│           ├── occade
│           ├── graphics3d
│           └── (examples: drawstuff, imagetool, etc.)
├── bsclib (file, sock, proc, http)
│   ├── ioport
│   │   └── pioneer
│   ├── pony ← course, sock, file, proc, cif [opt-in]
│   └── cdx ← sock, SDL
├── cif
│   ├── moa ← MySQL
│   └── trap
├── occSDL ← SDL 1.2
│   ├── sdlraster
│   ├── occGL ← OpenGL
│   │   ├── button
│   │   └── player ← Player/Stage
│   └── occade
└── ocuda ← CUDA
```

Arrows (←) indicate "depends on". Square brackets indicate optional deps.
