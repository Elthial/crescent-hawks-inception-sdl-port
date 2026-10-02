# Sol: first human inspection — 2026-09-18

Owner-reported evidence, distinct from automated fixtures:

- Animations, NPC movement and animated tiles appeared to work.
- Katrina/HQ, shops and other buildings worked.
- The15 C-bill allowance and stock investment worked.
- Locust training worked; the mission took too long. This does not yet
  distinguish intended mission failure from a host timing problem.
- Palette had a green tint, including loading screens.
- Intro music had an annoying constant whine.

SDL corrections from this report:

The display decoder now uses200-line RGBI output rather than350-line EGA
secondary-channel interpretation. Original register values remain unchanged.
See EGA_SCANOUT.md for the monitor mapping and verification.

The audio renderer now rejects clocked tones at/above its24kHz Nyquist limit
before generating samples. The original204B music code uses PIT divisor14
for high-bit notes: roughly85.2kHz, which point-sampling at48kHz previously
aliased to roughly10.8kHz. This is a concrete aliasing fault and a likely
explanation of the reported whine, not yet a listening-confirmed diagnosis.
No SIF bytes, note commands, PIT divisors, gate writes or game rules changed.
Direct software-gate output remains supported; this is not a full analogue
speaker/filter simulation. Regression checks cover live and retained silence
for divisor14 plus audible ordinary tones. Seven targeted Release checks pass.

Next human check: run Run.ps1 again for a fresh copy of the rebuilt executable,
inspect title/text/border colours and listen to the intro. Earlier prepared
private directories retain their earlier EXE. Report whether the whine is gone
and whether any notes/effects disappear; remaining square-wave harmonic aliasing
and physical speaker timbre are not certified by these fixes.

## Follow-up human results and TODOs

Sol: the owner confirmed that both the green palette tint and intro music
whine appear resolved in the rebuilt version. This confirms those two fixes,
not general sound-effect accuracy or combat correctness.

### TODO: match original sound effects against DOSBox-X recordings

Owner reports that lasers, machineguns, enemy weapon sounds and Mech
destruction sounds are all wrong. Treat the current SDL sound-effect duration
profile as unvalidated; successful sample-generation tests are not fidelity.

- Record these effects from the original game running in DOSBox-X. Keep
  recordings and game assets private/ignored, not in Git.
- Record the equivalent C17 effects and compare pitch, duration, pulse/gate
  transitions, repetitions and which effect is selected for each action.
- Record DOSBox-X configuration/cycle settings and the triggering action so
  comparisons can be repeated. Distinguish dispatch errors from synthesis
  and timing errors; calibrate the SDL layer against the recordings before
  claiming matched playback.

Sol: ordinary Mech destruction is verified to dispatch ID8 (terrain damage),
not ID7 (arena destruction). Missile, repeating-projectile, destruction and
laser calls likewise retain IDs1,3,8 and9 from the ASM-reviewed game path.
The likely mismatch is therefore the SDL physical rendering: its three
busy-loop timing is CPU-rate dependent and its ideal digital square wave lacks
the filtering of an original PC speaker/emulator mixer. Owner listening checks
selected240 cycles/ms as the IBM-PC-compatible preservation baseline; 750 was
also acceptable and3000 preserves the owner's nostalgic DOSBox-default sound.
The SDL default now converts all three native loop costs through240 cycles/ms;
750,1510 and3000 are named profiles for later selection. No effect stream,
divisor or gate command changed. `scripts/Prepare-DosBoxSoundComparison.ps1`
prepares a private fixed-240-cycle
DOSBox-X run whose Capture > Record audio to WAV output remains ignored.
`scripts/Capture-C17SoundEffects.ps1` records the corresponding C17 interpreter
output as private WAVs. Paired recordings are still needed to address filtering
and speaker softness rather than timing.

