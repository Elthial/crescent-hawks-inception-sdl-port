# Generated variable suffix cleanup

Sol: checked the annotated `Btech` C/header files and the preservation
`Reimplementation/src/Original` C/header files for generated decimal variable
suffixes, including the requested `_2535` example. That example no longer
occurs, and active variable names in these two trees have already been cleaned.

This follow-up removes stale generated names from explanatory comments and
gives the remaining address-style goto targets descriptive names:

| Native location | Label |
| --- | --- |
| 1CD3:0181 | `FinishScriptAction` |
| 1CD3:0957 | `DisplayStockTransactionResult` |
| 1CD3:0961 | `DisplayServiceBalance` |
| 183B:0826 | `PlanAndExecuteEnemyTurn` |
| 0800:044A | `UpdateExplorationAnimationAndNpcs` |
| 1AE8:0DC4 | `RestorePartyMapAnchorAfterAttack` |
| 1AE8:0DDA | `AdvanceWeaponSlot` |
| 1AE8:1B0C | `ApplyAttackCasualtyAndWreckState` |
| 1AE8:142F | `SelectMissileImpactAnimation` |
| 1AE8:19EA | `PlayNonMissileAttackSound` |
| 1AE8:1A07 | `RenderAttackImpact` |
| 1AE8:1AEB | `RestoreShooterAnimationFrame` |
| 1CD3:070D | `DisplayNoStockTransaction` |
| 1CD3:01B5 | `SelectScriptDialogueText` |
| 1CD3:01BC | `FinishDialogueAction` |
| 1631:08F9 | `AssignWeaponTargetsAndMeleeOverrides` |

No preservation C logic needed changing. No expressions, storage layouts,
branch destinations or original bugs were changed. The affected segment
audit scripts pass their static witnesses; this is not gameplay validation.

Method address suffixes (for example `DrawCall_EGA_CharacterPos_0377`), critical
slot indices and weapon designations are not generated variable suffixes.
They remain intact. Historical scratch-pad expressions in `BTECH.h` and
`BTECH_3092.c` have now been replaced with plain-language record-copy,
assignment and enemy-retirement notes, removing the last generated local
variable suffixes from comments as well. The untouched expanded
Reko output remains the historical cross-reference, not a cleanup target.
