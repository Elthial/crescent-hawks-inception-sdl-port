# Phase 3B — dependency security refresh

## Review boundary

This block updates the two existing runtime packages without changing logging,
graphics, parser, decoder, or command APIs:

| Package | Previous | Updated |
|---|---:|---:|
| `log4net` | `2.0.12` | `3.4.0` |
| `System.Drawing.Common` | `5.0.2` | `10.0.12` |

These were the current stable releases shown by their official NuGet package
metadata on 2026-09-12 and both advertise compatibility with `.NET 8`.

## Verification

- NuGet restore and build report no vulnerability advisories.
- The dependency-free harness passes all **42 assertions**.
- All public read-only commands run successfully.
- All 26 ignored BLD files still pass conservative disassembly.
- The complete legacy asset extraction produces 425 BMP files totalling
  1,785,910 bytes both before and after the update. A deterministic manifest of
  relative paths, sizes, and individual SHA-256 values has the same combined
  SHA-256 in both runs: `8370399A10845562C0C409F28A6698417A212593629AECAFCD33B43357C130DE`.

Generated BMPs and original game assets are not committed.

## Portability limitation

`System.Drawing.Common` remains Windows-only in modern .NET. This refresh
removes the vulnerable legacy version but does not solve cross-platform image
export. Replacing GDI+ with a portable encoder belongs in a separate graphics
architecture block so decoded pixel behaviour can be compared independently.

This historical limitation was resolved in Phases 3P–3R: maintained graphics,
map, sprite, and animation exports now use dependency-free encoders, and the
legacy extractor plus `System.Drawing.Common` reference have been removed.
The now-superfluous `log4net` wrapper was retired at the same time, leaving the
maintained command-line project without third-party package dependencies.

References:

- [log4net package metadata](https://www.nuget.org/packages/log4net/)
- [System.Drawing.Common package metadata](https://www.nuget.org/packages/System.Drawing.Common)
- [Microsoft compatibility note](https://learn.microsoft.com/en-us/dotnet/core/compatibility/core-libraries/6.0/system-drawing-common-windows-only)