Owner supplied two DOSBox-X WAV references on 2026-09-22: a battle sequence
at1510 cycles/ms (`btech_003.wav`) and an intro excerpt (`btech_000.wav`).
They remain private outside Git. DOSBox-X's speaker source confirms that its
unipolar output slews between levels, passes through two14kHz low-pass stages,
and is slowly recentred by the mixer. The SDL output response now mirrors that
pipeline without changing original PIT divisors, gates or effect streams.
`Capture-C17SoundEffects.ps1` accepts240,750,1510 or3000 cycles/ms and defaults
to the supplied1510-cycle comparison profile.

### TODO: investigate computer-controlled combat and deep-water movement

Owner used the let-the-computer-play-match scenario against an enemy believed
to be a Wasp or Stinger (identity uncertain). One enemy destroyed all three
player Mechs while the friendly Mechs ran left and were shot from behind.
Mechs also appeared to walk into deep water. The enemy seemed able to fire
indefinitely, or the friendly team appeared to prioritize running over firing.

This is a suspected conversion/gameplay bug, not a confirmed original bug.
Do not explain it away or alter tabletop rules without reproducing it.

- Reproduce from a private save/scenario; record exact chassis, loadouts,
  terrain, starting positions and automatic-control option.
- Compare the same scenario with the original binary in DOSBox-X.
- Trace friendly/enemy target selection, approach distance, movement orders,
  facing, path/collision checks and whether an attack plan survives movement.
- Check why friendly units repeatedly move left/away rather than engage;
  verify side/combatant/record indexing and coordinate/direction conversions.
- Verify water-depth/passability restrictions and movement budgets against
  original behaviour, including whether the displayed terrain matches the
  terrain used by collision/planning.
- Trace weapon selection, ammunition consumption, per-round firing flags,
  heat and shutdown for both sides. Determine whether apparent unlimited
  fire is valid for the actual weapon or a state-update/dispatch error.
- Add regression coverage only after the cause and expected behaviour are
  established. Preserve any demonstrated original bug separately from a
  C17 translation error.

Initial report only; later investigation found the undersized shared
BTSTATS/BLD host buffer described below. Its overwrite reached host combatant
activity/position and graphics globals, making it a credible cause of invalid
AI state after opening a detail scan. Re-run this scenario from the fresh fixed
executable before changing verified AI rules. Sound calibration remains undone.

### Confirmed original bug: death/play-again restart retains fog

Owner clarified the trigger: Jason died, the game asked whether to play again,
and the restarted game placed the party outside the Citadel. Previously
revealed areas outside the Citadel also appeared to remain. This supersedes
the initial attribution to loading another save. Original-bug status is unknown.

Negative reproduction: loading a later save and then a Citadel save did NOT
show this behaviour; fog-of-war worked as intended in that sequence. Save
loading therefore appears to restore fog correctly in this observed case.
The owner's working hypothesis is missing reset state on an in-process
restart, not missing fog data in saves; this is not yet code-confirmed.

- Reproduce Jason's death followed by accepting play again, without closing
  the application. Record position and revealed areas before death and after
  restart; compare with starting a new game in a fresh process.
- Repeat this exact death/restart sequence in the original binary under
  DOSBox-X before classifying it as an original bug or a C17 conversion error.
- Trace the death/replay branch and new-game initialization, including party
  coordinates, fog storage, map/cache state and overview/display buffers.
  Determine whether the outside-Citadel spawn and retained visibility share
  a missing reset or are separate problems.
- Retain the successful later-save → Citadel-save sequence as a control.
  Do not change the save loader or blindly clear loaded fog: saves must retain
  their own explored areas.
- Existing matched native/C17 GAME1 load checks explicitly excluded fog and
  did not exercise death/restart. Add a death/play-again regression once
  expected native behaviour is established.

Sol: code/ASM investigation completed. `Load_Game_Map_Data` restores the native
Citadel start coordinates but never clears `MapFogOfWar`; it only ORs in the
initial Citadel reveal. This exactly distinguishes an in-process replay from a
fresh process and from a save load. Preserve it as BUG-024. The reported
outside view is the native Citadel-map start, not evidence that the coordinate
reset failed.

