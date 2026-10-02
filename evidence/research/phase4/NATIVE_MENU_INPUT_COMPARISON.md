# Native menu input comparison

Sol: diagnostic work on the pinned Spice8616.1.0 plain-emulation host; no
game methods, native memory, or keyboard handling were patched.

The private 2026-09-18 run using `--Cycles 3000` reached exploration but did
not reach the requested statistics-entry breakpoint. At relocated
`1E56:0CEB` (live17D3:0CEB for image017D) a breakpoint showed the menu's
BP-0C selection changing 3, 2, 1, 0 while BP-8 remained FFB8 (converted W).
The last three observed cycles were308906955,308908894,308910833.
Enter had been queued during this sequence. The source at0C1B calls the input
wrapper and converter again each iteration; the gate itself is not a
replacement input mechanism. These observations establish the repeated command,
not whether it arose from queued input, timing, or emulator execution.

One paused observation read BIOS40:1A/1C as0424/0424. The buffer bounds at
40:80/82 were041E/043E, so these pointers are valid for this emulator's buffer.
Do not diagnose corruption by assuming these values must be segment-relative
001E/003E. Their interpretation must be checked against the pinned emulator;
do not simply add40h*16 to these already0400h-based values. Equal head and
tail at a sampled stop is not a full keyboard-event history.

The pinned CLI's rejected-enum diagnostic confirms three execution modes:
`InterpretedThenCompiled`, `InterpretedOnly`, `CompiledOnly`. The launcher now
accepts an optional `-JitMode`, records it, and otherwise omits the argument.
A100,000-instruction interpreted-only smoke exited0 with all required exports;
an omitted-mode smoke did likewise without passing `--JitMode`. Invalid modes
are rejected before preparing private inputs. This does not yet prove that
interpreted-only execution fixes gameplay input, or that a JIT fault exists.

Next comparison: repeat the menu-gate capture in interpreted-only mode and
record each command and selection before pursuing statistics stack write
watchpoints. Do not modify preservation C to compensate for this unconfirmed
emulator difference. Logs, dumps and screenshots remain ignored private data.

## Interpreted-only comparison

Sol: the next private run, with `--JitMode InterpretedOnly` and no speed
argument, also reached exploration and the pause menu, but did not reach the
statistics parent. At the same menu gate, cycles345883822 showed BP-8=0013,
BP-0C=0, BP-2=1. This first gate visit is **not an input read**: the ASM
initial draw/drain jumps directly to0CEB before reaching0C1B. With BP-2=1,
the gate branches back to the first keyboard read without using BP-8. The0013
is unused residual stack data, not a delivered control key. At346103788 it showed BP-8=0020,
BP-0C=0, BP-2=0: Space confirmed Return to game, despite the operator trying
to move selection with X. This is not a successful reproduction of the earlier
W sequence and does not establish equivalent faults in both modes. It does
show that switching to interpreted-only did not make this input sequence reach
the desired option. Some operator events were issued while a breakpoint pause
was active; removal of a breakpoint does not itself resume the emulator.

A separate BIOS-tail write watchpoint at physical041Ch successfully stopped
after X down: CPU F000:0015, cycles625312757; head/tail values0434/0436.
The actual key WORD at physical0434h (40:0034) was2D78, the correct X key.
Memory at40:041E was zero, further demonstrating why these emulator buffer
values must not be treated as ordinary segment-relative offsets. This verifies
that the local tool can trigger a memory-write breakpoint and deliver that key;
it does not identify the statistics palette locals' last writers.

All comparison breakpoints were removed and the emulator resumed for its
bounded finish. No memory or game methods were patched. Before another
statistics attempt, trace event delivery from the key-buffer write through the
blocking BIOS return and menu converter; do not keep repeating blind key bursts.

## Pinned BIOS wait-loop finding

Sol: both bounded gameplay runs subsequently exited0 and exported the required
files. Queueing key down and key up together before resuming was also tried;
it did not prevent the later interpreted-only menu trace from consuming more W
commands before Enter. Treat held-key timing as an unconfirmed hypothesis,
not an established explanation or a solved operator-input issue.

