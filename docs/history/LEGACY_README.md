# Crescent Hawks Inception — preservation C17

Original-code conversions belong in `src/Original/`. SDL hardware/OS replacements
belong in `src/SDL/`. `../ReverseEngineering/Btech/` remains the annotated reference.
No research helper functions are admitted into the preservation build.

## Visual Studio

Open `CrescentHawksInception.sln` with Visual Studio and select `Debug|x64` or
`Release|x64`. The populated C++ Makefile project lists every preservation C
source and header under Original/SDL filters. Building it invokes the same
CMake/SDL3 build as `Build.ps1`, so there is no second, divergent source list or
link setup. Visual Studio requires the same C/C++ tools as the command-line
build; running the game requires the original assets. If a source file is added or removed, update
`CMakeLists.txt` and rerun `Generate-VisualStudioProject.ps1`.

The debugger starts in `../Chinception` so the original external assets are
available. **F5 may update saves in that directory.** For a fresh, isolated
human inspection instead, use `Run.ps1`, which copies the assets into an
ignored private directory before launching the executable.

The original routine names and call relationships are retained. Where a routine
accessed DOS/BIOS/hardware, its replacement body redirects to a separately named
SDL implementation. Game drawing, demo playback, asset interpretation and rules
must not be gutted merely because their names mention DOS or graphics.

## Current build

```powershell
./CrescentHawksInception/Reimplementation/Build.ps1
./CrescentHawksInception/Reimplementation/Build.ps1 -Headless
./CrescentHawksInception/Reimplementation/Build.ps1 -Headless -Configuration Release
./CrescentHawksInception/Reimplementation/Build.ps1 -Game -OriginalAssetDirectory (Join-Path $PWD 'CrescentHawksInception/Chinception')
./CrescentHawksInception/Reimplementation/Build.ps1 -Game -Configuration Release -OriginalAssetDirectory (Join-Path $PWD 'CrescentHawksInception/Chinception')
# Focused work: build only the named test runner and run matching CTest cases.
./CrescentHawksInception/Reimplementation/Build.ps1 -Configuration Release -Target test_platform -TestFilter '^sound\.original_timed_effect_(playback|close)$'
# Human-inspection build without compiling or running the test suite.
./CrescentHawksInception/Reimplementation/Build.ps1 -Configuration Release -Target CrescentHawksInception
```

Uses C17 and strict compiler warnings. SDL3 is pinned/checksummed and downloaded
only into ignored build directories, or supplied using `-SDL3Directory`.
The default build still compiles and runs the full suite. `-Target` builds only
the named CMake target(s); without `-TestFilter`, it runs no tests. `-TestFilter`
selects CTest names by regular expression and fails if it matches nothing.
When using both, include every executable needed by the selected tests.
Portable builds use CMake/CTest; `-DCHI_BUILD_SDL=OFF` builds rule tests without SDL.
`-Configuration Release` selects an optimized build in `build-release` or
`build-headless-release`, keeping Debug outputs separate. Test assertions stay
enabled in either configuration; production optimization flags are unchanged.
The optimized headless suite passes all117 tests. Both Debug and Release pass
the SDL/local-asset suite and link the real game executable. Release is not a
complete audible-fidelity certification: effect timing now defaults to the
owner-validated IBM-PC-compatible240-cycles/ms profile. The physical speaker
response is calibrated against supplied DOSBox-X recordings but still requires
owner listening confirmation.

The SDL build now produces the real game executable, preservation libraries and
tests. The entry calls original `Setup_Game`, not an invented bootstrap or party.
Real-asset startup captures all376 sprites and reaches intro-music setup in the
integration probe. Full gameplay, visible/audio fidelity and documented native
residual-state paths remain unfinished. Mech statistics now loads the original
artwork and returns normally in connected SDL testing. Its palette start and
the first-use combat impact flag use documented deterministic presentation
initializers instead of reproducing incidental BIOS stack contents. Gameplay
rules and subsequent original hit/miss carry-over remain unchanged. This is
still a development build, not a certified playable release.

Run from the repository root after building:

For human inspection with protected original saves, use:

```powershell
./CrescentHawksInception/Reimplementation/Run.ps1
```

This runs the Release executable in a fresh, hash-verified private copy of the
assets and saves under ignored `CrescentHawksInception/Chinception/validation/c17-human-*`. It copies
the C executable and its DLLs, not the original DOS executable. Save changes
stay in that private directory. `-PrepareOnly` prepares without launching and
prints the exact launch command. This is a development build: start with new
game/exploration, menus, training and services; report the first failing action.
Use `-SoundCycles 1510` to compare directly with the supplied1510-cycles/ms
DOSBox-X battle recording. Accepted profiles are240,750,1510 and3000; they
change CPU-bound effect duration, not PIT pitch or music tempo.

