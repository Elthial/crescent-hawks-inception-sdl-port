# `BTECH_0FDC` mission teardown

## Review boundary

This block covers `Mech_Mission_0629` from `0FDC:0BF2` through its return at
`0D48`. It computes training outcomes, applies skill gains and Kurita-attack
state, restores temporary map bytes, clears the jailbreak overlay, and restores
the active BLD when necessary. The next routine begins at `0FDC:0D49`.

## Training result byte

Every mission first clears byte `3092:D30D`. Its previous scratch-pad name,
`Bool_HasNonLocustTrainingMech`, described only one branch and obscured the
shared role. The byte is now named `Bool_TrainingMissionPassed` because missions
0 through 7 set it under these mission-specific success conditions:

| Mission | Condition which sets `D30D` |
|---:|---|
| 0 | Objective resolved and the adjusted signed timer is below `0x00D7`. |
| 1 | Jason's mech-name byte is not `L`; the Locust cannot retrieve the rubble. |
| 2..7 | Jason's mech record is not destroyed and combat WORD `3092:3992` is zero. |
| 8..9 | No condition; `D30D` remains zero. |

No code in the reviewed executable reads `D30D`; all static references are the
four writes in this teardown. It is a persistent saved-state byte and may be a
vestige of removed training-result handling. Its exact set conditions are
verified even though its absent downstream consumer is unexplained.

The meaning of combat WORD `3092:3992` is not yet sufficiently stable to rename.
It participates in escape and targeting paths elsewhere, so the existing
`Bool_EnemiesWithinRange_3992` name should not be treated as proven semantics.

## Mission 0 grading and restoration (`0C18-0C44`)

If Jason selected the Locust (`Name == 'L'`), the game subtracts `0x0032` from
the WORD mission timer before grading. The final comparison is signed (`JGE`),
so the maintained source now makes that signed interpretation explicit:

```text
passed = objectiveResolved && (signed short)adjustedTimer < 0x00D7
```

The subtraction raises the effective Locust time allowance by 50 processed
updates. Finally the original byte saved from map location `246C:200A` is
restored.

## Mission 1 grading, tile restoration, and skill (`0C45-0C7F`)

The selected rubble tile is restored unconditionally from the saved byte. A
successful non-Locust run sets the training-result byte. Jason's piloting byte
is sign-extended into a WORD; if it is below level 3 (`SkillLeveL_Good`), it is
incremented once and its low byte is stored back. This skill increase is not
conditioned on the pass-result byte in the teardown itself.

## Missions 2 through 7 (`0C80-0D1C`)

These missions set the pass result only if Jason's mech-name byte is not `FF`
and combat WORD `3992` is clear.

Missions 5, 6, and 7 then process gunnery and piloting independently. Each skill
byte is sign-extended to a WORD. If the current value is below level 4
(`SkillLevel_Excellent`), a fresh random low bit (`0` or `1`) is added. The two
skills therefore have independent 50-percent chances of gaining one level.
These rolls occur regardless of whether the pass-result condition was met.

When the WORD Kurita interruption flag at `3092:3772` is nonzero, teardown sets
the destroyed-Citadel byte `D310`. If Jason's mech is still present it also sets
`D311`. The old `Bool_TrainingMech_Destroyed` name contradicted that condition;
it is now `Bool_TrainingMechSurvivedKuritaAttack`. Like `D30D`, `D311` has no
static reader in this executable. The interruption WORD is cleared after this
handling for every mission in the `2..7` range.

## Common cleanup and shared asset buffer (`0D1D-0D48`)

All mission numbers clear WORD `3092:398E`, disabling the four parked jailbreak
'Mech sprites. If WORD `3092:4594` is nonzero, BTSTATS currently occupies the
buffer normally used by the active BLD. The routine reloads the BLD selected by
`3092:3FF8` before returning.

Assembly proves both the saved BLD index and the callee parameter are WORDs:
teardown pushes `word ptr [3FF8]`, and `Load_And_Decode_Indexed_BLD_1D30` reads `[BP+06]`
as a WORD and stores AX back to `3FF8`. The prior `unsigned long` field and
`unsigned char` parameter were Reko type extrapolations and have been corrected.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0BF2-0D48`;
- signed timer and sign-extended skill comparisons at `0C24-0C76` and
  `0CA4-0CE3`;
- exact `D30D`, `D310`, and `D311` writes at `0BF2-0D0C`;
- the BLD restore call at `0D1D-0D43` and callee argument handling at
  `1D30-1D6C`;
- an all-file static-reference search for the write-only outcome bytes.

The dispatch ranges, pass conditions, skill arithmetic, map restoration,
Kurita-state writes, common cleanup, and BLD index width are directly verified.
The broader meaning of combat WORD `3992` and the intended consumers of the two
write-only outcome bytes remain unresolved. No Astra review is required.