### TODO: general game pacing/timing and missing SRM visuals

Owner reports a broader pacing problem: combat encounters trigger much too
often, as well as SRM missiles not appearing visible during firing. Investigate
overall simulation/update cadence, not just projectile playback. Modern CPU
speed or missing host throttling is the suggested cause, not yet an established
diagnosis. This may affect world/NPC updates, encounter checks, combat,
animations and countdowns; do not assume one local effect delay fixes it.

- Compare exploration/world-update frequency and encounter-check frequency
  with the original binary under DOSBox-X over comparable routes and elapsed
  time. Record original emulator settings and distinguish random variation
  from excessive checks per movement/input or per real-time second.
- Audit which original updates are input-driven, retrace/timer-driven or
  deliberately unpaced. Check busy polling and key repeat for unintended
  extra simulation ticks; keep rendering/audio cadence separate from rules.
- Establish a CPU-independent host pacing policy at the appropriate SDL
  boundaries, preserving the original relationship between movement, world
  updates, encounter checks and countdowns. Verify rather than blindly slowing
  every loop or reducing encounter probabilities to conceal a cadence fault.
- Reproduce SRM firing and compare visible projectile/impact playback with
  the original binary under DOSBox-X, recording timing settings.
- Check projectile selection, sprite/line drawing, visibility/camera gates,
  framebuffer composition, presentation and restoration before assuming speed.
- Measure projectile-frame duration and SDL presentation count. Determine
  whether frames are skipped, overwritten before presentation or simply too
  brief to see; test bounded slower playback to distinguish those cases.
- If timing is the cause, pace the affected SDL presentation/wait boundary
  independently of CPU throughput rather than adding arbitrary delays to
  targeting, damage or other gameplay rules. Check other projectile types too.

Sol: no throttling or projectile changes made. The SDL retrace boundary already
paces waits at the original EGA 70 Hz, while the verified main loop advances a
world tick per input or after its native ten-retrace idle countdown. The buffer
overrun could corrupt combat presentation/state but does not by itself prove a
timing diagnosis. Re-test first, then compare DOSBox-X cadence before changing
the verified encounter roll or adding delays.

Sol: follow-up retest reports that an SRM is visible only on its final travel
frame. The ASM-reviewed missile loop does copy every intermediate frame from
the A800 working viewport to A000, but it contains no explicit retrace wait;
real EGA scanout made those completed copies observable. Buffered SDL previously
presented only at later input/retrace boundaries and collapsed the CPU-speed
burst to its last frame. The SDL hardware boundary now explicitly presents and
paces each completed viewport transfer at the configured retrace rate. Original
projectile coordinates, sprite stream and game waits remain unchanged. Human
confirmation of the rebuilt executable is still required.

The same retest still raised possible Mech movement into deep water. Static
review does not justify a preservation-rule change: ASM-confirmed combat path
and step routines reject the map's blocking tile codes but do not call the
separate exploration-only `LocalTerrainFlags == 15` deep-water message gate.
Capture the combat position/tile code and compare the same move in DOSBox-X;
it may be native combat passability, a displayed-footprint impression or a map
decode error. Do not add a new combat water prohibition without that evidence.

Sol: a deterministic eight-round connected replay now runs a friendly Locust,
Stinger and Commando against one Wasp through the complete original planner and
executor on clear terrain. Every friendly step remains directed toward the
enemy and attacks execute. This rules out a general C17 side/index/sign reversal.
One apparent range increase was traced to the enemy moving laterally during the
same interleaved round; the friendly itself still advanced. The exact reported
terrain case remains open: native `Position_0006` is only a greedy local
eight-direction obstacle search and may detour poorly, while combat still has
no explicit exploration-style deep-water rejection. Capture the precise tile,
positions and chassis in C17 and DOSBox-X before changing preservation logic.

