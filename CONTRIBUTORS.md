# Contributors

This file consolidates attribution from the per-subsystem `AUTHORS`
files and the git changelog.  The two sections are complementary:
the AUTHORS files capture pre-git copyright ownership and role-based
authorship of the original toolchain; the git history captures who
has touched the tree since the SVN → git import.

## Subsystem heritage (pre-git)

Drawn from [`docs/AUTHORS`](docs/AUTHORS) and
[`compiler/occ21/AUTHORS`](compiler/occ21/AUTHORS).

### `compiler/occ21` (occam compiler and libraries)

Original copyright: **INMOS Ltd.** and **SGS-Thomson
Microelectronics Ltd.** (1995–1997).

Subsequent modifications:

- **Michael Poole** &mdash; bug fixes, KRoC output, native DEC Alpha
  output, language enhancements.
- **Jim Moores** &mdash; user-defined operators, `INITIAL`
  declarations.
- **Dave Beckett** &mdash; portability and configuration; author of
  the GNU autotools build system across the tree
  (github: [dajobe](https://github.com/dajobe)).
- **Fred Barnes** &mdash; `MOBILE` data types, `STEP` in replicators,
  and many other language and back-end extensions.
- **David Wood** &mdash; further occ21 modifications (1998–2000,
  per `docs/AUTHORS`).
- **Carl Ritson** &mdash; co-maintained occ21 modifications with
  Fred Barnes (2006–2007).

### `runtime/ccsp` (CCSP runtime kernel)

- **Jim Moores** &mdash; original author (1999, University of Kent).
- **Fred Barnes** &mdash; modifications 1999–2006, then with
  **Carl Ritson** 2006–2007.

### `translator/tranx86` (ETC → IA32 / MIPS / SPARC / PPC translator)

- **Fred Barnes** &mdash; original author (2000–2006, University of
  Kent).
- **Carl Ritson** &mdash; co-maintained modifications with Fred
  Barnes (2006–2007).

### KRoC libraries, utilities, demonstrators

Copyright 2003–2007 **Fred Barnes**, **Carl Ritson**,
**Adam Sampson**, **Peter Welch**, **David Wood**, University of
Kent, and others.

The pre-git tree was maintained by Fred Barnes
&lt;F.R.M.Barnes@kent.ac.uk&gt;.

## Git changelog

Every author who has landed at least one commit since the git history
begins, sorted by commit count.  Date range is the years of their
first and last commits.

| Commits | Period    | Name                |
| -------:| :-------- | :------------------ |
| 793     | 2006–2017 | Adam Sampson        |
| 609     | 2007–2011 | Carl Ritson         |
| 525     | 2005–2015 | Fred Barnes         |
| 401     | 2008–2011 | Christian Jacobsen  |
| 386     | 2025–2026 | Jim Moores          |
| 310     | 2008–2020 | Matt Jadud          |
|  98     | 2010      | drew                |
|  34     | 2010      | Ian Armstrong       |
|  29     | 2008–2009 | Neil Brown          |
|  21     | 2010      | Anthony Smith       |
|  20     | 2008–2010 | John Simpson        |
|   9     | 2009–2012 | Peter Welch         |
|   7     | 2013      | Martin Ellis        |
|   6     | 2007      | Damian Dimmich      |
|   5     | 2010      | David Gilmore       |
|   5     | 2010      | djs                 |
|   4     | 2011      | Steve Pretty        |
|   4     | 2010      | Radu Creanga        |
|   3     | 2010      | Aaron Ryan          |
|   1     | 2014      | Sarah Mount         |

Notes:

- "Carl Ritson" appears in commit metadata as
  `C.G. Ritson <cgr@kent.ac.uk>`.
- "Christian Jacobsen" is rendered "Christian Jacbosen" on the
  original commits (typo preserved on the commits, name corrected
  here).
- The handles `drew` and `djs` are how the SVN-imported commits
  identify those authors; their full names are not recorded in the
  changelog.

To regenerate the git table:

```sh
git shortlog -sne --all
```
