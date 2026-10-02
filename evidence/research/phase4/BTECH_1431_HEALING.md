# Sol: 1431:000A healing workflow

Scope: the full retained healing routine000A–035C, checked against clean
expanded-EXE1431 ASM and initialized3EDB text/dice tables. The initial annotation
pass retained Reko suffixes; the subsequent in-place preservation cleanup removes
them and compiles the actual routine with a narrow test memory/UI adapter. See
[current cleanup](../preservation/1431_HEALING_CLEANUP.md). The full application
is not yet executable C. No external game assets copied or changed.

## Recovery gate and medic selection

Nonzero BYTE3092:D335 immediately branches to02FB: draw message box, scan
living characters for signed Health != signed Body*10, display recovery-needed
or nobody-wounded text, wait/read a key and return. No dice, timer reset or
medical inventory selection happens. This applies to paid doctors too.

With timer zero,0034–005E selects the greatest signed Medical skill among
living slots0..7. Initial skill/slot are0/Jason; only strict improvement changes
them, so the first tied medic wins. Zero or negative skill never beats the
initial0. If nobody has positive skill, slot0 remains the default, even if
dead; the original has no replacement-default validation. Missing C braces
had made skill assignment unconditional within the living-character branch.
The signed BYTE value and conditional braces are now reconstructed.

Raw1CD3 action17 computes D325 injury state. No injury and service argument0
displays nobody-wounded, drains pending keys then reads one key. No injury with
nonzero service argument returns silently. Neither path sets recovery timer.

## Equipment and paid-doctor override

Own-party treatment starts with torn cloth(1), uses medkit(2) if owned, and
uses field surgery kit(3) if owned and selected skill>=3. A nonzero WORD service
argument overrides skill with the service tier and equipment with medkit for
signed tiers<=2 or hospital facilities(4) for tiers>2. This is distinct from
inventory equipment IDs. No consumable-clear operation exists in this routine.

Descriptions use native FAR pointer tables, not WORD addresses. Medical skill
text maps levels1,2,3,4,5+ to First Aid, Advanced First Aid, Advanced First Aid,
Field Surgery, Hospital Surgery. This unusual level3 mapping is what the biased
native addresses do; it is not silently changed to a more intuitive label.
Equipment text is at25F2+(equipment-1)*4. Skill0 displays fumbling prose.

## Healing dice matrix

ASM reads BYTE2601+effectiveSkill*4+equipment, with equipment1..4. The real
matrix starts2602 and has eight rows. The prior4B88 expression was false.
These are initialized EXE data, not an external asset table:

| Skill/tier | Cloth | Medkit | Field kit | Hospital |
| --- | --- | --- | --- | --- |
| 0 | 01 | 01 | 01 | 01 |
| 1 | 02 | 02 | 02 | 02 |
| 2 | 02 | 02 | 03 | 04 |
| 3 | 03 | 04 | 06 | A1 |
| 4 | 06 | A1 | C1 | 82 |
| 5 | A1 | C1 | 82 | A2 |
| 6 | C1 | 82 | A2 | C2 |
| 7 | 82 | A2 | C2 | A3 |

Entries are hexadecimal. If the high nibble is zero, the BYTE is the number
of D6 rolls, multiplier1. Otherwise high nibble is the multiplier and low
nibble the D6 count. CBW/SAR4/AND0F preserves multiplier extraction even for
high-bit entries. A1=10*1D6;82=8*2D6;A3=10*3D6. Each living character whose
health differs from maximum gets an independent sequence of rolls.

01BC tests the old count then decrements; positive counts roll that many D6,
zero rolls none, and the local ends atFFFF. The maintained C's `while(count==0)`
was reversed.01C6 keeps the low WORD of multiplier*sum. Current Health is a
signed BYTE; its WORD sum is clamped only above signed Body*10, then the low
BYTE is stored. No lower clamp or corrupt-state policy was added. Overfull
health is also considered injured and can be reduced to maximum by this pass.

## Completion and payment boundary

After treating the injured party,02AF restores white text, waits/prompts,
redraws health/cash sidebar and writes D335=3F (63). It does this after the
pass, not only if a specific gain reaches full health. Exploration main-loop
updates decrement D335 toward zero once per processed world tick; no automatic
second healing award occurs on expiry. Paid and own-party treatment share it.

1CD3's paid-service parent debits before calling this routine. The recovery
and nobody-injured early exits do not refund that debit for arbitrary bare calls.
The subsequent [hospital audit](BTECH_HOSPITAL_SCRIPT_GATING.md) confirms the
shipped script clears recovery and checks injuries before charging, so those
blocked/no-injury charges do not arise through its examined treatment path.
Display/wait wrapper17EA's separate FAR declaration cleanup remains a boundary;
its misleading name does not mean it reads a key itself.

## Verification and next scope

`Verify-HealingTranscriptions.ps1` checks every encoded dice BYTE, native
post-decrement roll counts, signed medic selection/ties/dead slots, table-origin
bias, inventory/doctor overrides, normal-range health clamp/BYTE writes and
recovery/no-injury exit behavior. Models are synthetic, not execution of the
annotated C or gameplay traces; signed-overflow/corrupt-state cases still need
runtime policy review. Specialist/services and existing regressions are rerun.

Results:8568 healing assertions,3374 specialist/services,65887 lifecycle and
113 shop assertions pass, as does whitespace validation.

Hospital script gating is resolved in the follow-up audit. Next: remaining
1CD3 arena/recruitment consistency. All retained1431 healing logic now has a
first ASM-based review; this is not a gameplay-equivalence certification.
