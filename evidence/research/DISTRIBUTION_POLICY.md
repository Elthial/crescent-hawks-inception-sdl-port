# Original-files and distribution policy

This document records the project's preservation and distribution boundary. It
is a project requirement, not a general legal opinion.

## Local reference installation

- `chinception/` contains the copyrighted original game installation used for
  research and verification.
- The directory is local-only and is covered by the repository's
  `chinception/` ignore rule.
- Files in this directory may be read, decoded, traced, or executed locally by
  research tools, but must not be staged, committed, packaged, or uploaded.
- Derived copies of external assets—such as extracted pictures, maps, animation
  frames, audio, or full text/data dumps from external files—must likewise not
  be added to distributable source or build output.
- The two small README screenshots in `docs/screenshots/` are a narrowly scoped
  illustrative exception requested for project presentation. They are not
  licensed as project source or reusable game assets; review their publication
  rights before making the repository public.

`BTech-Reko-expanded/` is also ignored and contains another copyrighted
`BTECH.EXE` used to generate the clean Reko baseline. The binary is subject to
the same local-only rule. The generated ASM, disassembly intermediate, and
pseudo-C are research inputs; before publishing any generated material, retain
only the portions and derived annotations needed to reconstruct the replacement
and review them under the project's executable-derived-material boundary.

## Replacement boundary

The end product replaces the original game's `.EXE`. It does not replace or
redistribute the rest of the original installation.

```text
user's original installation
  original levels, maps, art, animation, audio, and other data files
                         │
                         ▼
                replacement C17 executable
```

Consequently:

- the replacement must locate, validate, and load the original external files;
- a clean checkout must not contain enough external game data to run by itself;
- a user must provide a compatible original copy;
- loaders should preserve original filenames and formats where practical;
- converted caches or debugging exports remain local and ignored.

The planned later C# port is a separate effort; this repository's current
preservation executable is C17 with an SDL3 platform layer.

## Executable-derived material

For this project's scope, material embedded within the original executable may
be represented in maintained source when needed to replace the executable. This
includes reconstructed program logic, text strings, lookup tables, constants,
and data arrays found inside the `.EXE`.

The original executable binary itself remains a local research input and is not
committed. Executable-derived source should retain provenance—segment/address or
file offset—so it can be audited against the original.

## Tests and tools

- Unit tests should use synthetic minimal data where possible.
- Integration tests may read `chinception/` locally and should skip clearly when
  the original installation is absent.
- Test snapshots must contain structural facts or hashes, not copied external
  asset payloads.
- Tools must not default to writing extracted copyrighted assets inside tracked
  directories.
- CI and release packaging must not require or upload `chinception/`.
