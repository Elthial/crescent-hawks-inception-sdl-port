# Phase 3M1 — bounded CMP/ICN image decoder

## Review boundary

This block replaces the unbounded legacy full-screen RLE path with a reusable,
dependency-free decoder. It verifies the shared format 1/format 2 grammar and
portable indexed PNG rendering needed by the forthcoming MECHSHAP spritesheet.
It does not yet define sprite rectangles or export the reorganized sheet.

## Assembly evidence

`1F3D:049D` reads the compression byte and dispatches format 1 to `207F:22F8`
or every other retained value to `207F:2368`. In both routines:

- positive signed controls copy that many following literal bytes;
- negative controls repeat the following byte after negating the control;
- zero reads a little-endian 16-bit repeat count and one value byte;
- output stops after exactly `0x7D00` packed bytes.

Format 1 advances the destination with `STOSB`. Format 2 adds `0x00A0` after
each output byte, counts `0xC8` rows, then subtracts `0x7CFF`. This writes the
logical stream down 200 rows before moving one byte right in the normalized
160-packed-byte-wide image. Each packed byte expands high nibble then low
nibble into a 320×200 palette-index image.

`CompressedImageDecoder` also validates the three-byte container header,
declared stored length, supported format, input bounds, output bounds, and
zero-length extended runs. It preserves packed bytes and exposes defensive
palette-index copies.

## Portable PNG

`EgaIndexedPngEncoder` generalizes the dependency-free indexed PNG path used by
ANM export and optionally writes a transparent palette index. The ANM wrapper
now delegates to it without changing ANM output.

## Verification

The synthetic harness passes **125 assertions**, including both traversal
orders, nibble expansion, malformed stored length, and indexed transparency.
The ignored original `MECHSHAP.CMP` decodes as format 2, consumes all 16,915
payload bytes, and produces a coherent 320×200 image. No original or extracted
graphics are committed.

