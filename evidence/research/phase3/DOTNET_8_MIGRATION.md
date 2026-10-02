# Phase 3A — .NET 8 migration

## Review boundary

This block changes only the maintained `InceptionTools` target framework from
unsupported `netcoreapp3.1` to the supported `net8.0` long-term-support line.
It does not change parser, decoder, extraction, command-line, package, or file
format behaviour.

Package upgrades are intentionally separate. `log4net 2.0.12` and
`System.Drawing.Common 5.0.2` still produce security advisories and must be
reviewed in their own block because graphics and logging behaviour can change.

## Compatibility checks

- The dependency-free verification harness passes all **42 assertions**.
- The main project builds for `net8.0` with no compiler errors.
- `inventory`, raw and decoded `inspect`, `dump-save`, `dump-weapons`, and
  `disassemble-bld` run directly without a runtime roll-forward setting.
- All 26 ignored local BLD files still pass conservative disassembly.

The original game files remain read-only inputs and no extracted assets are
committed.
