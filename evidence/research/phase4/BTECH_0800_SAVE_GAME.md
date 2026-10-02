# `BTECH_0800` save-game workflow

## Reviewed block

- Address range: `0800:35D3-378C`.
- Maintained name: `Save_Game`.
- This covers the complete routine.

## Map-room prohibition

Saving is rejected while `CacheMapRoomLoaded` at `3092:D34E` is nonzero. The
routine draws the standard message box and displays:

```text
You can't save the game inside the map room.
```

It then follows the same common UI cleanup as every other path.

## Slot and media handling

The save menu offers six slots and Cancel. Results `0..5` replace character
four of the writable `Game0` string with `'1'..'6'`; result 6 performs no disk
operation.

As in `Load_Game`, the original selects logical disk value 3 and repeatedly
opens `INFOCOM.CMP` as a media-presence marker. The marker handle is closed as
soon as the open succeeds. Logical value 3's precise user-facing label remains
an installation-flow question documented with the load path.

The selected save is opened with control word `0x8101` and creation-permission
word `0x0180`. The clean assembly supplies all three parameters; the old C
omitted `0x0180`, closed an uninitialized marker handle, and checked only
`0x00FF` rather than the 16-bit failure value `0xFFFF`.

## Exact serialization

The function writes four blocks in this order:

| File offset | Bytes | Source |
|---:|---:|---|
| `0x0000` | 1 | Stack byte initialized to format marker `0x0C`. |
| `0x0001` | `0x0F44` | Contiguous state beginning at `3092:C614`. |
| `0x0F45` | 2 | Packed party X at `246C:A44B`. |
| `0x0F47` | 2 | Packed party Y at `246C:A44D`. |

The resulting file length is `0x0F49`, exactly mirroring `Load_Game` and the
maintained InceptionTools record.

Each write's returned byte count is compared with its requested length. A
short write sets `FileWriteFailed`, but subsequent blocks are still attempted
and an opened handle is always closed. This avoids the previous uninitialized
failure flag and records the executable's precise partial-write behaviour.

## Failure and common exit

Create failure or any short write produces the EGA-red message:

```text
Save game failed! Check your disk.
```

The executable string ends with control bytes `06 0F`; these have been retained
in the annotated source. The original CGA colour substitution remains omitted
from the maintained EGA-only path.

Success, failure, Cancel, and the map-room prohibition all finish by:

1. restoring the underlying menu with `Menu_Draw_MultiSelect(FALSE)`;
2. redrawing health and C-Bills with borders; and
3. selecting logical Game Disk 1.

The disk restoration was absent from the old transcription.

## Porting notes

- Reuse the same canonical save record for load and save.
- Preserve the `0x0C` marker and exact original field boundaries.
- Replace DOS media prompting with validation of the configured original-game
  installation, while retaining `INFOCOM.CMP` as an external requirement.
- Prefer writing a complete temporary file and atomically replacing the target
  so a short host write cannot corrupt an existing save.
- Keep the original prohibition on saving inside the Cache map room unless a
  deliberately versioned new save format can serialize that scene safely.

No Astra review is requested. The branch structure, parameters, marker, write
sizes, return-count tests, strings, and cleanup are explicit in the clean
assembly.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:35D3-378C`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, strings at
  `3EDB:0673-06FE`.
- `Btech/BTECH_0800.c`, `Save_Game`.
- `docs/formats/SAVE.md`.
