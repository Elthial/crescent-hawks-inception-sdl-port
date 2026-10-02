# Save files

## Physical layout

All six local slots (`GAME1`–`GAME6`) are exactly `0x0F49` bytes. The annotated
`Load_Game` and `Save_Game` functions perform these operations:

| File offset | Size | Confidence | Operation |
|---:|---:|:---:|---|
| `0x0000` | 1 | V | Format marker `0x0C`; the load path compares the byte and the save path writes it explicitly. |
| `0x0001` | `0x0F44` | V | Contiguous game-state block copied to/from `3092:C614`. |
| `0x0F45` | 2 | V | Party map X word copied separately. |
| `0x0F47` | 2 | V | Party map Y word copied separately. |
| `0x0F49` | 0 | V | End of file. |

The six observed headers are all `0x0C`. The clean load and save assembly now
confirms this marker in both directions.

## Leading state block

| File offset | Size | Confidence | Meaning |
|---:|---:|:---:|---|
| `0x0001` | `8 × 0x11` | V | Player infantry records. |
| `0x0089` | `8 × 0x11` | V | Enemy infantry records. |
| `0x0111` | `4 × 0x7D` | V | Player lance mech records. |
| `0x0305` | `4 × 0x7D` | V | Enemy mech records. |
| `0x04F9` | `0x0800` | P | World-map visibility bit field. |
| `0x0CF9` | 1 | P | Citadel/story mission state. |
| later | variable | H/U | Flags, inventory, finance, party auxiliaries, map state, and other data before the effect arrays below. |
| `0x0E44` | `0x40` | V | Persistent map-effect sprite indices. |
| `0x0E84` | `0x40` | V | Persistent map-effect packed page nibbles. |
| `0x0EC4` | `0x40` | V | Persistent map-effect low X bytes. |
| `0x0F04` | `0x40` | V | Persistent map-effect low Y bytes. |
| `0x0F44` | 1 | V | Next map-effect insertion slot; final direct-copy state byte. |

The first byte of each mech record is also the first name character. Its high
bit is **Probable** as the no-mech/destroyed marker; `0xFF` is the verified empty
or destroyed sentinel. The Phase 3N text editor exposes the inverse as
`mech.GROUP.SLOT.active=true|false` while preserving the combined raw byte.

Because the state block is a direct memory copy, an address `3092:AAAA` within
`C614..D557` maps to:

```text
fileOffset = 1 + (AAAA - C614)
```

This relation is the preferred way to validate later field offsets.

## Historical parser defect

The former sequential `InceptionTools/FileTypes/SaveFile.cs` reconstructed the
correct leading record arrays but its unknown-block lengths drifted before
finance and the end of the file. It then read four bytes called `Position` beginning at
`0x0F44`, one byte too early, and leaves the final byte at `0x0F48` unread.

The correct final structure is two little-endian words at `0x0F45` and `0x0F47`.
The byte at `0x0F44` remains part of the copied `0x0F44`-byte state block.

Phase 3D replaces that implementation with `Records.SaveGameRecord`, which
validates the exact file length, retains the whole saved-state block, and reads
the final X/Y words at their verified offsets. `SaveFile` is now only an
obsolete path-based wrapper over the canonical model.

Phase 3N adds the editable text round trip documented in
[Command-line save-state text editor](../phase3/SAVE_STATE_TEXT_EDITOR.md).
It patches typed fields over the exact original byte array, so undocumented
state remains intact and an unchanged round trip is byte-identical.

## Original write behaviour

`Save_Game` checks the returned count after each of its four writes: 1,
`0x0F44`, 2, and 2 bytes. Any short write sets a failure flag, but the original
continues attempting the remaining blocks and closes the file before reporting
the error. A replacement should retain the exact framing while using a safer
temporary-file and atomic-replace strategy where the host platform permits it.

## Next reconstruction step

Use the direct-copy address formula to convert documented `3092:C614..D557`
fields into file offsets, then confirm each with the assembly instruction width.
Do not infer integer byte order from visually plausible save values alone.

## Code references

- `Btech/BTECH_0800.c`, `0800:32B3` (`Load_Game`).
- `Btech/BTECH_0800.c`, `0800:35D3` (`Save_Game`).
- `InceptionTools/FileTypes/SaveFile.cs`: partial parser with the documented
  offset drift.
