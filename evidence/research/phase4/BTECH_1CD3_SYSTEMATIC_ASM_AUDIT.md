# Sol: systematic BTECH_1CD3 C-to-ASM comparison

Checked2026-09-17 against baseline `ae3ea18`: all five retained methods.
Three locally matched, two mismatched. The dispatcher includes all47 jump-table
actions; checking it does not mean all47 action transcriptions match.
Executable C is unchanged. Sol comments and fresh registry entries queue fixes.

## Evidence and boundaries

Private `BTech-Reko-expanded/BTECH.reko/BTECH_1CD3.asm` SHA256:
`4860628C7A56152FFBC530E2FFEE1A1BE8D126D7FCFC355665D3E77AFEBC2C5A`.
Corresponding `.dis` SHA256:
`0E02E6803C30C7753D6DCEBE729E05D64908C9B196990A2A3C1DE378E768F4EE`.

Read the entire ASM segment, all switch destinations and helper bodies, the
shared-label paths, `.dis` fire-cleanup branch and final181E return, native3EDB
strings and selector words. Accounted for meaningful arguments, locals, reads,
writes, arithmetic, branches, loops, call arguments, side effects and returns.
Stack probe/Alloca and saved-register/frame mechanics are compiler abstractions.
Shared callees have contracts, not new certificates from this file's review.

Memory views require native BYTE/WORD/DWORD and16:16 FAR bindings, not the physical
layout of the scratchpad header. Native DWORD money is32 bits and wraps; short
is16 bits. The maintained graphics contract is EGA adapter2. Adapter0 conversion
and colour-map switches and adapter3 palette sources are deliberate omissions,
not newly invented missing game logic. Literal text is platform-bound but its
content/control bytes still need to agree.

## Whole-method ledger

| Method / range | Result | Local workflow checked |
| --- | --- | --- |
| Citadel_Building_Dialogs /0004–17C5 | Mismatch | Unsigned raw-action gate; all47 destinations; training/finance/shop/personnel/services/arena/story/NPC/ending flows; shared text/crew/exit paths |
| Draw_Message_Box /17C6–17E9 | Matched under shared-call contracts | Layout7, top sidebar, border0 and return; no input wait |
| Display_Text_From_Memory_ScreenRetrace_KeyboardInput /17EA–1808 | Matched under FAR/shared-call contracts | Forward both pointer WORDs to03F5, then086A timed presentation/input bridge; no separate ASCII read |
| Display_Text_Shop_Cannot_Afford_Text /1809–181D | Mismatch | Inline3EDB:4F7E display and return; literal period omitted |
| Display_Text_4FA0_Value /181E–1832 | Matched under FAR/shared-call contracts | Inline3EDB:4FA0 CR/NUL byte array, not stored-pointer dereference; complete return from `.dis` |

## Every dispatcher action

Raw actions below are F5 operands. Current C labels are raw action minus one.
Table1762 contains47 WORD destinations, not47 independent methods. A row's
subflow agreement is conditional on proper memory/callee contracts; only the
whole parent method receives a registry status.

