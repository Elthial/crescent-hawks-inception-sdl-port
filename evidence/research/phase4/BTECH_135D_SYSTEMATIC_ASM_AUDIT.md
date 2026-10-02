# Sol: systematic BTECH_135D C-to-ASM comparison

Checked 2026-09-17 against baseline `1ca61f7`: all13 retained methods,
7 locally matched and6 mismatched. This is a whole-local-flow semantic review,
not compiled-C, emulator or gameplay validation. Executable C is unchanged;
comments and summaries identify corrections for a later approved fixing pass.

## Evidence and comparison contract

Private original-version listings in `BTech-Reko-expanded/BTECH.reko`:

- `BTECH_135D.asm` SHA256 `A377A450B0448BF27C59A577A8723FC638390CDB83FA02060EDBFADB6B6FC7D1`.
- `BTECH_135D.dis` SHA256 `D505A297FCBB3BCDDDEF2190E003AF172ABE8F52DA673B6A337A7D2CDEC02607`.
- `BTECH_3EDB.asm` SHA256 `BF0095488D585D4CD18EFF48B52FEC7FDB9C8E115B037D2325A2D2BA200EFFDD`.

Read all local branches, meaningful arguments/temporaries, memory accesses,
loops, calls and side effects. Checked the truncated door tail through its FAR
return in `.dis`, and static data/selector cells in3EDB. Compiler stack probes,
register saves and frame setup are abstracted. Named memory views assume native
BYTE/WORD and16:16 FAR addressing, including offset wrap; these scratchpad
declarations are not independently certified compilable layouts. Retained EGA
adapter2 behavior is the comparison target: adapter0 palette and helper paths
are deliberately omitted. Decoded asset upload targetsA400, not a true-color VGA
surface. Shared callees are used under their documented contracts, not certified
by checking this file.

## Whole-method results

| Method / native range | Result | Checked local workflow |
| --- | --- | --- |
| Draw_STARLEAG_ICN_AND_Game_Logic /0004–01E8 | Mismatch | Save/hide four mech name bytes; eight pilot skills; disk/map/descriptor setup; decode/upload cache art; draw; entrance opening; script; replay eleven door latches |
| StarLeague_Secret_Passageway_Discovered /01E9–0287 | Matched | Copy18 passage bytes in six rows of three, redraw, dialogue/key and message flag |
| StarLeague_Cache_PhoenixHawk /0288–02A7 | Matched | Found-flag gate and ENDMECH script dispatch; no local found-flag write |
| Display_StarLeague_Cache_Dialog_Window /02A8–02D1 | Mismatch | Message panel, native text, key wait and message flag |
| StarLeague_HyperPulse_Power_Dialog_Window /02D2–0326 | Matched | Wrapped X increment, masked coordinates, three switch positions, dialogue and power flag |
| OverHead_Map_Function /0327–03A9 | Matched | Swap250 overview bytes with flag-selected alternate buffer; no display call |
| StarLeague_Security_Terminal /03AA–04AA | Mismatch | Masked position lookup across33 signed-BYTE table entries; used-code gate; colour FAR text; choice and selected-code write |
| HPGTransmitter /04AB–0559 | Matched | Two positions; power, exact WHITE-code and parts gates; errors or WINSCENE and3EDB:01A8 success WORD |
| StarLeague_Map_Room /055A–079B | Mismatch | Parts script and independent entry check; dialogue; state/position, art upload, tile backup and map replacement, descriptors and collision mask |
| Draw_STARLEAG_ICN_Scene /079C–0912 | Matched | Return dialogue/position; restore768 tiles before buffer reuse; decode/upload; descriptor rebuild; map-room flag and collision mask |
| Map_Interactable_Play_Sound /0913–097F | Matched | Packed map address, wrapping BYTE increment/XOR/decrement tile toggle and sound0F |
| Cache_StarMap_CorrectPassword /0980–0AB5 | Mismatch | Seven required offsets, reject extra selected stars, success/failure dialogue/sounds, WHITE latch and failed-selection reset |
| StarLeague_Key_Codes /0AB6–0D44 exit | Mismatch | Coordinate lookup or direct replay; three signed code requirements; validation/consumption; entrance exception; latch; four-tile frame addressing, redraw and playback |

## Corrections queued, not implemented

1. Cache setup reads/writes the first BYTE of each125-byte mech record's name.
   `Mech.Name` is a16-byte array: current scalar save/assignment must become
   `Name[0]`. The remaining setup order matches, including restoring eight
   piloting skills to8 and replaying doors0..10, not entrance11.
2. Cache panel DS:20CE has two spaces after `gyros!`, not one.
3. Terminal DS:2187 includes `.\rDo you want to imprint ` after the heading.
   DS:21B9 also ends with control bytes06/0F omitted by the maintained literal.
   Colour pointer stride4, signed table conversion and code selection match.
4. Map-room DS:22E8 says `painted`, not `pointed`. The parts script executes
   before its flag write; room setup has no interaction loop. The first768 bytes
   of the uploaded decode buffer become the tile backup before map replacement.
5. Puzzle success DS:23B2 includes WHITE-code-installed and HyperPulse-ready
   lines after `Password accepted.`. Required stars are an unordered seven-offset
   set: all must be even and every extra selected star in97..F0 invalidates it.
6. Door playback compares original WORD argument with **FFFF(-1)** in all three
   tests. `.asm`'s abbreviated `FFh` immediate was previously misread as00FF;
   `.dis` explicitly confirms sign extension. Current00FF constant suppresses
   animation, sound0A and20-retrace waits for normal-1 lookup. It also wrongly
   gives positive255 playback after unbounded direct indexing. Correct the
   playback binding later. Historical door/final-consistency notes are corrected:
   this is NOT evidence of an original-game sentinel bug.

## Preserved native details

- Power coordinates use7E masks: Y0C..11 admits0C/0E/10 only. Terminal lookup
  uses7F masks and signed-BYTE Y plus signed X three-column spans.
- Overhead swaps250 bytes and is an involution with a stable selection flag.
  Native CBW temporarily sign-extends values but writes their original low BYTE.
- Passage copying leaves five untouched bytes between its three-byte rows; it
  does not set a persistent discovery flag. Phoenix discovery belongs to script.
- Return restores tiles before decoding overwrites their backup; it does not
  restore the exploration-step WORD. Puzzle failure does not clear prior WHITE.
- Door lookup accepts any negative WORD, but only exactly-1 animates. Replay
  uses the final frame and still consumes required codes without validation.
  Entrance11 clears selected codes and writes the twelfth latchD35A. Direct IDs
  have no bounds check; malformed zero requirements become signed-1 addresses.
- Door Y-region addressing has stride32 versus star toggle stride16. Four tiles
  cross column8 by adding56 beyond the normal increment into the next64-byte
  block. Each animated frame redraws and waits20 retraces.

`Verify-135DSystematicAudit.ps1` supplies static arithmetic/control witnesses and
checks13 registry records with7 matched/6 mismatched. It does not execute x86,
scripts, rendering or music. Body hashes bind these results to this checked C.
