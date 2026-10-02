# Connected Mechlub repair validation

Sol: `gameplay.local_original_repair_services` exercises the original
Citadel repair action and11B8:0002 service with actual SDL input, menus,
text/number formatting and EGA drawing. The connected fixture loads the
original assets through startup; it uses the EXE-owned Wasp template for
synthetic damaged-Mech scenarios, not a replacement repair implementation.

| Scenario | Original operation and required outcome |
| --- | --- |
| Exact21 C-bills | Three armour points at4 each, then one structure point at9; both restored, balance0. |
| Only3 C-bills | Neither the first armour point nor structure is affordable; damage and balance unchanged. |
|20 C-bills | Armour restored first for12; remaining8 cannot buy the structure point, which stays damaged. |
| Exact1000 C-bills | One destroyed heat sink restored for800, then both packed actuator bytes restored for a flat200. |
|999 C-bills | Heat sink restored first; remaining199 cannot buy actuator repair. |
| Engine hit | Otherwise intact Mech remains damaged; the facility rejects that repair without charging. |

Each case compares the whole125-byte Mech record against its expected result,
including unchanged weapons, ammunition, maxima and other components. Other
Mech records remain absent, the selected record must be0, and the full C-bill
balance must match. The heat-sink case damages a real intact sink slot found
in the Wasp template. Actuator restoration uses chassis maxima, not guessedFF.

The test-only operator sends repeated Return acknowledgements through SDL so
the original input drains and default-Yes confirmations execute. It does not
write game state from a worker, mock repair/menu/formatting methods or select
arbitrary outcomes. Initial damage and funds are controlled scenario inputs.

The isolated repair test separately checks numeric quote calls: three missing
armour points quote12 C-bills; two structure points quote18. That probe adapts
presentation/input only and runs the actual service and Yes/No bodies. It
checks values supplied to formatting, not rendered quote pixels.

Validation: all116 headless and157 SDL/local tests pass. The six connected
scenarios also passed ten repeat runs. External assets remain private local
files. Dummy SDL video/audio is used: these are behavioural connected tests,
not emulator, music, pixel or complete Mechlub-script/playthrough validation.
Existing unit tests cover additional declined, weapon-repair, over-maximum
and original BUG-008/009 cases; those are not newly claimed connected cases.
No original repair logic or bugs were changed by this checkpoint.
