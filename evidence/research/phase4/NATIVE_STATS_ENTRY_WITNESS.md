# Native statistics palette-state witness

Sol: one positive original-EXE combat-scan entry was captured on2026-09-18.
This is an observation of residual stack state, not an initializer or proof
that preservation C can use constants. No game code or memory was modified.

## Capture identity

- Private run: `run-20260918-154338-c06bc7fe`, under ignored `chinception/validation`.
- Pinned Spice86:16.1.0; original packed input hash
  `F2A9A023D79927B8072DE11DDD6E03DA6D3357D49181D8FBA6D12988BF8CC0EE`.
- Image load:017D; entry analysis0DAB:1AFE, live0728:1AFE, physical08D7E.
- Untouched local GAME1, native EGA/hard-disk startup and Load Game.
- Combat menu Scan Unit, Friends, Detail Scan; inspected Mech record2.
- Expanded-EXE prologue and both original return-address contracts checked
  by `Read-SpiceStatsEntry.ps1 -McpPort 48181`.

| Observed entry item | Value |
| --- | --- |
| CPU cycles, unchanged throughout stack reads |1404920900|
| SS:SP before statistics prologue |3858:5EB8|
| Caller return CS:IP |0728:16C7|
| Mech ID argument |2|
| Refresh frame at SS:SP-24 |0030|
| Cycle phase at SS:SP-2E |0047|

The caller is the native combat-scan PUSH CS/near CALL pair, not the normal
party-inspection FAR CALL. The reader was tested positively against this
entry and rejected a later non-entry snapshot in the same run.

## First refresh

Two additional execution breakpoints observed the original instructions:

| Native point | Cycles | Frame WORD | Phase WORD |
| --- | --- | --- | --- |
|0DAB:22BD, before frame mask |1406006334|0031|0047|
|0DAB:22CD, after frame mask, before pending-input call |1406006338|0001|0047|

The statistics frame BP was5EB6 at both points. Therefore the initial0030
survived screen construction, incremented to0031, failed equality with0010,
and was masked to0001. Phase0047 remained unnormalized: this first refresh
did not apply/patch the cycling palette. These observations agree with the
instruction order in the expanded `.dis`. They do not establish a later
palette-cycle trace, screen pixel parity, or audio parity.

## Input evidence and limits

At0800:002D (after scene15), an extra Enter from loading was still pending.
At0800:0070, SS:BP-1C was000D, not the posted pause Space. This explains why
that specific input iteration could not call the pause menu; it does not
prove the cause of every previous failed capture. The native drain occurs
before0070. A subsequently posted Space did not produce a pause-menu witness
before combat.

Combat browsing showed repeated decoded movement commands at1E56:0CEB:
current selection BP-0C cycled0/1/2 while command BP-08 remainedFFB0;
other transitions usedFFB8. An empty BIOS buffer was observed separately,
but no last-write/BIOS-interrupt trace was captured. Do not label this as an
original-game bug or change C input logic on that evidence alone. The next
capture should verify individual native menu transitions and distinguish
queued/held input from emulator behaviour.

## Remaining fidelity work

Repeat statistics visits from both original callers, with different Mechs
and preceding UI histories. Trace the last writes producing both residual
words. One entry cannot establish constant seeds or a general stack-history
model. The C diagnostic guard remains: substituting0030/0047 globally would
invent behaviour rather than preserve the binary.

Sol: static stack arithmetic identifies a candidate source for frame0030.
The scan caller's pre-argument SP is5EBE. Menu0B5E and statistics1AFE each
receive one WORD argument and four return bytes, so each establishes BP5EB6.
Menu reserves0C bytes and saves SI, leaving SP5EA8. Its input wrapper0259
uses four return bytes, saved BP and four locals, leaving SP5E9E; keyboard
0B8A uses four return bytes and saved BP, establishing BP5E98. Its saved DI
is5E96 and saved SI is5E94, exactly statistics BP-22. Menu SI is control3
shifted four bits,0030. The subsequent converter has no locals and does not
reach that address on its normal stack path. However, interrupts can use
released stack slots between these calls: this is not last-write proof or a
universal frame initializer. Phase0047 lies deeper, requiring BIOS/interrupt
history rather than a guessed menu value. No production initializer changed.

Breakpoints were removed and the bounded session resumed. It reached its
2500000000-instruction limit, exited0, and the launcher confirmed all required
exports present. Dumps/screenshots remain private, not repository assets.
The final dump is no longer the paused-entry witness; use the entry metadata
and breakpoint observations above, not final stack bytes, for this evidence.
