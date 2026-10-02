# Sol: original Mech-statistics parent — 2026-09-18

`src/Original/BTECH_0DAB_STATS.c` follows annotated `0DAB:1AFE` through the
`2304` palette-restore block and the `.dis` return epilogue. The EGA-only
pipeline still requires external `BTSTATS.CMP`; no image pixels are embedded.
The shared `3092:00A0` cache and `246C:244B` decoded workspace bind to existing
original global storage, not a new independent asset or state representation.

## Incorporated ASM corrections

- Mech classification uses signed WORD comparisons. Enemy combatants12..15
  normalize to records4..7; friendly records0..3 form a Lance.
- Pilot/rider party indices are unsigned bytes, but their character name IDs
  undergo native CBW signed extension. Valid native record/name indices remain
  the array contract; no new absent-pilot default is invented.
- Raw critical and structure offsets access bytes of the actual125-byte Mech
  records. Corresponding pseudo-code FAR casts and signed gates were corrected.
- Native labels and literal strings are retained, including unknown enemy
  crew, eight-character type truncation, destroyed weapons and actuator states.
- Leg status tests against nibble0F, not the maximum-actuator nibble. Arms test
  equality with maximum before zero: both-zero arms display ` OK`, unchanged.
- Operational sinks exclude destroyed A2 critical bytes. Both palette families
  retain their BYTE/WORD storage; applying the old palette precedes entry4 patch.
- Signed heat clamps to0..30. Every first frame draws eleven condition gauges
  and the heat gauge through the original helper/primitive boundary.

The stats-location boundary constants name the original display labels, not
proof of anatomy in the uncertain critical-damage section tables.

## Explicitly unfinished native residual state

The original reads unassigned native stack WORDs `BP-22` (frame) and `BP-2C`
(phase). This C parent has a temporary diagnostic abort at the first frame
increment after the initial retrace. This is NOT original-game behaviour and
does NOT make the statistics screen usable or preservation-complete.

The zero values in guarded C locals are inaccessible placeholders, not claimed
original initial values. No zero-initialization fix or host undefined read was
substituted. The later native refresh/apply/patch/mask, input/600-frame dismissal,
default palette restore and keyboard consumption remain represented in source
but cannot execute until a faithful residual-state representation is supplied.
An entry capture alone is only one witness; it does not prove a universal seed.

Sol: follow-up ASM inspection confirms that the parent's prologue reserves38h
bytes, but does not initialize BP-22 or BP-2C. After the gauge retrace it
increments BP-22 and compares the full WORD against10h before masking with0Fh.
On equality, it applies the current palette first, then increments BP-2C,
masks it with3 and patches the next palette entries. Thus a seed influences
visible refresh timing and colour phase, not an out-of-range palette lookup.
Even a single-redraw screen can encounter the refresh path when the incoming
frame word is000Fh. Pending keyboard input is tested afterwards, so a queued
key cannot justify bypassing this unresolved state. No initializer or bypass
has been invented; the temporary guard remains a playability blocker.

### Residual-word provenance and targeted capture

Sol: inspected the actual allocator `207F:2FDC` and both call sites in the
expanded `.dis`. Its successful branch pops the FAR return address, computes
SP minus AX, checks the stack limit, then pushes the return address at the
new stack bottom. It does not fill/zero the reserved locals. The statistics
prologue's `AX=38h` allocation therefore supplies no hidden initializer.
Nested calls made by statistics itself use stack below that reserved frame;
their normal stack frames cannot initialize BP-22 or BP-2C within it.

There are two native callers, not a single universal menu-entry history:

- `0800:378D` inspection calls `1467:0B98` Mech selection before statistics.
  That selector reserves four local bytes and invokes further UI methods.
- `0DAB:1467` scanning calls `1E56:0B5E` menu selection before statistics.
  That menu reserves twelve local bytes and saves SI. Its direct local
  assignments do not cover the deeper addresses reused by the statistics
  frame; earlier nested calls can leave values there.

Let S be the caller SP immediately before pushing the single Mech-ID argument.
The argument consumes two bytes, FAR CALL four and PUSH BP two, so the new
statistics BP is S-8. The unresolved words are consequently at SS:S-2Ah
(frame) and SS:S-34h (phase), using 16-bit offset wrap. At entry `0DAB:1AFE`
before PUSH BP, SP is S-6: capture WORDs at SS:SP-24h and SS:SP-2Eh. Capturing
at the first increment instead uses SS:BP-22h and SS:BP-2Ch.

The next emulator capture should log the entry SS/SP, caller return CS:IP,
Mech ID, both words, and their first refresh/apply/patch sequence. Include
both inspection and combat scan, plus repeated visits and different prior
menu choices. A final gameplay memory dump without the entry registers is
not enough to recover these frame-relative seeds. A capture establishes
witnesses, not proof of a constant initializer: if they vary, trace the last
writes to those two addresses in the preceding UI calls. A faithful C
representation must retain that dependency rather than initialize the
statistics locals or turn off their palette cycling. No runtime guard was
removed on the strength of this static investigation.

