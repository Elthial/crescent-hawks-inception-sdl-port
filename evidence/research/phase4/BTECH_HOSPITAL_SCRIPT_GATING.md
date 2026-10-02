# Sol: hospital script gating and facilities refund

Question: can the shipped hospital charge for doctor treatment while1431's
recovery gate blocks healing? **No, through the examined shipped script path.**
HOSPITAL.BLD clears the recovery countdown before checking injury or fees.

Evidence: read-only local HOSPITAL.BLD disassembly,0FDC:02E9 F4 state write
at3092:D30C+index,1CD3:0D44–0DBF service payment/refund,1431:000A recovery
gate. Offsets below are decoded payload offsets. External asset text/data is
not reproduced as a committed fixture; documentation describes control flow.

## Treatment path

| Offset | Operation and consequence |
| --- | --- |
| 02F2 | F4 state29=0: clears3092:D335, even if recovery was active |
| 02F5 | Raw action17 computes living-party injury stateD325 |
| 02F7 | F7 state19: injured goes0315; otherwise message/key and return0000 |
| 0378 | Require cash>=25; insufficient facilities funds returns0000 |
| 040A | Subtract25 C-Bills facilities fee |
| 040D | State27 can deny professional help; then use own-party treatment |
| 04EA | Hire-doctor prompt: yes opens tier menu, no goes04ED |
| 04ED | Set state1A=D326 to0 for own-party treatment |
| 05E2 | Tier menu selects1..7; Cancel also goes04ED, not a fee-refund exit |
| 061E | Raw action18 invokes native service payment/healing |
| 0620 | Return to hospital root0000 |

Successful own-party treatment costs the25 facilities fee. Successful doctor
treatment costs25 plus tier fee50/100/150/200/400/600/750. Doctor-tier0 is
not a professional: it invokes own-party healing without an additional fee.
Cancel in the doctor menu means use own-party healing; it does not cancel
the already-paid facilities visit. The same recovery reset applies to both.

If cash after facilities payment is below the selected doctor fee,1CD3:0D92
does not heal and adds25 back. This refunds the prepaid facilities fee;
the earlier isolated native-code interpretation as a possible gift/exploit
is superseded. In the normal script flow, net charge is zero on that failure.
No-doctor branch, doctor failure and unrelated bare calls should not be
conflated: direct raw18 callers can still receive the literal25 credit even
without prepayment, so a future API must preserve/document this convention.

## Recovery implications

Hospital treatment can be repeated while party recovery is active if characters
remain injured and fees can be paid:02F2 discards the old countdown, successful
healing resets63. No fixed real-time waiting period is enforced at the hospital.
This verified exemption is a compatibility behavior, not evidence of a bug.

The reset occurs **before** both no-injury and insufficient-facilities exits.
Thus choosing treatment can also clear recovery without actually healing or
paying. Its effect on subsequent own-party healing elsewhere follows from the
native gate, but player-visible timing/repeatability still merits an emulator
trace. Preserve literal script behavior by default; do not add a cooldown check
or move/reset payment ordering as a supposedly necessary fix.

No blocked-treatment charge or25-credit exploit is entered in the original-game
bug register on this evidence. The earlier concern about unguarded direct calls
remains an API boundary, not a reachable defect proved in the hospital script.

## Checks

`Verify-HospitalGating.ps1` composes synthetic countdown, injury, facilities,
tier fee, refund and healing-result models across boundary cash values. Optional
`-GameDirectory chinception` checks the relevant decoded instructions directly
against the local original file without writing it or committing an asset fixture.
This is static/model verification, not original-EXE gameplay execution.

Results:712 hospital checks (including eight local-file assertions),8568 healing
and3374 specialist/service assertions pass. Whitespace validation passes.

Sol: C17 integration now executes the real HOSPITAL.BLD loader/interpreter,
actual treatment/healing and normal FF exit with SDL keys. The selected
professional-help-unavailable branch starts with active recovery, clears it,
charges the25 facilities fee, heals with party Medical skill and resets63.
This is stronger evidence for that path than the earlier composed models,
but not a hired-doctor/emulator comparison. See preservation
`CrescentHawksInception/docs/CONNECTED_SERVICES.md` for precise fixture scope.
