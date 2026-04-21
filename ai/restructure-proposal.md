# KRoC Project Structure Proposal

## Current Structure & Identified Problems

The current layout is:

```
kroc/
├── tools/          compiler, translator, build driver, IDE plugin, MCU loaders, …
├── runtime/        CCSP kernel, libkrocif, support
├── modules/        stdlib, C bindings, SDL, examples, courses, robotics, GPGPU, …
├── tests/          code-gen tests, corner cases, performance comparisons
├── tvm/            Transterpreter VM (separate runtime for microcontrollers)
├── demos/
└── external-docs/
```

The user identified three concrete problems:

1. **Driver script buried in `tools/kroc/`**: The `kroc` shell script is the main user-facing
   entry point but is nested alongside internal build machinery.
2. **Tools vs Runtime boundary is unclear**: `tools/` conflates the compiler (`occ21`),
   translator (`tranx86`), user-facing driver (`kroc`), IDE plugin (`occplug`), firmware
   loaders (`brickload`, `macupdater`), and utility scripts — very different roles.
3. **`modules/` is an incoherent mix**: Core language libraries (`inmoslibs`, `bsclib`,
   `nocclibs`), C interop headers (`cif`), SDL/GL wrappers, robotics (`pioneer`, `player`),
   GPGPU (`ocuda`), a course (`course`), samples, and examples all sit side-by-side.

---

## How Comparable Projects Structure Themselves

### Erlang/OTP
```
otp/
├── erts/       Erlang Runtime System (VM + OS interface)
├── lib/        OTP applications (stdlib, kernel, mnesia, ssl, …)
├── make/       Internal build helpers
└── system/     Release assembly & documentation
```
The user-facing `erl` binary is a tiny wrapper in `erts/bin/`.  All shipped libraries are
under `lib/`, clearly separated from VM source.  Third-party packages live entirely outside
the tree.

### GHC (Glasgow Haskell Compiler)
```
ghc/
├── compiler/   The GHC compiler proper
├── rts/        The Haskell Runtime System (RTS)
├── libraries/  base, ghc-prim, integer-*, containers, …
├── utils/      Internal build utilities (only needed to build GHC itself)
├── ghc/        Thin driver (the `ghc` binary the user runs)
└── testsuite/  All tests
```
`utils/` is explicitly documentation-tagged "only needed to build GHC, not to use it."
Compiler, runtime, and stdlib are three distinct top-level concepts.

### OCaml
```
ocaml/
├── boot/       Bootstrapped compiler artefacts
├── bytecomp/   Bytecode compiler
├── asmcomp/    Native code compiler
├── runtime/    C runtime (GC, I/O, …)
├── stdlib/     Standard library
├── lib/        Additional distributed libraries
└── tools/      Small user-facing tools (ocamldep, ocamlprof, …)
```
The `ocaml` binary itself is assembled at the top level.  Examples and tutorials are kept in
a separate downstream repository entirely.

### Zig
```
zig/
├── src/        Compiler source
├── lib/        Standard library + compiler built-ins (ships with the compiler)
└── doc/        Language & stdlib documentation
```
Zig's design goal is a single `zig` binary.  Examples and community libraries are always
external.

### Racket
```
racket/
├── racket/     VM + runtime source (C)
├── pkgs/       Core packages (base, gui, …) — each independently versioned
└── collects/   Bundled collection bootstrap
```
Community packages are entirely separate.  The distinction between "core" and "contrib" is
enforced structurally.

---

## Common Patterns

Across all five projects:

| Concern | Pattern |
|---------|---------|
| Compiler | One directory, contains only compiler source |
| Runtime | One directory, contains only runtime/VM source |
| Standard library | One directory, clear versioning boundary |
| User-facing driver | Thin, lives at the top or in a dedicated `bin/` |
| Build infrastructure | `build/`, `make/`, or `utils/` — clearly marked as internal |
| Examples/samples | Either absent from tree or in a clearly named `examples/` |
| Tests | One top-level `test/` or `testsuite/` |
| Platform-specific targets (tvm, MCU) | Sub-projects or separate repositories |

