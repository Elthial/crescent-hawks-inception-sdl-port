# Sol: systematic BTECH_204B C-to-ASM comparison — complete

2026-09-17, baseline `18374be`: all19 retained implementations checked.
17 local matches under explicit native memory/register/platform contracts;
two dispatcher argument-binding mismatches. Executable C unchanged.

## Evidence and scope

Read the complete native bodies/branches in private
`BTech-Reko-expanded/BTECH.reko/BTECH_204B.asm`, SHA256
`557048B59BE86B495DA11564751228DB6289D5FA44F2FADFB8F50197F272F350`.
The033C listing truncates after its first load. Read its full intermediate
blocks in `BTECH_204B.dis`, SHA256
`82CB4CD4C5F1A7DA5814BB89BB03462798F1BFE5E830558092F0A3344CE3C951`,
and decoded its sixteen-byte original instruction tail directly from the private
expanded EXE, profile SHA256
`F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE`.
File offset uses MZ header paragraphs plus `(204B-0800)*16+033C`.
No original byte dump or copyrighted external asset is added to source.

| Implementation / native scope | Result | Checked operations |
| --- | --- | --- |
| Music_Note_To_Pit_Divisor_0139 0139–015F +00CF–00DD | Matched register model | Unsigned BYTE DIV12, frequency table,1234DE DWORD DIV, BYTE8-octave unmasked WORD SHL; C return represents DX. |
| Music_Write_Pit_Divisor_00B2 00B2–00BC | Matched output binding | CX low/high to port42; no gate/mode write. |
| Music_Emit_Pc_Note_0187 0187–019C | Matched | Any high-bit note chooses divisor14, otherwise pitch calculation; two PIT writes without gate change. |
| Music_Emit_Tandy_Note_01B8 01B8–01FD | Matched register/output model | AL note/AH channel, table/SAR by BYTE(octave-2), channel0/1/other latches and two PSG portC0 writes. |
| Music_Read_Stream_Byte inline024F/0298 | Matched extraction/DF-clear | Local LODSB source represented by shared FAR cursor, wrapped offset without selector carry. |
| Music_Start_Stream_0233 0233–024E | Matched frame adapter | Cursor WORDs/cadence low BYTE from parent BP+8/+A/+C, countdown1; no callback-binding store. |
| Music_Start_Tandy_Stream_020B 020B–0232 | Matched frame/output adapter | Same stream/cadence state plus PSG volume3 on three tone channels. |
| Music_Stop_Tandy_0160 0160–0178 | Matched | Idle callback offset, speaker gate clear, three PSG mute writes. |
| Music_Tandy_Stream_Tick_024F 024F–0297 | Matched DF-clear/non-reentrant | BYTE decrement/reload, first-zero stop/no cursor commit, four notes PSG0/PC/PSG1/PSG2 and final cursor. |
| Music_Pc_Stream_Tick_0298 0298–02E3 | Matched DF-clear/non-reentrant | BYTE decrement/reload, single-zero skip in same tick, double-zero stop/no cursor commit, cursor-before-PIT and divisor14 branch. |
| Music_Clock_Tick_0020 0020–0047 | Matched IRQ effect model | Callback first; old divider low BYTE zero reload/chain, else decrement preserving high/EOI/IRET; C returns chain flag. |
| Install_Music_Timer_0048 0048–0090 | Matched replacement binding | IVT vector8 save, divider16/initial0, idle FAR callback, DOS vector install and PIT36/FF/0F. |
| Restore_System_Timer_0091 0091–00B1 | Matched replacement binding | Saved FAR vector restore then PIT36/FF/FF, not arbitrary prior PIT settings. |
| PC_Speaker_ON 00BD–00C5 | Matched output binding | Port61 read/OR3/write. |
| PC_Speaker_00C6 00C6–00CE | Matched output binding | Port61 read/AND FC/write; no pitch change. |
| PC_Speaker_Set_Value_0179 0179–0185 | Matched | Idle offset0186 and gate-off only; misleading reset-frequency label corrected in summary. |
| Tandy_Music_Control_02E4 02E4–0305 | Mismatch | Signed selector/stride4/speaker-enable/setup/callback order understood; no-argument C indirect call omits parent-frame stream/cadence binding. |
| PC_Speaker_Music_Control_0306 0306–0327 | Mismatch | Same selector/dispatch issue with PC setup0233. |
| Music_Playback_Finished_033C 033C–034B | Matched raw-tail confirmation | Compare callback offset with0186, return WORD0/1; segment not tested. |

