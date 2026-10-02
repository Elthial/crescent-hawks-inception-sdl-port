# Phase 2D — structured weapon-table dump

## Review boundary

This block adds a read-only structured view of the 33 verified weapon records
captured from the expanded executable. It does not modify the legacy `Weapon`
class, extract external assets, infer the unknown heat/effect high nibble, or
implement combat.

The captured bytes are permitted executable-resident data. External game files
and extracted assets remain excluded from the repository.

## Command

```text
InceptionTools dump-weapons [--json]
```

Each record retains its complete 17 raw bytes and reports:

- the full fixed-width 11-byte name;
- table index and the verified infantry-equipment or mech-component ID domain;
- damage encoding and attack-count/cluster selector;
- personnel dice and fixed-bonus fields where bit `0x80` selects that encoding;
- the verified low heat nibble and the unresolved high nibble separately;
- packed, stored, and effective range thresholds;
- maximum range and skill index.

The text output is intended for quick comparison. JSON is the lossless
structured interchange form and includes all raw record bytes.

## Verification

The dependency-free harness now passes **34 assertions**. It checks the table
shape, full eleven-byte names, personnel repeated-attack encoding, the
table/component ID conversion, mech heat and range scaling, a missile cluster
selector, and preservation of the unresolved heat/effect high nibble.

The canonical field evidence remains in
[`docs/data-structures/WEAPON_RECORDS.md`](../data-structures/WEAPON_RECORDS.md).
