# `BTECH_0DAB` mech-status renderers

## Scope

This note covers `0DAB:174C-18E7`, the two small drawing helpers used by
`Examine_Screen_BTSTATS_CMP`.

## Vertical armour, structure, and heat gauge (`0DAB:174C`)

`Draw_Vertical_Mech_Status_Gauge_174C(x, bottomY, greenHeight, redHeight)`
draws two stacked, six-pixel-wide filled rectangles. Both heights and all
coordinates are native 16-bit words.

That width matters in the annotated C: the heat-gauge X coordinate is `0x0100`.
The previous `unsigned char` prototype would truncate it to zero in real C and
was another 32/16-bit decompiler-cleanup error, not an original-game behavior.

The red segment is drawn upward from `bottomY`. The green segment is then drawn
immediately above it. On the mech silhouette, green is current armour and red
is current internal structure. On the heat display, green is unused heat
capacity and red is current heat.

The earlier annotation reduced each segment to a one-pixel horizontal line.
The assembly instead calculates `topY = bottomY - height + 1` and passes the
calculated top and original bottom to the rectangle primitive.

There is one shipped edge case. If a red segment exists and the green height is
exactly one, the helper does not reset its cached top coordinate. It consequently
passes reversed Y bounds to `1F3D:01FB`, which rejects the rectangle. The last
green point is not displayed. This is recorded as BUG-006 in the original-game
bug register.

The heat instance is identified by its fixed bottom coordinate, `0x00B7`. Its
red segment flashes between EGA red (`4`) and bright yellow (`14`). The reset
delay is:

```text
10 - (currentHeat / 6)
```

Since the signed heat byte at `3092:006E + mechId` is clamped to `0..30` by the
caller, the delay ranges from 10 down to 5 redraws. The executable also retains
a historical alternate-adapter path
which uses palette values 2 and 3; the maintained source now documents the EGA
path without pretending those old branches never existed.

Three WORD globals at `DS:1218-121C` hold the countdown, reset delay, and
current flashing colour. The colour toggle modifies only the low byte, but the
surrounding reads and writes are WORD operations.

## Silhouette layout tables

The redraw loop covers eleven armour locations. Three parallel WORD tables
control it:

- `DS:1306`: mech-record offset of the corresponding current-structure byte;
- `DS:131C`: gauge X coordinate;
- `DS:1332`: gauge bottom-Y coordinate.

Their initialized values in the expanded executable are:

```text
DS:1306 structure offset = 001C 001D 001E 001F 0020 0021 0022 0023 0000 0000 0000
DS:131C gauge X          = 00F8 00D0 00E0 00C0 00C0 0088 00B0 00A0 0110 0120 0130
DS:1332 gauge bottom Y   = 0057 0037 0097 001F 0047 0057 0037 0097 0087 008F 0087
```

The first eight offsets select `CurrentStructure[0..7]` directly. A zero
structure offset suppresses the red segment for the three rear torso armour
locations, which have no independent internal-structure fields.

## Component-status pips (`0DAB:1858`)

`Draw_Component_Status_Pips_1858(column, row, totalPips, healthyPips)` draws
3-by-3 pixel boxes on an eight-pixel text grid. Pips start green and change
permanently to red when `pipIndex == healthyPips`; the fourth argument is
therefore a healthy count / first-damaged index, not a damaged-component count.

After the fifth pip the renderer returns to the original column and advances
one row. Engine, gyro, and sensors use three, two, and two total pips. The heat
sink display uses ten pips in two rows of five. If `healthyPips` is at least the
total, all visible pips remain green.

## Confidence

High. The rectangle bounds, colour transitions, word widths, table indexing,
and every known call site agree directly with the clean expanded assembly.
No Astra review is currently warranted for this block.
