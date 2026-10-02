# `BTECH_0800` character B/D/C sidebar row

## Reviewed block

- Address range: `0800:4AA6-4CAB`.
- Maintained routines: `Draw_Character_BDC_Sidebar_Row` at `4AA6` and its
  adjacent `Draw_BDC_Attribute_Bar` helper at `4BC1`.
- The next routine begins at `0800:4CAC`.

This block draws one compact party-member row in the left sidebar: the name,
the Body/Dexterity/Charisma bars, and the red damage overlay on the Body bar.
The letters `BDC` are drawn by the caller and correspond directly to those
three record fields.

## Inputs and record access

Both `4AA6` arguments are 16-bit stack words:

- `CharacterId` selects a 17-byte infantry record at
  `3092:C614 + CharacterId * 0x11`.
- `TextRowY` selects the character row used by the EGA text renderer.

The routine reads only five one-byte fields from that record: `Name` at `+00`,
`Body` at `+01`, `Dexterity` at `+02`, `Charisma` at `+03`, and `Health` at
`+0F`. Each load is followed by `CBW`, so the maintained reconstruction makes
the byte-to-signed-word conversion explicit instead of accepting Reko's
32-bit `int` guesses.

The name ID indexes the 16:16 character-name pointer table at `DS:01CA`. It is
drawn at text column 1 in bright white on black. The caller filters dead/unused
records before entering this routine.

## Coordinate conversion

After drawing the name, `TextRowY` is shifted left three bits. The attribute
bars therefore use pixel coordinates while the name uses text-cell
coordinates. Body, Dexterity, and Charisma are placed at text columns 9, 10,
and 11 respectively, producing pixel origins 72, 80, and 88.

## Attribute-bar geometry

`0800:4BC1` draws an attribute on a twelve-point scale. Four calls to the
clipped axis-aligned line primitive at `1F3D:031C` form a bright-yellow frame:

```text
top:    (x+1, y)    to (x+5, y)
left:   (x,   y+1)  to (x,   y+13)
right:  (x+6, y+1)  to (x+6, y+13)
bottom: (x+1, y+14) to (x+5, y+14)
```

The bright-green interior is the inclusive rectangle from `x+2` through
`x+4`, starting at `y + (12 - attribute) + 2` and ending at `y+13`. Higher
attributes consequently fill farther upward. The executable substitutes
different colours for adapter mode zero; those CGA-only alternatives remain
outside the maintained EGA path.

## Body-health overlay

Stored health is divided by decimal 10 with signed `IDIV`, giving the displayed
health in Body units. A zero quotient is changed to one. The routine then
computes:

```text
damagedBodyUnits = Body - displayedHealthUnits
damageTopY       = pixelY - Body + 14
```

When `damagedBodyUnits` is nonzero, an inclusive red rectangle covers x=74..76
from `damageTopY` through `damageTopY + damagedBodyUnits`. Full health skips
the overlay. At zero health, the minimum-one adjustment plus inclusive endpoint
causes the red rectangle to cover the complete Body fill.

The inclusive endpoints are the executable's exact calls to `1F3D:01FB`; they
should be preserved even though the existing annotated helper name misleadingly
calls that rectangle filler a horizontal-line routine.

## Confidence and porting notes

Confidence is high. Record offsets, stack widths, signed byte extension,
division, coordinates, colours, and primitive calls are direct assembly
translations. No Astra review is requested for this block.

A C# port should keep character fields as bytes, perform the explicit `/ 10`
display conversion, and put the text-cell-to-pixel conversion at this UI
boundary rather than embedding pixel coordinates in the character model.
