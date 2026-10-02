# Phase 3O — SIF and executable sound audio

## Review boundary

This block adds a bounded SIF parser, both original playback interpretations,
portable WAV rendering, an 18-effect executable sound catalog, approximate
sound-effect rendering, and Windows in-memory playback. It does not include the
external `WWOODBT.SIF` file or generated audio in source control.

## Commands

```text
InceptionTools inspect-sif [FILE] [--game-dir PATH] [--json]
InceptionTools export-sif-wav [FILE] [--mode pc-speaker|tandy] [--game-dir PATH] [--output FILE.wav] [--force]
InceptionTools play-sif [FILE] [--mode pc-speaker|tandy] [--game-dir PATH]
InceptionTools list-sound-effects [--json]
InceptionTools export-sound-effect-wav ID|NAME [--output FILE.wav] [--force]
InceptionTools play-sound-effect ID|NAME
```

`WWOODBT.SIF` is the default filename. `pc-speaker` is the default mode; select
`tandy` explicitly for the four-voice interpretation.

BLD disassembly resolves `E4` sound IDs through the same catalog and includes
the accepted tool name beside each numeric operand.

## Verification

Synthetic tests cover four-byte SIF framing, rest and pitch conversion,
inspection counts, both render modes, RIFF output, invalid frame alignment, all
18 sound IDs, first and final executable table records, and WAV generation for
every effect. Original-file smoke tests confirm the local SIF has 4352 bytes,
1088 frames, and renders in both modes.

## Remaining fidelity work

- Compare both music renderings with DOSBox-X recordings.
- Emulate the three Tandy tone channels' exact chip waveform and mixing.
- Derive calibrated busy-loop time from CPU-cycle behaviour for sound effects.
- Reproduce gate-toggle/noise algorithms cycle-for-cycle.
- Add MIDI export only after deciding how the PC arpeggiation and Tandy voices
  should map to tracks without losing the original distinction.
