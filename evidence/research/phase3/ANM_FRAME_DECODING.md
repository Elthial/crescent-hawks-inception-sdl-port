# Phase 3G — ANM frame decoding

## Review boundary

This block decodes the packed `0x0F20`-byte state for every advertised ANM
frame. It does not convert packed nibbles to displayed pixels, infer timing in
milliseconds, write extracted images, export GIF, or open a playback window.

## Verified behaviour

`AnimationFrameDecoder.DecodeFrame` follows clean assembly `207F:23EC`:

- positive controls consume and XOR that many literal bytes;
- negative controls consume one byte and XOR it repeatedly;
- zero controls read a big-endian 16-bit repeat count and one repeated byte;
- exactly `0x0F20` destination bytes are updated;
- the returned source count identifies the next frame;
- the previous packed frame is retained and updated by XOR.

`AnimationFrameDecoder.Decode` applies that operation for each nonzero playback
control in `AnmFileRecord`. Frames retain their control value, compressed-stream
offset and length, token count, and a defensive copy of accumulated packed
state.

`inspect-animation` now reports those frame boundaries and nonzero packed-byte
counts in text and JSON. It does not expose decoded frame bytes in command
output. The uninterpreted remainder after the last decoded frame is reported
and preserved by the container model.

## Verification

The synthetic harness passes **85 assertions**. It separately exercises literal,
signed-repeat, and big-endian extended-repeat tokens, exact source consumption,
accumulated XOR cancellation, prior-frame validation, and output-overflow
rejection.

All 22 ignored original ANM files decode successfully through all 198 advertised
frames. Each decoded frame ends on a token boundary; no source overrun or frame
overflow occurs. This is sufficiently direct local evidence that an Astra pass
is not required for the token grammar. Pixel layout and display conversion are
the next independent review block.
