# `BTECH_0800` load-game reconstruction

## Reviewed block

- Address range: `0800:33E8-34D2`.
- Parent routine: `Load_Game`.
- Review boundary: party-mech display reset begins at `0800:34D3`.

This block rebuilds working state which is not stored in the contiguous save
record, then reloads the map data surrounding the saved packed coordinates.

## Rebuilt state

The loader clears exactly `0x21` consecutive bytes at `3092:45DE`. These are
one-use Star League Cache security-code flags. The previous header declared 34
words; byte instructions and the loop bound prove 33 byte entries.

The saved `OuttakeFrequency` byte at `3092:D35B` is copied into the menu system's
16-bit working value at `305B:02F8`.

Two values in static segment `2FE8` are reconstructed from saved flags:

- `Current_Map` defaults to Citadel and becomes destroyed Citadel when
  `Kurita_DestroyedCitadel` is set.
- `Viewed_Holodisk` defaults to zero and becomes `0x0C` when the persistent
  viewed-holodisk flag is set.

The literal `0x0C` role in the latter cache is retained without assigning a
broader meaning not established by this block.

## Star League Cache branch

If the saved `InsideStarLeagueCache` flag is nonzero, reconstruction delegates
to `Draw_STARLEAG_ICN_AND_Game_Logic` (`135D:0004`). Normal world-region cache
loading is skipped.

## Coarse region calculation

Outside the Cache, the centre region is recovered from the high bytes of the
two packed party coordinates:

```text
centreRegion = (packedX | packedY) >> 8
```

This works because the X region column and Y region row occupy separate
nibbles. The value is passed to `Map_data_structure` to select the appropriate
16×16 coarse-map definition.

## Reloading the 3×3 neighborhood

The northwest region is `centreRegion - 0x11`: one column west and one row
north in a row-major table whose row stride is `0x10`.

The nested 3×3 loop:

1. rejects signed region indices below zero or at least `0x100`;
2. reads the byte map number from `2FE8:0030[region]`;
3. skips zero entries; and
4. loads valid maps into descriptor slots `row * 3 + column`.

After all nine candidates, `Map_NineGrid_Parent_1DA8` expands/finalizes the
neighborhood for collision and rendering.

This proves `2FE8:0030` is a 256-byte coarse-region-to-MTP lookup table, not
the single byte previously declared in `BTECH.h`.

## Porting notes

- Separate serialized state from derived lookup and rendering state.
- Decode packed coordinates through a dedicated value type rather than
  duplicating the bitwise expression.
- Keep the neighbor index signed until bounds checking is complete.
- Preserve zero map numbers as absent coarse regions.
- Clear the 33 transient security-code flags after loading unless later story
  analysis proves they should become serialized in a new-format save.

No Astra review is requested. All widths, bounds, map indices, and branches are
directly visible in the clean assembly.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:33E8-34D2`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_135D.asm`, byte accesses at `45DE`.
- `Btech/BTECH_0800.c`, `Load_Game`.
- `docs/data-structures/PACKED_MAP_COORDINATES.md`.
