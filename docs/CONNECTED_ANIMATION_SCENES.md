# Connected original animation scenes

Sol: `graphics.local_original_animation_scenes` uses the local original asset
directory and the existing connected-game fixture. Actual startup loads the
376 sprite pointers and graphics tables through the SDL backend. The test
then calls the original `Display_Animation_Scene` for O0 through O21, forcing
playback through the original caller-redraw mode. Each call executes original
disk/file selection, XOR frame decoding, packed-plane conversion, EGA drawing
and SDL retraces. O8 retains its native seven-pass siren loop. No game-method
doubles, replacement decoder or copyrighted external asset bytes are included.

The test checks native timing-token termination, final frame/stream bounds,
the selected filename and successful screen presentation. It also executes
O0's restore-game-view mode and checks the restored layout. A synthetic
four-byte instruction stream in original BLD storage connects the actual
ConditionalScene opcode to shipped O6.ANM and normal script exit. Replaying
the gate with input disabled and an exit-valued scene argument confirms that
playback is skipped while the argument is consumed: a following text-layout
instruction executes before the actual exit opcode.

This complements the existing independent per-frame ANM pixel/decoder tests:
those isolate scene UI/waits, whereas this fixture runs the connected parent
and actual SDL redirects. Dummy video/audio and accelerated host waits do not
establish visible timing, audible parity, emulator equivalence, probabilistic
outtake selection, every BLD scene branch or complete gameplay. No preservation
production behaviour was changed. All154 SDL/local tests pass.