### TODO: combat scan/detail screen graphics misplaced or corrupted

Owner supplied four screenshots showing scan/detail rendering faults for
friendly Stingers, a friendly Commando and an enemy Wasp. Main Mech artwork,
text and condition bars appear, but the right-hand detail area is corrupted
or misplaced: vertical red or blue/white stripes, scattered status pixels,
clipped headings, and rear-view/heat-sink panels partially cut off. The
Commando screenshot shows more of the intended right-hand panels than the
others. Both friendly and enemy scanning are affected.

- Reproduce detail scans during combat and compare with the original binary
  in DOSBox-X at the same320x200 logical resolution, including repeat scans
  and switching between units/chassis.
- Check statistics image loading/decompression, source/destination offsets,
  row strides, rectangle coordinates and framebuffer/staging-plane transfers.
- Check EGA write/read modes, plane masks, bit masks and latch state carried
  into drawing/restoration; distinguish stale state from address/width errors.
- Verify right-panel clipping/layout, rear-view and heat-sink positioning,
  and condition-bar placement against the original display. Do not assume
  an SDL-only fault until the original drawing conversions are checked too.
- Existing statistics tests establish return/state preservation and rendering
  execution, not visually correct complete panels. Add focused pixel/layout
  regressions after expected original output and the cause are established.

Sol: investigation found a host-C storage error before the renderer. The shared
`3092:00A0` buffer was allocated from the 9,000-byte BLD transform span, while
the shipped BTSTATS payload is 12,635 bytes. Every first detail-screen load
therefore overran the array by 3,635 bytes and corrupted unrelated host game
state. The buffer now covers the complete verified payload, compile-time checks
cover BLD/demo views, and the local-asset suite checks BTSTATS capacity. Isolated
full-panel rendering remains visually correct. Human replay is still required
to confirm that no independent carried EGA-state defect remains.

Sol: owner retest confirmed friendly/enemy detail scans now look correct. Close
this visual-corruption item unless a fresh fixed build produces a new example.

### TODO (high priority): enemy scan hangs/crashes with no Mechs present

Owner reports selecting Scan → Enemies when there were no Mechs caused a
crash. Screenshot shows the application marked Not Responding, the Enemies
option highlighted, infantry visible on the battlefield, and the computer-
movement message for Rex's Commando still displayed. Clarify which side lacked
Mechs and which enemy units remained active; do not infer that no enemies existed.
Treat as a reported hang/crash until process termination or an exception is
confirmed. It is separate from the scan/detail graphical corruption above.

- Reproduce enemy scanning in infantry-only/no-enemy-Mech encounters, including
  automatic combat, and compare the exact sequence with DOSBox-X.
- Trace scan dispatch and eligible-target cycling. Check Mech-only versus
  infantry-inclusive filtering, side/record ranges, active flags and unused
  coordinates; determine whether the browser endlessly searches for a target
  excluded by its current filter.
- The converted scan browser documents no all-ineligible-target escape.
  Investigate this as a candidate, not a confirmed cause or original bug.
- If cycling is unbounded, distinguish original logic from translation errors
  and SDL event starvation. Preserve proven original behaviour explicitly;
  do not silently invent a new target or change scan rules.
- Verify window-close/event handling during the failure and capture a bounded
  trace or exception diagnostic. Add regression coverage once diagnosed.

Sol: ASM/disassembly confirms the browser itself has no all-ineligible escape;
that is original logic and assumes combat lifecycle state guarantees a target.
The undersized BTSTATS/BLD host buffer could corrupt active flags and positions,
making that otherwise-invalid state reachable after a detail scan. Its overrun
is fixed. Re-test before changing the native loop; if a clean run still hangs,
trace the combat-end invariant rather than inventing a scanner exit.

Sol: owner retest confirmed enemy scanning with no Mechs now returns normally.
The shared-buffer corruption was the observed trigger; preserve the native
browser loop and close this reported crash unless it recurs in a fresh build.
