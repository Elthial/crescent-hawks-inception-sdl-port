# Sol: specialist training, armour repair and medical services

Scope: maintained cases0E/0F,14/15,16/17,1E/1F. Evidence: expanded-EXE
`BTECH_1CD3.asm` at0BE1–0CA3,0CF8–0DC1,0EDA–1054; initialized3EDB data;
local read-only REPAIR/HOSPITAL/ARMOR BLD inspection. Only the WORD argument
contract is changed in1431; its healing implementation remains for review.
This is research pseudo-C, not a compiled port. No external assets copied.

## Tech and Medical purchases and masks

Raw0F buys Tech apprenticeship; raw15 buys Medical seminar. Both require
unsigned DWORD cash>=500, debit500 with SUB/SBB, increment the selected skill
BYTE and OR character TrainingFlags with01 or02. There is no native level
cap or prior-training check in these purchase branches. BYTE increment wraps.
The scripts gate purchase by the training flags; no new guard was invented.

D31A starts as selected character slot. Signed BYTE IMUL17 locates that
record. After updates,0C9B overwrites D31A with1 and refreshes the balance.
Insufficient funds takes0C3D, writing0. Explicit overlapping header names now
distinguish selected slot from purchase result. The C must consume the slot
before overwriting the result; it must not accidentally train character1.

Raw10 tests Tech bit01; raw16 tests Medical bit02.0BF5 stores the BYTE mask
in D31B:0/1 for Tech,0/2 for Medical, not a pointer/SEQ or normalized bool.
The malformed address-of and parenthesis expressions are corrected. Names
`Skill_Tech` and `Skill_Medical` match actual named character BYTE fields.

REPAIR.BLD0C21 calls raw10 and0C23 tests state0F=D31B;0D33 calls raw0F.
HOSPITAL.BLD0AF0 calls raw16 and0AF2 tests state0F;0AF6 calls raw15.
Character+10 flags and script state result bytes are distinct storage.

## Injury query and medical service

Raw17 scans slots0..7, skipping NameFF.0D29 compares signed Health BYTE
with signed Body BYTE times10 and marks D325 for **inequality**, not only
under-health. Address-of expressions were false Reko reconstruction.

Raw18 CBW-extends service tier D326. Tier0 calls1431:000A with WORD0 for
own-party healing and charges no fee. Nonzero tier reads a signed WORD from
4F6E+tier*2, CWD-extends it to DWORD, compares cash unsigned, and debits before
calling the healing helper. The eight initialized WORD fees for tiers0..7
are0,50,100,150,200,400,600,750. HOSPITAL.BLD05EE–061B stores tiers0..7;
061E calls raw18. This is a service tier, not simply an inventory equipment ID.

On insufficient funds,0D92 displays the existing failure text, waits for
input, then **credits25 C-Bills** with ADD/ADC before refreshing balance.
This really exists in ASM. The follow-up hospital audit identifies it as a
refund of the25 facilities fee prepaid at HOSPITAL040A. No healing occurs on
this path; it is not a gift/exploit in that shipped context.

1431 compares and loads WORD[BP+06], so its BYTE argument declaration is
corrected to WORD. Its recovery gate, best-medic selection and health formula
are **not** certified by this parent pass. The parent debits before the helper
checks recovery/injury. The follow-up hospital audit confirms the script
clears recovery and checks injuries before charging; bare callers must honor
that calling convention.

## Armour query and point-by-point repair

Raw1F copies selected armour type BYTE into D32B, not its address. Raw20
sign-extends selected slot, armour type and remaining durability. Full
durability lookup4DDC+(type-1) is equivalent to4DDB+type. Equal durability
displays the original brand-new message and waits, without prompting/payment.

Missing points are full minus current. Repair rates are signed BYTEs at
4F3E+(type-1), equivalent to the old biased4F3D+type expression. Five real
rates for armour types1..5 are1,2,5,20,20. The former six-entry declaration
included a preceding price BYTE as a fictitious type0 rate; the real table
now has an explicit address and five entries. This is an overlay cleanup,
not a claim that the old type-indexed arithmetic was numerically wrong.

The displayed quote is IMUL's low WORD missing*rate. After yes/no confirmation,
each affordable point independently debits the CBW/CWD-expanded rate,
increments armour BYTE and refreshes cash. Exact balances succeed; partial
work is retained. SS:BP-2C tracks whether **any** point was repaired, selecting
partial-job versus no-work dialogue when cash runs out. It is not a boolean
for affordability of the entire quote. Final/partial/no-work messages and the
terminal key wait are preserved with original FAR string addresses.

No new bounds/sanity check, repair cap, all-or-nothing charge, free healing
or refund was introduced. Invalid/corrupt armour values retain native risks;
ordinary nonnegative missing-point cases are the synthetic test domain.

## Checks and next boundary

`Verify-SpecialistAndServiceTranscriptions.ps1` covers every training-mask
BYTE, selected-slot isolation and BYTE skill wrap,500 exact-balance purchase,
partial repair versus a numeric oracle, signed WORD fee expansion and the
25-credit failure path. These are synthetic instruction models, not executable
C or gameplay traces. Existing shop/training/lifecycle regressions are rerun.

Results:3374 specialist/service assertions,113 shop,65887 lifecycle,5373
training,2058 stock and2816 personnel pass; whitespace validation passes.

Next: review1431:000A's actual healing workflow, especially recovery gating
and medical skill/equipment selection. Remaining1CD3 arena/recruitment and
unmarked branches still need their own consistency pass.

Follow-up: the healing helper's internal workflow now has its first ASM-based
review in [the1431 healing notes](BTECH_1431_HEALING.md). The subsequent
[hospital script review](BTECH_HOSPITAL_SCRIPT_GATING.md) resolves recovery
reset, facilities prepayment and refund ordering.
