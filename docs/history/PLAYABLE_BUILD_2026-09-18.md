# Sol: preservation executable linkage

The preservation executable now links, but playability is unverified and
documented native residual-state paths remain unsupported. The executable target
starts at the original Setup_Game (0D27:0044), through a minimal SDL entry.
No original methods are replaced by production test doubles.

From the repository root:

```powershell
./CrescentHawksInception/Reimplementation/Build.ps1 -Game -OriginalAssetDirectory (Join-Path $PWD 'CrescentHawksInception/Chinception')
```

This runs the ordinary build/tests, then links the real executable.
The game target is included in the ordinary SDL build; -Headless cannot be combined
with -Game. Run the executable from a local original-game asset
directory. The entry does not distribute, extract or embed external assets.
Installed hard-disk startup, asset/sprite startup, Citadel initialization, the first-time
prompt, opening-animation entry, exploration movement/NPC activation and
main-thread close have real SDL integration
probes. Visible console/window usability and gameplay still require
interactive validation; dummy-driver tests are not an OS-specific playthrough.

## Verified linkage and remaining fidelity work — 2026-09-18

After converting allocation, combat execution/effects, component salvage,
weapon selection and the statistics screen, fresh MSVC linkage succeeds
without unresolved symbols from the actual startup dependency chain.

The statistics screen no longer aborts over incidental BIOS palette stack words.
It uses an explicitly documented zero-start redraw counter and palette phase.
First-use combat impact state likewise starts as no impact; original assigned
hit/miss carry-over remains. These are portable presentation decisions, not
recovered original assignments. Rules, damage and service costs are unchanged.
See [presentation decisions](../../ReverseEngineering/docs/phase4/NATIVE_RESIDUAL_STORAGE_CONTRACT.md).

The linked allocation/combat/salvage/weapon-selection methods still have
documented unsupported native residual-state paths. Linking them does not
certify those branches or complete their gameplay validation.

The complete sound-effect call chain now links through platform redirects.
This is reachable-link evidence, not a complete method/caller fidelity audit.
Likewise, a successful link will not prove behavioural fidelity, original-bug
coverage, gameplay timing, correct rendering/music or normal shutdown.
Do not resolve failures with safe substitute algorithms or no-op methods.

The shared EGA screen/palette now feed SDL presentation at input/retrace
boundaries; previously only the initial black screen was presented. See
[EGA scanout](EGA_SCANOUT.md) for checks and remaining visual validation.

Connected personnel planning/execution/return fire/casualties and Mech
targeting/ammo/damage/heat/shutdown/destruction/ejection pass without gameplay doubles on synthetic
encounters. See [connected combat](CONNECTED_COMBAT.md) for remaining workflows.

Whole-Mech recovery/rejection also pass through actual dialogs after connected
combat; these synthetic cases are not an original-binary gameplay comparison.

An additional local-asset fixture runs actual startup/sprite loading and then
graphics-enabled Mech combat through the compositor/effects/SDL presentation.
It uses dummy drivers and is not proof of visual or audible correctness.

Connected [medical/ammunition services](CONNECTED_SERVICES.md) also execute
the actual dispatcher, healing, selection, numeric entry and C-bill updates.

The local hospital fixture also executes its actual BLD treatment and normal
exit path; hired-doctor and other scene branches remain unproven.

The latest ordinary suites pass117 headless and164 SDL/local-asset tests.
These suites also pass with Release optimization and test assertions enabled;
both SDL configurations link the actual executable. This does not resolve
the separate physical sound-timing or manual-playthrough gaps.
The executable links; the preservation objective remains incomplete.

## Explicit unsupported paths found in the current source

Sol: completion requires resolving these, not merely linking their methods:

- Component salvage: SRM-6 bucket outside the16 initialized native bytes.
- Weapon selection:12-entry unterminated native list, negative menu index and
  signed personnel-target text reads outside represented data.
- Structure transfer: invalid raw offsets returning native stack residue.
- Allocation: oversized request returns an unassigned native FAR pointer.
- Save loading: unread marker byte depends on native stack residue. The C
  implementation now diagnoses this instead of reading an uninitialized host
  variable. It still preserves unchecked later read counts and the invalid-
  marker handle leak when the marker BYTE was actually read.
