# Phase 3J — ANM numbered PNG sequence export

## Review boundary

This block exports every accumulated frame from one ANM file as a deterministic
numbered PNG sequence. It does not assign presentation durations, create an
animated GIF, or play the sequence.

## Command

From the repository root:

```powershell
dotnet run --project InceptionTools -- export-animation-frames O0.ANM --game-dir chinception --output-dir O0-frames
```

If `--output-dir` is omitted, the command creates `O0-frames` in the current
directory. Output names preserve the installation filename stem and use a
zero-padded frame index:

```text
O0-frame-00.png
O0-frame-01.png
...
O0-frame-17.png
```

Existing numbered output files cause the command to fail before it writes any
frame. `--force` explicitly allows the complete numbered sequence to be
replaced.

## Implementation

`AnimationFrameExporter.ExportPngSequence` resolves and parses the ANM once,
then uses the accumulated frames already returned by `AnimationFrameDecoder`.
This is important because later ANM frames are XOR changes from earlier state,
not independent images.

The exporter calculates every destination filename and checks all collisions
before creating or replacing a frame. It returns an
`AnimationSequenceExportResult` containing the source filename, normalized
output directory, and one `AnimationFrameExportResult` per written frame.

The individual-frame exporter now shares the same internal ANM loader and PNG
writer, avoiding two subtly different decode paths.

## Verification

The dependency-free harness passes **102 assertions**. New coverage verifies:

- complete numbered output and deterministic filenames;
- preservation of accumulated XOR state in the second synthetic frame;
- collision rejection;
- preflight behavior that writes no earlier frame when a later file collides;
- explicit forced replacement of a complete sequence.

The command exports all 18 frames from the ignored original `O0.ANM` as valid
PNG files named `O0-frame-00.png` through `O0-frame-17.png`. Verification output
is under ignored `InceptionTools/obj/` and is not committed.

