# Sol: 1CD3 Citadel training skill access

Scope: case `0x04` (raw action `0x05`), expanded-EXE ASM `02A5–0379`.
Evidence is the local `Btech-Reko-expanded/BTECH.reko/BTECH_1CD3.asm`.
No other shopping/training dispatch blocks were rewritten.

## Values, tables and tuition

`02A5` reads selected skill BYTE `3092:D314`, sign-extends it with CBW,
then reads BYTE `3092:C618 + selectedIndex` and sign-extends that value.
These are Jason's bows/blade, pistol and rifle fields for normal indices 0–2.
The old address-of expression and nonexistent `Infantry.Skill[]` member are
replaced by an explicit FAR byte pointer and a signed WORD skill value.
`SkillOffset_2324` keeps its original numeric suffix.

The value selects a FAR description at `3EDB:4EA2 + skillLevel*4`.
The selected skill index separately selects a FAR description at
`3EDB:4EB6 + selectedIndex*4`. The latter is a single-dimensional three-entry
table, not `Skill_Level_Verbose[3][index]`.

Native `CMP ... ,4` stops training only on equality to `SkillLevel_Excellent`.
Otherwise `031B` uses signed BYTE IMUL125 and ADD75 to obtain a signed WORD
tuition cost: `75 + 125 * skillLevel`. Named training-cost constants preserve
the meaning of those numbers; 125 is not a mech record stride in this block.
Normal levels 0–3 cost 75, 200, 325 and 450 C-Bills; level 4 is refused.

## Purchase and failure paths

CWD sign-extends the cost into DX:AX. High WORD then low WORD are compared
unsigned with cash `3092:D370/D372`. JA rejects only a cost **above** cash;
an exact balance succeeds, correcting the C's strict `<` test.

`0344` sets learning BYTE `D315` to 1, increments only the selected skill
BYTE, then CWD/SUB/SBB debits the DWORD cash balance and jumps to `0961`
(shared balance-display path). It does not fall through to clear learning.
Maximum-level refusal and insufficient funds reach `030E`, clearing D315.
The existing flow is preserved, with no new wait period or training policy.

Signed selection, signed skill, BYTE increment wrap and the unsigned DWORD
interpretation of sign-extended negative costs are retained, not sanitized.
There is no original bounds validation here. Corrupt skill/selection values
may therefore index outside the description tables; this is not classified
as a reachable original-game bug without evidence of how such states arise.

## Verification and remaining scope

`scripts/Verify-TrainingDialogTranscriptions.ps1` checks every encoded skill
BYTE, affordability including exact balances, CWD conversion, WORD borrow
debits, and selected-byte increment isolation/wrap. Inputs are synthetic;
this does not execute the annotated C or establish gameplay timing.
Personnel, stock, text and menu regressions are also rerun.

Results: 5,373 training assertions, 2,816 personnel, 2,058 stock, 6,443
text and 3,175 menu assertions pass, as does whitespace validation.

This resolves the unmarked case-0x04 pointer/table/cost transcription errors.
It does not finish the entire `1CD3` dialog workflow, the callers that set
D314/D315, or the training-completion/timer lifecycle. Those need their own
bounded review. No copyrighted assets were added or modified.

Follow-up: D314/D315 callers and the two script-driven training cooldowns
are now traced in [the lifecycle review](BTECH_TRAINING_LIFECYCLE.md).
