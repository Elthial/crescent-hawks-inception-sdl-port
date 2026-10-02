# Phase 3K — ANM timing table and retrace counts

## Review boundary

This block decodes the original per-frame delay into vertical-retrace counts.
It does not assume a display refresh rate, convert those counts to milliseconds
or GIF centiseconds, write an animated GIF, or provide playback.

## Verified header layout

The clean assembly at `0800:1BBE` uses the loaded ANM base `246C:42C3`:

1. Read the current playback control at `base + frameIndex`.
2. Sign-extend it and subtract `0x41`.
3. Use that result to index a byte at `base + 0x20`.
4. Signed-multiply that table byte by the byte at `base + 0x32`.
5. Signed-multiply the resulting word by 3.
6. Arithmetic-shift the low word right twice.
7. Pass that result to `1F3D:0006`.

`1F3D:0006` calls `207F:0B40` once for each count. The latter polls EGA/CGA
input-status port `0x03DA`, bit 3, across a vertical-retrace boundary. The
decoded delay is therefore a count of hardware retraces:

```text
lookupIndex  = playbackControl - 0x41
tableValue   = file[0x20 + lookupIndex]
scaleValue   = file[0x32]
delayTicks   = signedLowWord(3 * signedByte(tableValue) * signedByte(scaleValue)) >> 2
```

The shifts implement signed floor division by four. All timing table and scale
values used by the original files are small positive values or zero, so no
overflow or negative delay occurs in the preserved set.

This identifies the formerly neutral 19-byte trailer as an 18-byte timing
lookup table at `+0x20..+0x31` followed by a scale byte at `+0x32`. Every one of
the 198 original playback controls is within `0x41..0x52`, exactly covering the
valid lookup-index domain `0..17`.

## Observed profiles

Most animations use table value 1 for every displayed frame and vary only the
scale. O10, O12, and O13 contain deliberate longer holds through larger table
values. O17 through O21 each contain one frame and a zero scale, yielding zero
inter-frame retraces. Across all original frames the decoded range is 0 through
112 retraces.

## Implementation

`AnmFileRecord` now exposes defensive `TimingTableBytes` and the
`TimingScaleValue` while retaining the complete raw header trailer.
`AnimationTimingDecoder` reproduces the original signed byte/low-word
arithmetic and rejects controls outside the table. `inspect-animation` reports
the lookup index, table value, scale, and exact retrace count in text and JSON.

No milliseconds field is presented as original file data. The duration of a
retrace depends on the active video mode. Phase 3L's GIF adapter separately
states its nominal 60 Hz refresh and nearest-centisecond rounding policy while
retaining these exact source counts.

## Verification

The dependency-free harness passes **106 assertions**. Added checks cover the
18-byte table and scale split, ordinary and variable timing lookups, exact
retrace arithmetic, and rejection of an out-of-range control.

All 198 frames across all 22 ignored original ANM files decode successfully.
The table boundaries, lookup, arithmetic, and wait target are explicit in one
local instruction chain, so no Astra review is required for this block.
