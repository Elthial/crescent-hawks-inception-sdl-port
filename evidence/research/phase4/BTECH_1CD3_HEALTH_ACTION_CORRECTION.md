# Script action 2F: Jason health deduction

Sol: corrected the annotated pseudo-code to match the already-correct C17
preservation implementation. No production C behaviour changed.

Native `1CD3:174E` selects ES from DS:569E, compares the BYTE at3092:C623
against5 and uses signed JLE to skip the deduction. At175A it subtracts4 from
that same BYTE. Jason's character record begins atC614, so C623 is offset0F:
Health. TrainingFlags is the following byte atC624 and is not modified.

The stale annotated action used TrainingFlags and misleading training-bit
constants despite its existing correction TODO. It now uses Health,
`ScriptHealthDeductionThreshold` (5) and `ScriptHealthDeduction` (4), matching
the names and values in preservation `src/Original/game.h`.

Health values6..127 lose four points. Values0..5 and80h..FFh are unchanged
because the native comparison is signed. This preserves the native high-bit
behaviour rather than treating Health as universally unsigned.

The existing original building-dispatcher test covers all256 Health BYTE
values with a distinct training-flags sentinel and passes. The systematic
1CD3 audit now also inspects the actual action's field and constants, rather
than only checking character layout and an independent arithmetic model.
All66625 static witnesses pass; static evidence is not gameplay validation.