| Raw | Native entry | Checked operation / discrepancy |
| --- | --- | --- |
|01 |0027 | Select Locust/Wasp/Chameleon template, copy125 bytes, bind pilot, march and launch training; timing gate signed versus unsigned C |
|02 |0187 | Debrief name/prose; initial-byte accesses and Locust/suffix text wrong |
|03 |01C2 | Retire training mech, return to Citadel, conditionally clear fire; C reverses D310 condition |
|04 |0231 | Three signed skill scores125*level+75; native right alignment missing |
|05 |02A5 | Skill descriptions, excellent-level refusal, tuition, increment/result flag; text wrong, FAR text-table decoding needed |
|06 |037A | Cash and nonzero stock display/count; right alignment and WORD row lost |
|07 |0725 | Stock investment; unsigned DWORD clamp and transfer agree; text/alignment discrepancies |
|08 |04AD | Stock sale; no-stock/refusal/zero branch and clamp/transfer traced; text/alignment discrepancies |
|09 |09C7 | Armour category selection, DWORD affordability/payment, durability CBW and distribution; local arithmetic/order agree |
|0A |096F | Cash within dialogue, colour and06/0F period suffix; EGA local flow agrees |
|0B |0A4B | Weapon category offsets6/9, price index decrement, DWORD payment and distribution; local flow agrees |
|0C |0AEA | Building-occupant dialogue call |
|0D |0B26 | Recount through raw0E, compact living-slot menu, row selection; name ID signedness differs |
|0E |0AF2 | Count living companions1..7 into D31A, excluding Jason |
|0F |0BFC |500-cost Tech training, BYTE skill increment/training bit, D31A result and balance; agrees |
|10 |0BE1 | Selected slot's training BYTE AND1 to D31B; agrees |
|11 |0CA4 | Four visible mech initial-byte count; C compares arrays withFF |
|12 |0CD8 | Mech repair call |
|13 |0CE8 | Mech modification call |
|14 |0CF0 | Mech upgrade call |
|15 |0C5C |500-cost Medical training, BYTE skill increment/bit/result and balance; agrees |
|16 |0C46 | Selected slot's training BYTE AND2 to D31B; agrees |
|17 |0CF8 | Any living party record with signed Body*10 != signed Health sets D325; agrees |
|18 |0D44 | Medical fee/service or fee-free party healing; charge/refund paths and balance display agree |
|19 |0DC2 | Set medkit BYTE D450 |
|1A |0DCF | Set surgery-kit BYTE D451 |
|1B |0E54 | Rebuild four friendly sprite families and call crew assignment0; C initial-byte accesses wrong |
|1C |0DDC | Save/hide four mech initials, set eight characters on foot; C initial-byte accesses wrong |
|1D |0E9B | Count saved uppercase initials; unsigned A..Z C gate is equivalent for all256 bytes |
|1E |0ED2 | Rex recruitment/ambush call |
|1F |0EDA | Selected character armour-type BYTE query |
|20 |0EF3 | Armour deficit/quote/prompt/per-point debit and increment/partial result; local flow agrees |
|21 |1052 | Rental Locust preparation call |
|22 |105A | Party mech arena preparation call |
|23 |1062 | Arena mission8, effect cleanup, alive latch, normal return or escape relocation/cache/crew reset; legacy array-as-value errors and mech initial-byte accesses |
|24 |13B2 | Traitor slot signed lookup, name CBW, FAR name-table display |
|25 |139A | Clear main-alive WORD and Jason Name BYTEFF; does not zero Health or latch D334 here |
|26 |13C6 | Next recruit name CBW and same FAR display tail |
|27 |13E6 | Crew assignment for WORD slot1 |
|28 |13EC | Jailbreak helper and dead-main-character latch |
|29 |140D | Laser grants/swap and signed randomized health cap; local flow agrees |
|2A |1515 | Stage eight26-byte roaming NPC records/actor16..23 positions; increment party Y WORD; agrees |
|2B |157E | Stage NPC0 from interactable7 toward interactable0, delayFF/waypoint70; agrees |
|2C |0CE0 | Ammo purchase call |
|2D |15DD | Disk2 ending asset load/decode/stage/palette/show, Star League tile restoration, wait/black/default palette/sidebar; EGA ordering agrees subject to raw buffer bindings |
|2E |0E2C | Restore four initials then rebuild sprites/crew; C initial-byte accesses wrong |
|2F |174E | Signed Health>5 then BYTE subtract4; C incorrectly changes TrainingFlags instead |

## Parent corrections queued

### Fire cleanup and initial-byte accesses

Native01F4 exits when D310 is nonzero;01F7 scans64 effect slots only when D310
is zero. The C `if (Kurita_DestroyedCitadel)` does the reverse. Native clears
X-low, packed page and sprite for masked family7C, leaving Y-low intact; the
current scan's internal operations agree. This is a C transcription discrepancy,
not an original-game bug. `.dis` independently confirms the zero branch.

Current Mech.Name is BYTE[16], but debrief, retirement, visible-count, hide/save,
escape restoration and sprite-rebuild paths compare/assign the array as if it
were one BYTE. Native accesses only the record's first BYTE. Use Name[0] during
fixing; do not replace/copy all16 name bytes or treat FF as destroying125 bytes.
Training-template copy and explicitly FAR relocation byte loops already carry
the intended125-byte record contract.

### Layout, widths and signed indexing

Transcript/finance draws natively use X=`39 - strlen(formatted text)`, not fixed21.
This affects skill scores, cash, each stock balance and the sale/invest balance
footer. The strlen return is currently discarded. Native stock row is full
WORD374E; VerticalOffset_2261 is BYTE and truncates it. Ordinary row values may
hide that width discrepancy but not the horizontal alignment difference.
The six-redraw training calibration test is native signed WORD>8 versus current
unsigned CpuTimingCalibration; high-bit values differ, ordinary calibration agrees.
Living name IDs in the compact party picker are CBW signed; current NameId is
unsigned. Slot/count metadata and the SS:BP-8 row-map binding otherwise agree.

Current StockMarketNames/Skill_level_Descriptions/Skill_Level_Verbose and name
tables are scratchpad DWORD encodings. Native loads their two WORDs as a FAR
pointer; passing the numeric DWORD directly to a pointer parameter does not
express that conversion. Fixed string literals likewise require a documented
FAR platform binding. Native WORD-shifted table offsets wrap; arbitrary host
array indices are not a substitute for invalid-input native memory semantics.

### Escape array-as-value expressions

At116E native reads WORD4036; if WORD406A==0 it instead reads WORD403E.
C assigns CharacterPosY_Mech and CharacterPosY_OnFoot arrays to a WORD and
compares Character_OnMap array withFALSE: these are addresses, not values.
Use CombatantPackedY_4036[0]/[4] and CombatantActive_406A[0] in fixing.
The remainder of escape relocation was checked: first vacant companion slot,
125-byte backup copy and initial restoration; if no vacancy restore original
slot0, if no saved mechs keep arena slot0. Rebuild cache at89 from map-file rows
78/88/98, load nonzero file enums with CBW, apply Starport patch, clear saved
initials/crew pairs and eight assignments, then clear rental mode. Normal return
instead removes/randomizes the arena patch and uses the party-restore helper.

