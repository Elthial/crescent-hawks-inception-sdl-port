# `BTECH_11B8` Arena combat map-patch selection

Sol: Correction from the [fresh systematic audit](BTECH_11B8_SYSTEMATIC_ASM_AUDIT.md):
the selector is WORD **246C:0010**, not3092:0010. DS:54EA resolves to246C.
The old address below is historical and must not be used for implementation.

## Review boundary

This block covers `11B8:137F-1440`, now named
`Arena_Select_And_Apply_Combat_Map_Patch_137F`. It is called only by raw BLD
action `0x23`, the shipped `ARENA.BLD` command that runs the Arena mech fight.
The dispatcher invokes it immediately before `Mech_Mission_0629(0x08)`.

## Variant selection

The routine masks the 16-bit random result with `0x0003` and stores the WORD
at `3092:0010`, now named `ArenaCombatMapPatchVariant_0010`.

| Selector | Setup operation |
|---:|---|
| `0` | Change no map tiles. |
| `1` | Install one contiguous temporary tile run. |
| `2` | Install two contiguous temporary tile runs. |
| `3` | Install seven temporary tiles at an eight-byte stride. |

The paired post-combat routine at `11B8:1441` reads the stored selector and
rewrites the same cells with tile IDs `0x40..0x43`. IDs `6A`, `6B`, `6F`, and
`71` are also members of the `ANIMATE.ICN` destination table. Combined with
the owner's identification of the Starport Arena crowd animation, this
establishes that the patch installs animated crowd tiles. Tile `70` is the
unanimated companion used at the ends of two crowd runs. Exact individual
artwork and collision meanings remain unresolved.

## Exact map writes

`MapTile` begins at `246C:101D`; the instructions below write byte tile IDs,
not pointers or WORDs.

| Variant | Absolute addresses | `MapTile` indexes | Tile IDs |
|---:|---|---|---|
| `1` | `1C8F`, `1C90-1C94`, `1CCD` | `0C72`, `0C73-0C77`, `0CB0` | `6F`, five `6A`, `70` |
| `2` | `1A78`, `1A79-1A7C`, `1AB5-1ABA`, `1ABB` | `0A5B`, `0A5C-0A5F`, `0A98-0A9D`, `0A9E` | `6F`, four `6A`, six `6A`, `70` |
| `3` | `1AD9`, `1CA1-1CD1` every eight bytes, `1CD9` | `0ABC`, `0C84-0CB4` every eight, `0CBC` | `71`, seven `6B`, `70` |

The former pseudo-C confused absolute segment addresses with loop counters and
invented bounds in the `E3xx/E5xx` range. For example, the five-write loop at
`11B8:13BB` was rendered as roughly fifty thousand writes. The replacement
uses the exact assembly counts and converts each absolute address to its
correct `MapTile` index.

## Script context

Decoded `ARENA.BLD` reaches action `0x23` after either preparing a rented
Locust (action `0x21`) or staging one of the player's mechs (action `0x22`).
After the fight, the script tests Arena result flags to choose the loss, win,
or stolen-rental-mech narrative. InceptionTools therefore labels action
`0x23` as `RUN_ARENA_MECH_COMBAT`.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:137F-1440`;
- `BTech-Reko-expanded/BTECH.reko/BTECH_1CD3.asm`, Arena dispatcher case;
- decoded shipped `chinception/ARENA.BLD`, payload offset `06A5`;
- the paired map rewrite at `11B8:1441-152E`.

The selector width, loop counts, tile addresses, tile values, Arena call site,
action number, and animated-crowd role are verified. Exact individual tile
artwork and collision semantics remain unresolved. No Astra review is needed
for this block.
