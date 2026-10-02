# Phase 3H — ANM pixel layout

Sol: Current checkpoint (2026-09-17): the legacy extractor/converter migration
is complete; no maintained Write2ModeConverter caller remains. The description
below records the earlier phase boundary, not a current migration task.

## Review boundary

This block converts each accumulated `0x0F20`-byte ANM frame into an 88×88
array of EGA palette indices. It does not render an image, select RGB palette
values, infer frame timing, export GIF files, or provide playback.

## Verified data path

The original EGA path is visible as one continuous chain in the clean assembly:

1. `0800:1AFD` passes `246C:244B` to `207F:23EC`, which produces exactly
   `0x0F20` accumulated frame bytes.
2. In graphics mode 2, `0800:1BA0` calls `207F:0572` with source `246C:244B`,
   destination `246C:336B`, and length `0x0790` words (`0x0F20` bytes).
3. `207F:0572` reads four source bytes at a time. Its eight calls to
   `207F:05BF` distribute the high nibble and then low nibble of each byte into
   the EGA plane accumulators. The stored output order is plane 0, 1, 2, 3.
4. `207F:1E37` reads the transposed buffer from `246C:336B`. For each of 88
   rows it writes four plane bytes for each of 11 screen-byte columns. The
   first three plane writes cancel the implicit `MOVSB` destination increment;
   the fourth retains it, so a four-plane group advances one screen byte.
   Eleven bytes plus the row-end `0x1D` increment equals the 40-byte stride of
   a 320-pixel EGA scanline.

This proves that the decompressed ANM representation precedes the hardware
plane transpose and contains two pixels per byte: the first pixel is the high
nibble and the second is the low nibble. `0x0F20 * 2 = 7744 = 88 * 88`.

## Implementation

`AnimationPixelDecoder` validates the exact packed-frame length and expands it
to one palette index byte per pixel. `AnimationPixelFrame` exposes fixed 88×88
geometry and defensive copies of its palette-index array.

`inspect-animation` now reports, without exposing copyrighted frame contents,
the nonzero-pixel count and a 16-entry palette-index histogram for every frame.
The old `EGA.Write2ModeConverter` follows the same nibble order but allocates a
misleading full 320×200 buffer. It remains for the legacy extractor and now has
an explicit migration TODO.

## Verification

The dependency-free synthetic harness passes **90 assertions**. New cases cover
88×88 geometry, high-nibble-before-low-nibble order, defensive pixel copies,
packed-length rejection, and complete 7744-pixel inspection histograms.

All 198 advertised frames in the 22 ignored original ANM files convert without
an invalid palette index or length error. The result is supported by the full
decode/transpose/draw instruction chain, so it does not need an Astra review.
