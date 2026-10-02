# Phase 3Q — portable MTP map export

## Review boundary

This block replaces `MAP`, `STARMAP`, and `EGA.DrawMapToFile` with bounded MTP
records, portable indexed-PNG composition, raw metadata preservation, and
single/batch command-line exports. It deliberately preserves the legacy map
ordering profiles pending executable-side confirmation.

## Commands

```powershell
dotnet run --project InceptionTools -- inspect-map MAP1.MTP --game-dir chinception
dotnet run --project InceptionTools -- export-map MAP1.MTP --game-dir chinception --output MAP1.png --metadata MAP1.json
dotnet run --project InceptionTools -- export-maps --game-dir chinception --output-dir maps
```

`inspect-map --json` returns structured summary data. Exports refuse collisions
unless `--force` is supplied. Batch export parses and renders all fifteen maps
and preflights all thirty PNG/JSON paths before writing anything.

## Bounded map model

`MtpMapRecord` requires the exact `0x21D + width × height` length for maps 1–14
and preserves every header sub-block and source tile ID. `MAP15.MTP` uses its
separate verified raw 32×24 record. Arrays are returned as defensive copies.

The metadata exporter writes:

- source and rendered dimensions;
- selected tile-set file;
- the probable legacy tile-order profile;
- all raw standard-header blocks as hexadecimal;
- source and normalized tile-ID arrays.

The `0x80` and `0x100` blocks contain visible text but are not simple string
lists: adjacent entries can deliberately overlap or repeat prefixes. The old
NUL-split interpretation produced false merged names, so the maintained model
keeps these bytes raw until their offset/index tables are reconstructed.

## Tile composition

The renderer decodes the assigned ICN through `CompressedImageDecoder`, treats
its 64,000 palette indices as 250 sequential 16×16 tiles, validates every tile
ID, composes the final indexed image, and writes it without `System.Drawing`.

The preserved assignments are:

- `BTTLTECH.ICN`: maps 1–10, 12, and 13;
- `DESTRUCT.ICN`: map 11;
- `STARLEAG.ICN`: map 14;
- `MAP.ICN`: map 15.

## Verification

The dependency-free harness passes **178 assertions**. New cases cover exact
standard/star-map sizes, defensive byte arrays, truncated-input rejection,
profile selection, PNG/JSON output, and overwrite refusal.

All fifteen ignored original MTP files render successfully. FFmpeg-decoded RGB
SHA-256 values match the fifteen existing legacy reference PNGs exactly, proving
pixel-for-pixel parity independent of PNG compression. Generated assets remain
ignored and are not committed.

## Remaining evidence task

The two retained 8×8-block transformations reproduce known output but remain
Probable. Confirm their selection and direction against the original map draw
and loading paths before treating the storage-order names as final.
