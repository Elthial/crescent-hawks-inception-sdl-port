# `BTECH_0800` combatant sprite compositor

This document records bounded review passes over `0800:0E4B-172C`. The old
maintained name `Draw_Menu_MultiSelect_0E4B` is provisional: its callers and
state writes show that it is the common combatant sprite compositor used by
combat, movement targeting, and combat animation as well as menu-driven target
selection.

## Review block 1: setup and infantry scan (`0800:0E4B-119E`)

### Combatant groups

The function first refreshes combatant positions with `0800:2A93`, saves the
packed camera coordinates, and clears the 24-byte visibility table at
`3092:42F6`. The original also saves `(cameraX | cameraY) >> 8` before the
refresh call, but no later instruction reads that stack local; the maintained
annotation omits this dead compiler temporary.

Its first drawing pass uses position-table offsets `0` and `12`, with eight
entries in each group. Adding four produces the actual combatant IDs:

| Position offset | Combatant IDs | Meaning |
|---:|---:|---|
| `0..7` | `4..11` | friendly infantry |
| `12..19` | `16..23` | enemy or NPC infantry |

Combatant IDs `12..15` are enemy mechs and are deliberately left for the next
pass. A position-X word of `0xFFFF` means that infantry slot is absent.

### Packed-coordinate projection

For each present infantry combatant:

```text
screenX = worldX - cameraX + 26
screenY = worldY - cameraY + 12
```

If world and camera coordinates have matching high-byte pages, the ordinary
visible ranges are `X=13..39` and `Y=0..24`. Broader signed guards
`X=-115..167` and `Y=-3968..3992` allow the packed-coordinate representation
to cross an adjacent page. The raw `.dis` again proves that the abbreviated
assembly immediate `8Dh` represents sign-extended `0xFF8D`, not positive 141.

Accepted coordinates are masked with `0x7F`, converted to pixels by multiplying
by eight, and stored in the per-combatant screen-position tables at
`3092:324C` and `3092:327C`. `3092:42F6[combatantId]` is set to one, proving it
is an on-screen visibility table. Combat effects later consult it to decide
whether projectile and impact animation endpoints are available.

### Map tile and terrain overlap

The renderer converts the projected position into an index in the 24-column
visible map buffer. Camera and unit parity select one of four WORD masks at
`DS:0372`:

```text
{ 0x0009, 0x0003, 0x000C, 0x0006 }
```

The map byte beneath each visible combatant is saved at `3092:3750`. This field
was previously called `DistanceToTarget`, but combat/fire code tests it against
tile ranges such as `0x10..0x3F`; its actual name is now
`MapTileUnderCombatant`.

On ordinary maps, qualifying foreground terrain clips two rows from an
infantry sprite, or four rows for tile family `0x20`. As elsewhere, the clean
executable temporarily changes byte `+1` of the selected sprite's metadata,
draws it, and restores that byte.

### Open items

- Rename `Draw_Menu_MultiSelect_0E4B` after the complete function has been
  reviewed, so the final name reflects every responsibility.
- Reconstruct `3092:39FA` as a true array of 16:16 far pointers. The current
  maintained array operation does not express the metadata-byte edit exactly.
- Continue at `0800:119F`, which collects visible mech sprites for a subsequent
  depth-sorted draw pass.

### Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:0E4B-119E`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.dis`, particularly the preserved
  `0xFF8D` bound at `0800:0F8D`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, initialized WORD values at
  `3EDB:0372-0379`.

## Review block 2: collect visible mechs (`0800:119F-140B`)

### Combatant groups and output

The second pass visits two groups of four combatants:

| Group offset | Combatant IDs | Meaning |
|---:|---:|---|
| `0` | `0..3` | friendly lance mechs |
| `12` | `12..15` | enemy mechs |

A position-X word of `0xFFFF` marks an unused slot. The `.asm` abbreviates the
sign-extended immediate as `0FFh`, while the raw `.dis` preserves `0xFFFF`.
Every accepted mech updates the common visibility, screen-position, map-tile,
and terrain-overlap tables, then contributes four values to temporary arrays:
pixel X, pixel Y, overlap rows, and sprite-table index. At most eight mechs can
be collected. The last-used index begins at signed `-1`, so that value means
the collection is empty.

