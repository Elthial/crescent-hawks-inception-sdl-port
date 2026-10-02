# InceptionTools long-term toolkit scope

## Product intent

InceptionTools is intended to become the maintained C# toolkit for inspecting,
extracting, converting, and—where safe—modifying data from a user-supplied copy
of *BattleTech: The Crescent Hawk's Inception*. It is not merely an asset-dump
utility for the port.

Original game files and extracted copyrighted assets remain local to the user.
Tests committed to this repository use synthetic fixtures or metadata rather
than copied game content.

## Desired capabilities

### Graphics and mech sprites

- Decode the original graphics formats through reusable library APIs.
- Locate and extract mech sprites, including all relevant facings/states.
- Assemble selected mech sprites into deterministic sprite sheets with metadata
  describing frame rectangles and source provenance.
- First audit the existing extractor so working functionality is retained rather
  than duplicated.

Phase 3M now provides the verified bounded source decoder and a deterministic
MECHSHAP PNG/JSON export containing all Locust, Commando-style, effect, and
personnel sprites. Its personnel blocks identify red teammates, light-blue
Jason Youngblood, and the grey enemy/civilian pool. Distinguishing grey enemies
from civilians remains follow-up work. Phase 3P adds maintained portable
inspection and single/batch PNG export for all twelve CMP/ICN files, including
the three known legacy palette overrides. MTP map composition is now the main
remaining legacy graphics caller. Phase 3Q subsequently replaces that caller
with bounded MTP records and portable single/batch PNG+JSON export for all
fifteen maps, reproducing the prior rendered pixels exactly. Phase 3R then
removed the unreferenced legacy extraction classes and the Windows-only
`System.Drawing.Common` dependency.

### Animation

- Decode original ANM data into an ordered frame sequence with timing metadata.
- Export individual frames and complete numbered PNG sequences without relying
  on the legacy Windows-only graphics path.
- Export animations as looping GIF files.
- Provide a command that previews/plays an animation continuously without
  requiring export.
- Preserve raw timing/control values when their semantics are not yet known.

### BLD inspection and disassembly

- Load and decode original BLD payloads losslessly.
- Display offsets, raw bytes, opcode names, typed operands, branch destinations,
  strings, and executable-side calls.
- Produce a human-readable disassembly or structured representation without
  pretending uncertain gameplay semantics are verified.
- Preserve enough source information for exact control-flow and round-trip
  validation.

### Save-game inspection and editing

- Decode characters, mechs, inventory, finance, positions, and known flags.
- Allow controlled edits to character records, mech records, and verified game
  flags.
- Preserve unknown bytes by default.
- Validate field ranges, create a backup before writing, and support dry-run and
  before/after diff output.
- Refuse unsafe writes when the save profile or length is not recognized.

Phase 3N provides the first complete editing path: reusable typed fetch/update
APIs plus `export-save-state` and `import-save-state` text commands. It exposes
all currently mapped character, mech, finance, position, and story-state fields;
preserves all other source bytes; fingerprints the source; and always writes a
different output file. Named flag expansion remains later work.

### SIF sound and music

- Reconstruct and play the original SIF event stream.
- Export a faithful WAV rendering as the baseline interchange format.
- Export MIDI when the recovered event model maps cleanly to notes, timing, and
  instruments; otherwise clearly label any approximation.
- Additional audio/video containers can be adapters over the verified event or
  rendered-audio model rather than part of the core parser.

Phase 3O now parses the headerless four-channel SIF stream, reproduces the
executable's note-to-PIT conversion, renders both PC-speaker and Tandy playback
interpretations, and exports or directly plays WAV audio. It also exposes all
18 executable-resident sound effects by ID/name. Exact Tandy chip timbre and
cycle-calibrated sound-effect noise/duration remain fidelity work; MIDI remains
deferred until the two original playback arrangements have an explicit mapping.

## Architecture direction

The toolkit should separate:

1. bounded binary readers and lossless format models;
2. semantic inspection/editing services;
3. exporters and converters;
4. command-line presentation and optional interactive preview UI.

Every inspection command should work read-only. Modification commands must be
explicit, narrowly scoped, reversible through backups, and tested against
synthetic fixtures before use on an original save.

## Delivery order

Phase 2 establishes installation discovery, manifests, fixtures, and comparison
commands. Phase 3 modernizes the project and then develops the capabilities in
small reviewed slices: existing graphics audit, BLD disassembler, structured
character/mech/weapon access, animation export/preview, save editing, and SIF
playback/export. Exact ordering may change when Phase 2 shows which existing
features already work reliably.
