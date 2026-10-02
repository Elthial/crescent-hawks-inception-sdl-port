# `BTECH_0DAB` weapon-component salvage

## Reviewed block

- Address range: `0DAB:031F-0429`.
- Parent routine: `Salvage_Armour_Dialog`.
- Scrap-value payout begins at `0DAB:042A`; the separate whole-'Mech salvage
  routine begins at `0DAB:04F9`.

This pass inventories intact weapon critical slots from enemy mech wrecks and
uses them to clear the destroyed bit on matching friendly critical slots. It
runs only for Tech level 3 (`SkillLeveL_Good`) or higher.

## Inventory semantics

The routine scans all 35 critical bytes at enemy record `+0x33..+0x55` for each
of the four word-flagged wrecks. An unmodified byte in the inclusive component
ID range `0x10..0x20` increments a byte-sized counter for that exact ID.

This range covers Small Laser through SR Missile 6. It excludes Kick (`0x21`)
and Heat Sink (`0x22`). A destroyed component has bit `0x80` set and therefore
falls outside the accepted intact range without any explicit mask.

The counters represent salvageable **critical slots**, not complete weapons.
A multi-slot weapon contributes once per intact slot and a damaged friendly
weapon consumes one matching count per destroyed slot repaired. The previous C
misidentified the operation as an ammo/infinite-laser check; the assembly never
compares against `Ammo_Laser_Infinite` (`0xFF`) here.

## Applying salvaged slots

Friendly mech slots `0..3` are visited in order. Empty/destroyed records whose
first name/status byte is `0xFF` are skipped. Critical slots are then visited in
ascending record order. A slot is repaired only when all of these are true:

1. raw component bit `0x80` is set;
2. the masked component ID is within `0x10..0x20`;
3. the corresponding salvaged-slot counter is nonzero;
4. helper `183B:273D` approves the critical slot's supporting location.

Repair is exactly `rawComponent &= 0x7F`, followed by decrementing the matching
byte counter. This makes both mech order and critical-slot order significant
when fewer replacement slots exist than damaged slots.

## Uninitialized SRM-6 bucket

The native stack table is addressed through:

```text
[bp + componentId - 0x34]
```

Component IDs `0x10..0x20` therefore require 17 bytes at `bp-24..bp-14`.
However, the initialization loop clears only 16 bytes at `bp-24..bp-15`.
The final bucket for component `0x20` (SR Missile 6) remains whatever byte was
already present at `bp-14`.

Enemy SRM-6 slots increment that indeterminate byte and friendly destroyed
SRM-6 slots test it. This is an original uninitialized-stack defect, not a Reko
artifact. The annotated pseudo-code marks the missing initialization; the
compiled preservation C instead guards accesses to that unknown byte rather
than invoke host undefined behaviour. That guard is temporary, not native
behaviour. A deterministic C# port will need an explicit compatibility policy:
zero is the safest deterministic value, but it is a bug fix rather than an
exact model of the DOS stack residue.

Sol: a subsequent static caller trace identifies the normal keyboard reader's
saved DS WORD as a concrete overlapping writer candidate. See
[SRM-6 entry trace](NATIVE_SRM6_SALVAGE_ENTRY.md). Intervening interrupts and
demo paths remain unverified; the guard is not removed by that address match.

The component bounds, byte counters, repair gates, and initialization defect
are explicit in the clean ASM and require no Astra confirmation. Anatomical
interpretation of `183B:273D` remains under candidate A-005.
