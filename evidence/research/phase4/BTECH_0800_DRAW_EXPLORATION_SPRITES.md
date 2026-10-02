# BTECH_0800 exploration sprite drawing

## Review block 1: party infantry (`0800:051B-074D`)

### Current symbol and purpose

- Maintained symbol: `Draw_Infantry_And_Mechs`.
- The complete function is the exploration-map sprite compositor. This first
  reviewed subsection draws living party characters whose `MechAssignment` is
  `8` (on foot).
- Confidence: **Verified** for control flow, record tests, table widths, and
  adapter branch structure; **Probable** for the terrain-overlap interpretation.

### Interface and side effects

- No stack arguments and no return value.
- Calls `Draw_Persistent_Map_Effects_2A93` before drawing unless the party is inside the
  Star League cache.
- Clears twelve word-sized active/on-map entries beginning at `3092:406A`.
- Counts drawable on-foot party members in `3092:006A`.
- Writes the selected two/four-row overlap amount to `3092:32B2[characterId]`.
- Draws through one of two graphics paths selected by `3EDB:4FBA` and restores
  the temporarily adjusted sprite metadata afterward.

### Pseudocode

```text
if not insideStarLeagueCache:
    prepareCharacterPosition()

onFootCount = 0
clear active flags for friendly combatants 0..11

for characterId in 0..7:
    if character.name != 0xFF and character.mechAssignment >= 8:
        slot = onFootCount++
        screenX, screenY = partyScreenPosition[slot]
        mapCell = visibleMapBase + partyMapOffset[slot]
        adjust mapCell for odd camera X/Y
        mask = partyOcclusionMask[slot][cameraParity]
        tile = visibleMap[mapCell]

        overlapRows = 0
        if normal BattleTech map and tile/mask request overlap:
            overlapRows = (tile high nibble == 0x20) ? 4 : 2

        sprite = spritePointerTable[baseSprite[characterId]
                                    + animationOffset[characterId]]
        temporarily shorten/clip sprite by overlapRows
        draw sprite at screenX, screenY
        restore sprite metadata
```

### Corrected Reko artefacts

- The tests of character bytes `+0x00` and `+0x0C` are value comparisons. The
  maintained output previously compared their addresses.
- Stack locals and the `02AE` through `0332` tables are word-sized. Reko had
  collapsed several of them to bytes, pointers, or spurious divisions by
  `0x07AE`; the assembly performs direct word indexing and addition.
- `0332` is selected as `slot * 4 + cameraParity`, where parity is the two-bit
  combination of camera Y bit 0 and camera X bit 0.

### Open items

- Reconstruct the `3092:39FA` table as an explicit array of 16:16 far sprite
  pointers. The current maintained-C expression loses this representation.
- Preserve the mutually exclusive draw branches: adapter value `2` calls
  `207F:0377`; other values call `207F:28EB`, with adapter value `0` also using
  the vertical clip boundary at `B780`.
- Reconcile the aliases covering `3092:406A-4099`. Assembly clears entries
  `0..11` here, while combat code addresses combatant IDs through `23`, strongly
  indicating one 24-word active/on-map table ending immediately before `409A`.
- Continue at `0800:074E`, the map-NPC/enemy-infantry drawing pass.

### Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:051B-074D`.
- `Btech/BTECH_0800.c`, `Draw_Infantry_And_Mechs`.
- [Character record layout](../data-structures/CHARACTER_RECORDS.md).

## Review block 2: map NPC/enemy infantry (`0800:074E-09FD`)

### Purpose and selection

- Iterates combatant IDs `16..23`. Combat treats these as enemy-infantry slots;
  exploration also uses them for the grey civilian/hostile map NPC sprites.
- Draws a slot only when byte `+0` of its `0x1A`-byte runtime record (base
  `3092:D1F9 + combatantId * 0x1A`) is zero.
- Uses the same sprite-pointer selection, terrain overlap, and adapter-specific
  draw paths as the party-infantry pass.
- Confidence: **Verified** for arithmetic and branching; the semantic name of
  runtime-record byte `+0` remains **Unknown**.

### Packed-coordinate projection

The maintained output had reduced coordinate differences to bytes and emitted
impossible-looking complements and decimal constants. The assembly keeps all
of this arithmetic in 16-bit words:

```text
encodedX = int16(enemyX - cameraX + 0x001A)
encodedY = int16(enemyY - cameraY + 0x000C)

reject same-X-page wrap unless encodedX is 13..39
reject same-Y-page wrap unless encodedY is 0..24

visible when:
    encodedX is 0x008D..0x00A7 (signed)
    encodedY is 0xF080..0x0F98 (signed)
    neither same-page wrap guard rejected it

screenCellX = uint16(encodedX) & 0x007F
screenCellY = uint16(encodedY) & 0x007F
screenPixels = (screenCellX * 8, screenCellY * 8)
```

The unusual ranges are part of the game's packed world-coordinate wrapping and
must not be normalized into ordinary Cartesian bounds until that encoding has
been specified independently.