Inspection of the **installed package's managed IL**, rather than unverified
upstream master code, gives a specific candidate cause. The inspected
`Spice86.Core.dll` SHA256 is
`07E53EBF63AB4254E78AFDBD273147368ED429BF793AAD8E1B025861578F155B`.
No package binary or native game memory was modified during this inspection.

- `KeyboardInt16Handler.WriteAssemblyInRam`, IL0070..0093, emits its
  buffer-check callback, JNZ4, INT09h, JMP-short -10 for blocking AH00/AH10.
  When the BIOS key buffer is empty, this explicitly invokes interrupt9 again.
- `CallbackHasKey`, IL0007..000D, passes the buffer's `IsEmpty` value to
  `SetZeroFlag` with its second argument false. This is the callback controlling
  that polling branch; it does not test the keyboard controller's output status.
- `BiosKeyboardInt9Handler.Run`, IL0041..004E, reads port60h unconditionally.
  Its earlier port64h read is inside a debug-logging branch, not a guard that
  rejects an empty output buffer.
- `Intel8042Controller.ReadByte`, IL0012..0025, tests `IsDataPending` and
  returns cached `_dataByte` immediately when false. Thus an empty-buffer
  software INT9 can read a previous make code, not a newly delivered key.

This chain can manufacture repeated keys during the blocking BIOS wait while
the last make byte is still cached. It is a stronger explanation than JIT mode
or CPU rate alone, but still needs an isolated reproduction and a corrected
wait-path comparison before calling the gameplay jitter fixed. It must not be
used to change preservation game logic or label the original binary as buggy.

Next: verify an emulator-only pending-output/interrupt wait correction against
an isolated key-read probe, then repeat the native game menu trace. Retain
the no-game-code-overrides policy; do not patch the EXE or replace its methods.

## Isolated reproduction and optional BIOS-only correction

Sol: official emulator source was checked out in ignored tooling storage at the
installed NuGet package's recorded commit
`bf6f3b6acae1b15713761523cebc744af01e3abe`. The emitted runtime callback
instructions are FE38 plus a callback index; exported memory images replace
them with INT/NOP representations. A dump image is therefore not an exact
template for the live BIOS stub. The correction validates the live form,
including the dynamic callback bindings, before writing anything.

`--KeyboardWaitSelfTest` now executes our own synthetic DOS key-read program.
In both interpreted-only and default-JIT modes, for AH00 and AH10, one KBC make
code yields1177 twice in the unmodified BIOS. With the optional correction,
the first read yields1177 and the second preserves the A55A not-yet-read
sentinel until the instruction bound. Delivering a genuine Enter make code
before the second read yields1C0D instead. The caller's IF=0 is restored after
the first read in every case. All twelve scenarios pass. A deliberately changed
stub is rejected without partially applying the correction.

The opt-in headless correction changes only INT09 at the BIOS wait loop to
STI/NOP, allowing genuine IRQ1 delivery rather than polling an empty controller
and consuming its cache. Both bytes remain in reserved F000 firmware storage;
no original EXE or game method is patched or overridden. The callback addresses
and IRET remain unchanged. Default captures are still unmodified, and the
private manifest records whether the correction was selected. A changed BIOS
layout or callback binding fails closed. Interactive GUI correction is not yet
supported by this host and is rejected before private-input preparation.

Both corrected and default100,000-instruction original-EXE launcher smokes
exited0 with required exports. The default has no correction argument; the
corrected manifest contains it. Code-override options still return2. This is an
isolated emulator regression plus startup/export evidence, **not yet a successful
game-menu capture or statistics-stack recovery**. Earlier failed fixture runs
opened .NET error dialogs; failures are now caught and return a nonzero status.
No failed probe processes remain. Synthetic probes contain no game assets;
downloaded emulator source, package binaries and game dumps are not staged.

## Corrected native-game navigation

Sol: the next original-EXE run used the optional BIOS correction and loaded
untouched GAME1 through the original prompts and load menu. The first Yes sent
while the second question was being drawn was drained; a second Yes after the
question was ready reached native0800:32B3. Preserve these native input drains.