- Asset loading: failed or partial two-byte length prefix; complete valid
  assets are covered, but inherited prefix bytes on read failure remain unsupported.

These remaining diagnostic aborts are conversion limitations, not original bugs. Normal
valid-path tests do not cover them. Native residue needs an explicit faithful
representation and evidence of its calling context; guessed zero defaults and
host undefined reads are not preservation solutions. Visible rendering,
music/timing, connected training/shops and a complete playthrough also remain
uncertified despite the current integration coverage.

## Practical manual check

### Default host-device startup/quit probe — 2026-09-18

Sol: the Release `test_startup_runtime settings-quit` executable also passed
from the private original-asset directory with no `SDL_VIDEODRIVER` or
`SDL_AUDIODRIVER` environment override. It exited normally with code0 and no
stderr diagnostic after real Setup_Game, external sprite loading, new-game
Citadel construction and the original Pause/Settings/Yes quit workflow.
This checks the default host-device path beyond dummy-driver CTest coverage.
The operator is still scripted SDL input with accelerated retraces; this is
not a manual playthrough, observed window-usability check or listening test.
Private stdout/stderr logs remain under ignored `Chinception/validation`.

Sol: freshly rebuilt Release `test_connected_combat` also passed six modes
without SDL video/audio environment overrides: `rendered-mechs`,
`repair-services`, `medical-services`, `ammunition-services`,
`mech-statistics` and `training-mission`. Each ran from the same private
original-asset copy used for the native GAME1 capture, exited0, and emitted
no stderr diagnostic. The real game executable was also freshly relinked.
These checks exercised rendered Wasp combat with ammo/heat assertions,
full/partial/unaffordable repairs and engine rejection, treatment/recovery
gating, ammunition funds/capacity handling, statistics dismissal without
Mech/economy changes, and the Locust training late-failure return.
Each owned probe had a45-second host limit; none needed forced termination.
They extend integration evidence to default host devices, not native-game
rule comparisons: the fixture still supplies synthetic scenario state,
scripted SDL input and accelerated retraces. No visual observation or
listening claim is made. Logs remain private under the capture's `logs/`.

### Remaining work priority

Sol: prioritize ordinary gameplay and the SDL experience, not BIOS instruction
replication. The source review separates the remaining cases as follows:

- Gameplay-reachable preservation boundary: component salvage with a Good or
  better technician and an SRM-6 slot. ASM032D zeroes BP-24 through BP-15 only;
  0377 increments the inclusive component20 bucket at BP-14. This is genuine
  gameplay-affecting unknown storage, not a hardware service to replace.
- Dense-loadout boundary: twelve collected weapon components leave the native
  menu list unterminated. Preserve its documented bug; do not silently substitute
  the collector count for the original string-length operation.
- Lower-priority exceptional inputs: incomplete save marker/asset prefix,
  oversized allocation and invalid raw indices. Their guards must remain clearly
  reported limitations, but they need not precede valid-asset playthrough work.
- Practical sound work: original delay-loop arithmetic feeds retained SDL
  samples under a CPU-rate profile. The owner selected240 cycles/ms as the
  IBM-PC-compatible preservation default; 750,1510 and3000 are selectable
  comparison profiles. The SDL output boundary now mirrors DOSBox-X's slew,
  two-stage low-pass and DC correction, calibrated against the owner's supplied
  WAVs. Listening approval is still required; no new BIOS emulation is involved.

The next acceptance evidence should be a usable valid-asset playthrough and
audible effects/music, followed by focused handling of the gameplay boundaries.
Neither a green test count nor deterministic guessed salvage quantities closes
those gaps.

Sol: a [matched native/C17 GAME1 load](../../ReverseEngineering/docs/phase4/NATIVE_SAVE_LOAD_COMPARISON.md)
now confirms characters, Mechs, persistent story state, economy and coordinates
against the same private file through actual loaders. The native session also
quit normally and exported its trace. It does not close combat/salvage,
procedural-map output or manual-playthrough validation.

