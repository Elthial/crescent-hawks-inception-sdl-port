# `BTECH_0800` packed map-view relocation

For the coordinate layout and worked boundary examples, see
[Packed map coordinates](../data-structures/PACKED_MAP_COORDINATES.md).

## Review block (`0800:17BB-1816`)

`0800:17BB` moves the global packed map-view position at `246C:A44B/A44D` to a
requested X/Y coordinate. Its previous name, `CharacterPos_Compare`, described
only the comparisons and hid the routine's important map-streaming side effects.
It is now named `Move_Map_View_To_Packed_Position_17BB`.

The two arguments are 16-bit unsigned packed coordinates. The assembly compares
them with `JC` and `JA`; signed comparisons would be incorrect here. Movement is
strictly axis ordered:

1. Decrease Y through `207F:158C` until it matches the target.
2. Increase Y through `207F:163B` until it matches the target.
3. Decrease X through `207F:16E3` until it matches the target.
4. Increase X through `207F:17C5` until it matches the target.

Only one loop in each opposing pair can execute for a valid target. Y always
reaches its target before X begins moving.

### Why this cannot become a direct assignment

Each `207F` helper changes one packed sub-cell. Within a coarse map cell this is
just a low-byte increment or decrement. At the `00/7F` boundary, however, it
also changes the coarse coordinate and shifts/refills the cached 3x3 map-tile
neighbourhood. The Y helpers change the high byte by `0x10`; the X helpers
change it by one.

This explains why combat and targeting code repeatedly call the routine after
saving `A44B/A44D`: they temporarily relocate the loaded map view to a
combatant or effect position so map-dependent work uses the correct local tile
cache, then restore the former view through the same mechanism.

### Bounds caveat

The individual step helpers refuse to cross the outer packed-map bounds
(`X=00..0F`, `Y=00..F0` in their coarse components). Consequently this routine
assumes its callers provide reachable coordinates. An out-of-range target could
leave a loop retrying a step which the helper refuses to perform. No caller in
this review block validates the target locally.

### Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:17BB-1816`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_207F.asm`, step routines
  `207F:158C`, `163B`, `16E3`, and `17C5`.
- Combat and effects callers in segments `1543`, `1631`, `183B`, and `1AE8`.

## Review block 2: signed relative relocation (`0800:1817-186E`)

The maintained pseudo-C previously skipped directly from `0800:17BB` to
`0800:186F`. Raw bytes at `0800:1817-186E` contain a complete far-call routine,
including a conventional stack frame and `RETF`, which Reko failed to promote
to a function.

The reconstructed routine accepts signed 16-bit `DeltaX` and `DeltaY` values.
It consumes each stack-argument copy as a counter:

- A negative delta calls the corresponding decrement helper and increments the
  counter until zero.
- A positive delta calls the corresponding increment helper and decrements the
  counter until zero.
- A zero delta performs no movement.

As with absolute relocation, the order is Y followed by X. All movement passes
through the four `207F` helpers, preserving coarse-region boundary handling and
the cached 3-by-3 map-neighbourhood updates.

The routine is named `Move_Map_View_By_Signed_Delta_1817`. No direct call to
`0800:1817` was found in the expanded executable's current static assembly or
disassembly references. It may be unused library output or reached indirectly;
either possibility remains open.

### Raw instruction outline

```text
while (DeltaY < 0) { call 207F:158C; ++DeltaY; }
while (DeltaY > 0) { call 207F:163B; --DeltaY; }
while (DeltaX < 0) { call 207F:16E3; ++DeltaX; }
while (DeltaX > 0) { call 207F:17C5; --DeltaX; }
```

### Evidence

- Raw executable bytes printed at `0800:1817-186E` in
  `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`.
- The four called step routines in
  `BTech-Reko-expanded/BTECH.reko/BTECH_207F.asm`.

## Review block 3: absolute arithmetic-only relocation (`0800:186F-191A`)

