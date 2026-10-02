# Phase 2A — InceptionTools baseline and installation inventory

## Review boundary

This block adds only read-only installation discovery and inventory reporting.
It does not modernize the legacy target framework, change any decoder, extract
assets, or write to original game files.

## Existing toolkit baseline

The root `InceptionTools/` project targets unsupported `netcoreapp3.1` and uses
`log4net 2.0.12` and `System.Drawing.Common 5.0.2`. The current .NET SDK reports
known package vulnerabilities; dependency/framework modernization remains
Phase 3 so it can be reviewed independently from behavioral changes.

Existing functionality includes:

- RLE and EGA conversion paths for CMP/ICN graphics;
- `MECHSHAP.CMP` sprite extraction, including Locust and Commando frames;
- MTP map rendering;
- all 22 ANM files decoded to individual image frames;
- preliminary save, character, mech, and weapon classes.

The old entry point prompts for a directory and immediately runs every extractor.
That legacy no-argument path is preserved in this block.

## Installation discovery

The new locator uses this precedence:

1. `--game-dir PATH`;
2. the `BTCHI_GAME_DIR` environment variable;
3. a directory containing `BTECH.EXE`, or an ignored `chinception/` directory,
   found while walking upward from the current working directory.

Every result must be an existing directory containing `BTECH.EXE`. Failure is
explicit and does not fall through from an invalid higher-priority setting.

## Inventory command

```text
InceptionTools inventory [--game-dir PATH] [--hash] [--json] [--output FILE]
```

Without `--output`, the report is written to standard output. SHA-256 is opt-in
because reading file headers and lengths is sufficient for the routine check.
The command records only metadata: name, category, required status, length, up
to eight header bytes, validation result, and optional hash.

Validation currently covers:

- required-file presence for executable and runtime asset families;
- `MZ` signature for `BTECH.EXE`;
- `DEMOFILE` length `0x03FF`;
- optional save-slot length `0x0F49`;
- each BLD little-endian payload length against `file length - 2`.

Unexpected installation extras are reported, not rejected. Saves are optional
because an otherwise complete installation can legitimately have empty slots.

## Local original-installation result

The ignored `chinception/` reference installation was scanned without hashes or
output-file creation. Result: **0 required files missing and 0 invalid files**.
All 26 BLD payload lengths and six present save-slot lengths validated. No
copyrighted bytes, generated asset output, hashes, or manifest were committed.

## Verification

`InceptionTools.Verification/` is a dependency-free `net8.0` console harness
that links only the new installation classes. It creates a temporary synthetic
installation and verifies locator provenance, MZ and fixed-length checks, valid
and invalid BLD lengths, missing/additional files, JSON output, and opt-in
SHA-256 behavior.

The legacy project builds successfully when run with the normal Windows SDK
profile. It cannot run directly on this machine without allowing a major .NET
runtime roll-forward because the .NET 3.1 runtime is absent; that compatibility
fact is another reason to keep Phase 3 modernization separate.

This records the Phase 2 baseline. The maintained project was subsequently
retargeted to `.NET 8` in Phase 3A; see
[`DOTNET_8_MIGRATION.md`](../phase3/DOTNET_8_MIGRATION.md).
