# Native residual storage and preservation C

Sol: this is an implementation decision record, not completed functionality.
The original methods are converted and the executable links. The initial
strict-stack proposal below is now narrowed by the user's instruction that
replicating incidental BIOS behaviour is excessive. No game VM is proposed.

## Portable presentation decision

Sol: the C statistics parent now deliberately starts its palette redraw
counter and phase at0 rather than emulating inherited BIOS stack words.
This is a documented presentation normalization, not a recovered assignment
in the original binary. It removes the normal statistics-screen abort.
The sixteen-redraw comparison, masking, apply-before-patch ordering, original
palette tables,600-redraw auto-dismiss, input consumption and palette restore
remain. Record reads, heat clamping and other game-state writes are unchanged.
The original annotated pseudo-code still records the native unassigned locals.

The existing parent test retains224 first-frame probes and now also runs its
actual continuation for auto-dismiss and interactive return. Presentation and
asset boundaries are isolated: this is not a visual playthrough certification.
Gameplay-affecting unknown salvage/attack state is not defaulted by this decision.
Here "attack state" means damage/rules state, not the effects-only flag below.

Sol: the second presentation exception initializes `Combat_Mechanics`' entry
attack-applied flag toFALSE. The high-bit personnel-weapon-versus-Mech branch
can skip its native assignment, but reads the flag only through the effects
parent's impact sound/animation decision. It still applies no damage. An
unassigned first use therefore presents no invented hit impact instead of
aborting. After actual hit/miss assignments the same-call carry remains;
do not reset per actor, weapon or slice. BUG-023's assigned-result inheritance
remains covered for both preceding hit and preceding miss. The existing
execution test now also runs the previously stopped first-use branch with
graphics enabled and confirms no hit-impact sound or target damage.

Sol: connected validation also runs `test_connected_combat mech-statistics`
against the user's local original assets. Real startup supplies the font and
graphics resources; the actual statistics method loads/decompresses the shipped
BTSTATS.CMP, draws through EGA/SDL, completes automatic dismissal and restores
the palette. A test-only SDL Enter event acknowledges its final native key read.
The Mech record, starting heat10 and C-bills1234 remain unchanged. No original
method is doubled. SDL uses dummy video/audio and accelerated test retraces, so
this confirms connected loading/return, not visual accuracy or native pacing.
All159 SDL/local-asset tests and the preservation-game link pass.

Sol: a local BMP preview from that connected fixture was inspected. The Wasp
artwork, type/tonnage/pilot/rider labels, two weapon entries, armour/structure
gauges, heat gauge and actuator statuses are visibly present and legible.
This preview is actual SDL EGA scanout **after default-palette restoration**,
not an in-loop palette-animation capture or pixel comparison with the original.
It is an initial visual sanity check, not playthrough certification. The optional
test command accepts a private BMP destination as its third argument; ordinary
tests do not export artwork. The preview remains under ignored chinception.

From the original-asset directory, after building, an example private export is:

```powershell
& '../Reimplementation/build/test_connected_combat.exe' mech-statistics './validation/c17-mech-statistics-preview.bmp'
```

Use dummy SDL drivers for unattended runs. Windows sandbox restrictions may
require approval for the preview file write; failures report the SDL error.
The strict contract below remains relevant where residual reads affect rules,
data or observable original bugs; BIOS-stack replication is not a blanket gate.

## What the original binary actually preserves

An unassigned native local reads bytes left in the DOS stack by previous
calls or interrupts. Releasing a frame changes SP; it does not erase memory.
Recreating an uninitialized C local on a modern host is not equivalent: that
read has undefined behaviour, and the host call frames have different layouts.
Neither clearing every local nor copying a single emulator witness preserves
the original dependency.

The captured combat Scan path proves the distinction. Its statistics counter
inherits the keyboard routine's saved SI,0030. Its palette phase inherits BIOS
interrupt flags,0247. The previous witness had0047. The counter's first native
transition is0030 ->0031 ->0001, with comparison before masking. Both phase
words happen to share low two bits, but that is not a universal input contract.
The pinned emulator's keyboard-intercept callback also changes the saved carry
flag. A platform-specific BIOS effect must not be labelled a tabletop rule.

Evidence: [final write watchpoints and native return](NATIVE_MENU_INPUT_COMPARISON.md),
[earlier entry witness](NATIVE_STATS_ENTRY_WITNESS.md).

## Required storage contract

Any faithful explicit representation must satisfy all of these conditions:

- BYTE/WORD reads refer to retained native storage with16-bit offset wrapping,
  not host pointers or modern local variable addresses.
- Allocating or releasing a native frame does not initialize or clear it.
- Writes retain width, order and caller-relative location. Returning from a
  callee does not discard the retained bytes.
- Known state comes from actual preceding writes or an explicitly identified
  capture/replay input. Unknown bytes remain unknown until written.
- Hardware/BIOS contributions belong in the platform layer, not invented game
  rules. Their provenance must distinguish original hardware from emulator
  behaviour, including the optional keyboard-wait correction.
- C game methods remain the execution engine. Modelling observable storage
  must not replace them with an instruction interpreter or a second game.
- Instrumentation must cover every writer of an observed slot, including
  intervening drawing calls and interrupt frames. Modelling only the final
  menu's saved SI is insufficient to establish another caller's footprint.

## Boundaries still requiring this contract

| Native read | Observable dependency | Current handling |
| --- | --- | --- |
|0DAB:1AFE BP-22/BP-2C|First palette delay and phase; both Inspect and combat Scan callers|Documented portable presentation start at0; guard removed|
|0DAB:0002 BP-14 BYTE|SRM6 bucket outside the sixteen initialized salvage counters|Guard only when bucket is accessed|
|1AE8 BP-56 WORD|Effects can inherit an unassigned attack-applied flag|Portable first-use no-impact; retain assigned same-call carry|
|1543:0004 full twelve-byte weapon list|strlen reads beyond the unterminated native local list|Guard for full list|
|1F3D:0665 BP-2 WORD|Failed/partial asset prefix inherits prior stack bytes|Guard for incomplete prefix|
|1F3D:05BC allocation result|Oversize error branch leaves FAR result unassigned|Guard for oversized result use|

This list does not certify corrupted indices, arbitrary pointer values or all
exceptional native memory accesses. Their separately documented bounds remain.

## Implementation gate

Before removing a guard, demonstrate the representation against its full
caller/callee path, not just a fixture with injected values. A fixed seed may
be useful for labelled replay tests, but is not a recovered original default.
Statistics and first-use impact state are the explicitly documented
presentation exceptions above; native provenance remains recorded without
reproducing BIOS storage or changing attack damage.
Other original bugs retain their existing instruction-confirmed behaviour.

Remaining gameplay-affecting storage still needs an evidence-backed handling
decision. Until that is implemented and checked, keep those guards and describe
the build as incomplete. Do not extend the presentation exception into silently
changing salvage quantities, damage outcomes or other original rules.
