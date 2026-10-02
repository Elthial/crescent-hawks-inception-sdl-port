# Phase 3M2 — MECHSHAP sequence spritesheet export

## Review boundary

This block exports one portable, transparent PNG spritesheet and JSON metadata
from a user-supplied `MECHSHAP.CMP`. It organizes the Locust and Commando-style
mechs by executable-defined animation sequence, followed by shared debris,
fire, impacts, wreckage, fallen sprites, and all personnel/infantry graphics.
The three colour-coded personnel blocks are labelled by their known gameplay
roles. It does not yet replace every legacy sprite caller.

## Command

```powershell
dotnet run --project InceptionTools -- export-mech-spritesheet `
  --game-dir chinception `
  --output artifacts/spritesheets/MECHSHAP-spritesheet.png `
  --metadata artifacts/spritesheets/MECHSHAP-spritesheet.json
```

The default outputs are `MECHSHAP-spritesheet.png` and a same-basename JSON
file. Both outputs are collision-checked before either is written; `--force`
allows replacement.

## Verified source rectangles

Startup code at `0D27:0410..082C` makes 376 calls to `1F3D:070A`. Each call
supplies the sprite ID, source X in eight-pixel units, source Y, width in
eight-pixel units, and pixel height. `MechShapeSpriteCatalog` reproduces those
loops and individual calls exactly, covers every ID `0x000..0x177` once, and
rejects duplicate or out-of-image rectangles.

This is stronger than inferring equal cells from the artwork. Source sprites
have three verified sizes: 24×24 mechs, 8×8 personnel/effects, and selected
16-pixel-wide fire, wreck, and fallen shapes with differing heights.

## Animation rows

The first 48 rows are actual direction-specific control sequences:

- eight Locust walk rows, eight fire rows, and eight kick rows;
- eight Commando walk rows, eight fire rows, and eight kick rows.

Direction order is north, northeast, east, southeast, south, southwest, west,
and northwest, matching the executable strings at `3EDB:006D`. Walk streams
are stored at `2FE8:0270..02EF` and selected by the far-pointer table at
`3EDB:025A`. The direction-specific fire/kick pointer tables begin at
`3EDB:2D58`; their streams begin at `3EDB:3EF0`. Repeated frames and reused
poses are deliberately repeated in output rows because that is the real
playback order.

The remaining rows keep all other extracted content at the bottom:

| Rows/category | Source IDs | Treatment |
|---|---:|---|
| debris | `0x082..0x091` | one sequence row |
| fire | `0x07C..0x07D` | one sequence row |
| impacts | `0x07E..0x07F` | one sequence row |
| mech wreckage | `0x080..0x081` | one sequence row |
| fallen personnel | `0x176..0x177` | one sequence row |
| red teammates | `0x010..0x077` | core row plus seven extended rows |
| light-blue Jason Youngblood | `0x0A6..0x10D` | core row plus seven extended rows |
| grey enemies or civilians | `0x10E..0x175` | core row plus seven extended rows |

The palette roles were identified from gameplay: red denotes Jason's teammates,
light blue denotes Jason Youngblood, and grey is shared by enemies and civilian
NPCs. Grey therefore must not be treated as synonymous with hostile; encounter
state or character data must supply that distinction.

## Output contract

Every output cell is 24×24. Smaller source rectangles are horizontally centred
and bottom-aligned. Palette index zero is made transparent as an explicit
export policy. JSON metadata records:

- source file, compression format, dimensions, and total unique sprite count;
- sheet/cell dimensions and transparent index;
- category, sequence name, and row;
- playback frame index and original sprite ID;
- exact source rectangle, output cell rectangle, and content position.

The real output is 480×1848 with 77 named rows and 470 placed playback frames.
Those placements reference all 376 unique original sprite IDs.

## Verification

The dependency-free harness passes **135 assertions**. New checks cover catalog
extent and variable geometry, exact Locust/Commando block boundaries, sequence
ordering, personnel role labels, complete unique-ID coverage, source-pixel
copying, PNG/JSON output, metadata content, and overwrite protection.

The ignored original output was visually checked for row alignment and bottom
grouping. FFprobe independently reads it as a 480×1848 indexed PNG. Extracted
outputs remain under ignored `artifacts/` and are not committed.
