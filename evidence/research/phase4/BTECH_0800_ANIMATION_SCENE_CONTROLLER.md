# `BTECH_0800` animation-scene controller

## Scope

This note covers `0800:48B7-4AA5`, reconstructed as
`Display_Animation_Scene`. The next routine begins at `0800:4AA6`.

The controller chooses whether a numbered outtake should play, builds its
`O<n>.ANM` filename, loads the original asset, drives the frame decoder, and
optionally restores the game view. It does not contain the image data: a
compatible replacement executable must continue to load the user's original
ANM files.

## Parameters and filename

Both stack arguments are 16-bit words:

- `AnimationSceneId` selects `O0.ANM` through `O21.ANM`.
- `PlaybackMode` controls forcing and post-animation redraw behaviour.

The filename is assembled in the shared buffer at `3092:0012`: the controller
writes `O`, formats the scene ID in decimal, then appends `.ANM`.

## Playback decision

The initial random test ANDs an 8-bit random value with the selected outtake
frequency mask. The known masks are `0003`, `000F`, and `003F`, giving nominal
playback chances of 1/4, 1/16, and 1/64 when the scene is eligible for random
suppression.

Only scenes O1, O3, O4, O5, O7, and O16 can be suppressed by this setting.
O0, O2, O6, O8-O15, and O17-O21 are forced by the controller's scene-ID test.
Playback mode 2 also forces any requested scene.

The modes inferred from their observed effects are:

| Value | Maintained name | Behaviour |
| --- | --- | --- |
| `0000` | `AnimationPlayback_RestoreGameView` | Play normally, then wait and rebuild the game view. |
| `0001` | `AnimationPlayback_CallerOwnsRedraw` | Apply normal random selection, leaving the final animation frame/redraw to the caller. |
| `0002` | `AnimationPlayback_Force_CallerRedraw` | Force playback and leave redraw to the caller. |

These names describe behaviour, not symbols recovered from the original
source.

## Disk and optional-file handling

The routine selects disk 1 first. Scenes O10 through O15 then select disk 2;
all other scene IDs retain disk 1.

Late optional scenes O17 through O21 are probed before loading. The executable
closes the returned handle and compares its 16-bit value with `FFFF`, the DOS
failure sentinel. If it matches, the game restores its menus/sidebar and returns.
Sol: The systematic audit on 2026-09-17 resolved the abbreviated ASM `FFh`
using `BTECH_0800.dis` at `49A9`, which explicitly records `FFFF`. The earlier
`00FF` claim was wrong. Current C still compares `00FF`; that transcription
mismatch is queued for an approved correction, not an original-game bug.

The selected file is loaded at `246C:42C3`, with a maximum length of `3F00`.
The compressed frame stream begins 33 bytes later at `246C:42F6`.

## Playback loop

Before each playthrough the controller clears `1E78` bytes at
`246C:244B-42C2`, resets the frame number, and sets the 16:16 stream cursor to
`246C:42F6`. The original cursor is two words at `3092:0064-0066`; the decoder
receives their values and advances only the offset word by its returned source
byte count. No segment carry is performed.

The first frame is decoded unconditionally. For scenes other than O8, the
controller then waits 50 vertical retraces. Further frames are decoded while
the next playback-control byte indexed by the frame number is nonzero.

O8, the siren alarm, is special: it skips the fixed 50-retrace wait and repeats
the complete animation seven times. The assembly expresses this as a counter
initialised to six plus a post-decrement replay test.

## Return behaviour

Only playback mode 0 restores the ordinary game presentation. It waits 60
vertical retraces, rebuilds menu state, and redraws the top graphic/sidebar.
For O0 it also plays the mech-startup sound before that wait. Modes 1 and 2
return without rebuilding the view because their callers continue the visual
sequence themselves.

## Evidence and confidence

The parameter widths, scene tests, disk range, cursor words, frame-loop tests,
and wait counts are direct translations of the clean assembly. Confidence is
high. The semantic names for the three presentation modes are high-confidence
behavioural descriptions. The probe sentinel is now confirmed as `FFFF` in the
intermediate listing; the current C comparison does not yet match it.

Related format details are in [ANM](../formats/ANM.md), and the per-frame path
is documented in [ANM frame decoder and renderer](BTECH_0800_ANM_FRAME_RENDERER.md).
