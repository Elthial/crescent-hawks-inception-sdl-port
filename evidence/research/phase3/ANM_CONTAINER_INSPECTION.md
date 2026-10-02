# Phase 3F — ANM container inspection

## Review boundary

This block models and inspects the ANM container without decoding frames. It
fixes the legacy false size field, preserves all bytes, and deliberately leaves
the probable multi-frame XOR stream semantics unchanged.

## Command

```text
InceptionTools inspect-animation FILE [--game-dir PATH] [--json]
```

The output reports actual file length, the 32-byte playback-control region, the
separate 19-byte preserved header trailer, compressed-stream length, first
playback terminator, observed 128-byte block alignment, and the known 88x88
packed frame-buffer geometry. JSON includes every header byte but does not
include the copyrighted compressed stream.

`Records.AnmFileRecord` is the reusable lossless API. Its byte-array properties
return defensive copies. The old internal `AnimationFile` now delegates to this
model; its `Size` property is actual file size and its decoder start remains
`0x33`. Inspection of all originals confirms their documented first-zero counts,
and shows that O6 terminates at playback byte `+0x1F`; bytes `+0x20..+0x32` are
therefore not reported as additional playback controls.

## Verification

The synthetic harness passes **76 assertions**. New checks cover region lengths,
terminator handling, post-terminator preservation, frame geometry, text/JSON
output, defensive copies, and truncated-header rejection. The command is also
checked against all 22 ignored original ANM files; no extracted frames or
original bytes are committed.

Phase 3K subsequently identifies the preserved `+0x20..+0x32` region as an
18-byte timing lookup table and one scale byte. Phase 3F's neutral name records
the deliberately limited knowledge at this earlier review boundary.
