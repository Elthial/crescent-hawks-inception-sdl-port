# Preservation and porting action plan

## Goal

Understand the complete workflow of the 1987 DOS game *BattleTech: The
Crescent Hawk's Inception*, document its executable and data formats, improve
the InceptionTools C# tooling, and establish a verified basis for a portable C#
implementation that requires the user to provide an original game copy.

The deliverable replaces the original `.EXE` and deliberately continues using
the user's original external levels, maps, artwork, animations, audio, and other
data files. Executable-embedded logic, strings, constants, tables, and arrays may
be reconstructed in source; the original executable and external files are not
distributed.

The imported `UnBattletech-main/` tree is reference material and optional
tooling. This repository will not publish its findings back into that imported
project. Its Spice86 and DOSBox-X setups may be reused later when they provide
useful, repeatable evidence. Its Reko output is not the address-comparison
baseline because its input executable version is uncertain.

`BTech-Reko-expanded/` is the clean pre-annotation Reko baseline, containing
ASM, `.dis` intermediate output, pseudo-C, and the executable used to generate
them. It replaces UnBattletech's Reko material for comparison with `Btech/`.
Its executable is the unpacked analysis form of the packed/encrypted installed
`chinception/BTECH.EXE`; both profiles are retained because their raw file
offset domains differ.

## Working principles

- `chinception/` is the ignored local reference installation and must never be
  committed or uploaded.
- Original external assets are required runtime inputs, never replacements or
  bundled resources.
- The C# result replaces the original executable, not the original data files.
- The maintained decompile and planned port target the EGA pipeline. Removed
  Tandy, CGA, and other obsolete graphics implementations are historical
  executable context, not current reimplementation requirements.
- Documentation correctness precedes implementation.
- Hand-authored findings are preserved and cross-checked, not overwritten by an
  automated decompile.
- Binary evidence and observed runtime behaviour outrank inferred names.
- Reverse-engineering changes are submitted in small function blocks with a
  review stop after each block.
- Unknowns and disagreement remain explicit.
- The 16-bit little-endian data model is recorded explicitly: native words and
  near offsets are normally two bytes, far pointers normally four, and packed
  byte fields remain byte-sized when the instructions and record stride prove it.
- Reko's 32-bit fallback types are treated as uncertain annotations. Original
  16-bit C `int` and near-pointer widths are the default unless assembly proves a
  dword or 16:16 far pointer; embedded assembly boundaries are documented.
- Exceptionally ambiguous blocks may be queued for a focused Astra Medium
  confirmation pass in `docs/investigations/ASTRA_REVIEW.md`.

## Phase 1 — Consolidate the research

Status: **complete and approved by the project owner**. Structure references,
the `BTECH.h` coverage audit, original-file header verification, dedicated
format documents, normalized segmented memory maps, and an assembly-verified
BLD loader/opcode specification are present. Detailed decoder/runtime
experiments are routed to Phase 2 or 4.

1. Create the top-level documentation structure and evidence policy.
2. Convert the useful content of `BTECH.h` into readable references while
   retaining `BTECH.h` unchanged as the historical scratch pad.
3. Consolidate all known file-format notes under `docs/formats/`.
4. Document mech, weapon, and character records and their important arrays.
5. Build a contradiction/open-question register for incompatible claims.
6. Add provenance links back to relevant source addresses, functions, and
   imported research.
7. Document the known unpacking/relocation relationship between the installed
   executable and the expanded Reko segments before transferring file-offset
   claims between profiles.

Exit criteria:

- Each known format has an indexed reference or an explicit unknown-status
  entry.
- Mech, weapon, and character layouts have offset tables and confidence labels.
- No disputed field is presented as settled.
- The first review with the project owner is complete.

## Phase 2 — Establish verification fixtures

Status: **complete**. Phase 2A adds configurable installation discovery and
a read-only metadata inventory. Phase 2B adds bounded hexdumps, explicit raw and
decoded-BLD offset domains, stronger graphics/map/sound validation, and expanded
synthetic checks. Phase 2C adds a read-only structured dump of verified save,
character, mech, finance, and final-position offsets without using the legacy
drifting parser. Phase 2D adds a lossless structured view of the verified
33-record weapon table and its packed combat fields. Phase 2E adds lossless BLD
container decoding and conservative human-readable disassembly through the
first unresolved variable inline table.

1. Define a configurable original-game installation path, using ignored
   `chinception/` as the local development default when present.
2. Inventory expected files without committing copyrighted originals.
3. Validate names, lengths, headers, and optionally known hashes.
4. Add tiny synthetic fixtures for parser edge cases.
5. Add raw-byte inspection and structured dump commands.
6. Compare tool output against annotated code, disassembly, runtime traces, and
   the ignored local reference installation.

Exit criteria: important structure and format claims can be reproduced with a
documented command and a user-supplied original game copy.

## Phase 3 — Modernize and extend InceptionTools