### Native text versus current simplified dialogue

| Offset | Native requirement / difference |
| --- | --- |
|48B3 | Full Locust debrief includes `, the results are hardly surprising. `; C only says Locust |
|48EE | Comma immediately before space/text and final trailing space; C inserts a leading space and omits the final one |
|491A | `By looking at your transcript, I see you are `; current text changes meaning and cannot correctly precede the skill description |
|4948 | Includes `any more`; C drops `any` |
|4999 | Two leading CRs, correct C-bills/tuition spelling, period and two trailing spaces; all differ |
|49D3 | CR + None, not None-period |
|49D9 | Two CRs + no-stock statement with final period |
|49F6 | Sell prompt ends CR |
|4A09/4B52 | Two CRs before amount prompt, two trailing spaces |
|4A1C | Stock account: casing/no trailing space differs |
|4A2B/4B77 | Two CRs before clamp narration; sale prose also simplified in C |
|4A6A | Two CRs + `You have sold a total of `, not `You have a total of ` |
|4A86/4BD8 |06/0F before ` of ` / ` in `; investment also loses trailing space |
|4A8D/4BDF | Native `bringing your balance in that stock to `; C substitutes words/spacing |
|4AC0/4C0B | Two CRs then You sell/You invest, no invented leading/trailing spaces or capital Invest |
|4ACB/4AF5 | Two CRs before refusal text; cash refusal uses capital C-bill |
|4F7E | Cannot-afford helper ends period; current literal lacks it |

4ABC/4C07 are already passed through DS+native offset0957, preserving06/0F/period.
4C18's raw FAR literal already preserves its controls.4B3B/4B67, Wasp/Chameleon
names and raw FAR repair strings match.2D ending filenames/palette sources agree
under the EGA binding; no external asset content has been copied into this audit.

### Raw2F updates Health, not TrainingFlags

Native174E/175A explicitly read/subtract BYTE3092:C623. The character record
beginsC614: Health is offset0F=C623; TrainingFlags is offset10=C624. Scenario29
already correctly accesses that sameC623 as Health. Current raw2F C instead
uses TrainingFlags and misleading CharacterTraining_* constants. Its byte
predicate/subtraction matches in isolation but it reads and writes the wrong
field. Correct to signed Health>5 then Health-=4 during fixing, with names that
describe the health threshold/cost. Do not infer bit clearing or an unidentified
training bit from subtract4. The historical FINAL_CONSISTENCY flag interpretation
is superseded. Direct ASM/layout evidence establishes the affected field;
narrative purpose of this small health deduction still needs caller/gameplay context.

## Confirmed important subflows

Dispatch is unsigned `(Action-1) <= 2E`: raw0 is rejected asFFFF, not case0.
Tuition uses signed skill*125+75, CWD to DWORD and unsigned cash comparison;
exact cash succeeds, excellent is equality4, increment wraps BYTE, result D315
is sticky only as each path writes it. These current arithmetic operations agree.
Stocks use DWORD values, selected index*4, unsigned clamping, zero-transaction
helper and matching transfer order with native wrap; only presentation/bindings
above prevent matching the parent. Purchases debit before distribution; no rollback
is introduced when the recipient helper refuses. D31A is reused as count/slot/
purchase result, and D31B is mask0/1 or0/2, not canonical TRUE1 for both skills.

Medical service tier0 calls healing without final balance display. Nonzero tier
sign-extends a signed WORD fee, debits before healing and displays balance;
insufficient funds invokes linebreak/refusal/timed wait/key read then refunds25
and displays balance. This composes with the hospital's prepaid facilities fee,
not an unconditional direct-call gift policy. Armour repair charges one affordable
point at a time, increments BYTE armour and updates balance; it retains partial
work, does not require payment for the whole quote, and reports none/partial/all.
Negative/corrupt deficits retain native WORD-counter behavior; no clamp was added.

Saved uppercase initial tests agree for all256 bytes despite different signed
syntax. Scenario29 grants0..signed companion count inclusively and rereads count;
health is capped at signed Body*6 or*7, not healed. NPC26-byte strides, destination
preservation in2A, destinations7->0 in2B and party Y wrap agree. Raw2F's field
discrepancy is documented above. Story meaning of the health deduction and
health cap remains uncertain, but this audit does not need an
expensive Astra run to establish their directly visible instructions.

## Validation and follow-up

Verify-1CD3SystematicAudit.ps1 checks dispatch, reversed cleanup, row alignment,
score/cost sign extension, WORD/BYTE widths and all47 retained C cases/registry.
Existing stock/training/staging tests are regression checks, not original gameplay.
Three matched helper records certify local semantics under the stated bindings,
not compilation or integration. Corrections, gameplay verification and previously
queued callee mismatches remain separate work.
