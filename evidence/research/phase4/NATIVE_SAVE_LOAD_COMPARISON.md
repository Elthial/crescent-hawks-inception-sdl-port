# Sol: native and C17 existing-save load comparison

## Verified scope — 2026-09-18

The packed original binary and the Release C17 preservation build both loaded
the same unmodified private GAME1 copy through their actual load menus and
file/map routines. Both matched the file for all16 character records
(272 bytes), all8 Mech records (1000 bytes), all100 persistent story bytes,
the16 bytes of C-bills/company balances, and both party-position WORDs.
No replacement gameplay methods, memory writes or code overrides were used.
This is save-load state evidence, not a gameplay, pixel, music or timing match.

## Native witness

Private capture: `chinception/validation/run-20260918-184320-183d8dcc`.
Packed EXE SHA256:
`F2A9A023D79927B8072DE11DDD6E03DA6D3357D49181D8FBA6D12988BF8CC0EE`.
Spice86 used its documented emulator-only INT16 wait correction and a10-billion
instruction ceiling. The original binary was unchanged.

Breakpoints confirmed original `0800:50C8` startup, `0800:32B3` Load_Game and
then `0800:0000` Main_Game_Loop. The last witness was paused at relocated
`017D:0000`, SS/DS3858, cycle1630280718, before the main-loop prologue.
The live read of relocated `3092:C614` (`2A0F:C614`) covered3908 bytes.
Independent file comparisons reported zero differences in these ranges:

| Fields | Offset within loaded C614 block | Bytes |
| --- | ---: | ---: |
| Characters | 0 | 272 |
| Mechs | 272 | 1000 |
| Persistent story state | 3320 | 100 |
| C-bills and stocks | 3420 | 16 |

Party coordinates at relocated `246C:A44B` (`1DE9:A44B`, four bytes)
matched the final four file bytes. CPU cycles were unchanged across these
reads. Fog-of-war and roaming/effect state were not compared; map rebuilding
and redraw can mutate those fields. This does not certify the entire3913-byte
save payload or procedural-map output.

## Input findings and actual save selection

The untouched binary asks for the graphics adapter before the disk layout.
The native sequence is3 for EGA,3 for hard disk, then Enter. The maintained
EGA-only C version intentionally omits the adapter prompt, so its first3
selects the hard disk. Do not reuse its shortened sequence for native captures.

GAME5 was initially intended, but four injected Down presses returnedFFE0 at
the native keyboard bridge and did not change the save-menu selection.
The loaded character bytes uniquely identified GAME1 among the six private
files, and its coordinates independently agreed. Consequently this is a
GAME1 comparison, not a GAME5 witness. Prefer native letter direction keys
(`X` for south) for future paced menu captures; they worked for this session's
Pause/Settings navigation. Do not classify this emulator-key incompatibility
as a bug in the C loader or proof of bad original menu logic.

## C17 witness and termination

Release `test_connected_combat load-existing-save GAME1` ran from this same
private game directory, using the actual C Load_Game and SDL backend. It
returned code0, independently compared the same field groups with the file,
and verified source bytes unchanged. The private GAME1 SHA256 also still
matched canonical GAME1 after both load checks. SDL device overrides were
not set; input remained scripted and retraces accelerated in the C fixture.

The native capture subsequently returned through Space/Pause, Settings,
Quit and Yes. Its process exited0 and the launcher verified nonempty CPU,
memory and execution-flow exports. Final shutdown dumps are not the paused
load-entry snapshot and must not be presented as such. Assets, save bytes,
logs, rendered artwork and dumps remain ignored private material.

Remaining work includes a native gameplay/service/combat comparison, SRM-6
salvage residue, visual/audible checks and progression through a playthrough.