Status: **in progress**. Phase 3A retargets the maintained toolkit from
unsupported `.NET Core 3.1` to `.NET 8` without changing parser behaviour.
Phase 3B updates the two legacy dependencies and verifies identical BMP output.
Phase 3C introduces a shared bounded little-endian reader and migrates verified
inspection paths without changing their output models.
Phase 3D replaces the legacy character, mech, weapon, and drifting save parsers
with reusable lossless record models while retaining obsolete compatibility
adapters where useful.
Phase 3E adds targeted read-only character and mech queries by save group and
slot, with text and JSON command-line output.
Phase 3F adds a lossless ANM container model and read-only metadata inspection,
and corrects the legacy class's false first-word size interpretation without
yet changing animation frame decoding.
Phase 3G implements the assembly-verified ANM token grammar, exact source cursor
accounting, retained XOR frame state, and per-frame inspection metadata. It does
not yet convert packed frames to pixels or export/play animations.
Phase 3H verifies the EGA conversion chain and adds bounded expansion of packed
ANM nibbles into 88×88 palette-index frames plus non-content inspection
statistics. Rendering, export, timing, and playback remain later blocks.
Phase 3I adds dependency-free, overwrite-safe export of a selected frame to an
88×88 indexed PNG using an explicit standard-EGA palette default. Whole-sequence
export, animation timing, GIF creation, and playback remain later blocks.
Phase 3J adds preflighted, overwrite-safe export of every accumulated ANM frame
as a deterministic numbered PNG sequence. Timing, animated GIF creation, and
looping playback remain later blocks.
Phase 3K identifies the final 19 ANM header bytes as an 18-byte timing table and
scale byte, reproduces the executable's signed arithmetic, and exposes exact
per-frame vertical-retrace counts. Display-rate conversion, GIF creation, and
playback remain later blocks.
Phase 3L adds dependency-free looping GIF89a output for one ANM or the complete
22-file set. It converts exact retrace waits using an explicit nominal 60 Hz
policy, preserves zero delays, preflights batch decoding and collisions before
writing, and validates all 198 original frames with an independent decoder.
Phase 3M1 verifies and implements the bounded full-screen CMP/ICN RLE decoder,
including format-2 column traversal, nibble expansion, and portable indexed PNG
transparency. It is the loader foundation for reorganized MECHSHAP sheets.
Phase 3M2 reproduces all 376 executable-defined MECHSHAP source rectangles and
exports a transparent PNG plus JSON metadata. Locust and Commando silhouettes
are arranged into real directional walk/fire/kick rows; debris, fire, impacts,
wreckage, fallen sprites, and all infantry/personnel sets follow at the bottom.
The personnel blocks are now labelled as red teammates, light-blue Jason
Youngblood, and grey enemies-or-civilians; hostility cannot be inferred from
the shared grey palette alone.
Phase 3N adds a command-line text save editor over reusable fetch/update APIs.
It fingerprints the selected source, patches only changed verified fields,
preserves undocumented bytes, reports byte-level changes, and refuses to
overwrite the source save. Unchanged round trips are exact for all six local
original saves.

1. Use the root `InceptionTools/` project as the maintained tooling location.
2. Upgrade from the obsolete `.NET Core 3.1` target without changing decoded
   behaviour.
3. Separate reusable parsing APIs from command-line presentation.
4. Add bounded little-endian readers and useful malformed-input diagnostics.
5. Implement BLD container, payload decoding, text, opcode, operand, and control-flow
   parsing, initially as a lossless disassembler rather than a gameplay engine.
6. Provide simple APIs and CLI output for mech, weapon, and character data.
7. Add regression tests based on user-supplied originals and synthetic data.

Exit criteria: tools can inspect all documented formats and export BLD, mech,
weapon, and character information without embedding original assets.

The broader extraction, animation, save-editing, and sound goals are maintained
in [the InceptionTools roadmap](INCEPTIONTOOLS_ROADMAP.md).

## Phase 4 — Reconcile and annotate the executable

Work through bounded function clusters in this order:

1. Startup, hardware detection, and file loading.
2. Main loop, input dispatch, and mode transitions.
3. Map loading, movement, collision, and encounters.
4. Party, character, inventory, and save-state handling.
5. Building selection and BLD execution.
6. Dialogue, shops, and story-state transitions.
7. Mech initialization, equipment, repair, and salvage.
8. Combat setup, turns, targeting, attacks, damage, heat, and resolution.
9. Graphics, animation, sound, and shutdown.

For each review block, record:

- segment/address and existing symbol;
- proposed name and purpose;
- inputs, outputs, calling convention, and side effects;
- global memory read or written;
- callers and callees;
- concise pseudocode;
- evidence, confidence, and unresolved questions;
- exact proposed annotation changes.

No subsequent block is modified until the owner has had an opportunity to
review the current block.

Sol: owner authorized larger blocks and all remaining `1631` work on
2026-09-16. That segment's local workflow review is now complete; detailed
changes/evidence and shared-helper boundaries are in the
[1631 checkpoint](phase4/BTECH_1631_SEGMENT_REVIEW.md). This exception does
not authorize unrelated segment-wide rewrites or a C# implementation pass.

## Phase 5 — Define the portable C# implementation

Sol:owner clarified platform direction: audit the gameplay-visible contracts,
not DOS internals. File/memory operations, presentation/UI and audio scheduling
must be replaceable services. SDL is a possible C backend, not yet selected;
C# rules/state modules should be independent of UI and rendering. Startup and
music boundary evidence: [platform audit](phase4/BTECH_STARTUP_MUSIC_PLATFORM_AUDIT.md).

1. Convert verified binary structures into stable C# models.
2. Separate original-file loaders from platform-independent game logic while
   retaining the original installation as a required runtime dependency.
3. Write subsystem specifications and dependency maps from verified workflows.
4. Preserve compatibility quirks explicitly rather than accidentally.
5. Translate verified behaviour in vertical slices with comparison tests.
6. Keep possible future Godot collaboration at the adapter/UI boundary rather
   than making Godot a current requirement.

Exit criteria: the implementation can advance through verified game workflows
using an original installation as its asset source.