Sol: the training operator now counts observed native world-update phases,
not posted SDL keys (the original input drain can discard batches). It waits
for532 observed updates before departing, retains the real late-failure and
map/party-restoration assertions, and uses a bounded40-second host allowance.
This is test scheduling only; the original mission timer/rules are unchanged.

Sol: before wiring timed playback, measured all18 actual effect invocations with the original `BusyWaitScale=5`
on this host through `test_platform --sound-timing`. Debug durations ranged from
0.0041 to2.2715 milliseconds; Release ranged from0.0011 to0.0748 milliseconds.
Those measurements exclude the diagnostic's200ms spacing between effects.
The former asynchronous speaker callback sampled current control/divisor state,
not a retained history of short commands, so these bursts are at risk of being
missed entirely. This is a demonstrated portable timing problem, not evidence
of a corresponding fault in the original executable. Use a timed SDL audio
sequence retaining transitions, not just an arbitrary minimum delay after sound
has already been switched off. Native reference durations remain to calibrate.

Repeat the diagnostic using either `build/test_platform.exe --sound-timing` or
`build-release/test_platform.exe --sound-timing`; it needs no original assets.
For unattended measurement set dummy SDL video/audio drivers. Without those
overrides it uses the actual SDL output device, but listening is a separate
manual check. It neither edits assets nor changes production timing.

Sol: the SDL PCM queue now retains ordered samples before live speaker scanout,
with an eight-second capacity, atomic overflow rejection and shutdown cleanup.
The actual device callback and deterministic queue tests use the same consumer.
Effect delays now produce timed samples; retaining arbitrary PCM alone would
not repair the original routines' host-speed bursts.
The SDL producer now captures explicit-duration speaker spans using the shared
PIT/gate oscillator. Fractional sample units carry between spans; overflow
rejects without advancing the oscillator or fractional time. Queue tests verify
silence/tone/silence after gate-off and sub-sample accumulation. The original
delay loops retain costs of18,30 and10.5 virtual cycles for fixed tone, sweep
and noise respectively. At the owner-selected240-cycles/ms default these become
75us,125us and43.75us per iteration. Original stream interpretation,
divisor/gate changes and WORD arithmetic are unchanged. Playback waits for
retained samples while servicing close and music. The single-shot stream retains
exactly14,400 nonzero samples (300ms) at scale5. The later DOSBox-X response
calibration changes only device output timbre, not those retained native spans.

Sol: the connected exploration fixture also supports an optional private BMP
export. From the local game-assets directory, run:

```powershell
$env:SDL_VIDEODRIVER='dummy'
$env:SDL_AUDIODRIVER='dummy'
& '../Reimplementation/build/test_startup_runtime.exe' exploration './validation/c17-citadel-preview.bmp'
```

The destination directory must already exist. This test-only export retains
the actual EGA scanout after scripted window close; it does not substitute a
renderer or change the game executable. The locally viewed Citadel frame has
coherent map artwork and a legible South/Jason/C-bills sidebar. This is a visual
sanity check, not an original-game pixel comparison or a manual playthrough.
Keep exported artwork under ignored local assets, never in source control.

Use a backup of your saves with local original assets. Build with the command
above, then use the launch block in the project README; focus the SDL window
for input. Record the first failing action and any console diagnostic.

1. Start a new game; check the title, introduction, Jason, map and sidebar.
2. Walk around the Citadel, open the Space pause menu and return to exploration.
3. Visit Katrina, a shop and the training centre. Confirm menus accept a single
   intended command and the visible C-bill changes make sense.
4. Play Locust training mission1, move/fire and return. Compare the result and
   training lockout with your expectations from the original game.
5. With a Mech available, open its statistics screen and return. Inspect damaged
   armour/structure, then try a repair with sufficient and insufficient money.
6. Exercise a combat round and Flee; check targeting, damage, heat, effects and
   return to exploration. Finish through Settings/Quit and also check window close.

This checklist is work still to perform, not a claim that these manual checks
have passed. Automated connected scenarios cover many of the same methods but
use accelerated pacing and dummy video/audio, which cannot verify usability,
visible composition or audible fidelity.
