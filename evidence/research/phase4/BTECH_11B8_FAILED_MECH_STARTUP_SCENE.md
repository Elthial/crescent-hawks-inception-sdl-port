# `BTECH_11B8` failed mech-startup scene

## Review boundary

This block covers `11B8:16B2-1761`, now named
`Play_Failed_Mech_Startup_Scene_16B2`. Mission 9 calls it after either of
Jason's first two distinct parked mechs fails to start.

## Asset selection and load

The routine explicitly selects logical game disk 1, then loads the original
`O0.ANM` asset at `246C:42C3` with a maximum read length of `3E80`. The old
pseudo-C omitted the disk-selection call and wrote the filename with different
case. DOS is normally case-insensitive, but `O0.ANM` is the executable's exact
embedded string at `3EDB:1E92`.

This specialized path does not call the general animation-scene controller.
It shares that controller's frame decoder and memory layout while deliberately
playing only an initial portion of the startup sequence.

## Workspace and stream setup

The routine clears exactly `1E78` bytes at `246C:244B-42C2`. This is the shared
graphics workspace containing the persistent `0F20`-byte packed XOR frame and
adapter-specific rendering space. The former pseudo-C bound `1E7D` was not in
the executable.

It resets WORD `3092:E48A` to zero and initializes the 16:16 compressed-stream
cursor at `3092:0064-0066` to `246C:42F6`, immediately after O0's `33`-byte
control/timing header.

## Eight-frame failure prefix

`Decode_Draw_Next_ANM_Frame` advances the frame-number WORD after drawing.
The loop consequently compares the *next* playback-control byte with `49`, not
a remaining-frame counter:

```text
do
    decode and draw next frame
while O0.playbackControl[nextFrame] < 49h
```

The shipped O0 control bytes are the sequence `41-52`, followed by zero. The
condition therefore draws frames `0..7` and stops before frame 8, whose control
is `49`. Normal successful-start playback uses the general controller and
continues through all 18 frames.

Each of these first eight controls selects a timing-table value of one; with
O0's scale of seven, the shared renderer waits five vertical retraces per
frame. The prefix duration is therefore determined inside the ANM playback
data rather than by this wrapper.

## Failure presentation and return

After frame 7 the routine:

1. plays sound effect `0E`, `Sound_MechStartUpFailed`;
2. waits `3C` (60) vertical retraces;
3. selects menu layout 4; and
4. redraws the top graphic/sidebar.

The Mission 9 caller subsequently selects layout 6 and displays the text that
the mech refuses to start.

## Corrections to the former pseudo-C

- Restored the missing game-disk-1 selection.
- Corrected the cleared workspace length from `1E7D` to `1E78` bytes.
- Replaced the misleading pointer-shaped scratch-buffer access with the exact
  far address `246C:244B`.
- Identified the loop test as an ANM playback-control threshold.
- Established that the failure scene is frames `0..7` of the same O0 startup
  sequence used on success.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:16B2-1761`;
- the shared renderer at `0800:1AFD-1C11`;
- the general scene controller at `0800:48B7-4AA5`;
- inspected original `chinception/O0.ANM` controls and timing fields;
- Mission 9 failure caller and follow-up at `0FDC:0B0D-0B2A`.

The file, disk, load bound, clear span, cursor, frame range, sound, wait, and UI
restoration are verified. No Astra review is needed for this block.