---

## Proposed Structure

```
kroc/
├── bin/                  ← NEW: user-facing entry points
│   └── kroc.in           (driver script; currently tools/kroc/kroc.in)
│
├── compiler/             ← RENAMED from tools/occ21
│   └── occ21/
│
├── translator/           ← RENAMED from tools/tranx86
│   └── tranx86/
│
├── runtime/              ← UNCHANGED top-level name, but reorganised inside
│   ├── ccsp/             (CCSP kernel — unchanged)
│   ├── libkrocif/        (KRoC interface layer — unchanged)
│   └── support/          (POSIX wrappers — unchanged)
│
├── stdlib/               ← NEW: split out of modules/
│   ├── inmoslibs/        (Inmos standard libraries)
│   ├── bsclib/           (basic string/conversion)
│   ├── nocclibs/         (noCC libraries)
│   ├── fmtoutlib/        (formatted output)
│   ├── useful/           (utility procs)
│   ├── time/             (timer support)
│   ├── random/           (random numbers)
│   └── cif/              (C interface headers — part of core language ABI)
│
├── contrib/              ← NEW: optional/platform-specific modules
│   ├── graphics/         (occGL, occSDL, sdlraster, raster, graphics3d, cdx)
│   ├── media/            (video)
│   ├── robotics/         (pioneer, player, ioport, button)
│   ├── gpu/              (ocuda)
│   ├── ui/               (ttyutil, trap, moa, pony, udc)
│   └── occade/           (occade game framework)
│
├── tvm/                  ← UNCHANGED: Transterpreter is a distinct sub-project
│
├── build-tools/          ← RENAMED from the non-user parts of tools/
│   ├── occbuild/         (build system abstraction — currently tools/kroc/occbuild)
│   ├── occamdoc/         (documentation generator)
│   ├── mkoccdeps/        (dependency scanner)
│   ├── plinker/          (TVM linker)
│   ├── slinker/          (static linker)
│   ├── tinyswig/         (SWIG wrapper generator for occam)
│   ├── tenctool/         (TEN-C tool)
│   └── schemescanner/
│
├── platform/             ← NEW: hardware/OS loader tools
│   ├── brickload/        (LEGO Mindstorms firmware loader)
│   ├── macupdater/       (macOS update helper)
│   ├── winstub/          (Windows launcher stub)
│   └── winupdater/       (Windows updater)
│
├── ide/                  ← NEW: editor integrations
│   └── occplug/          (jEdit plugin — currently tools/occplug)
│
├── examples/             ← NEW: split out of modules/
│   ├── course/           (teaching material — currently modules/course)
│   └── demos/            (standalone demos — currently top-level demos/)
│
├── tests/                ← UNCHANGED name, reorganised inside
│   ├── cgtests/          (code generation tests)
│   ├── corner-cases/
│   └── benchmarks/       (currently tests/ccsp-comparisons)
│
├── doc/                  ← RENAMED from external-docs/
│
└── ai/                   ← UNCHANGED: planning documents
```

---

## Rationale for Each Change

### `bin/` — user-facing entry points
The `kroc` driver script is what end users type.  Burying it in `tools/kroc/` implies it is
an internal build artifact.  Moving the `.in` template to `bin/` makes the install layout
obvious: `configure` generates `bin/kroc` from `bin/kroc.in`, and `make install` copies it to
`$(prefix)/bin/`.

### Split `compiler/` and `translator/`
`occ21` and `tranx86` are conceptually distinct phases (front-end vs. back-end code
generator) and are independently useful (you can run `occ21` without `tranx86` to get
ETC code, or run `tranx86` on pre-compiled ETC).  Keeping them separate mirrors the GHC
split between `compiler/` (front-end+middle-end) and the code generators.

