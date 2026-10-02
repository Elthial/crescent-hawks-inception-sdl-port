# Sol: connected headless combat — 2026-09-18

`gameplay.original_connected_headless_combat` links the same original-game and
DOS/SDL libraries as the executable. It defines no replacement game methods,
random generator, dice routine, path helper or presentation redirect. SDL uses
dummy video/audio drivers, with the original no-combat-graphics setting.

The fixture supplies a synthetic open combat map, two on-foot pistol users,
valid native character/combatant IDs and a fixed initial native RNG state.
The original procedural cache constructor and map adjacency/view setup execute.
Each round calls original `Combat_Computer_Control` for both sides, checks their
chosen opponents, then calls complete `Combat_Mechanics` and its real callees.

The deterministic encounter finishes in two rounds. Assertions check:

- Both sides select each other through original AI targeting.
- Generated approach movement is executed, not just recorded as an order.
- Enemy return fire injures Jason, who remains alive and active.
- Enemy health reaches zero, the character is dead and its casualty flag is set.
- The defeated combatant is inactive and its attacking target link is cleared.
- The original camera anchor is restored after each execution round.

This closes an integration gap between separately isolated planning and execution
tests. It does not prove an original-binary trace comparison or a full combat
parent/menu workflow. The fixture's state/map are synthetic, not a loaded mission.
Further critical-damage, salvage and visible-effects paths still need connected
workflows. Existing native residual-state guards are unchanged;
no-combat-graphics is a real original setting, not a bypass added to production.

## Connected Mech cases

`gameplay.original_connected_mech_combat` uses two intact EXE-owned Wasp
templates, with valid friendly/enemy records and assigned pilots. Actual AI
chooses opponents and assigns weapon targets; complete execution consumes exactly
one SRM2 round from each ammo pool (50 to49), damages both Mechs' armour, restores
the camera and applies end-of-round heat. Starting heat20 becomes15 on both sides:
five firing heat minus ten intact sinks (four engine plus six critical-slot sinks).
No movement order contributes heat in this fixture. Native weapon/component
indexing and the AI's different approach-range decoding remain unchanged.

`gameplay.original_connected_mech_shutdown` starts the same intact Mechs at the
native shutdown threshold30. AI issues no firing targets, ammo remains50, armour
is unchanged and the same ten sinks cool both Mechs to20. An initial fixture
expectation of22 incorrectly counted only four critical-slot sinks; inspecting
the EXE-owned Wasp template confirmed six. The test expectation was corrected,
not production cooling code.

These cases exercise connected Mech targeting, missile consumption, hit/armour
damage and heat/shutdown. They do not establish fatal Mech critical chains,
ejection/salvage, visible effects or loaded-mission/binary-trace parity. Both use
the original graphics-disabled setting, not new production bypasses.

`gameplay.original_connected_mech_autoplay` covers the reported automatic-side
failure with three friendly chassis (Locust, Stinger and Commando) against one
enemy Wasp for eight complete rounds. It uses the original group planner,
per-unit AI, path/step generator, interleaved executor, weapons, heat and damage
on an unobstructed combat map. Every friendly plan keeps its horizontal steps
toward the enemy, every executed friendly position advances or holds rather
than retreating, all three retain the enemy target and attacks execute. The
enemy moves concurrently, so its lateral step can make the game's range metric
rise by one for a flanking friendly even while that friendly advances; this is
not evidence that the friendly reversed direction.

This regression rules out a general side-ID, target-ID, X-sign or movement-plan
execution reversal in the C17 port. It does not reproduce the owner's exact
procedural terrain or starting state. The ASM-confirmed movement routine is a
local eight-direction obstacle search, not a global pathfinder, and can choose
a detour when the preferred headings are blocked. Exact loaded-scenario trace
comparison remains necessary before classifying the observed long retreat.

## Connected destruction and crew handling

Two further modes use severely damaged Wasp fixtures with no remaining armour
or internal structure, while retaining valid original weapons/crew records.
Actual AI and execution carry damage through the transfer/fatal-section path,
ejection and unconditional effects cleanup. No fatal/ejection method is mocked.

- `gameplay.original_connected_enemy_wreck`: retires the enemy Mech, preserves
  its `W` name initial, sets its casualty flag and registers the ordinary wreck.
  Original enemy handling does not spawn infantry or rewrite the pilot's stored
  Mech assignment; that asymmetry is asserted, not corrected.
- `gameplay.original_connected_friendly_ejection`: retires the friendly Mech,
  places Jason on foot and activates him at the original Mech position, with
  the wreck registered and the main character still alive.

