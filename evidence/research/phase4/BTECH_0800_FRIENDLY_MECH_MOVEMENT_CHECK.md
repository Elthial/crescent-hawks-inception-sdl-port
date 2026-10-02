# `BTECH_0800` friendly-mech movement and building check

## Review block (`0800:1FBE-218E`)

This final slice of `Map_Interactables_Building_Or_Items` runs only if the
deep-water and on-foot passes did not already block or consume the requested
movement. It scans the four active friendly-mech slots at `3092:406A`, checks a
two-cell collision footprint for each mech, and offers building entry using a
wider doorway footprint than the on-foot scan.

The function then returns the 16-bit `MovementBlockedOrHandled` value in `AX`.
A false result allows `Character_Movement_On_Map` to apply the movement.

## Mech projection

The four signed X and Y probe offsets at `DS:045C` and `DS:0474` are projected
into the same staggered 24-by-24 interaction buffer used by the on-foot pass:

```text
projectedX = mechProbeX[slot] + deltaX + 0x1A
projectedY = mechProbeY[slot] + deltaY + 0x0C

cellOffset = rowOffsets[floor(projectedY / 2)]
           + signedShiftRight(projectedX - 0x0D, 1)
```

The packed party X/Y parity corrections are then applied, and `246C:09ED` is
added before reading the tile byte from `246C:07AD`.

The former maintained C version of this calculation was syntactically
incomplete, added `0x0D` rather than subtracting it, and added the map origin to
the fetched tile value rather than the array index.

## Two-cell collision footprint

The primary projected cell is compared with the active minimum
blocking/interactive tile code at `DS:0150`. If it is below the threshold, the
routine checks one horizontally adjacent cell:

```text
if parity(projectedX) differs from parity(partyPackedX):
    adjacentOffset = primaryOffset - 1
else:
    adjacentOffset = primaryOffset + 1
```

If either cell contains a code at or above the threshold, movement is marked
blocked/handled. This establishes that an exploration-map mech occupies two
staggered collision cells; testing only its centre would allow half of the mech
to pass through blocking terrain.

## Building-entry footprint

Provided no other modal message is active, each active mech is also compared
with the first twelve building triggers. Y must match exactly after applying
the movement delta. X may match the mech centre or either adjacent coordinate:

```text
buildingY == mechY + deltaY
buildingX == mechX + deltaX - 1,
             mechX + deltaX,
          or mechX + deltaX + 1
```

This three-position X test gives the larger mech a wider doorway trigger than
the exact X/Y match used for an on-foot party member. A match displays
“Will you enter the {name}?” with Yes selected by default and dispatches the
BLD only when Yes is confirmed.

The original exits a matched building scan by setting its counter to `0x64`;
the maintained C uses `break`. Building prompting occurs before a collision
result terminates the outer mech loop, matching the original control flow.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:1FBE-218E`.
- Signed probe tables at `3EDB:045C-0463` and `3EDB:0474-047B`.
- Active friendly-mech words at `3092:406A` and packed mech positions at
  `3092:4004`/`4036`.
- Building trigger arrays at `3092:4564` and `3092:4596`.

The map arithmetic and both footprint widths are explicit in the assembly and
do not require an Astra confirmation pass.
