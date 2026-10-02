# Sol: systematic BTECH_1467 C-to-ASM comparison

Checked2026-09-17 against baseline `25e5592`: all four retained methods.
Two locally matched; two mismatched. Crew assignment additionally retains an
unresolved DS/SS effective-address binding. Executable C remains unchanged.

## Evidence and contract

Private `BTech-Reko-expanded/BTECH.reko/BTECH_1467.asm` SHA256:
`ACAA8B1FA061BD07B3B9DD18F7F877B35EA7E44999ADA162F2AD873A5A523731`.
Corresponding `.dis` SHA256:
`04FD640AB1A4FB534D51CAA48F057623799B3B10F352F3E2338AB261D8AC3128`.
Read the whole segment, including the dormant quiz encoded as raw bytes and
the movement-clear return retained in `.dis`. Read3EDB native text, selectors
and quiz tables. Compared meaningful arguments, reads/writes, temporaries,
branches, loops, calls and exits. Compiler frame/probe/saved-register machinery
is abstracted. Shared callees have documented contracts, not fresh certificates
from this review. EGA adapter2 is the maintained target; quiz adapter0 table
conversion is deliberately omitted. Literal text requires a platform text binding;
embedded native formatting must still be preserved. Memory views require native
BYTE/WORD and16:16 FAR offset semantics, not arbitrary host struct layouts.

## Whole-method ledger

| Method / range | Result | Checked local workflow |
| --- | --- | --- |
| Assign_Pilot_and_rider_to_Mechs /0002–08A7 | Mismatch; extra unresolved binding | Reset or synchronize seats; compact character/mech maps; both panels; Done checks, pilot-count/abandonment branches; detach old seats; vehicle/seat choice; displacement; traitor warning; write assignments; repeat and final snapshot cleanup |
| Copyright_Check_Mech_Quiz /08A8–0B97 | Matched under EGA/native call contracts | Conditional asset load, decode/upload; intro; three distinct random parts,20 answer labels, target rectangle/connector, answer check; screen/sidebar cleanup; sticky failure and cooldown; return |
| Display_Text_Mech_Names /0B98–0D7D | Mismatch | Clear compact mapping; choose saved/live initials; build rows; direct pilot-slot name prefix; hidden-name reconstruction; selection menu and selected global; return row count |
| Combat_Clear_Movement_Plans_0D7E /0D7E–0DC3 | Matched |24-by24 BYTE zeroing at3092:40B4..42F3, including complete return |

## Corrections queued, not implemented

### Assignment text and memory widths

Native2622 ends06/0F/CR, not two spaces.2643 on-foot,264D pilot,2655 rider
and265D Done endCR.2663/2675 seat headings contain CR and06/0F then06/02
colour controls, rather than substituted spacing.2687 starts06/0F and ends
period plus two spaces;26AF and2740 start06/0F.275B endsCR. Missing controls
change line layout and text colour; the simplified menu is not fully equivalent.

Explicit near `Mech *`, `Infantry *` and `unsigned char *DynamicText` locals
cannot preserve native3092 FAR addresses when DS is the3EDB text/table segment.
Represent these as proper FAR/native-address views in a later correcting pass.
Living character Name bytes are CBW-sign-extended before FAR name-table indexing;
assigned mech indexes are also CBW, unlike current unsigned BYTE indexing.
Traitor D331 is signed by CBW before comparison. Valid ordinary IDs agree;
high-bit corrupt values do not. Seat IDs themselves use unsigned MUL/zero
extension as written: do not indiscriminately make every index signed.

The final0838 row-map loads use BX+BP with implicit **DS**, unlike earlier
BP+SI addressing using **SS**. Current C assumes ordinary local arrays for both.
The `.dis` agrees with DS; a later exploration snapshot with DS!=SS does not
resolve addresses at this assignment point. Keep A-003 open and capture this
block's DS,SS,BP and effective addresses before claiming exact final write-back.
No new original-game bug is asserted from that unresolved binding.

### Mech-selection memory views

`PartyMech` and hidden-name `DynamicText` have the same explicit near-pointer
loss of native3092 segment. Remaining local operations match under proper FAR
views. Its direct unsigned PilotId index into CharacterNameList is genuinely
native; this is different from assignment's Infantry[PilotId].Name lookup.
No FF pilot guard or zero-row menu guard exists here. Do not silently change
those native behaviors during a transcription correction.

## Confirmed details worth preserving

- Reset clears all eight character assignment bytes to8 and both seats of all
  four mechs toFF. Synchronization mode does not first clear assignments and
  eligibility is exact Name[0]==FF, not a generic high-bit test.
- Local character maps are initialized once; live rows are overwritten on each
  redraw. Vehicle maps are reset each vehicle menu. Menus require their native
  valid-row contract; there is no extra cancellation/rollback path here.
- Dynamic mech text trimming uses native strlen minus1, stops at the leading
  separator, and removes trailing spaces. Vehicle-name scratch is truncated at
 001D, but the displayed pointer still addresses the original mech name.
- Done requires every existing mech to have a pilot. Insufficient qualified
  personnel permits abandonment after confirmation; only name sentinel and
  linked character assignments change. Menu repeats afterward. Final success
  clears four D452 snapshot bytes and D324, not whole mech records.
- Old seats are detached before reading the vehicle choice. Qualified personnel
  auto-take an empty pilot seat or consent to replacing it; otherwise ride.
  Replaced pilot moves to rider, dismounting any old rider. Rider replacement
  dismounts only that rider. Traitor warning sets WORD374A and does not veto.
- Quiz raw bytes initialize question number0, not81. Retry resets duplicate
  flag each random draw. Random returns a zero-extended BYTE, so C %10 matches
  signed IDIV10. Three parts are distinct; incorrect answers remain sticky.
  Target X/Y and answer rows are ten-WORD tables28C4/28D8/28EC; twenty labels
  are FAR pointers4DE2. Four lines bound X/Y-3 through+5; connector is a filled
  rectangle from(128,Y-1) to(X-3,Y+3). Fail writes BYTE D320=FF; return0/1.
  Dormancy is a caller/profile fact, not missing behavior inside this routine.
- Mech selection restores hidden first initials into temporary strings without
  changing live names. WORD0068 holds selected mech; AX returns number of rows.
- Movement-plan clearing writes576 bytes to zero, not the02 end-plan sentinel.

Static witnesses check compact mappings, reset/synchronization semantics,
distinct quiz selection, table geometry and576-write coverage. These models
do not execute x86 or validate menus/gameplay. Registry body hashes bind results
to this checked C. No expensive model call was used; A-003 remains the existing
specialist/runtime investigation rather than being declared resolved.
