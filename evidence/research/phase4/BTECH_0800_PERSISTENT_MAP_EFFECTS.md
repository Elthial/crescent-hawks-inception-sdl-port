# `BTECH_0800` persistent map-effect renderer

## Reviewed block

- Address range: `0800:2A93-2C4F`.
- Maintained name: `Draw_Persistent_Map_Effects_2A93`.
- Previous name: `CharacterPosition_2A93`.
- Called before the exploration and combat unit-sprite compositors.

The old name described one internal calculation rather than the routine's
purpose. This function projects and draws the game's saved 64-slot collection
of fires, weapon impacts, and wrecks.

## Parallel saved arrays

The renderer consumes four consecutive 64-byte arrays. A producer at
`183B:27C9`, now named `Register_Persistent_Map_Effect_27C9`, writes them using
the byte at `D557` as a circular insertion index.

| Memory range | Save-file range | Maintained name | Meaning |
|---|---|---|---|
| `3092:D457-D496` | `0x0E44-0x0E83` | `MapEffectSpriteIndex_D457` | Sprite-table index. |
| `3092:D497-D4D6` | `0x0E84-0x0EC3` | `MapEffectPackedPage_D497` | X page in low nibble; Y page in high nibble. |
| `3092:D4D7-D516` | `0x0EC4-0x0F03` | `MapEffectPositionXLow_D4D7` | Low seven bits of packed world X. |
| `3092:D517-D556` | `0x0F04-0x0F43` | `MapEffectPositionYLow_D517` | Low seven bits of packed world Y. |
| `3092:D557` | `0x0F44` | `NextMapEffectSlot_D557` | Next insertion slot, wrapping after `0x3F`. |

These offsets occupy the final `0x101` bytes of the direct-copy save-state
block. Therefore the visual effect collection—including transient-looking
impact sprites—is persisted in save files.

New-game initialization resets the insertion index and clears the packed-page,
X-low, and Y-low arrays. It does not clear the sprite-index array; the cleared
coordinates are the mechanism used to remove the old entries until their slots
are overwritten. The renderer does not consult a separate per-slot active flag.

Confidence: **verified** from the renderer, producer, save-copy boundary, and
their exact byte operations.

## Coordinate reconstruction and projection

The producer accepts two 16-bit packed coordinates. It stores their page
nibbles together and their low seven bits separately:

```text
page = uint8((worldX | worldY) >> 8)
xLow = uint8(worldX & 0x007F)
yLow = uint8(worldY & 0x007F)
```

The renderer reverses that split, subtracts the camera anchor, and adds the
screen-grid margins:

```text
worldX = xLow | ((page & 0x0F) << 8)
worldY = yLow | ((page & 0xF0) << 8)
projectedX = int16(worldX - cameraX + 26)
projectedY = int16(worldY - cameraY + 12)
```

It then applies the same signed neighbouring-page bounds and same-page wrap
guards seen in the exploration sprite renderer. Visible coordinates are masked
with `0x007F` and multiplied by eight to obtain pixel positions.

## Sprite and animation behaviour

Each sprite byte indexes the 16:16 far-pointer table beginning at `3092:39FA`.
The retained EGA path passes that sprite and the pixel coordinates to
`207F:0377`.

After a visible effect is drawn, `(sprite & 0x7E) == 0x7C` selects sprite IDs
`0x7C` and `0x7D`. XOR with one alternates those two IDs. Existing executable
constants identify them as the large and small fire images. Off-screen fire
does not advance because the toggle occurs only after the draw call.

The original executable selects `207F:0377` when graphics-adapter value `2`
is active and otherwise calls `207F:28EB`. The maintained source deliberately
retains the EGA pipeline; the removed branch is documented, not restored.

## Corrected decompiler artefacts

- All four effect arrays contain bytes, despite the former `wD...` names.
- The loop counter and projected arithmetic are 16-bit in the assembly.
- Page nibbles are shifted into bits `8-11`/`12-15`; the old C omitted those
  shifts and had misleading operator precedence.
- Screen margins are added before visibility testing, not while masking the
  final coordinates.
- The signed X lower bound is `0xFF8D` (-115), not positive `0x008D`.
- The same-page Y viewport is `0..24`, not a test beginning at 12.
- The fire-frame test indexes the current effect slot; the old C referenced an
  unrelated and undefined `CharacterId`.
- Producer coordinates are 16-bit stack words, not bytes.

## Open representation issue

`3092:39FA` still needs a proper array type for 16:16 far sprite pointers. The
maintained call temporarily indexes its flat word view as `sprite * 2` for the
offset and `sprite * 2 + 1` for the segment. This is already tracked by the
sprite-compositor reviews and does not require Astra confirmation.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:2A93-2C4F`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.dis`, preserving the full
  `0xFF8D` signed comparison immediate.
- `BTech-Reko-expanded/BTECH.reko/BTECH_183B.asm`, `183B:27C9-2834`.
- `Btech/BTECH_0800.c`, `Draw_Persistent_Map_Effects_2A93`.
- `Btech/BTECH_183B.c`, `Register_Persistent_Map_Effect_27C9`.
