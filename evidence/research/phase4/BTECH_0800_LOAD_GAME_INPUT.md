# `BTECH_0800` load-game selection and input

## Reviewed block

- Address range: `0800:32B3-33E7`.
- Parent routine: `Load_Game`.
- Review boundary: post-load state reconstruction begins at `0800:33E8`.

The player selects one of six slots. The executable changes the final digit of
the writable string `Game0` at `3EDB:0154`, producing `Game1` through `Game6`.
Menu result 6 cancels without opening a save.

Before opening the selected save, the original selects logical disk value 3
and repeatedly tries to open `INFOCOM.CMP`. A failed marker-file open invokes
the normal disk prompt and retries; a successful handle is closed immediately.
The exact user-facing meaning of logical value 3 remains unresolved because
the existing prompt distinguishes only Game Disk from Disk 2. The machine-code
behaviour and marker filename are nevertheless explicit.

The selected `GameN` file uses DOS's 16-bit `0xFFFF` failure sentinel. The old
C incorrectly checked `0x00FF` and also closed an uninitialized INFOCOM handle.

## Save framing

The first file byte is read into a stack local and compared with `0x0C`. The
old transcription compared the DOS read function's returned byte count with
`0x0C`, which could never validate a one-byte read correctly.

On a valid marker the loader reads:

| File range | Size | Destination |
|---|---:|---|
| `0x0000` | 1 | Format marker, must be `0x0C`. |
| `0x0001-0x0F44` | `0x0F44` | Contiguous state beginning at `3092:C614`. |
| `0x0F45-0x0F46` | 2 | Party packed X at `246C:A44B`. |
| `0x0F47-0x0F48` | 2 | Party packed Y at `246C:A44D`. |

Total size is exactly `0x0F49` bytes, matching all preserved save samples and
the maintained InceptionTools save model.

Before reading the state block, `Current_Map` defaults to Citadel and the
separate viewed-holodisk cache is cleared. The following block derives those
caches again from the newly loaded persistent flags.

## Porting notes

- Keep the slot display index separate from its one-based filename digit.
- Validate the byte read from the file, not the read operation's return count.
- Use a signed 16-bit view or explicit `0xFFFF` comparison for DOS failures.
- A portable loader can replace `INFOCOM.CMP` disk detection with installation
  validation, but should document the original requirement.
- Do not distribute `INFOCOM.CMP`; it remains an externally required original
  asset.

No Astra review is requested. The unresolved disk-value label needs broader
installation-flow tracing, not deeper interpretation of this instruction block.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:32B3-33E7`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, strings at `0154` and
  `05C7-0600`.
- `Btech/BTECH_0800.c`, `Load_Game`.
- Ignored original `Game1` through `Game6` samples, length comparison only.
