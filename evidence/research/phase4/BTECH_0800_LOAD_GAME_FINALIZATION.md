# `BTECH_0800` load-game finalization

## Reviewed block

- Address range: `0800:34D3-35D2`.
- Parent routine: `Load_Game`.
- This completes the routine.

The successful and invalid-save paths rebuild friendly-mech animation state at
`0800:34D3`. A missing save skips directly to its error display. All paths,
including Cancel, converge on the common UI and disk cleanup at `0800:35AD`.

## Friendly-mech display state

For lance slots `0..3`, the loader selects one of two exploration sprite
families using only the first byte of the mech's fixed name:

| First name byte | Sprite-family base |
|---|---:|
| `'L'` | `0x00`, Locust family |
| anything else | `0x92`, generic humanoid/Commando family |

The old C multiplied an already typed `Mechs[]` index by record size `0x7D`
and compared the entire name array. The assembly performs a byte comparison at
`C724 + slot * 0x7D`.

Each slot then receives:

- sprite frame `0` at `3092:409A[slot]`;
- movement direction `0` at `3092:3920[slot]`;
- animation-direction selector `0` at `3092:396C[slot]`; and
- animation cursor `2FE8:0270`, the beginning of the mech walk-stream family.

The cursor assignment is a 16:16 far pointer. It is not a host-sized pointer or
an indexed word at `01F6`.

`TraitorWarning` at `3092:374A` is also cleared after the four slots.

## Tileset correction

If the loaded state says the party is not inside the Star League Cache but
tileset ID 2 is still resident, `Load_And_Draw_BTTLTECH_ICN` restores the normal
BattleTech tiles. This prevents Cache artwork surviving after a load into the
ordinary world.

## Failure strings

The executable-owned messages have been restored exactly:

- `Game saved is invalid. Use only games saved from this version.`
- `Load game failed! File GameN` followed by the control-prefixed suffix
  `06 0F`, space, `not found. Press a key.`

The old transcription truncated both messages and omitted the two text-control
bytes in the missing-file suffix.

## Common exit

Every path calls:

1. `Menu_Draw_MultiSelect(FALSE)`;
2. `Draw_Health_and_C_Bills_Sidebar(TRUE)`; and
3. `Select_Game_Disk_And_Drive_28CC(1)`.

The last call was absent from the maintained C. It restores logical Game Disk
selection after the earlier save-media operation.

## Porting notes

- Preserve the original two-family artwork decision for compatibility, but a
  C# port should eventually identify mech type explicitly rather than inspect
  the first letter of its name.
- Animation cursors are transient and must be rebuilt after deserialization.
- Ensure cleanup runs after Cancel and both failure modes.
- Preserve control bytes only in an original-text renderer; a modern UI may
  represent their formatting directly.

No Astra review is requested. The loop stride, byte comparison, pointer writes,
messages, and common exit are explicit in the clean assembly and data segment.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:34D3-35D2`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_2FE8.asm`, animation streams at
  `2FE8:0270-02EF`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, strings at
  `3EDB:0601-0672`.
- `Btech/BTECH_0800.c`, `Load_Game`.
