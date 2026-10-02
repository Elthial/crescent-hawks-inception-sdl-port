# `BTECH_0800` pause menu

## Reviewed block

- Address range: `0800:2C50-2DA7`.
- Maintained name: `Game_Pause_Menu`.
- Caller: the main game loop.

This routine constructs the in-game pause menu, conditionally inserts the
pilot/'Mech allocation choice, dispatches the chosen action, and redraws the
underlying bordered view when that action returns.

## Detecting an available party 'Mech

The function examines the first byte of each of the four friendly mech records:

```text
for lanceMechId = 0..3:
    if byte[3092:C724 + lanceMechId * 0x7D] != 0xFF:
        partyHasMech = true
```

This is a value test of `Mechs[id].Name[0]`. The old maintained C multiplied an
already typed `Mechs[]` index by `MECH_RecordSize` and compared the name array
rather than its first byte.

Confidence: **verified** from the `IMUL 0x7D` and byte comparison in the clean
assembly.

## Two menu layouts

The routine initializes three word fields:

| Address | No party 'Mech | Party has a 'Mech | Meaning |
|---|---:|---:|---|
| `305B:0012` | 13 | 12 | Top row of menu-layout record 1. |
| `305B:0016` | 7 | 8 | Height of menu-layout record 1. |
| `305B:00A6` | 7 | 8 | Choice count of menu-control record 1. |

`0012` and `0016` belong to the 16-byte layout record selected by
`Menu_Memory_Variables(1)`. `00A6` belongs to the separate menu-control record
selected by `Display_Menu_Choices_And_Check(1)`. They occupy overlapping table
views in segment `305B`, which is why raw field names concealed their purpose.

## Exact executable-owned choice strings

The display routine treats carriage returns embedded in these NUL-terminated
strings as new menu rows:

| Address | Exact contents |
|---|---|
| `3EDB:051F` | `Return to game\rChange game settings` |
| `3EDB:0543` | `\rAllocate men in 'Mechs` |
| `3EDB:055B` | `\rInspect Character\rHeal Characters\rLoad Game\rSave Game` |
| `3EDB:0592` | `\rShow Overhead Map` |

The second string is emitted only when at least one friendly mech record is
present. The old transcription reduced three multi-choice strings to their
first visible line and changed the capitalization and punctuation of the
Allocate choice.

## Verified selection mapping

| Selection | Without a party 'Mech | With a party 'Mech |
|---:|---|---|
| 0 | Return to game | Return to game |
| 1 | Change game settings (`0800:3BD0`) | Change game settings (`0800:3BD0`) |
| 2 | Inspect character (`0800:378D`) | Allocate men in 'Mechs (`0800:4D57`) |
| 3 | Heal characters (`1431:000A`, argument 0) | Inspect character (`0800:378D`) |
| 4 | Load game (`0800:32B3`) | Heal characters (`1431:000A`, argument 0) |
| 5 | Save game (`0800:35D3`) | Load game (`0800:32B3`) |
| 6 | Show overhead map (`0800:3D40`) | Save game (`0800:35D3`) |
| 7 | — | Show overhead map (`0800:3D40`) |

The old maintained switch used the no-'Mech indices for its with-'Mech branch
and began with Settings at case zero. The clean jump table proves that the
with-'Mech branch uses selections `1..7`; selection zero invokes no action.

After any selection—including Return—the routine calls `1F3D:06C3`
(`EGA_DrawBox_Wrapper`) before returning.

## Porting notes

- Model these as two explicit choice lists rather than sharing numeric indices
  after insertion of Allocate.
- The three menu configuration values are 16-bit words.
- Preserve selection zero as a no-action return rather than dispatching the
  Settings screen.
- Keep the redraw after the selected child screen returns.

No Astra review is requested: record stride, byte test, menu strings, jump
table, and called targets are all directly visible.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:2C50-2DA7`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_305B.asm`, initialized menu tables.
- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, strings at
  `3EDB:051F-05A4`.
- `Btech/BTECH_0800.c`, `Game_Pause_Menu`.