The actual pause menu then advanced once for each X and confirmed Inspect:

| Cycles | Converted command BP-8 | Selection BP-0C | Continue BP-2 |
| --- | --- | --- | --- |
|918754092|FFB0 (X)|1|1|
|921000556|FFB0 (X)|2|1|
|923286730|FFB0 (X)|3|1|
|925553028|000D (Enter)|3|0|

This reached0800:378D at cycles925553059, SS3858:SP5F6E. The original
Mech-inspection Yes/No prompt and Mech-name selection screen were also reached.
This is a positive native-game navigation result, not a full jitter/playthrough
certification, and does not add GUI correction support.

For this inspection call footprint, future stats BP is5F56, so the watched
BP-22 and BP-2C slots are SS3858:5F34 and5F2A (physical3E4B4h/3E4AAh).
Early UI writes were captured, including repeated calls from text-border
drawing. These are **not final statistics seeds**; later routines reuse them.
To avoid logging every border push, the watchpoints were removed for the bulk
UI drawing. The subsequent staging breakpoint was mistakenly placed at
0DAB:0B98; the real `Display_Text_Mech_Names` entry is **1467:0B98**, runtime
0DEE:0B98, physicalEA78h (60024) for this image profile. Correct that target
before another provenance attempt. The statistics entry was not captured;
the instruction bound expired while the selector waited for confirmation.

The run exited0 and all required exports were verified. No original game or
asset bytes were patched. Do not infer palette initialisers from intermediate
UI snapshots, or remove the preservation guard on the strength of this run.

## Final writers captured through combat Scan

Sol: private run `run-20260918-165639-8b2876d7` used the optional keyboard
correction and a ten-billion-instruction bound. GAME1 was loaded through the
pause menu. That menu retained selection4 on reopening; an attempted fixed
three-X route therefore selected the overhead map, not Inspect. Subsequent
exploration entered an encounter. The successful route was combat Scan6,
Friends0, then Detail1. Read live menu state rather than assuming selection0.

The native browse entry at cycles2551653423 had SS3858:SP5EFA. Its future
statistics BP was5EB6, with residual counter at5E94 (physical254996) and phase
at5E8A (physical254986). Write watchpoints were installed after bulk browse
drawing, before selecting Detail, and retained until statistics entry:

| Cycles | Writer/state | Counter | Phase |
| --- | --- | --- | --- |
|2553686441|19FC:0B8F, keyboard prologue saves menu SI|0030|0247|
|2554778969|F000:0042, interrupt stack observed during BIOS wait|0030|0246|
|2555979329|F000:0011, hardware keyboard interrupt stack|0030|0246|
|2555979330|F000:0015, keyboard callback completed|0030|0247|
|2555979428|0728:1AFE, native statistics entry|0030|0247|

The counter's last observed write agrees with the ASM keyboard prologue:
`PUSH SI` retains menu3's index shifted by4. There were no later counter
writes before entry. The phase is not a game-defined seed: it is reused BIOS
interrupt-stack flags. IVT09 points to F000:0011. In the pinned emulator,
`DoInterruptWithoutBreakpoint` pushes flags; `BiosKeyboardInt9Handler.Run`
calls `KeyboardIntercept(calledFromVm: true)`, which uses
`SetCarryFlag(true, calledFromVm)` on the current interrupt stack. This explains
the observed0246-to0247 write. It does **not** establish that a real DOS BIOS
produces the same residual word, or justify embedding0247 in preservation C.

The entry verifier confirmed caller0728:16C7, Mech0 and SS3858:SP5EB8.
At the first refresh comparison, cycles2557042704, the counter was0031;
four instructions later at0DAB:22CD it was0001 after the mask. The phase
remained0247. These are observed native transitions, not host initialisers.
The unsupported-residual guard remains. The capture was initially left paused
at that refresh breakpoint; the continuation below completed it. Original
game and external asset bytes remain untouched.

### Return and normal game exit