### Map-cell and overlap lookup

The visible map is 24 cells wide. After removing the left screen margin of 13
cells, the function converts the sprite position to a map-cell index and adds
one cell/row when both the sprite and camera occupy the odd half of a cell. A
four-entry byte table at `DS:032E` selects the terrain-overlap mask from the
relative X/Y parity. Eligible terrain shortens the NPC/infantry sprite by
two rows, or four rows for tile family `0x2x`.

### Corrected Reko artefacts and open items

- Position arrays `3092:4004` and `3092:4036` are indexed directly by the full
  combatant ID and hold words. The current header's consecutive eight-element
  subgroup fields need to become aliases over two 24-word tables.
- Current-frame bytes at `3092:409A` and sprite-family bytes at `3092:D55E` are likewise indexed by
  the full combatant ID; their header representation remains incomplete.
- The current C now preserves signed 16-bit comparison constants explicitly.
- Sol: the later0800 sprite-reader reconciliation corrects all FAR reads and
  temporary header-BYTE edits; deleted non-EGA branches remain intentionally absent.
- Continue at `0800:09FE`, the friendly lance-mech drawing pass.

### Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:074E-09FD`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.c`, unannotated Reko output for
  the same range.

## Review block 3: friendly lance mechs (`0800:09FE-0BC8`)

### Purpose and selection

- Iterates the four friendly lance records at `3092:C724`, using the verified
  mech-record stride of `0x7D` bytes.
- A record is drawable when the first byte of its 16-byte name field is not
  `0xFF`. The maintained C previously multiplied a typed array index by
  `MECH_RecordSize` and compared the address of the name array.
- Surviving mechs are compacted into display slots independently of their
  original lance IDs.
- Confidence: **Verified**.

### Display-slot tables

Each compacted mech slot selects six word values from consecutive tables:

| Address | Maintained name | Exact values |
|---:|---|---|
| `DS:02CE` | `FriendlyMechScreenX` | `200, 200, 224, 176` |
| `DS:02D6` | `FriendlyMechScreenY` | `64, 112, 88, 88` |
| `DS:02DE` | `FriendlyMechMapCellOffset` | `126, 198, 152, 149` |
| `DS:02E6` | `FriendlyMechOcclusionMask` | `1, 1, 4, 4` |
| `DS:02EE` | `FriendlyMechOddCameraXOffset` | `1, 1, 0, 0` |
| `DS:02F6` | `FriendlyMechOddCameraYOffset` | `0, 0, 24, 24` |

These are specifically four compacted friendly-lance display slots. The
tables end at the next known table boundary, and the only reference in the
game's main data segment is the four-record friendly-mech loop at `0800:09FE`.
Enemy mechs are therefore not consumers of `DS:02CE-02FD`.

The assembly adds the correction words directly. Reko's divisions by `0x07AE`
and pointer-valued map index were decompilation artefacts. When camera Y is odd,
the low byte of the overlap mask is XORed with `0x05`; the high byte is retained.

### Sprite and terrain handling

```text
for lanceMechId in 0..3:
    if mechs[lanceMechId].name[0] != 0xFF:
        slot = next compacted live-mech slot
        screenX, screenY = mechScreenPosition[slot]
        mapCell = visibleMapBase + mechMapOffset[slot]
        apply odd-camera map corrections

        spriteIndex = baseSprite[lanceMechId]
                    + mechTypeSpriteOffset[lanceMechId]
        tile = visibleMap[mapCell]

        overlapRows = 0
        if normal BattleTech map and tile/mask request overlap:
            overlapRows = 8
            if tile family is 0x0F or 0x2F:
                overlapRows = 16

        friendlyMechOverlap[lanceMechId] = overlapRows
        temporarily shorten/clip sprite
        draw sprite at the slot's screen coordinates
        restore sprite metadata
```

Mechs use larger overlap depths than infantry because their registered sprite
rectangles are 24 pixels high. On adapter path zero, the alternative drawer's
vertical clip boundary is `screenY - overlapRows + 24`, capped at 200.

### Open items

- Reconstruct the common `3092:39FA` sprite far-pointer table and the two draw
  paths once the retained EGA adapter value is tied to startup selection.
- Give `3092:32AE` a stable name; this pass proves its first four bytes retain
  the terrain-overlap depth for the four friendly mechs.
- Continue at `0800:0BC9`, which projects party infantry and mechs back into
  world coordinates and marks their combatant slots active.

### Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:09FE-0BC8`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.c`, unannotated Reko output for
  the same range.
- [Mech record layout](../data-structures/MECH_RECORD.md).

## Review block 4: rebuild friendly formation state (`0800:0BC9-0D2F`)

### Purpose

After drawing the compacted friendly sprites, this block materializes their
packed world positions around the party anchor and marks their combatant slots
active. It does not move the party anchor permanently: every formation offset
is applied to the same saved `246C:A44B/A44D` position, which is restored after
each unit.

### Signed formation tables

The values are bytes and every assembly read is followed by `CBW`, proving
signed interpretation:

| Address | Maintained name | Exact signed values |
|---:|---|---|
| `DS:3A16` | `PartyInfantryFormationDeltaX` | `0, 0, 0, 1, -1, 1, -1, 1` |
| `DS:3A1E` | `PartyInfantryFormationDeltaY` | `0, -1, 1, 0, 0, -1, 1, 1` |
| `DS:3A26` | `FriendlyMechFormationDeltaX` | `0, 0, 3, -3` |
| `DS:3A2A` | `FriendlyMechFormationDeltaY` | `-2, 4, 1, 1` |

These tables are indexed by compacted live-unit slot, not by original party or
lance record ID.

### State rebuild

```text
anchor = (partyWorldX, partyWorldY)
onFootSlot = 0

for partyMemberId in 0..7:
    if member exists and member.mechAssignment >= 8:
        position = applyPackedDelta(anchor,
                    infantryFormationDelta[onFootSlot])
        friendlyInfantryPosition[onFootSlot] = position
        friendlyInfantryActive[onFootSlot] = true
        restore anchor
        onFootSlot++

friendlyMechSlot = 0
for lanceMechId in 0..3:
    if mech.name[0] != 0xFF:
        position = applyPackedDelta(anchor,
                    friendlyMechFormationDelta[friendlyMechSlot])
        friendlyMechPosition[friendlyMechSlot] = position
        friendlyMechActive[friendlyMechSlot] = true
        restore anchor
        friendlyMechSlot++
```

The concrete destinations are `3092:400C/403E/4072` for compacted party
infantry and `3092:4004/4036/406A` for compacted friendly mechs. These are
subgroup aliases into the all-combatant position and active tables identified
in earlier review blocks.

### Direct helper correction

This call flow proves `0800:191B` accepts two signed 16-bit relative deltas.
Its maintained signature and local names were changed from unsigned `PosX` and
`PosY` to signed `DeltaX` and `DeltaY`. The helper repeatedly changes the packed
world position by one unit and performs bit-7 boundary correction; it is not a
position clamp despite the old `Combat_PositionBracketing` name.

### Follow-up status

The subsequent `0800:191B-19BE` review renamed this helper
`Offset_Packed_Position`. Assembly confirmed the ambiguous maintained constants:
`0xFF` represented signed word `FFFF` (-1), while `0xF0` represented signed word
`FFF0` (-16). The conditional jail-mission sprite pass at `0800:0D30` has also
been reviewed.

### Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:0BC9-0D2F` and
  helper `0800:191B-19BD`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, initialized bytes at
  `3EDB:3A16-3A2D`.

## Review block 5: Mission09 fixed jail-mech overlay (`0800:0D30-0E46`)

### Purpose

While `3092:398E` is set, the exploration renderer adds four instances of the
same sprite at packed world positions `0D13:702C`, `0D17:702C`, `0D1B:702C`,
and `0D1F:702C`. Mission09 sets the flag immediately before its map redraw and
clears it on exit. Its interaction code proves these are the four parked
jail-courtyard 'Mechs: after escaping his cell, Jason climbs their boarding
ladders and tries their activation switches. The third unique 'Mech attempted
starts, and Jason steals it to escape. This is attempt-count behavior, so it is
not tied to one fixed physical 'Mech or courtyard position.

The far pointer used by the draw call is `3092:3C42:3C44`. Since the common
sprite-pointer table starts at `3092:39FA` and each entry is four bytes,
`(3C42 - 39FA) / 4 == 0x92`. This proves the overlay uses the
`MECH_Sprite_COMMANDO`/generic humanoid-mech artwork rather than an unidentified
jail-specific asset. It does not by itself prove that all four chassis are
literal Commandos.

### Coordinate correction

The original computes the projected coordinates before testing visibility:

```text
screenX = (0x0D13 + objectIndex * 4) - cameraX + 0x1A
screenY = 0x702C - cameraY + 0x0C
visible screen window: X 13..39, Y 0..24
then: screenX &= 0x7F; screenY &= 0x7F
pixel position: screenX * 8, screenY * 8
```

The unannotated pseudo-C moved the `+0x1A/+0x0C` operations after the tests and
turned the final `AND 0x7F` operations into additions of masked constants.
It also rendered a sign-extended `0xFF8D` lower bound as positive decimal 141.
The clean `.dis` confirms the intended signed lower bound is `-115`; the
clean `.asm` abbreviates the encoded immediate to `8Dh`.

### Open items

- Reconstruct `3092:39FA` as an explicit array of 16:16 far pointers.
- The clean executable also contains the non-EGA `207F:28EB` draw branch. The
  maintained source deliberately keeps the retained EGA `207F:0377` path.

### Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:0D30-0E46`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.dis`, especially the preserved
  `0xFF8D` immediate at `0800:0DDA`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:0814` and
  `0FDC:0D1D`, which set and clear `3092:398E` during Mission09, plus
  `0FDC:0B06-0C0F`, which handles the four parked 'Mechs.