Both retired Mech coordinates end at FFFF:FFFF. The initial friendly fixture
mistakenly compared the final crew position with the final Mech position:
ejection retains both temporarily, but the effects parent subsequently clears
the Mech coordinates at native1AE8:1E03. ASM confirmation corrected the test,
not production state handling. This differs from an isolated ejection assertion.

Also rechecked the Inferno comment in both C and pseudo-code: native0B60 clears
BP-34, but that word is only read by the earlier infantry-repeat loop at08F1,
not the Mech path. The Mech damage loop tests BP-7C at0CCA. Removed the stale
claim that a Mech attack-count loop still needed reconstruction. No logic changed.

These fixtures prove their severe-damage paths, not every probabilistic critical
slot dispatch, rider/boundary variant, salvage choice or original-binary trace.

All146 SDL/local-asset tests pass; the separate headless suite remains116 tests.
No external copyrighted game file is read, embedded or exported by this fixture.

## Combat-to-component-salvage handoff

Sol: `gameplay.original_connected_wreck_salvage` extends the enemy-Wasp
destruction fixture through the actual `Salvage_Armour_Dialog` parent. Initial
encounter inputs give Jason Average Tech skill and one destroyed friendly heat
sink. After actual combat destruction, a real SDL Return-key event acknowledges
the native salvage prompt. No game method is replaced by a test double.

Assertions confirm that the friendly sink is restored, the enemy remains a
destroyed Mech, its casualty flag remains available for whole-Mech salvage, and
the initial100 C-bills remain unchanged because the selected technician is in
party slot zero. That payout gate is an original bug, not a desired new rule.

The original sink repair loop does not gate repair on a positive salvage pool;
this test therefore does not prove conservation of salvaged components. It
also does not cover whole-Mech recovery, nonzero-slot scrap payouts or the
unresolved SRM-6 stack bucket. All147 SDL/local-asset tests pass. No production
game logic changed in this checkpoint; these are synthetic encounter inputs,
not a comparison with an original-binary gameplay recording.

## Whole-Mech recovery and catastrophic rejection

Sol: two additional connected fixtures run the real `Salvage_Mechs_Dialog`
after the actual enemy-Wasp destruction path. Rex is a living on-foot pilot,
and native Tech selection chooses him over Jason. A test-only SDL timer sends
Return events after the native yes/no keyboard drain; it never reads or writes
game globals and replaces no gameplay method.

- `gameplay.original_connected_whole_mech_salvage`: Excellent Tech recovers the
  catastrophic wreck into the first empty Lance slot, restores the saved Wasp
  initial, binds Rex, copies the enemy ammunition, forces one engine/gyro hit
  and raises the native structure bytes at indices3/4 to one. The non-L sprite
  fallback remains Commando, even for this Wasp.
- `gameplay.original_connected_wreck_rejection`: Good Tech rejects the same
  catastrophic wreck, leaves Rex on foot and the Lance slot empty, but consumes
  the enemy casualty flag. Both paths leave the source enemy record destroyed
  and do not change C-bills.

All149 SDL/local-asset tests pass. These cases do not prove every damage gate,
full-Lance outcome, random multi-wreck selection or visible gameplay parity.

## Asset-backed, graphics-enabled Mech combat

Sol: `gameplay.local_original_rendered_mech_combat` runs actual `Setup_Game`
in automatic installed-hard-disk mode and closes at the milestone where all376
combat sprites and the intro music are loaded. It retains those allocations,
loads the actual BTTLTECH.ICN pipeline, then executes the synthetic Wasp
encounter with graphics enabled. Both combatants must be visible according to
the real compositor, and the EGA-to-SDL screen presentation must succeed.
No game method or graphics primitive is replaced by a test double.

Both SRM launchers expend one round and both Mechs take armour damage. Heat
is checked against the actual surviving engine/critical sinks, engine hits
and compositor terrain-cooling state. The old graphics-disabled expectation
of15/15 was not applicable: the first asset-backed run ended11/12. Native
animation selection consumes RNG, and real rendering supplies cooling terrain
information; equivalent initial seed bytes do not guarantee identical later
rolls when these optional original paths execute. Only the test expectation
changed, not native gameplay logic.

All150 SDL/local-asset tests pass. Original assets are read locally and neither
copied into tracked files nor exported. This dummy-driver test proves an actual
asset/rendering/combat execution path, not visual correctness, all effect types,
every residual-stack branch, audible music or parity with a recorded binary run.
