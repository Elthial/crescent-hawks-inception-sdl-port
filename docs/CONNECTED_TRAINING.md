# Connected first training mission

Sol: `gameplay.local_original_training_mission` extends the existing connected
game fixture. Actual startup loads sprites and graphics; `Load_Game_Map_Data`
performs the native new-game reset, nine-region procedural construction and
local MAP1.MTP loading. The test selects a Locust and invokes the original
StartTraining building action, including O0.ANM, the animated hangar march,
and the southeast-corner mission controller.

A test-only operator reads positions on the SDL main thread and posts actual
keyboard events. It never moves Jason by assigning coordinates or replaces
mission, movement, terrain, NPC, graphics or timer methods. It travels east,
south to the objective, west to the east-side hangar approach, north and west
into the hangar. An initial route attempted the south wall and stalled at
0C3C:C058; changing the operator route resolved that without modifying native
collision logic. Input is bounded so a stalled route fails rather than hangs.

Before travelling, counted non-movement Enter inputs exceed the native pass
time plus the Locust allowance. The mission must report failure despite
reaching its objective and returning. The test checks restoration of the
temporarily changed map tile, then calls the original LeaveTraining action
and checks retirement of the loaned Mech, Jason's on-foot assignment and the
Citadel return position.

This is a connected original-code workflow with real local assets, not an
emulator comparison. It bypasses the training-centre BLD dialogue by invoking
its original service action directly. It does not certify successful timed
runs, other training missions, enemy combat, school purchases, visible timing,
audio parity or a complete playthrough. External asset bytes remain ignored
and untracked; no preservation production behaviour was changed.
