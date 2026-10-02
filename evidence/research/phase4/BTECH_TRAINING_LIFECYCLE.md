# Sol: selected skill, training result and cooldown lifecycle

Scope: callers of `1CD3` school action, the related script state bytes, and
their existing main-loop countdowns. No training mission combat code rewritten.
Evidence: expanded-EXE ASM, plus local read-only inspection of CITADEL.BLD
and TRAINING.BLD with InceptionTools. External assets are not copied into docs
or fixtures. Script offsets below are decoded payload offsets, not file offsets.

## Why direct-reference searches missed the callers

`0FDC:02E9` (F4) stores a BYTE at `3092:D30C + stateIndex`.
`0FDC:0298` (F7) tests that same address for nonzero. `0FDC:02D3` (F5)
calls `1CD3:0004` with the raw action operand. These indirect accesses overlap
the individually named header fields; they are not separate variables.

| State index | Address | Confirmed school/mission meaning |
| --- | --- | --- |
| `07` | `D313` | Combat-school introduction seen |
| `08` | `D314` | Selected combat skill: bows/blade, pistol, rifle (`0..2`) |
| `09` | `D315` | Immediate course purchase result |
| `14` | `D320` | Mech-mission cooldown, shared with dormant copyright quiz |
| `15` | `D321` | Combat-school course cooldown |

## Combat-school sequence (CITADEL.BLD)

- `0CFB/0CFF`: test/set introduction state07; first-visit introduction path
  clears state15 at `0E68` before reaching the class gate.
- `0E6D`: nonzero state15 branches to the not-yet-available response at `0F0F`.
  The narrative describes the next day; it is not a calendar implementation.
- `0F03`: raw action04 displays three course prices (C case03).
- `0F05`: menu09 chooses a course or exits. `0F59`, `0F60`, `0F67` store
  state08 as 0, 1, 2, respectively. All converge at `0F6A`.
- `0F6A`: raw action05 executes the purchase (C case04). It immediately
  increments the selected skill, debits cash and sets D315=1 on success;
  maximum-level refusal or insufficient funds clears D315.
- `0F6C`: F7 tests state09. Failure skips to `10D8`; success reaches waits
  at `0F73/0F74`, sets state15=FF at `0F75`, then displays completion prose.
  The script does not defer the skill award until cooldown expiry.

D315 is a last-action result, not an ongoing learning flag. Existing source
name `Bool_Skill_Learning` is retained with a corrected explanatory comment.
It may remain 1 after the course; countdown processing does not clear it.
The next native course attempt overwrites it. The waits before the script's
timer write matter: the purchase is already committed at that point.

## Timers and mech missions

`0800:02C7–02EE` independently decrements D320, D321 and D322 only when
nonzero. They saturate at zero and run only on processed exploration world
ticks. FF therefore requires 255 such ticks to expire, not a fixed real-time
day. The existing scheduler processes a tick on input; idle countdown reset
to10 gives a tick every eleven retrace waits after a processed tick. Blocking
dialog waits do not themselves run this main-loop countdown code.

TRAINING.BLD `0024` gates entry on nonzero state14. `0A87` calls raw action01
(mission runner) and `0A89` sets state14=FF after the mission returns, before
the pass/fail narrative branches. This is separate from the combat-school
timer. The dormant quiz failure also writes D320=FF (`1467`), so its former
quiz-only source name was incomplete. It is now
`TrainingMissionAndQuizCooldown_D320`; D321 is `SchoolTrainingCooldown_D321`.
Both keep address suffixes; decrement algorithms are unchanged.

D322 remains unclassified. The old D344:D345 map-patch countdown hypothesis
is not the timer controlling the two gates identified here; its initiating
write still needs a separate investigation. No guessed global rename applied.

## Verification / next boundary

`Verify-TrainingLifecycleTranscriptions.ps1` checks state-address aliases,
all BYTE initial countdowns through expiry, immediate purchase versus later
script cooldown, result persistence and idle scheduler spacing. Synthetic
models do not establish live gameplay timing or execute the annotated C.
Training arithmetic, personnel, stock and menu regressions are rerun.

Results: 65,887 lifecycle assertions, 5,373 training arithmetic, 2,816
personnel, 2,058 stock and 3,175 menu assertions pass. Whitespace validation
also passes; these are not live gameplay traces.

Next bounded review: any remaining unmarked `1CD3` shopping/training branches,
or the D344:D345 initiating write. No asynchronous training system, new timer
policy, original-game bug claim or external asset modification was introduced.
