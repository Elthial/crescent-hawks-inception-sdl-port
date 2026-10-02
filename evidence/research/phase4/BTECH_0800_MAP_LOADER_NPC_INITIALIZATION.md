# `BTECH_0800` map-loader post-load state

## Reviewed block

- Address range: `0800:30BB-3205`.
- Parent routine: `DOS_Load_Map_Files`.
- Companion helper reviewed: `1543:0C72`.
- Function boundary: `DOS_Load_Map_Files` returns at `0800:3205`.

This final loader stage conditionally installs a temporary Starport map patch
and constructs the eight roaming map-NPC routes used by combatant IDs 16-23.

## Temporary Starport map patch

When map 2 (Starport) loads while `3092:D343` says its countdown remains
active, the loader calls `1543:0C72` with argument zero.

The helper has been renamed from the loop-shaped
`While_Less_Than_26_Function` to
`Starport_MapPatch_SaveApply_Or_Restore_0C72`. Its argument is a 16-bit stack
word, not the prior `unsigned char` extrapolation.

The patch covers `0x26` bytes beginning at `246C:1C1D`, which is byte offset
`0x0C00` in the current MTP payload at `246C:101D`:

| Argument | Operation |
|---:|---|
| `0` | Save all `0x26` live bytes to `3EDB:5804`, then replace positions whose byte in `3EDB:2B8E` is nonzero. |
| nonzero | Restore all `0x26` saved bytes from `3EDB:5804`. |

Zeros in the override array mean “leave this position unchanged”; they are not
written into the map. The patch therefore overlays selected tiles while
retaining an exact copy for restoration.

The main-loop countdown restores the saved bytes with argument one when the
timer expires and the player is within the relevant packed-coordinate area.
The owner's hypothesis that this timer represents the wait between training
missions remains plausible, especially in the Starport context, but is not yet
promoted to a verified story meaning. Calls from combat mech destruction and a
building-script path should be reviewed with their surrounding mission flow.

## Overhead-map guard

The Starport patch handling occurs first. The loader then checks
`3EDB:014C` (`OverheadMapActive_014C`). When the overhead screen is active it
returns without changing roaming-NPC state.

This matters because the overhead screen loads map files for presentation. It
must not randomize the routes or erase the positions belonging to ordinary
gameplay merely because the player viewed the map.

## Eight strided roaming-NPC records

For ordinary gameplay the loader walks NPC slots `0..7`. Each record is a
field view at `3092:D390 + slot * 0x1A`, already represented by
`RoamingMapNpcRecordView`:

| Field | Initialization |
|---|---|
| `MovementDelay` (`+09`) | `1` |
| Start waypoint | `Rand_0x00_to_0xFF() & 7` |
| Destination waypoint | `RoamingNpcWaypointLink_3768[start]` |
| `WaypointPair` (`+08`) | `(start << 4) | destination` |
| Destination X/Y (`+04/+06`) | MTP interaction coordinates for `destination` |
| Current X/Y (`+00/+02`) | Same destination coordinates |

The preserved MTP link bytes all contain waypoint values in the range `0..7`.
No mask is applied after the table lookup in the original code.

## Live combatant mirrors

Each NPC slot maps to combatant ID `slot + 0x10`. The loader mirrors its
initial destination into the 24-entry all-combatant world-position tables at
`3092:4004` and `3092:4036`.

It also initializes:

- animation-direction table `3092:396C[combatantId]` to `0xFF`, forcing the
  updater to select an animation direction; and
- sprite/frame table `3092:409A[combatantId]` to `0x10`, the original initial
  sentinel.

The old C treated `397C` and `40AA` as unrelated eight-byte NPC arrays. They
are actually the slices reached by indexing the larger all-combatant tables at
IDs 16-23. Likewise, the apparent parallel arrays at `D390`, `D392`, and so on
are fields in records separated by the explicit `0x1A` stride.

## Suppression on maps 11 and later

If `MapNumber > 0x0A`, the route fields are still initialized, but each NPC's
movement delay and live world X/Y are then cleared. This suppresses roaming
NPCs on the destroyed Citadel and later maps without erasing the route data
that was just constructed.

Maps 1-10 retain the live positions and one-tick initialization delay.

## Porting notes

- Treat roaming NPCs as eight strided runtime records mapped to combatant IDs
  16-23, not as eight ordinary `Infantry` character-stat records.
- Keep the MTP waypoint-link table separate from the coordinate arrays.
- Do not reroll routes during overhead-map rendering.
- Preserve the first-update transition: the loader initially stores the low
  destination, then the one-tick delay lets the updater materialize the high
  start waypoint.
- Model the Starport patch as a reversible overlay with zero meaning “no
  override,” rather than copying its entire array blindly.
- Retain the training-delay interpretation as a hypothesis until the timer's
  initializing mission path is identified.

No Astra review is requested. The record stride, table indices, constants,
branches, and patch-copy behaviour are explicit in the clean assembly. The
remaining uncertainty is story meaning rather than machine-code control flow.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:30BB-3205`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_1543.asm`, `1543:0C72-0CDD`.
- `Btech/BTECH_0800.c`, `DOS_Load_Map_Files` and the main-loop countdown.
- `Btech/BTECH_0800.c`, `Update_Roaming_Map_Npcs_24C2`.
- `Btech/BTECH_1543.c`, reversible map-patch helper.
- Original ignored `MAP1.MTP` through `MAP14.MTP`, waypoint-link bytes only.