`0800:186F` moves `246C:A44B/A44D` to an absolute packed X/Y destination but,
unlike `0800:17BB`, it performs no map streaming. It changes the words directly
and applies the packed-coordinate boundary corrections inline. Every identified
caller belongs to combat code, so its maintained name is `Combat_Move_Position`;
the more exact “move packed position without map streaming” description remains
beside the function as a `Sol:` comment.

The order remains Y first, then X, and all comparisons are unsigned. The four
normalization cases are:

| Direction | Intermediate boundary value | Correction | Result |
|---|---:|---:|---:|
| North | `6FFF` | `AND F07F` | `607F` |
| South | `7080` | `ADD 0F80` | `8000` |
| West | `0CFF` | `AND 0F7F` | `0C7F` |
| East | `0D80` | `ADD 0080` | `0E00` |

The two `183B` callers first save the current packed position, use `191B` to
derive formation positions for friendly infantry or mechs, copy those results
to the combatant position tables, and then call `186F` to restore the saved
words. The `1AE8` caller similarly relocates the words to a combatant before
performing relative coordinate calculations. These are temporary arithmetic
uses; shifting the actual cached map neighbourhood would be unwanted work.

Despite the combat-scoped name, the routine does not lock the player to a
movement table or impose a combat-area boundary. It reads no table and performs
no range clamp: it only walks the shared coordinate words toward the supplied
target while normalizing region crossings. Avoiding transient noncanonical
coordinates is part of its behavior, but restricting combat movement is not.

The destination must be a canonical packed coordinate with local bit 7 clear.
A target in the noncanonical `80..FF` local-byte range can be skipped when the
inline correction jumps directly between adjacent canonical regions. The
routine then terminates on the wrong side of the requested value because each
directional loop is visited only once.

### Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:186F-191A`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.dis`, confirming unsigned
  `UGT/ULT` comparisons and masks `F07F/0F7F`.
- Callers at `183B:0245`, `183B:02CA`, and `1AE8:10AC`.

## Review block 4: relative combat offset (`0800:191B-19BE`)

`0800:191B` is the relative counterpart to `Combat_Move_Position`. Its
maintained name is now `Offset_Packed_Position`. It accepts signed 16-bit X/Y
deltas, changes `246C:A44B/A44D` directly, and performs the same four inline
packed-boundary corrections as `186F`. It does not stream map regions.

The neutral name is intentional: `0800:051B`, the ordinary exploration sprite
compositor, also uses it to place friendly infantry and mechs around the party
anchor. Combat owns most of the remaining callers, but not the operation itself.

The routine consumes negative counters upward toward zero and positive counters
downward toward zero. Y is completed before X. Because the arguments are stack
copies, consuming them has no caller-visible effect and the routine returns no
value.

This routine is used for several combat-coordinate operations:

- applying signed-byte infantry and mech formation offsets;
- testing nearby positions during collision and movement planning;
- converting screen-relative targeting offsets into packed world positions;
- constructing the expanded visibility bounds around combatants.

The formation tables at `DS:3A16`, `3A1E`, `3A26`, and `3A2A` contain signed
bytes. Callers sign-extend each byte with `CBW` before passing it to `191B`.

### Corrected signed constants

Several maintained calls had lost the sign-extension evident in assembly:

| Call site | ASM word | Correct delta | Former annotation |
|---|---:|---:|---:|
| `0800:25A5` | `FFF0` | `X = -16` | `X = 0xF0` |
| `0800:2610` | `FFF0` | `Y = -16` | `Y = 0xF0` |
| `1631:17AB` | `FFFF` | `X = -1` | `X = 0xFF` |

The matching positive probes remain `+16` and `+2` respectively. This is a
direct example of why byte-looking hexadecimal constants in the 32-bit Reko
pseudocode must be checked against the original 16-bit register value.

### Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:191B-19BE`.
- Formation-table callers at `0800:0C1F`, `0800:0CCD`, `183B:0207`, and
  `183B:028C`, all using `CBW` before the call.
- Signed constant callers at `0800:25A5`, `0800:2610`, and `1631:17AB`.
