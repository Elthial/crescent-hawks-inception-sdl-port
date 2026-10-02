# `BTECH_0800` ANM frame decoder and renderer

## Review block (`0800:1AFD-1C11`)

`0800:1AFD` performs one complete ANM playback step. Its maintained name is now
`Decode_Draw_Next_ANM_Frame`: it delta-decodes one frame, renders it through the
selected graphics path, advances the compressed-stream cursor, waits for the
frame's file-defined delay, and increments the playback index. The callers own
the loop: the general scene player stops at a zero control, while the mech
start-failure scene stops early when the next control reaches `0x49` (`I`).

## Decode and EGA rendering path

`207F:23EC` reads from the 16:16 cursor at `3092:0064-0066`, XOR-decodes exactly
`0x0F20` bytes into the persistent packed-frame buffer at `246C:244B`, and
returns the number of compressed bytes consumed in `AX`. Retaining that buffer
between calls is essential because ANM frames are deltas, not independent
images.

For graphics-adapter value 2, `207F:0572` converts `0x0790` words from packed
nibbles at `246C:244B` into four EGA planes at `246C:336B`. `207F:1E37` then
draws the 88-by-88 image. The maintained pseudocode intentionally retains this
EGA pipeline only, matching the project's earlier removal of Tandy/CGA paths.

The original executable still records the discarded branches:

- adapter 0 preprocesses the decoded buffer through `207F:0163`, then draws
  the result through `1F3D:0086`;
- adapter 1 sends the decoded buffer directly to `1F3D:0086`;
- adapter 2 performs the EGA plane conversion before `207F:1E37`;
- values above 2 skip that conversion but still enter `207F:1E37`.

Those branches are documented for fidelity but have not been restored.

## Stream cursor and timing

After rendering, the routine adds the decoder's 16-bit `AX` result only to the
offset word at `3092:0064`. The segment word at `3092:0066` is unchanged. This
is a real-mode far pointer and must not be treated as a native 32-bit flat
pointer in the eventual C# port.

The ANM header loaded at `246C:42C3` supplies the delay:

```text
control       = file[frameIndex]
timingIndex   = signedByte(control) - 0x41
multiplier    = signedByte(file[0x20 + timingIndex])
scale         = signedByte(file[0x32])
retraceDelay  = (multiplier * scale * 3) >> 2
```

The original uses signed byte multiplication and arithmetic right shifts, then
passes the resulting word to `1F3D:0006`. The old maintained expression read
the control and timing data through `seg3092` and indexed an unrelated
`a42A2`; both were decompiler artifacts. The data belongs to the loaded ANM
buffer in segment `246C`.

`3092:E48A` is also a word, despite its former byte declaration. It indexes the
playback-control sequence and is incremented once after the wait.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:1AFD-1C11`.
- `docs/phase3/ANM_FRAME_DECODING.md` for the XOR delta grammar.
- `docs/phase3/ANM_PIXEL_LAYOUT.md` for packed-nibble and EGA-plane layout.
- `docs/phase3/ANM_TIMING.md` for the timing-table boundaries and retrace
  arithmetic across all 22 original ANM files.

The instruction chain and prior exhaustive asset checks agree, so this block
does not need an Astra confirmation pass.
