# Phase 3E — targeted save-record queries

## Review boundary

This block adds read-only selection and presentation of one character or mech
record from a save. It does not write saves, interpret unknown bytes, or assign
physical limb names to the packed actuator nibbles. Later BTSTATS review verifies
those limb names without changing this command's neutral output contract.

## Commands

```text
InceptionTools dump-character FILE SLOT [--group player|enemy] [--game-dir PATH] [--json]
InceptionTools dump-mech FILE SLOT [--group player|enemy] [--game-dir PATH] [--json]
```

`--group` defaults to `player`. Character slots are `0..7`; mech slots are
`0..3`. Invalid groups and slots are rejected before a record is returned.

Both commands use `SaveGameRecord` through `SaveGameInspector`, so exact save
length validation, verified offsets, complete mech names, tail fields, and
neutral packed-actuator handling are shared with `dump-save`. The existing full
save text format is unchanged because its record lines and the targeted command
lines use the same formatting functions.

## Verification

The synthetic harness passes **68 assertions**. Added cases cover
case-insensitive group selection, verified character and mech file offsets,
targeted text and JSON rendering, invalid group rejection, and mech slot bounds.
No original save bytes are embedded in the tests.