## Two queued corrections: dispatch setup arguments

02E4/0306 create a BP frame and select a four-byte CS record: setup offset then
interrupt-callback offset. Selector1's NEAR020B/0233 setup does not create a new
frame; it directly reads the dispatcher's extra words at BP+8/+A/+C. Those words
are FAR stream offset/segment and cadence. The retained C models instead call
a `void (near*)()` with no arguments and no representation of parent-frame
access. The explicit-parameter setup functions therefore receive no guaranteed
cursor/cadence. Binding those values is a substantive correction, not a comment
or a compiler-frame abstraction. Both bodies have `Sol:TODO ASM mismatch` notes.

Signed JGE13 remains permissive: negative selectors enter, and only0/1 have
confirmed valid table records. WORD stride/offset arithmetic is not a safe
thirteen-entry API. Near targets/CS records are logical research addresses,
not host function pointers. Selector0 stops, selector1 starts; timer install/
restore are separate FAR entries. Setup routines themselves do not bind the
callback; the dispatcher writes its paired offset after setup completes.

## Contracts and deliberate abstractions

- `SoundState` is a named logical view of writable, overlapping native CS
  fields, not the sequential physical layout of an ordinary host struct.
  Hardcoded204B denotes the image's logical relocated CS. Port/BIOS/DOS services
  and IRQ register saves are reference/platform bindings, not modern code.
- The clock model covers idle0186 and valid stream callbacks024F/0298 in the
  expected CS only; arbitrary FAR callbacks are intentionally not implemented.
  Callback runs on every IRQ; only the system-IRQ chain uses divider16. Initial
  zero chains immediately, then every17 ticks, testing old AL before decrement.
  PIC acknowledgement/IRET versus saved-handler FAR jump become its return flag.
- Stream LODSB inherits DF: extracted readers/ticks certify DF-clear intended
  playback only, not arbitrary interrupted CPU flags. Native does not CLD here;
  no new CLD correction is invented. BYTE countdown reload0 means256 ticks.
- Native Tandy holds local SI until all four note writes finish; C increments
  the shared cursor eagerly. Equivalence requires stable, non-reentrant stream,
  state and IO bindings with no observers of intermediate cursor values. It
  does not certify callback reentrancy or asynchronous host scheduling. PC commits
  its cursor before PIT output. Both stop branches preserve the original cursor.
- Pitch helpers return native register outputs through C values. The embedded
  tables/WORD arithmetic match; unmasked8086 shift counts and SAR are preserved.
  High-bit PC note means divisor14, not literal mute; Tandy has no rest test.
- Stop routines change only callback offset, not its segment or cursor.033C
  intentionally tests offset alone. This does not guarantee silence/completion
  for arbitrary callback segments. Former gate/frequency/cadence summaries were
  corrected without executable changes.

Omitted idle RETF0186, extra entry019D, setup01FE and dormant interpolation
helpers00DE/00F5/010A remain classified native-only dependencies, not extra
retained implementations falsely promoted. No audible or gameplay validation
is claimed by this static comparison.

Checkpoint verification passed187,656 startup/music synthetic assertions and
312-definition/340-check inventory verification, including comment-only C.
All312 retained method-body hashes are current; global totals223 local matches,
85 mismatches and4 unresolved bindings/returns. Private EXE profile unchanged;
`git diff --check` passed. No executable C changed. Paused for user instructions.
