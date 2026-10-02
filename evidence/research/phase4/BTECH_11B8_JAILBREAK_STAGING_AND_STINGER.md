# `BTECH_11B8` Jailbreak staging and Stinger award

## Review boundary

This block covers `11B8:152F-16B1`, now named
`Run_Jailbreak_Mission_And_Award_Stinger_152F`. The shipped `JAIL.BLD` invokes
it with raw action `28` at payload offset `06FD`, immediately before the text
describing Jason's newly acquired Stinger and the rest of the party escaping.

The dispatcher subtracts one before indexing its table, so raw action `28`
appears as zero-based pseudo-C case `27`.

## Temporary Jason-only party

The routine saves the byte-sized Name IDs of party slots `1..7` in
`3092:3FEA-3FF0`, then writes `FF` to those live Name fields. Jason's slot zero
is not saved or hidden.

It separately saves `Name[0]` from all four friendly `0x7D`-byte mech records
in `3092:D452-D455` and writes `FF` to each live initial. Only the first name
byte is changed: the four complete mech records remain in place.

Jason is placed on foot and the packed world position is changed to the Jail
entrance:

```text
X = 0D10
Y = 7024
```

The routine then runs `Mech_Mission_0629(9)`. Mission 9 is the already-reviewed
Jailbreak loop in which Jason tests parked mechs until the third distinct
machine starts.

## Post-mission restoration

After the mission, the player position becomes `0D00:7014`. Party Name IDs
`1..7` and all four friendly mech initials are restored from their byte
caches. Each D452 cache entry is then cleared to `FF`.

This is not a record backup: neither infantry records nor existing mech
records are copied away. The Name bytes are presence markers used to hide the
records temporarily from ordinary party and map logic.

## Stinger award

When `3EDB:014A` remains nonzero, the routine scans friendly mech slots `0..3`
for the first record whose restored `Name[0]` is `FF`. It copies exactly one
complete `0x7D`-byte reference Stinger from `2FE8:03EA` into that slot and
stops scanning.

If all four slots were occupied before the mission, no Stinger is installed.
Unlike the earlier Rex/Commando routine, this search remains within bounds and
does not overwrite a fifth record.

## Dispatcher condition correction

At `1CD3:13EC`, assembly compares the protagonist-survival WORD with zero. A
zero result sets byte `3092:D334`; a nonzero result skips that write. The old
pseudo-C had the condition reversed and claimed a successful Jailbreak marked
a main character dead. The field is now named
`RequiredMainCharacterDead_D334`, and the condition matches the executable.
`JAIL.BLD` immediately tests state index `28` (address `D30C + 28 = D334`) and
exits when it is nonzero, independently confirming the field's purpose.

## Corrections to the former pseudo-C

- Party and cached mech-name values are bytes, not pointers or structures.
- Friendly mech records use a `0x7D` stride; only `Name[0]` is hidden.
- The award copies exactly `0x7D` bytes from the reference Stinger.
- Setting the old loop variable to five was the compiler's loop-exit idiom;
  the transcription now uses `break`.
- The award is conditional on protagonist survival and available roster space.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:152F-16B1`;
- `BTech-Reko-expanded/BTECH.reko/BTECH_1CD3.asm`, `1CD3:13EC-140C`;
- decoded shipped `chinception/JAIL.BLD`, payload offsets `06FD-07D9`;
- reference mech table at `2FE8:02F0`, with Stinger at `2FE8:03EA`;
- Mission 9 setup and interaction logic in `BTECH_0FDC.c`.

Record widths, cache ranges, mission number, coordinates, restoration,
survival condition, first-empty-slot search, and Stinger source are verified.
No Astra review is needed for this block.