Sol: `scripts/Read-SpiceStatsEntry.ps1` now reads a paused-entry dump and
rejects unrelated final gameplay dumps. Native EXE caller bytes confirm
0800:3821 FAR CALL (return3826) and 0DAB:16C3 PUSH CS followed by16C4
near CALL (return16C7), not a three-byte call beginning at16C3. Both construct
the four-byte return layout used above. The reader checks these returns,
entry CS:IP/prologue and the known expanded EXE hash. Its address/rejection
self-checks pass, and an existing ordinary final capture is correctly rejected.
No actual paused statistics entry witness or universal seed is claimed.

Sol: the local MCP control path is now tested against a bounded original-EXE
run: manual pause, keyboard press/release to pass EGA/disk setup, execution
breakpoint at1F3D:002F, unchanged paused cycles and matching native entry
bytes, breakpoint removal and resume. This establishes an alternative to
manual debugger controls for the targeted capture, not a statistics seed.
The installed tool names and invocation examples are in the Spice86 host
README. A live paused snapshot must read CPU and stack before resuming;
resumed normal-close final dumps are no longer entry witnesses.

Sol: first live statistics-capture attempt loaded the untouched local GAME1
through native startup prompts and Load Game. Its Jason record was alive,
health80, assignment0, with party Mechs present. The initial navigation
selected healing, not inspection; the cause was not established.
After returning, the original game entered a combat encounter. A scan-menu
attempt did not reach statistics before the bounded session's remaining
instruction allowance. Thus **no statistics seed was captured**. Do not use
any of that session's final stack bytes as palette initializers.

An additional breakpoint at207F:0BA1 observed BIOS Enter returning AX=1C0Dh
(scan1C, ASCII0D), ruling out a guessed Enter-as-linefeed explanation for the
navigation difficulty. The next capture should pause at Main_Game_Loop entry
and post the pause key before the first world/encounter update, then inspect
the actual persisted menu selection before choosing Inspect. GAME5 is another
untouched local save with party Mechs; a quieter save alone does not guarantee
the absence of an encounter. No C input, combat or palette logic was changed.
The session resumed after breakpoint removal, reached its3000000000-instruction
limit, exited0 and saved all required exports under ignored private storage.

Sol: a second native attempt stopped successfully at0800:0000 after loading
GAME1 and posted Space before resuming. It nevertheless entered combat before
inspection; statistics entry was not reached. The run ended at its1500000000
instruction bound, exited0 and retained all required private exports. No seed
or palette compatibility initializer was recovered.

The expanded `.dis` confirms that main-loop entry first calls animation48B7
with scene15, before reaching input polling at002D. Input handling at0070
skips movement for a decoded Space;0080 calls the051B sprite compositor,
then0107 calls the pause menu before the later world-tick encounter check.
The compositor does not call combat. The preservation C retains this order.
We did not observe the decoded key at BP-1C, so this capture does not prove
why the posted Space failed to open the pause menu. A live read of the pause
control's stored selection was zero; a nonzero initial selection is not a
verified explanation for the previous attempt either.

Next, pause at002D (after the initial animation), then verify the decoded
key at0070 before interpreting navigation results. Verify each menu transition
before confirming a choice. Do not reorder native input, drawing or encounter
dispatch as an emulator workaround. Main-entry key injection alone has not
produced the required statistics entry witness.

Sol: the entry reader now also accepts a local MCP port. It pauses explicitly,
reads entry code and the five required wrapped stack WORDs, and checks that
CPU cycles and entry/stack registers remain unchanged. The same native caller
and prologue checks apply as for a dump. It leaves execution paused even on
rejection, so follow-up inspection cannot silently resume before its reads.
Hexadecimal parsing and malformed-response self-checks pass. No positive live
statistics witness or preservation palette initializer is claimed yet.

Sol: subsequent live capture produced one positive combat-scan entry:
Mech2, return0728:16C7, SS:SP3858:5EB8, frame0030 and phase0047. Native
first-refresh breakpoints observed frame0031 before mask and0001 after,
with phase0047 unchanged. The live reader passed entry/prologue/caller and
unchanged-cycle checks. See [the detailed witness](NATIVE_STATS_ENTRY_WITNESS.md).
This supersedes the earlier no-witness status, not the unresolved-state guard.
Repeated visits and last-write provenance are still required; no constants
were added to preservation C.

## Verification and next work

224 probes execute the actual parent, layout, native copy/formatter, tables and
gauge helper through the first retrace, where a TEST-ONLY presentation callback
stops the probe. They cover all eight record IDs, signed heat classes, cached
versus uncached assets, actual friendly rider names, and invalid enemy crew IDs
which the unknown-crew branch must not dereference. Native location labels,
weapon colours, actuator text, pip/gauge call totals and heat clamping pass.

File/decompression/upload, text/rectangle presentation and the retrace boundary
are isolated in this probe. It is NOT a full-screen, pixel, asset-decode,
auto-dismiss, input-loop or emulator test. Separate local asset tests still
exercise real loaders/decoders; the table test independently compares the EXE.

The real preservation executable now links through original `Setup_Game` with
the separate SDL backend and without missing original methods or no-op stubs.
Linkage is not playability or behavioural completion. Resolve the residual-state
guards here and in other converted parents, exercise real startup/normal close,
and validate end-to-end gameplay, rendering and music before certification.

Checkpoint validation: all116 headless and136 SDL/local-asset tests pass,
with a successful real-executable link. No interactive playthrough was performed.
