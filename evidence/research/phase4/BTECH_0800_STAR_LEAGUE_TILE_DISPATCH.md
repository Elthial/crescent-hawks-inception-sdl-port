# `BTECH_0800` Star League Cache tile dispatcher

## Review block (`0800:1E89-1FBD`)

This fourth slice of `Map_Interactables_Building_Or_Items` handles a blocking or
interactive tile while the party is inside the Star League Cache. It converts
the active on-foot formation probe to packed 16-bit world coordinates, then
dispatches the tile code according to whether the map-room puzzle view is
loaded.

All codes at or above the active threshold consume the attempted movement,
even if no individual handler displays a message. The checks are independent
`if` statements in the executable rather than a switch or exclusive chain.

## Packed interaction position

The handler position is calculated with 16-bit wrapping arithmetic:

```text
worldX = partyPackedX + movementDeltaX + onFootProbeX[slot]
worldY = partyPackedY + movementDeltaY + onFootProbeY[slot]
```

These words are passed to helpers which mask or normalize the local-coordinate
bits they require. The former maintained C used host-sized `unsigned int`
values, despite every load, addition, stack argument, and helper parameter
occupying one 16-bit word.

## Map-room puzzle view (`3092:D34E != 0`)

| Tile code | Action |
|---:|---|
| `94` | Climb the ladder back to the main Cache map. |
| `97..F0` | Toggle a star-map cell and play the map-interaction sound. |
| `8C`, `8D` | Submit/check the selected-star password. |

The `97..F0` helper changes the corresponding map tile as well as playing a
sound; these codes are not merely audio triggers.

## Main Cache map (`3092:D34E == 0`)

| Tile code | Action |
|---:|---|
| `7E` | Check the key panel at the current X position. |
| `7F` | Check the same multi-tile panel using `X - 2`. |
| `80` | Check the same multi-tile panel using `X - 4`. |
| `F5` | Reveal the secret passage, but only at packed X `0C01..0C04` and Y `C054`. |
| `B6`, `B7` | Operate a security-code terminal. |
| `F6..FF` | Offer the hidden Phoenix Hawk sequence if it has not already been found. |
| `83`, `A5..A7` | Display the cache/gyro clue dialog. |
| `4D` | Operate the HPG transmitter. |
| `3A`, `3D` | Operate the Hyperpulse Generator power control. |
| `28` | Handle the Cache map-room locations, including the cache and ladder. |

The three key-panel calls pass `(normalizedX, worldY, -1)`. The signed `-1`
sentinel tells `StarLeague_Key_Codes` to locate a panel from the supplied world
position rather than use a preselected/opened-cache ID. The old maintained C
incorrectly passed `0xFF` as Y, declared all three parameters as bytes, and
assigned `0xFF` to its local Y after each call. None of those Y substitutions
exists in the assembly.

## Porting implications

The tile buffer combines visual/collision codes with scripted interaction
triggers. A C# port should preserve the raw byte and route it through a
map-state-specific dispatcher; interpreting it only as a graphics tile number
would lose story and puzzle behavior.

The directly involved helper signatures have been corrected to 16-bit stack
arguments. In particular, the third `StarLeague_Key_Codes` parameter must be a
signed 16-bit value so `FFFF` remains `-1`.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:1E89-1FBD`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_135D.asm`, helper entry points
  `135D:01E9`, `0288`, `02A8`, `02D2`, `03AA`, `04AB`, `055A`, `079C`,
  `0913`, `0980`, and `0AB6`.
- Executable strings and state changes in the maintained helper annotations,
  which establish the user-visible purpose of each trigger.

The dispatcher and its arguments are explicit in the assembly. No Astra
confirmation is required for this block.