### `stdlib/` — core language libraries
The `inmoslibs`, `bsclib`, `nocclibs`, `cif` headers etc. are part of the language standard
library.  They ship with every KRoC installation and form part of the language's ABI.  They
belong next to `runtime/`, not mixed with optional SDL wrappers.

### `contrib/` — optional platform-specific modules
SDL, OpenGL, CUDA, robotics, and hardware-specific modules are optional add-ons.  They may
not build on all platforms, may depend on third-party libraries, and should be clearly
separated from what is required to compile and run basic occam programs.  This mirrors the
OTP `lib/` split between core applications (kernel, stdlib) and optional ones (mnesia, odbc).

### `build-tools/` — internal build machinery
`occbuild`, `occamdoc`, `mkoccdeps`, `plinker`, `slinker`, `tinyswig` are tools used to
build the KRoC distribution itself or to build occam programs, but they are not the language
runtime.  GHC's `utils/` directory serves exactly this role and is explicitly documented as
"not needed to use GHC, only to build it."

### `platform/` — hardware-specific loaders
`brickload`, `macupdater`, `winstub`, `winupdater` are entirely unrelated to compilation or
the runtime.  They are distribution and deployment tools for specific hardware or OS targets.
Isolating them prevents them from confusing someone navigating the compiler source.

### `ide/` — editor integrations
The jEdit plugin (`occplug`) is similarly orthogonal to the compiler.  It belongs in its own
clearly named home, consistent with how GHC maintains `ghc-vis`, `ghcide`, etc. externally.

### `examples/` — teaching material and demos
The `course/` directory and `demos/` are neither part of the standard library nor test cases.
They are documentation-by-example.  Moving them to `examples/` prevents confusion about
whether they represent canonical language behaviour.

### `tests/` stays, `benchmarks/` renamed
`ccsp-comparisons/` contains performance comparisons against Go, Erlang, Haskell, pthreads
etc.  Renaming to `benchmarks/` makes its purpose instantly clear.

### `tvm/` stays as a sub-project
The Transterpreter is a completely separate runtime for microcontrollers.  It has its own
build system, its own toolchain requirements, and its own lifecycle.  Keeping it at top level
(unchanged) signals that it is a peer to the main KRoC, not a module of it.  Long-term it
would be a good candidate for its own repository.

---

## What Does Not Change

- `runtime/ccsp/`, `runtime/libkrocif/`, `runtime/support/` — internal organisation is fine.
- `configure.ac` / `Makefile.am` structure — autotools is already used correctly; this is a
  directory renaming exercise, not a build system overhaul.
- The `ai/` convention for planning documents.
- Any `.occ` source file content — only directory organisation changes.

---

## Migration Difficulty Assessment

| Change | Effort | Risk |
|--------|--------|------|
| Create `bin/kroc.in` symlink / move | Low | Low — configure path adjustment only |
| Rename `tools/occ21` → `compiler/occ21` | Low | Low — adjust `configure.ac` paths |
| Rename `tools/tranx86` → `translator/tranx86` | Low | Low — same |
| Create `stdlib/` from `modules/` subset | Medium | Medium — Makefile.am changes |
| Create `contrib/` from `modules/` remainder | Medium | Low — no code changes |
| Rename `tools/` build tools → `build-tools/` | Medium | Medium — many path references |
| Move `tools/occplug` → `ide/occplug` | Low | Low |
| Move `tools/brickload` etc. → `platform/` | Low | Low |
| Move `demos/`, `modules/course/` → `examples/` | Low | Low |
| Rename `tests/ccsp-comparisons` → `tests/benchmarks` | Low | Low |
| Rename `external-docs/` → `doc/` | Low | Low |

The most impactful change by far is splitting `modules/` into `stdlib/` and `contrib/`,
because `Makefile.am` files in both `modules/` and downstream modules reference each other.
A careful audit of `#INCLUDE` and `#USE` paths in the modules and their `configure.ac`
fragments would be required.

The safest migration order would be: compiler → translator → build-tools → platform → ide →
examples → stdlib/contrib split (last, as it has the most cross-references).
