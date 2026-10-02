# Phase 3P — portable CMP/ICN graphics export

## Review boundary

This block replaces the legacy `System.Drawing` path for the twelve general
`.CMP` and `.ICN` graphics files with reusable inspection/export APIs and
dependency-free indexed PNG output. It does not yet replace the legacy MTP map
renderer or remove its Windows-only support classes.

## Commands

```powershell
dotnet run --project InceptionTools -- inspect-image BTTITLE.CMP --game-dir chinception
dotnet run --project InceptionTools -- export-image BTTITLE.CMP --game-dir chinception --output BTTITLE.png
dotnet run --project InceptionTools -- export-images --game-dir chinception --output-dir images
```

Add `--json` to `inspect-image`; add `--force` to either export command to
replace existing output. Batch export parses and encodes every input and checks
all output collisions before writing any image.

## Layout and palettes

Every source expands to the verified 320×200, four-bit indexed surface. `.CMP`
files are exported at 320×200. `.ICN` files retain the legacy 16×4000 vertical
tile-strip view, which places each sequential 16×16 tile directly after the
previous tile and remains convenient for map-tool consumers.

The standard EGA palette is used unless the old extractor specified one of its
known file profiles. The following compatibility substitutions are preserved:

| File | Palette substitutions |
|---|---|
| `BTTITLE.CMP` | index 1 becomes black |
| `INFOCOM.CMP` | index 9 becomes dark blue; index 5 becomes light blue |
| `ENDMECH.CMP` | index 1 becomes black; index 13 becomes light blue; index 9 becomes dark blue |

These palette choices preserve the hand-authored legacy extractor knowledge;
their executable origins remain to be verified separately.

## Verification

The dependency-free harness passes **171 assertions**. New tests cover CMP and
ICN display profiles, inspection metadata, indexed PNG dimensions, and
overwrite refusal.

All twelve ignored original files decode to exactly `0x7D00` packed bytes,
consume their complete payloads, and batch-export successfully. Representative
title-screen, INFOCOM, and tile-strip images were visually checked. Generated
images remain under ignored `artifacts/`; no new copyrighted asset is committed.

## Next migration block

Replace `MAP`, `STARMAP`, and `EGA.DrawMapToFile` with bounded MTP records,
portable tile composition, structured metadata, and single/batch CLI exports.
