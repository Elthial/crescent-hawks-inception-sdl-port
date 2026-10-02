# Sol: complete combat execution parent C conversion

`CrescentHawksInception/src/Original/BTECH_1AE8_MECHANICS.c` translates the complete
annotated `1AE8:000C–12C6` method rather than inventing a turn engine. It executes
twelve interleaved movement/fire slices for 24 combatants, consumes signed step
pairs, checks occupancy/crushing, updates animation cursors, resolves one eligible
weapon per actor per slice, and updates completed orders and final Mech heat.

Attack handling retains the original range/path gates before ammunition use,
proficiency byte-counter rollover, seven contiguous character skills, movement,
sensor and heat penalties, native narration, personnel damage dice/repeats and
mutating armour absorption, Mech hit-location/cluster rolls, raw-record damage
transfers, criticals, ejection, fired bits and effects handoff. Shared kick damage
is still mutated from tonnage. Original bugs remain, including ammo ordinal 10
aliasing walk movement and kick actuator lookup using combatant IDs rather than
the computed Mech record ID. Mechanics' missed-laser fire predicates retain
native table indices 16–18; unlike the effects ranges, they were already correct.

Global storage added for native movement cursors at `3092:0078` and the 576-byte
saved tile cache at `3092:4314`. Parity/previous-tile globals were moved from the
renderer translation unit to existing combat data; their types do not change.
Geometry, row strides, combatant ranges, movement slices and heat thresholds use
the existing named constants where confirmed. Skill-byte traversal uses the
whole character record's byte view, not pointer arithmetic beyond one member.

## Remaining native stack contract

BP-56 attack-result state is assigned on ordinary hit/miss paths, and persists
between attacks during the invocation. Some Mech-target attacks skip assignment.
The C routine tracks whether it has an assigned value. Before the first assignment,
visible effects that would consume the value explicitly stop with a diagnostic.
This temporary guard is NOT native behaviour and NOT a recovered entry value.
The full method is therefore not certified on all visible-combat workflows.

Disabled-graphics/dead-party entry to the effects routine does not read that
argument. Passing its placeholder on those paths has no observable effect;
that does not justify defaulting the native flag to false for visible effects.
No residue is fabricated or silently carried from an unrelated host C stack.

Sol: rechecked09F1/0A05 against the ASM: bit80 personnel weapon encodings
skip damage and every BP-56 assignment against a Mech, reaching the0CD3
handoff. Effects1A07 tests that WORD and1A10 plays sound4. Added paired
laser-only/laser-then-Pistol controls for both assigned hit and assigned miss.
Final Mech armour matches each laser-only control; the later Pistol inherits
the earlier attack's impact sound decision. This is preserved BUG-023, not a
reason to reset the flag between attacks. These tests use actual execution
and effects bodies with controlled RNG and isolated presentation/audio. They
do not supply the unknown first-attack entry value or certify emulator parity.
Checkpoint: all116 headless and156 SDL/local tests pass; the expanded execution
scenario also passed ten repeat runs. No original attack behaviour changed.

`1631:1122` damage transfer is also converted. All mapped offsets match the ASM
jump table. Fatal offsets 1F/20 set the destruction flag but natively return an
unassigned WORD. The only original caller immediately ejects and checks the
Mech's destroyed name before another damage iteration, never dereferencing that
returned offset. The C helper returns an unobservable placeholder on this path,
not a claimed native AX. Invalid offsets explicitly stop and remain unsupported.

## Verification and limits

`gameplay.original_combat_execution` runs actual execution/effects, dice, range,
terrain-path, occupancy, packed movement, animation, critical dispatch, ejection,
damage transfer and heat code. Controlled random BYTE inputs exercise:

- Pistol damage dice and armour, hit/miss and equality-at-target-number.
- Proficiency rollover and the correct skill update.
- Successive armour halving across four SMG repeats and personnel death cleanup.
- Mech ammo consumption, damage location, enemy-record normalization, shutdown
  heat and out-of-range rejection without ammo use.
- Fatal structural overflow through real ejection and wreck registration.
- Disabled-graphics attack with unknown entry flag and one real movement step.
- All mapped raw damage-transfer offsets and fatal destruction flags.

Map-cache production, camera cache movement, rendering/audio and UI are isolated
test boundaries. This is not a full-map NPC-planning combat simulation, emulator
comparison, or visual/audio timing certificate. Native-valid table/storage
indices remain contracts; corrupt pointers/indices are not sanitized or emulated.
111 headless and 130 SDL/local-asset tests pass. Real executable linkage now has
three missing original parents: salvage, Mech-stats screen and weapon selection.
No production no-op stubs or copyrighted external assets were added.
