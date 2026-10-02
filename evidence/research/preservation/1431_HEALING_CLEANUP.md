# Sol: in-place preservation cleanup — healing

Baseline `f72d487`. Clean the existing `Heal_Characters` at `1431:000A` in
`Btech/BTECH_1431.c`; retain its name, address and callers. No duplicate game
implementation, architectural split or original-game bug fix.

## Naming and rules

The game uses character Health with a maximum of Body × 10. Do not rename this
to the tabletop MechWarrior six-hit injury track, or assume identical tabletop
medical rules. Medical skill, medical equipment and D6 healing are the relevant
game concepts. UI layout, FAR text addresses and timer ticks remain system names.

| Former local | Clean local |
| --- | --- |
| MedicalEquipmentLevel (argument) | medicalServiceTier |
| EffectiveMedicalSkill_430 | effectiveMedicalSkill |
| BestSkilledMedicId_431 | partyMedicId |
| PartyMember_432 / PartyMemberId | partyMemberId |
| MedicalEquipmentOption_443 | medicalEquipment |
| Character_BodyStat | maximumHealth |
| HealingAmount_MedicalSkill | healingRollEntry |
| NumberOfHealingActions | healingDiceRemaining |
| HealingMultiplier / HealingDiceSum | healingMultiplier / healingDiceTotal |
| HealingAmount / AmountHealed | healthRecovered / healthAfterTreatment |

Eight slots are `FriendlyCharacterCount`, not four-Mech LanceSize. A paid
medical service tier is not an equipment ID: `MedicalService_UsePartyMedicAndEquipment`
selects party treatment; signed tiers above `MedicalService_LastMedkitTier`
use hospital facilities. `MedicalSkill_HospitalSurgeryTier` is the minimum
effective tier that displays Hospital Surgery, not a new mechanical healing rule.
Name the injury-query action, menu layout and description-table biases rather
than removing their meaning. Keep exact EXE text readable, with original address
comments; do not substitute an address for a known sentence.
Persistent global/table member names remain for their own later cleanup passes.

## ASM equivalence and preserved quirks

Re-read the complete `1431` ASM and its return in `.dis`. Keep signed BYTE
skill, Body, Health and name loads; WORD calculations and low-BYTE stores.
The packed roll entry is unsigned. `HealingRoll_DiceCountLowNibbleMask`
keeps the low hexadecimal digit as the D6 count; shifting by
`HealingRoll_MultiplierHighNibbleShift` moves the high digit into the multiplier.
For example A2 means 2D6 x 10, while 82 means 2D6 x 8. A zero multiplier digit
defaults to one. This produces exactly native CBW/SAR4/AND15 results for all
256 possible entries without the redundant multiplier mask or nested expression.
There are at most 15 D6 and multiplier 15; arithmetic stays in signed WORD
range for all Body/Health bytes. Reload Health after the display/dice calls,
matching native `01D9`, instead of relying on those helpers not changing it.

Keep strict best-medic comparison and first-tie selection, slot-zero default,
dead-slot skip, inequality injury check, upper-only clamp, overfull-health
reduction, no equipment consumption, no billing inside this method, and timer
reset to 63 even for zero gain. Early returns do not reset the timer. Do not
add validation for malformed service tiers or repair caller-level billing bugs.
Valid table/index bounds and native FAR memory views remain explicit porting
contracts; malformed wrapped addresses are not emulated by host C arrays yet.

## Verification

`scripts/Verify-PreservationHealing.ps1` compiles the actual file as C17
with `/W4 /WX`, substituting only its shared header with a minimal test
memory/UI adapter. Constants are extracted from the real shared header.
Tests cover all packed healing BYTE values, all 65,536 Body/Health BYTE pairs,
party equipment gates, paid doctor tiers, description selection, tied medics,
dead slots, recovery/no-injury exits and the post-dice health reload.
Result: 789,731 checks passed. These include instrumentation assertions, not
789,731 distinct gameplay scenarios. Not an emulator comparison or full-game build.

Readability correction: restore all nine healing text locations from the original
EXE, rather than reconstructed prose. For example252D is only `The doctor`;
2538 appends ` uses his `. The shared256F fragment is ` to assist the wounded.`,
not a results heading. See [text restoration audit](EXE_TEXT_RESTORATION.md).

## Following cleanup order

1. Mechlub repairs and service actions.
2. Salvage.
3. Mech upgrades/modifications.
4. Combat workflows, using BattleTech rules terminology where the binary supports it.

Preserve original faults in every block; correct only decompiler discrepancies.