### Projection differences from infantry

Mechs use the same packed-coordinate projection as infantry but have a larger
screen footprint. On the same packed page their accepted screen-cell ranges
are `X=11..39` and `Y=0..26`. Their broad adjacent-page guards are
`X=-117..167` and `Y=-3968..3994`.

For map lookup, screen Y selects a 24-column row. A Y-parity difference between
the mech and camera changes the terrain mask from `0x01` to `0x04` and records
a one-byte adjustment at `3092:4554[combatantId]`.

The column offset is obtained through two deliberately biased DS lookup bases:

- `DS:0364[screenX]` supplies the normal map-column offset. Since visible mech
  X begins at 11, the first reachable initialized entry is actually `DS:037A`.
- When camera X is odd, `DS:039E[screenX]` supplies a second correction; its
  first reachable entry is `DS:03B4`.

The byte from map buffer `246C:0795` is copied to `3092:45CE[combatantId]`.
Combat code later compares this value and uses the `4554` parity adjustment,
but the exact gameplay meaning of the `0795/45CE` byte remains unresolved.

### Terrain overlap and deferred drawing

Qualifying foreground terrain clips eight rows from a mech sprite. Tile family
`0x00` or `0x20` with low nibble `0x0F` clips sixteen rows. These are stored in
`TerrainOverlapRows[combatantId]` and later also serve as combat cover values.

Unlike the infantry pass, mechs are not drawn immediately. Their temporary
records are collected first because the next block sorts them by pixel Y before
drawing, preserving front-to-back visual order.

### Open items

- Determine the semantic meaning of map buffer `246C:0795` and its cached
  per-combatant byte at `3092:45CE`.
- Give `3092:4554` a final name after tracing every parity-adjustment consumer.
- Continue at `0800:141A`, the mech depth-sort and draw pass.

### Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:119F-140B`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.dis`, especially the `0xFFFF`
  unused-position sentinel and signed `0xFF8B` lower bound.
- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, initialized lookup data
  reached through the biased `DS:0364` and `DS:039E` bases.

## Review block 3: depth-sort and draw mechs (`0800:141A-1615`)

If no mechs were collected, the signed last index remains `-1` and this block
is skipped. Otherwise, a pairwise ascending sort compares pixel Y and swaps all
four parallel fields together: pixel X, pixel Y, sprite index, and terrain
overlap. This is painter's-order sorting: mechs nearer the top of the viewport
draw first, then lower-screen mechs draw over them.

Each collected position is the mech's ground anchor. The 24-by-24 sprite draw
origin is therefore `pixelX - 8, pixelY - 16`. Before drawing, the renderer
subtracts the terrain-overlap depth from byte `+1` of the selected sprite's
metadata; it restores the byte immediately afterward.

The clean executable again branches by graphics adapter: adapter `2` calls the
retained EGA drawer at `207F:0377`, while other adapters call `207F:28EB`. The
maintained annotation represents only the retained EGA path. Sol: the later
sprite-reader reconciliation now uses the376-entry FAR table and transient
header-BYTE edits; this pointer TODO is closed (2026-09-17).

### Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:141A-1615`.
- The stack arrays populated by `0800:1394-1405` and consumed together by the
  swap sequence at `0800:145A-14F0`.

## Review block 4: Mission09 parked-mech overlay (`0800:1616-172C`)

The compositor ends with a second implementation of the Mission09 overlay
already found at `0800:0D30-0E46`. This is not another set of objects: both
blocks project the same four parked jail-courtyard 'Mechs at packed world
positions `0D13:702C`, `0D17:702C`, `0D1B:702C`, and `0D1F:702C`.

The duplicated implementation has the same screen origin, visibility limits,
`& 0x7F` packed-coordinate conversion, sprite-table entry `0x92`, and graphics
adapter branch. The maintained annotation keeps the duplication visible because
it reflects the original executable's control flow; a future C# port can safely
share one helper between the exploration and combatant compositors.

This completes the full `0800:0E4B-172C` compositor review. Its provisional
`Draw_Menu_MultiSelect_0E4B` name can now be replaced with a rendering-oriented
name in a subsequent small refactor.

### Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:1616-172C`.
- The matching exploration implementation at `0800:0D30-0E46`.
