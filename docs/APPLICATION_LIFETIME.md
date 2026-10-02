# Sol: SDL application lifetime and real startup — 2026-09-18

## Reproduced integration fault

A probe called the REAL original `Setup_Game` with an SDL window-close event
at the initial DOS drive-selection prompt. It timed out after three seconds:
the old keyboard replacement converted window close to Escape, which is not
a valid drive number, so the original prompt immediately blocked again.
This was a port/platform fault, NOT an original-game bug.

## Separate SDL lifetime boundary

`SDLBackend_RunApplication(Setup_Game)` now owns the main-thread window-close
boundary. SDL quit from blocking keyboard reads, input availability, retrace
waits or timed sound waits returns to the launcher, which tears down video,
audio and timer bindings.
At the time of this test no original menus, drive-selection loops or gameplay
branches were rewritten. The later installed-mode adaptation documented below
removes that obsolete startup interaction from the SDL build.
Actual gameplay Escape remains an Escape key, not application termination.
Outside an application scope, low-level keyboard/quit compatibility is unchanged.

The boundary uses standard C `setjmp`/`longjmp`, wholly in the SDL layer, and
is main-thread-only and non-reentrant. Audio/timer worker callbacks never jump
through the game stack or tear down video. SDL shutdown occurs on the main thread
after the original entry returns or is interrupted by an OS close request.
The SDL event pump/read releases its internal resources before the jump; no
unwind is performed inside an SDL callback or while an audio mutex is held.
This does not replace or recover the separate unsupported native-state aborts.

## Tests and evidence

- `sound.original_timed_effect_close`: starts the actual explosion effect and
  posts SDL quit after100ms. Verifies the application scope returns as closed
  before the effect returns normally, with a two-second response bound. After
  caller-owned teardown, retained playback is silent and effect state is cleared.
  Reopening then plays the actual single-shot stream with1,152 nonzero samples.
  No original sound commands or game branches are replaced; dummy drivers
  make this an integration check, not listening or native timing evidence.

- `platform.original_startup_window_close`: real startup/input/retrace methods
  and SDL backend; verifies normal entry return, gameplay Escape and quit during
  installed startup, input probing and retrace boundaries.
- `startup.local_original_asset_sequence`: opt-in original assets, real startup,
  original file loaders, decompressors, graphics transfers, tileset/sprite capture,
  SDL input and intro-music setup. Installed hard-disk state is automatic; no
  synthetic drive-selection keys are queued. All376 captured sprites, both startup tilesets,
  EGA mode, final sprite dimensions and original music cadence are verified.
  A main-thread callback requests window close only after that real milestone;
  the worker merely schedules it through SDL's main-thread callback facility.

- `gameplay.local_original_new_game_entry`: continues the same actual startup
  with SDL Space/Y events, constructs and loads the Citadel map, confirms the
  first-time-player prompt and closes during the opening door animation after
  at least one actual decoded frame. Verifies Jason's initial health/on-foot
  assignment, 20 C-bills, party/Mech initialization, world position and the
  shared native startup/procedural seed address. It does not advance exploration
  or validate NPC/combat behaviour. Entry uses 1000 host retraces per second so
  timer callbacks cannot miss the animation; this is not original timing.

- `gameplay.local_original_exploration`: continues beyond all six opening-door
  frames and original view restoration into the actual world loop. One SDL X
  input moves Jason south by exactly the configured step count (one initial
  tile); original finance ticks pay the first 15 C-bills, and NPC update ticks
  activate a roaming NPC from its initial delayed position. Verifies preserved
  health/on-foot assignment, live party and Citadel map. No gameplay methods
  are mocked, and no game state is written by the probe's callbacks.

The asset probes use dummy SDL video/audio drivers and accelerated retraces.
They validate actual integration, NOT OS-specific click delivery, visible image
quality, audible music, original timing, broader exploration or combat. The
original files are read locally; no external pixels/music are embedded or exported.
Test cleanup releases captured allocations; the standalone application terminates
after SDL teardown, with process-owned game allocations reclaimed by the OS.

The actual game executable is now part of the ordinary SDL build, not just an
optional unresolved-symbol probe. Latest suite counts are maintained in the
[build/readme status](../README.md).

## Installed-mode startup adaptation — 2026-09-22

The SDL executable no longer asks how many floppy drives the host owns. The
recovered DOS prompt remains disabled beside `Setup_Game` for preservation
reference, while startup directly sets the state produced by original option3:
`HasHardDisk=TRUE` and `SecondFloppyDriveAvailable=FALSE`. Assets and saves
therefore use the executable's working directory. Logical game-disk calls and
the original missing-file retry prompt remain intact; save/load code was not
hardcoded to a host drive letter.
Native residual-state guards, gameplay workflows and emulator comparisons remain.
