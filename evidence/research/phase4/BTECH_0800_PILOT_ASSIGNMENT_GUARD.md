# `BTECH_0800` pilot-assignment dismount guard

## Reviewed block

- Address range: `0800:4D57-4DC6`.
- Maintained routine: `Menu_Assign_Pilots`.
- Sole caller: the pause menu at `0800:2C50`.
- The next routine begins at `0800:4DC7`.

This routine allows the pause-menu pilot/rider assignment workflow only when
the party is sufficiently far from terrain blocks marked as populated or
otherwise unsafe for dismounting.

## Neighbourhood safety check

Movement finalization builds a row-major 3×3 neighbourhood of combined
terrain flags at `246C:07A4-07AC`. It is centred on the party's current local
16×16 map block:

```text
0 1 2    northwest  north  northeast
3 4 5    west       centre east
6 7 8    southwest  south  southeast
```

`0800:4D6C-4D84` tests bit `80` in all nine bytes. If any byte carries the
bit, dismounting and pilot assignment are refused. The maintained constant is
`MapTile_BlockDismounting`.

The old pseudo-C expression lacked parentheses:

```c
terrainFlags & MapTile_BlockDismounting != FALSE
```

Under C operator precedence that can test bit zero instead of bit `80`. The
cleaned annotation explicitly evaluates `(terrainFlags & 0x80) != 0`.

The exact source data which contributes this high bit is not resolved in this
block. Its gameplay meaning is strongly supported by the refusal text:
“It would be dangerous to dismount so close to a populated area.”

## Allowed path

When all nine flags are clear, the routine calls
`Assign_Pilot_and_rider_to_Mechs(0)`. At `1467:005B`, mode zero synchronizes
each character's `MechAssignment` byte from the current live mech `PilotId`
and `RiderId` fields before entering the common assignment UI. It does not run
the nonzero mode's initial clear-to-on-foot/reset-occupants pass.

After that UI returns, the routine redraws the party-health/C-Bills sidebar
with its top-graphic flag enabled, then explicitly redraws the top graphic once
more. Both calls exist in the executable and should not be deduplicated merely
because the higher-level sequence looks redundant.

## Refused path

When any neighbourhood byte carries bit `80`, the routine:

1. draws the standard message box;
2. displays the refusal string from `3EDB:0ACE`;
3. performs the usual retrace/input check through `1CD3:17EA`; and
4. consumes a key through `Keyboard_Get_ASCII_Hex_Input`.

The assignment routine is never entered on this path.

## Confidence and porting notes

Confidence is high for the nine-byte scan, mask, branch, calls, and message.
Calling the bit “populated/unsafe-to-dismount terrain” is a high-confidence
behavioural description; the precise map-file encoding that produces it is a
separate question. No Astra review is requested for this block.

A C# port should expose this as a query over the current 3×3 terrain-block
cache followed by the existing assignment operation. Keep the safety rule out
of the character and mech record models: it belongs to world/presentation
workflow state.

The cache construction is documented in
[movement-cache finalization](BTECH_0800_MOVEMENT_CACHE_FINALIZATION.md).
