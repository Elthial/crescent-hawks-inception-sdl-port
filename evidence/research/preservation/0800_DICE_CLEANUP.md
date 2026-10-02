# Sol: in-place preservation cleanup — dice

Baseline `619cea7`. The separate preview has been removed. Clean the original
annotated methods in `Btech/BTECH_0800.c`, retaining native address comments:

| Address | Former identifier | Clean identifier |
| --- | --- | --- |
| 0800:19DD–19F2 | Rand_Dice_Two_D6 | Roll2D6 |
| 0800:19F3–1A12 | Rand_Dice_D6 | RollD6 |

The RNG dependency remains the original `Rand_0x00_to_0xFF`; its own cleanup is
not part of this block. Update existing callers without changing their logic.
No standalone game implementations or new module architecture are introduced.

## Actual method changes and native comparison

- Use `uint16_t` and explicit `(void)` prototypes in the existing shared header.
  Most of the application is still pseudo-C; only this bounded section is
  certified as compilable C17 in this checkpoint.
- Replace literal7/5/1 with `D6CandidateMask`, `D6FaceCount`, `D6FirstFace` and a
  zero-based candidate test. Mask7 yields0..7; `>=6` exactly matches native
  signed CMP5/JG on this restricted range. Reject6/7 rather than modulo folding.
- Name the local `candidate`. Each rejection consumes another original RNG byte,
  with no new retry cap or safety guard.
- Explicitly roll `firstDie`, then `secondDie`, retaining the first result like
  native SI. Do not rely on unspecified operand evaluation order in an addition
  of two stateful function calls. Preserve WORD result narrowing and native sum.
- Preserve stack-probe/prologue abstraction; these are compiler mechanics, not
  gameplay operations. Original game bugs are not fixed.

Both complete ASM routines were rechecked. Their fresh registry identifiers
and body hashes are updated. For other changed methods, reverse the two name
substitutions and require the resulting body hash to equal its prior certificate
before retaining the existing status. This proves caller-body changes are
identifier-only, not a fresh assertion that their outstanding mismatches are fixed.
An unrelated changed method must remain stale instead of receiving a new hash.

The test script mechanically extracts the actual enum and the two source bodies
into a temporary build input, then compiles them with the deterministic test RNG.
There is no maintained replacement implementation for testing. Run
`./scripts/Verify-PreservationDice.ps1`: C17 `/W4 /WX`, all256 RNG BYTE inputs,
exact rejection read counts and all36 accepted face pairs with rejection prefixes.
This does not compile the whole game or validate original RNG statistics/gameplay.

The previous preview files are recoverable in commit `619cea7`. Future cleanup
blocks follow this in-place approach, with address/name mappings here and
`Sol:` comments beside meaningful behaviour changes.

Verification: the actual source bodies compile with MSVC C17 `/W4 /WX` and pass
597 assertions. Caller reverse-substitution checks passed for all eight affected
caller bodies; the two dice bodies were rechecked against ASM. All312 registry
body hashes are current, and generated summaries/inventory and whitespace checks
pass. No original game bugs or previously queued caller mismatches were fixed.
