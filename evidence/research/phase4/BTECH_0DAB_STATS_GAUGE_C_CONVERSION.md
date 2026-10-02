# Sol: native statistics gauges and tables — 2026-09-18

Converted original `0DAB:174C..1857` into
`CrescentHawksInception/src/Original/BTECH_0DAB_GAUGE.c`.
The annotated template remains the source shape; the expanded ASM supplies
signed `JLE1` height tests, signed `IDIV6`, wrapped WORD decrement/sign tests,
and the low-BYTE colour toggle. Corrected the template's previously recorded
unsigned height/division mismatches as well.

The rectangle primitive remains the existing original call, not an SDL call
inside gameplay code. Its hardware implementation lives behind the existing
separate platform boundary. BUG-006 is retained: one-unit green above nonzero
red receives reversed Y bounds and is rejected by the original primitive.

## Original storage

- `3EDB:1218/121A/121C`: countdown/reset WORDs initially ten, colour initially zero.
- `1306/131C/1332`: three eleven-WORD structure-offset/X/bottom-Y tables.
  Eight structure offsets select actual Mech structure bytes; the final three
  are zero because rear torso armour has no independent internal structure.
- `1348/1358`: mutable sixteen-entry EGA BYTE and MCGA WORD palettes.
- `1378/137C`: four-phase BYTE/WORD replacements for palette entry four.

These tables are EXE-owned data, not copyrighted external image-file contents.
The silhouette still requires the original external `BTSTATS.CMP`.
Do not make the MCGA tables BYTE arrays merely because rendering is EGA-only:
the original parent patches both tables, and explicitly scales the MCGA index.

## Validation and limits

The actual gauge method passes 264737 call-boundary scenarios: normal heights,
every native WORD in each signed height/division/countdown class, coordinate
wrapping, preserved colour high bytes, BUG-006, and successive flash calls.
Only the filled-rectangle presentation boundary is isolated in that unit test.
This does not prove rendered pixels or emulator timing.

The table suite verifies native record offsets and geometry, then the exact
little-endian fingerprint of all 126 bytes at `3EDB:1306..1383` (`0A95DD72`).
An optional local test independently reads those bytes from the unpacked EXE
and compares every byte to compiled C data. No external assets are embedded.

115 headless and 135 SDL/local-asset tests pass. The original statistics parent
is still unconverted. Its palette frame and phase locals are genuinely unwritten
native stack WORDs (`BP-22` and `BP-2C`); arbitrary host uninitialized reads or
assumed zero initialization are not a faithful solution. Initial residual values
and the complete screen workflow remain to be resolved and validated.

Fresh real-executable linkage confirms `Examine_Screen_BTSTATS_CMP` remains
the sole unresolved symbol; no stub was added to force a successful link.
