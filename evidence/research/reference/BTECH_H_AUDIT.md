# `BTECH.h` consolidation audit

## Purpose

`Btech/BTECH.h` is the historical 1,860-line research scratch pad. It remains
unchanged. This audit records which parts have a maintained home, which notes
are deliberately deferred to later workflow analysis, and which scratch-pad
claims must not be copied without fresh evidence.

Confidence notation follows [the research method](../RESEARCH_METHOD.md).

## Coverage

| Scratch-pad area | Approximate lines | Maintained destination | Phase 1 status |
|---|---:|---|---|
| ASCII, DOS, adapter, register, palette, screen and timing constants | `5..115` | [DOS hardware and input](DOS_HARDWARE_AND_INPUT.md) | Covered; questionable buffer constants remain explicitly unresolved. |
| Sound-effect IDs | `116..136` | [Asset and game identifiers](ASSET_AND_GAME_IDS.md) | Indexed as historical semantic labels; call-site verification deferred. |
| Movement commands and menu settings | `138..160` | [DOS hardware and input](DOS_HARDWARE_AND_INPUT.md) | Raw values retained; semantic directions are P pending remapper tracing. |
| Mech constants, template indexes and `0x7D` record offsets | `162..320` | [Mech record](../data-structures/MECH_RECORD.md) | Consolidated with assembly corrections and confidence labels. |
| Upgrade IDs, weapon/component IDs and range brackets | `321..405` | [Mech record](../data-structures/MECH_RECORD.md), [weapon records](../data-structures/WEAPON_RECORDS.md) | Consolidated; limb/nibble ownership is verified, while individual actuator-bit meanings remain open. |
| Character IDs, skills and medical equipment | `406..429` | [Character records](../data-structures/CHARACTER_RECORDS.md), [asset and game identifiers](ASSET_AND_GAME_IDS.md) | Consolidated; training-flag bits `2..7` remain open. |
| Map coordinates, tiles and mission labels | `430..504` | [Asset and game identifiers](ASSET_AND_GAME_IDS.md), [MTP maps](../formats/MAPS.md) | IDs are indexed; coordinate ownership and mission semantics are deferred to Phase 4 map/story workflow review. |
| Animation, BLD, MTP, tileset and sprite IDs | `505..607` | [Asset and game identifiers](ASSET_AND_GAME_IDS.md), [format index](../formats/README.md) | Indexed with uncertain scene descriptions labelled H/U. |
| `Infantry`, `Mech`, and `Weapon` typedefs | `608..675` | The three data-structure documents | Replaced as authority by byte-offset tables based on stride and instruction evidence. |
| Segment pseudo-structures | `677..1656` | [Segment map](../memory/SEGMENTS.md), [3092 state](../memory/SEGMENT_3092_STATE.md), [type and alias rules](../memory/TYPE_AND_ALIAS_RULES.md) | High-value ranges normalized. The pseudo-structures remain an address catalogue, not compilable layouts. |
| Trailing free-form investigation notes | `1658..1860` | Structure documents, [contradictions](../investigations/CONTRADICTIONS.md), and Phase 4 queue below | Reviewed and routed; unresolved workflow claims are not promoted to facts. |

## Historical discrepancies not to propagate

- The scratch pad still contains `KEY_B = 0x42` and `KEY_W = 0x57`.
  The project-owner correction is `KEY_B = 0x41`; `KEY_W` is removed. This
  makes `KEY_B` a game-action label rather than evidence for ASCII letter B.
- `SCREEN_RES_Y = 119` has no retained reference and may belong to deleted
  non-EGA code or a 120-line viewport.
- `MECH_REF_UrbanMech = 0x08` conflicts with the eight-record template table;
  its verified zero-based table index is `0x07`.
- The `Mech` typedef expands each actuator field to four bytes. Assembly and
  the `0x7D` stride prove one packed byte at `+0x24` and one at `+0x25`.
- The `Weapon` typedef and old `2ED2` comments do not describe the verified
  `3EDB:2ED8` table with `0x11` stride.
- `InfantryValueArray_C60F` is not an independent party array. Those addresses
  are fields of the preceding `0x11`-byte character record.
- Reko-generated `int32` fields and pointers in segment pseudo-structures do
  not establish original widths. See the 16-bit type rules before reuse.

## Routed Phase 4 investigations

These useful notes require code-flow analysis rather than further Phase 1
header transcription:

1. overhead-map and map-position ownership;
2. Mech-Lube repair and upgrade workflows;
3. the `3EDB:5384..5722` segmented-pointer region;
4. cache/HPG state at `D34A` and `D34C`;
5. combat position/visibility arrays near `4004..407A`;
6. the probable `0x1A`-byte combatant working record at `D394`;
7. combatant-ID partitioning between friendly/enemy infantry and mechs.

This routing completes the Phase 1 scratch-pad coverage audit without claiming
that these later workflows are already understood.
