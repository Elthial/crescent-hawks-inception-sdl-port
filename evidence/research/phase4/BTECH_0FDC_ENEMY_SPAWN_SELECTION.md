# `BTECH_0FDC` enemy spawn selection

## Review boundary

This block covers `Mission_GenerateEnemies` from `0FDC:0E45` through `0F54`.
It selects the spawn origin, reference mech template, sprite family, initial
enemy pilot-record index, and final enemy-mech count. The actual mech generation
loop starts at `0F55`.

The preceding reset is documented in
[`BTECH_0FDC_ENEMY_GENERATOR_RESET.md`](BTECH_0FDC_ENEMY_GENERATOR_RESET.md).

## Request-word encoding

The first WORD argument is not a mission number. Its low seven bits request the
number of enemy mechs, while bit 7 selects the Mission08/world-region spawn and
random-template path. The second WORD similarly contains an enemy-infantry
count in its low seven bits; its bit 7 selects Jason's on-foot/jailbreak setup.

At the end of this block, the mech request is masked with `0x007F`. A nonzero
Kurita interruption flag overrides that result with four enemy mechs. The next
enemy pilot uses compact infantry-record index 8.

## Training and jailbreak path (`0EF5-0F31`)

With mech-request bit 7 clear, the reference template remains the Locust at
`2FE8:02F0`. The game first consumes two random bytes and creates a training
spawn origin:

```text
X = 0C65 + (randomByte & 7)
Y = C059 + (randomByte & 7)
```

If infantry-request bit 7 is set, both coordinates are then replaced with the
jail entrance `0D10:7024`. The discarded random values are still consumed and
must remain in a deterministic port.

Persistent BLD state byte `3092:D30C` is the training mission number at this
call site. For mission 2, X is replaced with `0C72` while the randomized Y is
retained. This is the disabled-Locust mission's fixed horizontal placement.

## Mission08/world spawn origin (`0E4E-0E8C`)

With mech-request bit 7 set, X is selected from one of two four-position bands
and Y from one four-position band:

```text
initial X = 0A10..0A13
if a separate random low bit is 1:
    replacement X = 0A28..0A2B
Y = 806F..8072
```

The initial left-band roll is always consumed, even when the right-band branch
replaces its result. Selecting the right band therefore consumes one additional
PRNG byte.

## Template and sprite-family tables (`0E8D-0EF4`)

Normal selection indexes the far-pointer table at `3EDB:13E2` and byte table at
`3EDB:140E` with `2D6 - 2`, producing indexes `0..10`:

| 2D6 | Template | Pointer | Sprite-family offset |
|---:|---|---|---:|
| 2 | Jenner | `2FE8:0561` | `00` |
| 3 | UrbanMech | `2FE8:065B` | `00` |
| 4 | Commando | `2FE8:0467` | `92` |
| 5 | Locust | `2FE8:02F0` | `00` |
| 6 | Wasp | `2FE8:036D` | `92` |
| 7 | Locust | `2FE8:02F0` | `00` |
| 8 | Stinger | `2FE8:03EA` | `92` |
| 9 | Wasp | `2FE8:036D` | `92` |
| 10 | Commando | `2FE8:0467` | `92` |
| 11 | Jenner | `2FE8:0561` | `00` |
| 12 | UrbanMech | `2FE8:065B` | `00` |

This gives the normal templates the triangular 2D6 distribution rather than a
uniform chassis distribution. Offset `00` selects the Locust-type sprite family
and `92` the upright/humanoid family.

When rental-mech Arena flag `3092:E48E` is nonzero, the routine scans exactly
four stored friendly-mech name initials at `D452..D455`. If any is `FF`, it
changes the requested enemy-mech count to two. The previous pseudo-C loop bound
`0x2BB2` was a severe Reko/transcription error; assembly compares the WORD index
with four.

Special solo mode still calls `Rand_Dice_Two_D6()` and subtracts two, but then
discards that result and replaces it with a uniform random index `3..6`. The
effective choices are Locust, Wasp, Locust, and Stinger: 50 percent Locust and
25 percent each Wasp or Stinger. The discarded dice calls are significant for
the deterministic PRNG stream and attract-mode synchronization.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0E45-0F54`;
- raw table bytes in `BTECH_3EDB.asm`, `3EDB:13E2-1418`;
- the verified `2D6` implementation at `0800:19DD`;
- the four-entry `D452` scan bound at `0E8D-0EAE`;
- count masking and Kurita override at `0F32-0F54`.

The request encoding, random-call order, coordinate ranges, table contents,
solo-mode override, and final count are directly verified. The higher-level
story name for Mission08 and the full purpose of special flag `E48E` remain
open. No Astra review is required.

The generated mech records and combatant state are documented in
[`BTECH_0FDC_ENEMY_MECH_GENERATION.md`](BTECH_0FDC_ENEMY_MECH_GENERATION.md).