The default sound profile is now read from `CrescentHawksInception.ini`, copied
into each private inspection directory by `Run.ps1`. Edit `sound_cycles=240` in
that file for a persistent choice; `-SoundCycles` overrides it for one launch.
When launching the executable directly, keep the `.ini` alongside the original
game data in the working directory. Without the file, the legacy 240 default
still applies. An invalid config value stops startup with a diagnostic.

To run directly against your canonical files instead (saves are NOT protected):

```powershell
$gameExecutable = (Resolve-Path './CrescentHawksInception/Reimplementation/build/CrescentHawksInception.exe').Path
Push-Location './CrescentHawksInception/Chinception'
try { & $gameExecutable --sound-cycles 1510 } finally { Pop-Location }
```

For the optimized executable, use `build-release` instead of `build` in that
launch path; keep the same original-asset working directory.

Original assets must be present in that working directory. The SDL preservation
build assumes the original option3 installation—hard disk C:, with both game
disks copied into the working directory—and skips the obsolete drive-count
prompt and acknowledgement. Keyboard input is taken from the focused SDL window.
Original movement keys are `Q W E / A D / Z X C` for the eight directions.
The numeric keypad also supplies those directions (`7 8 9 / 4 6 / 1 2 3`),
with Num Lock on or off; Shift reverses its digit/navigation mode. Keypad Enter
acknowledges prompts just like ordinary Enter. Keypad `5` and `0` retain their
original input values rather than being assigned new movement actions.
Closing the SDL window returns to the launcher without treating close as gameplay
Escape. See [runtime status](docs/PLAYABLE_BUILD.md).
Back up your save files before using Save Game in this development build.

## Source policy

The original overview controller and terrain/marker renderer now compile,
including fog, map-file reads, dynamic tiles and the attract-demo timeout.
Their synthetic tests check original call behaviour; emulator/visible gameplay
comparison is still required after complete game integration.
The complete47-action building-script dispatcher also compiles, connecting
training, finance, equipment, services, arena/jailbreak and ending workflows
through original method calls. Its tests cover every action and the original
arena terrain helpers; complete story/combat gameplay is still unfinished.

- Original methods: preserve their logic, original bugs and native call order.
- Original data: statically declared arrays/globals, with address provenance.
- Reconstructed record declarations: exact native byte layout; no host pointers
  inside serialized records. Mech critical anatomy remains neutrally represented.
- New OS/hardware replacement code: SDL folder only.
- Headless UI/dice/dispatcher adapters: tests only, not game implementations.
- Research helpers: do not copy; inline verified original operations into their
  original routine when that routine is converted.
- Original copyrighted external files remain local and excluded from Git.

## Tests

`graphics.original_text_and_font` checks original string rendering and all128
embedded glyphs through the actual SDL EGA primitive, including CR/wrap and
inherited drawing state. The font data is EXE-owned, not an external asset.

`assets.original_title_workflow` checks the original title-loader sequencing
and reversed work buffers. Local integration also builds its actual title screen
through real palette setup, file access, decoding and framebuffer transfer.

`graphics.original_palette_sequence` checks the original sixteen-register
palette setup, including signed BYTE handling and the initial retrace wait.
Platform integration checks the actual original-to-SDL palette redirect.
`graphics.original_map_viewport` checks the complete original cache compositor
and tile transfers at256 local coordinate pairs. Local asset integration also
runs14 actual map files through loading, cache expansion and EGA composition.
`graphics.original_framebuffer_copy` checks native rectangle staging/clipping
and latch copying, including address offsets and forward overlap behaviour.
Local map integration also copies each viewport to actual screen-plane memory.

`assets.original_mtp_loader` checks the original headered map loader, native
descriptor placement, retained EOF tails, NPC routes/positions and tileset gates.
Opt-in `assets.local_original_mtp` checks all14 headered original levels;
the raw MAP15 star map uses a different original loader. The original BTTLTECH
tileset loader and Starport patch are converted. The isolated map unit suite
still adapts hardware drawing; local image integration uses the actual redirect.
The same suite exercises original exploration movement through real scrolling,
both map-stream queue passes and cache expansion, including diagonal crossings.
Collision/building interaction is still isolated pending its full conversion.

