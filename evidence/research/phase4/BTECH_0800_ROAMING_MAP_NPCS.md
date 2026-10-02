# `BTECH_0800` roaming map-NPC updater

## Review block (`0800:24C2-2866`)

The routine formerly named `CharacterPos_24C2` has been renamed
`Update_Roaming_Map_Npcs_24C2`. It updates the eight map-specific roaming NPCs,
which occupy combatant IDs `16-23`; it does not update the player party.

The main loop calls it every third processed world tick. The mission loop at
`0FDC:0964` keeps its own WORD phase at `DS:5802` and calls the updater once per
four shared mission-world updates.

## Initialization by the MTP loader

`DOS_Load_Map_Files` initializes these same records at `0800:30E2-3204` when
the overhead-map screen is not active. For each slot it selects a random start
waypoint `0..7`, obtains the linked destination from MTP file bytes
`0x215-0x21C`, and stores `(start << 4) | destination` in `WaypointPair`.

Both the strided record and live combatant position initially receive the
destination coordinates. `MovementDelay` starts at one; on the first updater
call it reaches zero and the updater materializes the combatant at the packed
high/start waypoint. Animation-direction byte `0xFF` forces refresh and sprite
value `0x10` is the original initial sentinel.

Maps `0x0B` and later still receive route records, but their delay and live
combatant positions are cleared so roaming NPCs remain suppressed. See
`BTECH_0800_MAP_LOADER_NPC_INITIALIZATION.md` for the complete loader stage.

## Strided NPC field view

The code multiplies the NPC slot by `0x1A` and accesses these fields from
`3092:D390`:

| Relative offset | Width | Meaning |
|---:|---:|---|
| `+00` | word | Stored/current packed X position |
| `+02` | word | Stored/current packed Y position |
| `+04` | word | Current movement destination X |
| `+06` | word | Current movement destination Y |
| `+08` | byte | Packed start/destination waypoint pair |
| `+09` | byte | Movement-delay countdown |

Only these first ten bytes are claimed by the new `RoamingMapNpcRecordView`.
The remaining `0x10` bytes in each stride are still unclassified, so the view
is applied manually at `D390 + slot * 0x1A` rather than declaring eight full
records and accidentally claiming overlapping state.

## Delay and waypoint routing

While the delay is nonzero, it is decremented and the NPC does not move. When
it reaches zero, the high nibble of the waypoint-pair byte selects one of the
16 map interaction-coordinate entries where the NPC is materialized. The
destination words at `+04/+06` already contain the low-nibble waypoint's
coordinates, so movement then runs from the high waypoint to the low waypoint.

After the NPC arrives:

1. a new delay of `randomByte & 0x1F` is stored;
2. its live combatant position is cleared;
3. the reached low waypoint is shifted into the high/start nibble;
4. the low three bits select an entry from the map-file table at `3092:3768`;
5. that linked waypoint becomes the new low/destination nibble and its
   coordinates are stored at `+04/+06`.

`3092:D339` has a narrow, verified role: while nonzero it continually reloads
slot zero's delay before decrementing it. This holds Rick Atlas at his current
waypoint after his training-school invitation until Jason selects him in the
lounge's occupant menu.

The building conversation routine uses the same route state from the opposite
side. A nonzero movement delay means an NPC is waiting off-map at the
high-nibble waypoint and can appear in that building's `Talk to others` menu.
The low nibble supplies the destination building named in the NPC's response.
See
[`BTECH_0FDC_BUILDING_OCCUPANT_CONVERSATIONS.md`](BTECH_0FDC_BUILDING_OCCUPANT_CONVERSATIONS.md).

## Viewport, movement, and animation

The routine temporarily borrows the global packed camera coordinates. Using
`Offset_Packed_Position`, it constructs wrapped boundaries one `0x10` step
beyond the visible region. An NPC is moved and animated only when its combatant
position is strictly inside all four boundaries.

For an included NPC, signed 16-bit wrapped deltas are converted to screen
coordinates using X wrap `0x80`, Y wrap `0x0F80`, and screen origins `0x1A`
and `0x0C`. This corrects the old transcription's widened unsigned locals and
several assignments of screen constants back into world positions.

`Position_0006` receives six 16-bit arguments here: combatant ID, destination
X/Y, screen X/Y, and `TRUE` to enable its periodically alternating direction-
search order. Reko omitted the sixth parameter from its C signature and
represented it as an uninitialized local; the signature and all three known
callers have been corrected.

The movement direction at `3092:3920[combatantId]` selects the appropriate
infantry animation stream when it differs from the current animation direction
at `3092:396C`. The returned frame from `0800:1732` is stored at
`3092:409A[combatantId]`.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:24C2-2866`.
- Map-NPC initialization in `0800:30E2-3204`.
- Movement helper call and six pushed words at `0800:26DA-2720`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_1631.asm`, which reads its sixth word
  argument at `[BP+10]`.

The routine and recovered field meanings are explicit in the assembly and do
not require Astra confirmation. The unused bytes in each `0x1A` stride remain
an ordinary open layout question.