Sol: continuation confirmed Enter returned to the original caller0728:16C7
at cycles2558506506. The browse loop explicitly rebuilt selection0; it did
**not** retain Detail1 like the pause menu retains its choice. A second Detail
visit was consequently opened during navigation, then closed; it was not
instrumented as a second seed witness. Next the browse Done2 choice returned
to combat planning with Scan6 selected. Flee8 succeeded and displayed the
native "You have eluded your enemies! Press a key." prompt before exploration
resumed. These are native route observations, not proof of every flee outcome.

Pause selection7 was retained. W six times selected Settings1, then its Quit4
and native Yes confirmation terminated the game after3385366654 instructions,
well before the ten-billion bound. The host exited0; native DOS termination
logged AL=30. CPU registers, memory, listing, CFG and execution-flow exports
were saved and the launcher verified all required exports. The private capture
process is terminal; do not resume or recreate it based on the earlier paused
state. No original method patch, game code override or external asset upload
was used. The optional correction touched only the emulator's BIOS wait stub.

### Post-exit executable comparison rejects an operand mutation

Sol: required exports being present is not a successful executable-comparison
result. `Confirm-SpiceDumpCode.ps1` rejects this terminal dump: two instruction
operand bytes at1631:0B1E..0B1F changed from55A2 to0000. The instruction is
`MOV ES,[55A2]` in the AI weapon-target assignment block0B0C, not declared
writable music state. Do not add these bytes to the verifier's writable-data
allowlist or change the C to read DS:0000.

Eight relocated32-byte anchors match after3292 MZ relocations. Across116416
code-region bytes, the only other differences are fourteen already documented
writable204B music-state bytes. All five separately checked static tables
match. The trace has13807 distinct instruction starts in that code region,
with zero changed start bytes; an operand mutation is not excluded by that
start-byte metric. This does not prove the altered instruction was executed.

Read-only comparisons of the same operand in earlier dumps found55A2 in
`run-20260918-164549-e7bc72fb` (startup smoke),
`run-20260918-154338-c06bc7fe` (earlier Scan statistics), and
`run-20260918-164706-59495ab7` (Inspect selector). That narrows the evidence,
not the cause. A native write watchpoint at the operand's relocated physical
address is required to determine whether the mutation occurs during load,
navigation, Flee, native shutdown or emulator activity. No original-game bug,
intentional self-modification or preservation correction is established yet.
The positive live statistics slot witnesses remain separately recorded; the
terminal executable correspondence check remains failed, not certified.

### Writer identified: emulator root termination frame

Sol: private `run-20260918-171042-f786640e` installed a MEMORY_WRITE
watchpoint at physical67070 (runtime0FAE:0B1E). It started a new game and
selected Pause Settings1, Quit4 and Yes; no GAME1 load, Inspect, Scan or combat
was needed. The operand remained55A2 through startup and pause navigation.
After native Quit, the observed CPU state was F000:00A3, IsRunning=false,
SS0060:SPFFFE, cycles1269140866; the operand had become0000. The final register
export records StackPhysicalAddress67070. The host exited0 and verified all
required exports. The callback finishes termination rather than maintaining a
resumable game breakpoint; this process is terminal.

The exact pinned source explains the alias. `DosProcessManager.TerminateProcess`
restores SS:SP from the parent PSP, then calls `_stack.Poke16(0,
terminateAddr.Offset)` and `Poke16(2,terminateAddr.Segment)` before stopping
the root command process. `Stack.Poke16` writes through SS with a wrapped WORD
offset. Consequently0060:FFFE =67070 = runtime0FAE:0B1E; the zero return IP
overwrites this child-code operand after the child has quit. This is a root
emulator termination-frame artifact, not a discovered game instruction patch
or justification to change the AI C transcription. No Astra review is needed.

The correspondence verifier now reports this root-stack context when rejecting
a terminal mismatch. It still rejects the changed bytes, with no new allowlist
or inferred reconstruction of their pre-exit values. Use a pre-termination live
snapshot for complete executable correspondence; normal-exit exports remain
useful for their other, explicitly scoped observations.
