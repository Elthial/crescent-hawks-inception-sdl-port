# `BTECH_0DAB` whole-mech installation

## Reviewed block

- Address range: `0DAB:070D-07B4`.
- Parent routine: `Salvage_Mechs_Dialog`.
- The rejected-wreck message begins at `07B5`.

After bytes `+01..+7C` have been copied into an empty player-mech record, this
block restores its identity and makes the recovered wreck minimally operable.

## Identity and pilot

`Name[0]` is restored from `DestroyedMechNameInitial_323E[SelectedMechId]`.
The eligible pilot's **party slot**, not their character-name ID, is then written
to `Mech.PilotId` at record offset `+0x79`. The same player-mech slot is written
to that party record's `Infantry.Piloting` byte at offset `+0x0C`.

The old C shadowed the party-slot variable with the character-name ID used by
the prompt. Keeping those values separate is essential here.

## Sprite family

The player combatant's sprite-family offset at `3092:D55E + playerMechSlot` is
initially set to `0x00` (Locust). If the restored first name character is not
ASCII `L` (`0x4C`), it is changed to `0x92` (Commando). The executable therefore
identifies the chassis graphic solely from the first character of its name.

## Forced salvage damage

The copied wreck is modified as follows:

- `EngineHits` (`+0x75`) is set to `1`;
- `GyroHits` (`+0x76`) is set to `1`;
- `CurrentStructure[3]` (`+0x1F`) is raised from `0` to `1` if necessary;
- `CurrentStructure[4]` (`+0x20`) is raised from `0` to `1` if necessary.

All other copied armour, structure, ammunition, and critical damage remains.
The precise anatomical names for structure indexes 3 and 4 remain under Astra
candidate A-005, so the annotations retain the proven numeric indexes.

Finally, the player-slot loop is forced past its upper bound, leading to the
post-candidate state checks at `080E`.

No new Astra review is required for this block.
