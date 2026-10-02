# Phase 3D — canonical record and save models

## Review boundary

This block replaces duplicated and drifting parsing logic for character, mech,
weapon, and save records. It does not add save writing or assign meanings to
unknown bytes.

The reusable APIs are:

```text
CharacterRecord.Parse(bytes)
MechRecord.Parse(bytes)
WeaponRecord.Parse(bytes, tableIndex)
SaveGameRecord.Parse(bytes)
```

Every parser requires the exact verified record length and retains a copy of
all source bytes. `SaveGameRecord` additionally retains the complete
`0x0F44`-byte state block, so fields not yet understood are not lost. Byte-array
properties return defensive copies and parsed record collections are read-only;
future editing will therefore require an explicit writer rather than silently
mutating the inspection model.

## Actuators

The canonical mech model exposes two neutral packed values:

- `ActuatorByte24`, paired with maximum byte `+0x69`;
- `ActuatorByte25`, paired with maximum byte `+0x6A`.

Each exposes raw current/maximum values and independent low/high nibbles. At the
time of this phase, no left/right or arm/leg labels were assigned. Later review
of BTSTATS at `0DAB:1F86..21BE` verified that byte `24` is left, byte `25` is
right, low nibbles are legs, and high nibbles are arms. The neutral model remains
lossless; the old four-entry scalar dictionary remains only as an obsolete
compatibility property and is no longer used by `ToStats`.

## Legacy transition

- `Infantry` delegates to `CharacterRecord`; the former unknown fields now have
  canonical `MechAssignment` and `TrainingFlags` properties. Old property names
  are obsolete aliases.
- `Mech` delegates to `MechRecord`, reads all 16 name bytes, exposes the eight
  tail fields, and directs actuator consumers to the neutral packed model.
- `Weapon` delegates to `WeaponRecord`, reads all 11 name bytes, and exposes
  packed semantics through its canonical `Record`. Ambiguous legacy properties
  remain obsolete aliases.
- The old sequential `SaveFile` implementation has been replaced with a thin,
  obsolete path wrapper over `SaveGameRecord`; its guessed blocks and incorrect
  `file+0x0F44` position read no longer execute.
- Save and weapon CLI inspectors now adapt canonical records to their existing
  text/JSON output, so command output remains stable.

## Verification

The harness passes **62 assertions**. New coverage includes exact record-size
rejection, complete 16-byte mech names, character assignment/training flags,
lossless full-save bytes, all record counts, mech tail fields, canonical weapon
semantics, distinct current/maximum actuator nibbles, and defensive byte-array
views.
