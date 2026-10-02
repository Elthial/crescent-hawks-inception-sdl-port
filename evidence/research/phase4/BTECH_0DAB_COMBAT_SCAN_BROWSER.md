# `BTECH_0DAB` combat scan target browser

## Reviewed range

- Address range: `0DAB:1467-174B`.
- The next routine, a small status-line renderer, begins at `0DAB:174C`.

## Correct three-word interface

The clean caller at `183B:2704-2718` pushes three 16-bit arguments:

1. the combatant performing the scan;
2. the first combatant ID for the selected side;
3. whether 'Mech targets and detailed 'Mech scans are available.

The side base is `0` for friends and `12` for enemies. The previous pseudo-C
lost two arguments at its call site and exposed only two parameters in the
callee, causing the scanner ID and capability flag to be conflated.

The caller calculates the side base as `12 * menuSelection`. Its capability
flag normally remains one. It is cleared when an on-foot combatant attempts to
scan the enemy side, or when the scanning friendly 'Mech has destroyed sensors.
When the flag is zero, this browser adds four to the side base and therefore
cycles through only the side's eight infantry slots.

The old caller also showed an assignment of value four to that capability
flag. No such store exists: four is only the preceding menu-layout argument.

## Target iteration

Each side occupies twelve consecutive combatant IDs:

| Side | 'Mechs | Infantry |
|---|---|---|
| Friendly | `0..3` | `4..11` |
| Enemy | `12..15` | `16..23` |

The browser advances through this range, skipping targets whose active/on-map
word is zero. It also compares each coordinate word against `FFFF`, the normal
off-map sentinel used by encounter generation. Sol: The systematic audit on
2026-09-17 resolved the abbreviated ASM `FFh` using `.dis` at14B6/14C5, which
explicitly records WORD `FFFF`. The earlier `00FF` claim was wrong; current C
still tests `00FF` and needs an approved transcription correction.

With 'Mech scanning disabled, wraparound returns to `sideBase + 4` rather than
the beginning of the side. Otherwise it returns to `sideBase`.

When the rental-mech Arena word flag at `3092:E48E` is set, every enemy-
side advance above combatant 12 is clamped back to 12. Other combat routines
exclude combatant 13 under this flag, and mission setup installs a Spectator
'Mech there. The scan clamp therefore keeps that non-targetable spectator out
of the browser rather than merely expressing “Jason is alone.” The flag's
event-level name should be finalized when that mission setup is reviewed.

## Summary display

Friendly targets below ID 12 are delegated to the existing friendly combatant
description routine at `0DAB:18E8`.

For enemy 'Mechs, the original address expression is:

```text
3092:C33C + combatantId * 0x7D
```

For IDs `12..15`, this resolves exactly to live `Mechs[4..7]` at
`3092:C918..CA8F`. It is not an access to the destroyed/scratch 'Mech array,
as the old annotation claimed. The browser prints `Enemy` followed by that
record's chassis name.

For enemy infantry, combatant ID minus eight gives infantry record ID `8..15`.
The browser prints the human-health summary, then reads that record's weapon ID
and uses the `0x11`-byte weapon table at `3EDB:2ED8` to print its name.

Enemy summaries also calculate and display the compass direction from the
scanning combatant's packed position to the target's packed position. The
direction result indexes the eight far string pointers at `3EDB:01AA`.

## Scan menu

Every visible target is focused/highlighted through `183B:1774`, after which
the routine offers:

- `Scan`, which advances to the next valid target;
- `Detail Scan`, only for friendly or enemy 'Mechs when capability permits;
- `Done`.

If `Detail Scan` is selected, the routine opens the BTSTATS screen, clears the
exit result, and decrements the target ID. The unconditional loop increment
then returns to the same target, so its summary remains selected after leaving
the detailed display.

On exit it restores the menu shape and default selection and leaves text colour
bright white.

## Dead local state

Two instruction sequences have no effect on current behavior:

- a graphics-adapter-dependent local is initialized to eight, or two for
  adapter zero, and never read;
- a redraw flag is explicitly cleared and never changed, making the final
  conditional menu redraw unreachable.

These are likely remnants of an earlier version of the scan UI. They are
documented rather than treated as gameplay defects.

## Confidence

The signature, call arguments, ranges, record arithmetic, weapon lookup,
direction arguments, menu behavior, and dead locals are directly verified from
the clean assembly. The broader mission name for `E48E` remains intentionally
deferred, but no Astra review is required for this routine.
