# `BTECH_0DAB` heat-sink salvage

## Reviewed block

- Address range: `0DAB:0259-031E`.
- Parent routine: `Salvage_Armour_Dialog`.
- Weapon salvage is gated at `0DAB:031F` and begins at `0328`.

This pass counts intact heat sinks in enemy wrecks and restores destroyed heat
sinks in the four player-lance mechs. It runs for Tech level 2
(`SkillLevel_Average`) or higher.

## Counting enemy heat sinks

For enemy mech indexes `0..3`, the routine checks the same four word-sized
wreck flags previously reached as `3092:3954 + index * 2`. These are the enemy
interior entries of the combatant-indexed word table at `3092:393C`, so the
maintained C uses combatant IDs `0x0C..0x0F` explicitly.

Each flagged enemy's complete 35-byte critical-component block at record
offsets `0x33..0x55` is scanned. Every byte exactly equal to `0x22`
(`Heat_Sink`) increments a 16-bit counter. Destroyed heat sinks (`0xA2`) are not
counted because the comparison does not mask the destroyed high bit.

## Restoring friendly heat sinks

The routine scans friendly mech slots `0..3`, skips records whose first
name/status byte is `0xFF`, and examines all 35 critical slots in ascending
record order. A slot is restored from `0xA2` to `0x22` only when helper
`183B:273D` returns nonzero for that mech and raw critical-slot offset.

That helper maps the critical-slot group to an internal-structure byte and
normally returns the current value of that structure location. It has a
special case which returns one whenever mech byte `+0x7B` equals `0xC8`.
The special value's gameplay purpose and the current anatomical side labels
need a separate focused review; the heat-sink pass itself calls the helper
without interpreting the result further.

## Original counter-underflow quirk

The enemy heat-sink count does **not** limit repairs. After each restored
friendly heat sink the executable executes an unconditional:

```text
DEC word ptr [bp-2]
```

There is no preceding zero test and the count is not used again by this pass.
Consequently an Average-or-better technician repairs every qualifying destroyed
heat sink even if no intact enemy heat sink was counted. In that case the word
wraps from `0x0000` to `0xFFFF`.

This appears to be an original-game accounting bug or abandoned resource
constraint, not a decompiler error. A preservation port should reproduce it by
default and treat any resource-limited alternative as an explicit gameplay fix.

The control flow and underflow require no Astra confirmation. A narrow Astra
candidate has been recorded separately for the conflicting anatomical mapping
between critical-slot groups and internal-structure bytes.
