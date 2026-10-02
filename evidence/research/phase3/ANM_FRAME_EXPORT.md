# Phase 3I — ANM frame PNG export

## Review boundary

This block renders one selected ANM frame to a portable indexed-colour PNG. It
does not export a whole sequence, infer animation timing, create GIF files, or
provide a looping player.

## Command

From the repository root:

```powershell
dotnet run --project InceptionTools -- export-animation-frame O0.ANM 0 --game-dir chinception --output O0-frame-00.png
```

The output option may be omitted, in which case the filename is derived as
`O0-frame-00.png` in the current directory. Decimal and `0x`-prefixed frame
indices are accepted. Existing files are not overwritten unless `--force` is
present:

```powershell
dotnet run --project InceptionTools -- export-animation-frame O4.ANM 0x0C --game-dir chinception --output O4-last.png --force
```

The command reports the valid final frame index in its success message. An
out-of-range selection instead reports the actual frame count and exits with an
error.

## Implementation

`AnimationFrameExporter` resolves the case-insensitive installation filename,
parses and accumulates the ANM sequence, validates the requested frame, and
writes the chosen result. Its default `FileMode.CreateNew` prevents accidental
replacement of an existing export; `--force` selects `FileMode.Create`.

`AnimationPngEncoder` is dependency-free and does not use the legacy
Windows-only `System.Drawing` code. It writes:

- an 88×88 IHDR with 8-bit indexed-colour type 3;
- a 16-entry standard EGA PLTE chunk;
- unfiltered scanlines compressed with the .NET `ZLibStream`;
- CRC-protected IHDR, PLTE, IDAT, and IEND chunks.

ANM files contain palette indices but no RGB palette. This exporter therefore
uses the standard 16-colour EGA mapping already represented by the legacy
InceptionTools palette. If later executable analysis proves that a scene
changes EGA palette registers before playing an animation, custom palette
selection should be added as a separate reviewed feature rather than silently
changing this default.

## Verification

The dependency-free harness passes **97 assertions**. New checks validate the
PNG signature, 88×88 indexed header, decompressed filter bytes and pixel order,
frame selection, directory creation, overwrite refusal, forced overwrite, and
out-of-range rejection.

A local export of original `O0.ANM` frame 0 produces a coherent upright cockpit
image with the expected 88×88 dimensions and EGA colours. The verification file
is under ignored `InceptionTools/obj/` and is not committed.

