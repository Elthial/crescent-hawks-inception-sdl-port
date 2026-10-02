# Phase 2C — structured save, character, and mech dump

## Review boundary

This block adds a read-only structured view of verified save offsets. It does
not use the old `SaveFile` or `Mech` parsers, edit a save, resolve character-name
tables, or attach semantics to unknown flags.

## Why a new reader is used

At the time of this Phase 2 block, `InceptionTools/FileTypes/SaveFile.cs` parsed
the leading records correctly but its later guessed block lengths drifted. It
read a four-byte `Position` at `file+0x0F44`, one byte early. The legacy
`InceptionTools/Class/Mech.cs` read the correct two stored actuator bytes, but
duplicated each complete packed byte into arm and leg scalar values, losing the
independent low/high-nibble states.

Phase 3D subsequently replaced those parsing paths with canonical lossless
record models. A later BTSTATS review verified byte `24` as left, byte `25` as
right, low nibbles as legs, and high nibbles as arms; the canonical model keeps
its neutral byte/nibble names for lossless compatibility.

The Phase 2 reader instead uses the offsets documented from the original load
and save operations:

- sixteen `0x11`-byte character records at `file+0x0001`;
- four player mech records at `file+0x0111` and four enemy mech records at
  `file+0x0305`, all with `0x7D` stride;
- finance dwords beginning at `file+0x0D5D`;
- separate little-endian position words at `file+0x0F45` and `file+0x0F47`.

## Command

```text
InceptionTools dump-save GAME1 [--game-dir PATH] [--json]
```

The command requires the recognized `0x0F49` file length and performs no writes.
It emits offsets with each character and mech so every value can be checked in
the raw inspector. Byte arrays are JSON number arrays rather than encoded text.

Character output includes the three attributes, seven skills, weapon-table
index, mech assignment, armour, health, and training flags. Mech output includes
the complete armour, structure, ammo, critical-slot, occupancy, damage-counter,
and upgrade bytes.

Fields whose semantics remained probable at this phase were named accordingly
or by neutral byte offset. The actuator byte/nibble mapping is now verified;
byte `78` remains probable life-support state,
byte `7B` probable upgrade-package base, and the probable world-map visibility
bit count. State byte `file+0x0CF9` is likewise labelled by address rather than a
specific mission name.

## Verification

The dependency-free harness now passes **27 assertions**. Its synthetic save
places distinct values at the first character, first mech, finance, tail-field,
and final position offsets and verifies that each is returned from the expected
record.

A read-only dump of ignored `chinception/GAME1` reports the expected `0x0F49`
length, header `0x0C`, 16 characters, 8 mechs, and final position words at the
correct offsets. No save contents or generated dump were committed.
