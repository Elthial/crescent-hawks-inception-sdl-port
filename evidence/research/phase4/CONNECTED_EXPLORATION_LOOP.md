# Connected exploration-loop validation

Sol: `gameplay.local_original_exploration_loop` executes the actual converted
0800:0000 parent with local original assets and the separate SDL backend.
It extends the existing startup/exploration and isolated main-loop tests with
a combined timer-expiry, pause/return and three-row fog witness.

The existing connected fixture loads all376 combat sprites through the real
startup, then invokes the original new-game map loader. The scenario supplies
five-tick values for health recovery, training/quiz, school and the two still
unclassified world countdowns. It supplies a finance countdown255. These are
test inputs, not new gameplay initializers. No game methods are replaced.

The test-only SDL operator waits without posting keys until health recovery
expires. Thus scene15 and the initial idle ticks use real scene playback,
retrace, RNG, map rendering and world updates. It then posts Space, verifies
the original seven-choice on-foot pause menu with Return selected, confirms
Return after the native input drain, and acknowledges the next exploration
input. An SDL quit event exits through the application boundary, not a new
in-game escape command or a write to the original main-loop exit flag.

Checks require all five countdowns to expire, finance countdown advancement,
Jason's unchanged starting position, a live party and unchanged original
exit flag. The previously-unset fog bit must be revealed in the current row
and both neighbouring rows using the original packed-coordinate indexing.
The operator only reads game state on the SDL main thread and posts events;
it does not update coordinates, timers, selections or fog.

Validation: the156-test SDL/local suite passes; the new test also passes ten
repeat runs. Original assets stay in ignored local storage. This scenario
uses dummy SDL video/audio drivers: it is connected game-method validation,
not pixel/music parity, an original-EXE comparison, arbitrary menu coverage,
a statistics-screen visit or a completed interactive playthrough. Existing
startup exploration separately witnesses actual movement and NPC spawning;
training and combat scenarios cover other connected paths.
