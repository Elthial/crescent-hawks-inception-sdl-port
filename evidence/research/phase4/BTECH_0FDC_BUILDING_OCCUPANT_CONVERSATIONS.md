# `BTECH_0FDC` building-occupant conversations

## Review boundary

This block covers the complete function at `0FDC:17B9-19E0`, renamed
`Talk_To_Building_Occupants`. It implements the `Talk to others` option used by
the Citadel and several shops and public buildings.

The function has no arguments. Its activity category is supplied through BLD
state byte `3092:D319` immediately before action-dispatcher command `0C` calls
the routine.

## BLD-supplied activity category

The shipped scripts establish these writes immediately before the call:

| BLD file | D319 value |
|---|---:|
| `TRAINING.BLD` | `00` |
| `CITADEL.BLD` | `01` |
| `COMSTAR.BLD` | `02` |
| `WEAPON.BLD` | `03` |
| `ARMOR.BLD` | `04` |
| `REPAIR.BLD`, `HOSPITAL.BLD` | `05` |
| `BARRACKS.BLD` | `06` |
| `LOUNGE.BLD` | `07` |
| `ARENA.BLD` | `0A` |

The same value selects a phrase from the 11-entry table at `3EDB:4E32`, such
as what an NPC stopped in the building to do. The former name
`RandomConvoVariable` was therefore misleading; no random value is assigned
here.

## Finding occupants

The routine searches only the first eight bytes of the current MTP map's
alternate-BLD table at `3092:4602` for the D319 category. The resulting index
is a building/waypoint slot. If no match exists, the index reaches eight and no
NPC can match because route waypoints are three-bit values.

It then examines the eight roaming-NPC records at `3092:D390` with stride
`0x1A`:

- `MovementDelay != 0` means the NPC is waiting off-map in a building;
- `WaypointPair >> 4 & 7` is that current building waypoint;
- `WaypointPair & 7` is the next/destination building waypoint.

NPCs waiting at the current building slot are placed in a compact menu. Their
names come from the MTP file's eight fixed 16-byte character-name slots. If
none qualify, the game reports that nobody is interested in talking.

## Selection and conversation

The menu contains each qualifying name followed by `Done`. Its returned index
is compact, so the routine scans the eight slot flags again and decrements the
choice only for qualifying NPCs to recover the selected route record.

While the Citadel remains intact, the response has three dynamic parts:

1. a fixed greeting;
2. the D319-indexed reason the NPC stopped in this building;
3. the MTP building name selected by the NPC route's low destination nibble.

After the Citadel has been destroyed (`D310 != 0`), the selected occupant
instead gives one of eight random dismissive replies from the far-pointer table
at `3EDB:4E5E`. The old pseudo-C had this state test reversed and incorrectly
indexed that pointer table with a value shifted twice.

## Rick Atlas lounge path

The story identity and handshake are established by the shipped scripts:

1. after Rick Atlas invites Jason to meet him in the lounge,
   `TRAINING.BLD` sets persistent-state index `2D` (`D339`) to one;
2. the roaming updater uses that byte to hold NPC slot zero, Rick, at his
   current waypoint;
3. after `Talk_To_Building_Occupants` returns, `LOUNGE.BLD` tests state index
   `2E` (`D33A`);
4. a true result enters the Rick lounge scene, then the script clears both
   `D339` and `D33A`.

Before ordinary conversation, the executable has a special path requiring all
of the following:

- `HoldRickAtlasUntilLoungeConversation_D339` is nonzero;
- the waypoint retained from the **last waiting NPC scanned** is seven;
- the first compact menu choice was selected;
- NPC slot zero is one of the occupants in this building.

It sets `3092:D33A`, clears Rick's slot-zero movement delay, and returns without
displaying dialogue itself. The calling lounge script observes the flag and
owns the actual conversation text. Clearing the delay also releases Rick into
the travelling state used by the roaming-NPC updater.

The dependence on the last waiting NPC is explicit in both the segmented and
expanded assembly, but its source-level intention is unclear. It is preserved
exactly and recorded as Astra candidate A-007 rather than silently changed to
the current or selected building.

## Corrected transcription errors

The former pseudo-C:

- tested the address of `Infantry_OnMap` instead of the strided `D399`
  movement-delay byte;
- treated the strided `D398` waypoint byte as a flat array;
- lost the compact-menu-to-NPC-slot selection step;
- assigned zero to an entire array where the executable clears only `D399` for
  NPC slot zero;
- reversed the intact/destroyed-Citadel response branches;
- doubled the index into a C array of already-sized far pointers;
- represented a selected text pointer as an invalid unsized local array.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:17B9-19E0`;
- duplicate segmented image at `BTECH_code_0000.asm:0CA09-0CC30`;
- all shipped BLD `SET_STATE_BYTE 0D` plus action-dispatcher `0C` pairs;
- MTP header fields and map-specific alternate-BLD bytes;
- roaming-NPC initialization and update flow at `0800:24C2-2866` and
  `0800:30E2-3204`;
- text and far-pointer tables at `3EDB:16DE-1790` and `3EDB:4E32-4E7D`.

The menu construction, route-field meanings, ordinary responses, destroyed
branch, and exact special-path instructions are verified. Only the intended
story semantics of the unusual special condition remain open.

The next reviewed block contains the
[`stock suffix and BLD immediate-word reader`](BTECH_0FDC_TEXT_AND_WORD_HELPERS.md).
