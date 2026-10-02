# `BTECH_0800` on-foot building interaction

## Review block (`0800:1D8E-1E88`)

This third slice of `Map_Interactables_Building_Or_Items` consumes the tile code
fetched by the preceding projection block. It offers an exterior building-entry
prompt and then compares the tile code with the active tileset's blocking and
interaction threshold.

## Building-entry scan

The twelve building entries use parallel 16-bit packed-position arrays:

| Address | Meaning |
|---:|---|
| `3092:4564` | Building trigger X |
| `3092:4596` | Building trigger Y |

The scan runs only when `3092:D346` says the party is outside the Star League
Cache and the modal-message flag at `3092:D55C` is zero. The old maintained C
tested `MessageBox_Open` without comparing it to false, reversing this gate.

For an on-foot slot, both coordinates after the attempted movement delta must
exactly equal a building trigger. A match opens the standard message box and
constructs:

```text
"Will you enter the " + buildingName + "?"
```

The prompt defaults to Yes. A Yes result calls `Interact_with_BLD` with the
zero-based building index; No performs no BLD call. Either response exits the
building scan. In assembly this exit is encoded by assigning `0x64` to the loop
counter, which the maintained pseudocode now expresses as `break`.

## MTP building-name block

The current MTP loader reads `0x100` bytes into `246C:A561`. Names are addressed
with `buildingIndex << 4`, proving sixteen fixed slots of sixteen bytes each.
Only indexes `0..11` are considered building-entry triggers in this routine;
the allocation and file block nevertheless contain sixteen slots.

`BTECH.h` previously declared twelve 16-bit words, only 24 bytes. It now records
the actual `16 x 16` byte layout. Existing display call sites were adjusted to
index a name slot rather than manually multiplying the index by sixteen.

## Tile-code threshold

After the optional building prompt, `DestinationTileCode` is compared with the
word at `DS:0150`:

- below the threshold, the routine proceeds to the next active on-foot slot;
- at or above it on an exterior map, the movement is marked handled/blocked;
- at or above it inside the Cache, execution continues into the tile-specific
  dispatcher beginning at `0800:1E89`.

The threshold changes with the active map/tileset: observed assignments are
`0x55` for ordinary maps, `0x21` for the Star League Cache, and `0x8B` for its
map-room view. `UnknownMapVariable` is therefore a stale name; a probable human
name is `MinimumBlockingTileCode`. Its global rename is deferred until the
other collision callers have been reviewed.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:1D8E-1E88`.
- MTP reads at `0800:2F65-2F91`, including `0x100` bytes into `246C:A561` and
  `0x20` bytes into each packed-position array.
- Threshold assignments at `0800:2DA8`, `135D:079C`, and `135D:0AB6`.

The branch conditions, record strides, and file-read sizes are direct evidence;
this block does not require an Astra confirmation pass.
