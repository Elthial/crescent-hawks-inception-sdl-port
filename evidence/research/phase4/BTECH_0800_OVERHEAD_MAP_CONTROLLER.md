# `BTECH_0800` overhead-map controller

## Reviewed block

- Address range: `0800:3D40-3FAD`.
- Maintained name: `Show_Overhead_Map`.
- This covers the complete controller routine.
- Only the call/return contract of the following renderer at `0800:3FAE` was
  corrected. Its body remains the next separate review block.

## Marker position versus viewport position

The controller maintains two pairs of 16-bit packed coordinates:

| Pair | Purpose |
|---|---|
| Original party X/Y | Fixed player marker and the position restored on exit. |
| Viewport X/Y | Temporary centre used while scrolling the overhead display. |

The renderer reads the viewport from globals `246C:A44B/A44D`, but receives
the original party position as two arguments so it can draw the fixed marker.
Before returning, the renderer puts those marker arguments back into the
globals. The controller therefore restores its saved viewport before applying
the next arrow command.

The old pseudo-C collapsed both pairs into `Character_PosX/Y`. As a result, it
appeared to move the player marker with the viewport and finally restore the
scrolled viewport as the party's actual world position. Neither occurs in the
assembly.

## Renderer input contract

`Overhead_Map_Draw_3FAE` is not void. After drawing, it obtains a key through
`Keyboard_Get_ASCII_Hex_Input`, drains pending input, and returns the saved
16-bit key value in AX. The controller passes that value to
`Keyboard_Convert_To_MoveCommands`.

This also confirms that `Keyboard_Get_ASCII_Hex_Input` itself returns a signed
16-bit AX value rather than the `char` inferred in its old C signature. That
interface was corrected here because the overhead controller depends on it;
other input call sites remain for their own block reviews.

## Scrolling

Space exits. Arrow and diagonal commands move the temporary viewport by two
coarse map regions:

| Direction component | Boundary test | Change |
|---|---:|---:|
| North | Y `>= 0x2000` | Y `-= 0x2000` |
| South | Y `< 0xE000` | Y `+= 0x2000` |
| West | X `>= 0x0300` | X `-= 0x0200` |
| East | X `< 0x0D00` | X `+= 0x0200` |

The original south and east instructions modify the high byte directly:
`ADD BYTE PTR [A44E],20` and `ADD BYTE PTR [A44C],02`. In little-endian 16-bit
coordinates these mean `+0x2000` and `+0x0200`. The previous transcription
incorrectly treated them as additions of `0x20` and `0x02` to the complete
words.

When `InsideStarLeagueCache` is nonzero or `Kurita_DestroyedCitadel` is zero,
the controller marks the loop finished before rendering. It still renders one
screen and consumes one key, but does not allow another scrolling iteration.
Scrolling is enabled only after the Citadel-destroyed state becomes nonzero.
The Cache path also clears bit `0x80` of byte `3092:D178`. This address is
inside the fog bitmap: `D178 - CB0C = 0x66C`, or row 102, byte-column 12.
Because fog bytes are consumed most-significant-bit first, the operation clears
that byte's leftmost visibility cell. Why Cache map exit changes this one cell
remains unresolved.

## Restoring the exploration cache

On exit, the controller always restores the original party position. Outside
the Star League Cache it then rebuilds the ordinary 3-by-3 map neighbourhood:

```text
centreRegion = lowByte((packedX | packedY) >> 8)
firstCandidate = centreRegion - 0x11
```

Candidates are visited in three rows and columns, adding `0x10` between rows.
Only signed candidates from `0x00` through `0xFF` are valid. A nonzero entry in
the 256-byte `2FE8:0030` map-file lookup is loaded into display slot
`column + row * 3`.

The earlier C expression placed `>> 8` only on Y because of operator
precedence. The assembly performs OR first and then shifts AX, matching the
packed-coordinate documentation.

After loading, the first three pending-grid bytes at `246C:09F3` are set to
`0xFF`, the nine-grid cache is finalized, and the ordinary exploration view,
characters, border, and sidebars are redrawn. Word `DS:014C` guards the map
loader from disturbing roaming-NPC state during these presentation loads.

## Porting notes

- Keep the viewport local to the overhead UI in a new implementation; never
  mutate the authoritative party position merely to pan the display.
- Preserve the original packed words at asset/save boundaries, but expose
  coarse-region movement explicitly in the C# coordinate type.
- Bounds-check the signed 3-by-3 candidate indexes before accessing the
  256-entry map-file lookup.
- The Cache map-buffer exchange performed by `OverHead_Map_Function` needs its
  own review before being ported.

No Astra review is requested for this controller. Its coordinate locals,
input return value, movement constants, region calculation, and restoration
flow are explicit in the clean assembly. The separate renderer is larger and
will be assessed for Astra review during its own block.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:3D40-3FAD`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_1F3D.asm`, input return path at
  `1F3D:0259-031B`.
- `Btech/BTECH_0800.c`, `Show_Overhead_Map` and the renderer interface.
- `docs/data-structures/PACKED_MAP_COORDINATES.md`.
