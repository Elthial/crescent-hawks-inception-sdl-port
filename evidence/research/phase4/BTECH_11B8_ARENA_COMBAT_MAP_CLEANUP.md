# `BTECH_11B8` Arena combat map cleanup

Sol: Correction from the [fresh systematic audit](BTECH_11B8_SYSTEMATIC_ASM_AUDIT.md):
the selector is WORD **246C:0010**, not3092:0010. DS:54EA resolves to246C.
The old addresses below are historical and must not be used for implementation.

## Review boundary

This block covers `11B8:1441-152E`, now named
`Arena_Remove_And_Randomize_Combat_Map_Patch_1441`. It is the teardown paired
with `Arena_Select_And_Apply_Combat_Map_Patch_137F`.

The Arena dispatcher calls it after mission 8 only when
`Bool_AllowEscape_D32F` is zero. The stolen-rental-mech escape branch bypasses
the cleanup and follows its separate Starport map path.

## Selector and zero case

The routine reads the complete WORD at `3092:0010`, not a byte. Values `1..3`
select the same tile group installed before combat. Selector zero returns
without changing the map and consumes no random numbers.

## Cleanup writes

The routine does not restore saved original bytes. It replaces each temporary
Arena crowd tile with one of the ordinary tile IDs `40-43`. Interior cells
are independently randomized with `(random & 3) + 40`; endpoint values are
fixed.

| Variant | Fixed replacements | Randomized cells | RNG calls |
|---:|---|---|---:|
| `1` | `0C72=43`, `0CB0=42` | `0C73-0C77` | 5 |
| `2` | `0A5B=41`, `0A9E=42` | `0A5C-0A5F`, `0A98-0A9D` | 10 |
| `3` | `0ABC=40`, `0CBC=41` | `0C84-0CB4`, stride 8 | 7 |

All indexes are relative to `MapTile` at `246C:101D`. The corresponding
absolute destinations are exactly the cells written by setup at
`11B8:137F-1440`.

## Animated crowd identification

Setup uses tile IDs `6A`, `6B`, `6F`, and `71`. All four occur in the second
five-entry group of the executable's `ANIMATE.ICN` destination table. The
owner previously identified the two animation families as the Citadel fence
and Starport Arena crowd; this Arena-only call site resolves these four IDs as
crowd animation tiles. ID `70` is the static companion at the ends of two
temporary crowd runs.

The cleanup's randomized `40-43` replacements explain why no original-byte
backup exists: the program reconstructs varied ordinary scenery rather than
performing a byte-exact restoration.

## Corrections to the former pseudo-C

- `3092:0010` and the selector comparisons are 16-bit.
- Every map destination is a byte inside `MapTile`, not an invented struct
  member or pointer.
- The three loop counts are 5, 10, and 7 writes, not enormous loops ending at
  decompiler-generated `E3xx/E5xx` bounds.
- Variant zero performs no cleanup.
- The random value uses low byte `AL`; each selected interior cell receives
  one byte in the inclusive range `40-43`.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:1441-152E`;
- paired setup at `11B8:137F-1440`;
- Arena dispatcher at `1CD3:1140-1150` and its escape branch;
- animated destination table at `3EDB:04B0`;
- the owner's identification of the `ANIMATE.ICN` visual families.

Control flow, widths, destinations, loop bounds, RNG counts, and Arena role
are verified. The exact art represented by each individual crowd tile and the
semantic reason cleanup is skipped during escape remain open. No Astra review
is needed for this block.
