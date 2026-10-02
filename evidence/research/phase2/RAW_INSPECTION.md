# Phase 2B — bounded raw-file inspection

## Review boundary

This block adds a read-only hexdump and verified structural checks. It does not
attempt BLD gameplay interpretation, modify a save, export copyrighted assets,
or change the existing graphics/animation decoders.

## Command

```text
InceptionTools inspect FILE [--game-dir PATH] [--offset N] [--count N] [--decode-bld]
```

`FILE` must be a single filename in the installation root; paths and traversal
components are rejected. Decimal and `0x`-prefixed hexadecimal numbers are
accepted. Reads are bounded to `0x1000` bytes and clipped at the end of the
selected domain.

The default offset domain is the raw file. With `--decode-bld`, offsets are
relative to decoded BLD payload offset zero, corresponding to raw `file+0x02`.
Each selected stored byte is transformed with the assembly-verified formula:

```text
decoded = ((stored + 0x29) & 0xFF) XOR 0xE9
```

The inspector labels the offset domain in every report and prints format facts
without assigning uncertain high-level meanings. Current facts include BLD
payload-length agreement, CMP/ICN stored length and compression byte, MZ
recognition, and known save length.

## Stronger inventory validation

Phase 2B also validates:

- CMP/ICN stored length equals `file length - 2`;
- CMP/ICN compression byte is `1` or `2`;
- known MTP profile lengths (`0x121D`, `0x061D`, `0x025D`, or `0x0300` by map);
- `WWOODBT.SIF` length is `0x1100` for the current documented profile.

Length mismatches now contribute to the inventory failure count and exit status,
not merely to display text.

## Verification result

The synthetic harness passes 18 assertions covering decoding, offset-domain
labelling, hexdump output, traversal rejection, bounded reads, valid and invalid
graphics headers, plus the Phase 2A cases.

The ignored `chinception/` installation still reports **89 files, 0 required
files missing, and 0 invalid files** under the stronger checks. A decoded BLD
inspection starts at the verified `FE 06 FD FA` prologue. No inspected bytes or
generated report were committed.