`rules.healing` calls the actual original healing conversion.
`ui.original_input_bridge` checks the original live/record/attract-demo input,
delay/exit tokens, key draining and retrace pause. SDL input availability is
non-consuming; menu Escape is not treated as an application quit.
`ui.original_numeric_text` checks actual native number formatting, WORD abs,
dynamic-value rendering and cursor/colour wrappers. High-bit decimal inputs
retain the original signed display, even for unsigned stored C-bills.
`rules.personnel_movement_and_combat_settings` checks friendly/enemy movement
budgets, armour penalties and the original combat-settings/Yes-No linkage.
Rule suites also call the converted dice, Mechlub repairs/ammunition/upgrades,
whole-Mech salvage, personnel loot, and weapon/armour distribution methods.
`rules.specialist_recruitment` checks rolled attributes/skills, specialty
effects, passenger seats, RNG reseeding and traitor/input handling.
`rules.arena_staging_and_restoration` checks owned-Mech damage retention,
rental discard, slot-zero aliasing, crew restoration and companion hiding.
`assets.original_bld_decode` checks native operands, filename selection and
the fixed-span decoder with isolated file/disk adapters.
`assets.original_graphics_decode` checks both native ASM compression loops
with generated streams. `assets.original_disk_and_raw_loader` checks the native
disk/prompt and both file-loading methods; OS/UI calls are isolated adapters.
Opt-in `assets.local_original_files` uses SDL-backed reads and the actual original
loaders/decoders on 26 BLD and 12 CMP/ICN files. Image checksums match the existing
InceptionTools decoder for this installation. EGA staging matches every decoded
pixel through the actual hardware redirect. Visible rendering/gameplay parity
is unverified.
`rules.armour_distribution` checks displaced suits and remaining points,
duplicate warning order, recipient selection and the original Jason shortcut.
`platform.original_redirects` calls the original method/import names and checks
their SDL redirects: speaker enable/off, signed keyboard values, binary reads/
writes, EOF/error handling, seek and create-without-truncation.

The invented map parser and its tests were removed. Asset import, map construction,
training, component salvage and headless combat suites now exercise the actual
converted original methods, not substitute algorithms. Current suites pass117
headless and164 SDL/local-asset tests; those results do not certify a full playthrough.

`sound.sdl_retained_pcm_queue` checks the SDL-only retained audio buffer: ordered
playback ahead of live speaker state, wrapping, atomic overflow rejection and
teardown. The device callback consumes this same buffer. This establishes a
safe output path for timed effects. Original delay routines now feed the timed
sample producer; audible comparison with the original remains outstanding.
The backend producer can now capture explicit-duration PIT/gate spans, retaining
silence/tone/silence after speaker-off and fractional samples between short spans.
It uses the same oscillator as live playback; rejected spans leave state unchanged.
`sound.original_timed_effect_playback` exercises the original single-shot stream:
it retains14,400 nonzero samples (300ms) using the preservation timing profile.
The original effect interpreter, divisors, gates and WORD arithmetic remain intact.
The three delay loops represent18,30 and10.5 virtual CPU cycles; the default
240-cycles/ms rate yields75us,125us and43.75us per iteration. The750,1510 and
3000 profiles are selectable with `--sound-cycles`. All18 effects produce
retained audio. SDL output now mirrors the supplied DOSBox-X speaker transition;
final audible approval remains a human listening check.

`sound.original_timed_effect_close` confirms window-close requests interrupt
actual effect playback, teardown clears pending output, and the SDL device can
reopen for another original effect. It does not establish native sound fidelity.

`gameplay.local_original_settings_quit` starts the original game with real assets,
opens Pause through SDL Space, navigates to Settings and Quit using SDL Down/Enter and
answers the original Yes/No prompt through SDL keys. It verifies normal return
through `Setup_Game` and platform teardown without an SDL close event. There are
no menu-selection presets or game-method doubles in this scenario. Test key
batches are spaced to let the original pending-input drains finish.

`gameplay.original_sdl_save_load_roundtrip` runs the original save/load controllers
through the real DOS-to-SDL file redirects for all six slots. It checks the entire
persistent block, native marker and little-endian camera coordinates.
It also confirms that overwriting a longer file preserves trailing bytes, and
that a valid-marker short payload leaves unread state/camera bytes unchanged,
as in the original unchecked-read path. These quirks are preserved, not repaired.
Only UI and map-rebuild boundaries are test adapters; it does not certify menu interaction or
the restored map rendering. Files are synthetic, confined to the ignored build
fixture directory, checked absent before creation and removed after success.

For a read-only check against a real complete save, run from its original-game
asset directory (example uses the compiled Debug validator):

```powershell
$env:SDL_VIDEODRIVER='dummy'
$env:SDL_AUDIODRIVER='dummy'
& '../Reimplementation/build/test_connected_combat.exe' load-existing-save GAME5
```

This optional command loads startup assets, navigates the real save menu using
SDL Down/Enter, then runs original file loading, map reconstruction and sidebar
methods. Party/Mech records, all100 story bytes, C-bills, stocks and camera
coordinates are compared with independent source-file bytes. The complete
source save is re-read and checked unchanged. It requires a complete3,913-byte
save named GAME1..GAME6; it is not a replacement for the short-read/tail tests.
All six local saves passed in Debug and Release. No game methods are doubled;
fog/NPC redraw changes and visible map/native gameplay comparisons are not
certified by these record checks. User saves/assets are not shipped with tests.

See [conversion log](docs/CONVERSION.md) and
[platform replacements](docs/PLATFORM_REPLACEMENTS.md).
