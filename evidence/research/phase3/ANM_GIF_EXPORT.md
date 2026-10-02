# Phase 3L — looping ANM GIF export

## Review boundary

This block converts decoded ANM sequences into portable looping GIF89a files.
It adds both a single-file command and an atomic-preflight batch command for
the complete `O0.ANM` through `O21.ANM` set. It does not replace the legacy
interactive asset extractor or add an interactive player UI.

## Commands

Export one animation:

```powershell
dotnet run --project InceptionTools -- export-animation-gif O0.ANM --game-dir chinception --output O0.gif
```

Export all 22 animations:

```powershell
dotnet run --project InceptionTools -- export-animation-gifs --game-dir chinception --output-dir artifacts/animation-gifs
```

Both commands refuse to overwrite existing files unless `--force` is passed.
The batch command checks all output collisions and fully decodes/encodes all 22
inputs before writing anything, so a missing or malformed later input cannot
leave an apparently complete partial set.

## Timing policy

The file's exact source unit remains the vertical-retrace count documented in
[ANM timing](ANM_TIMING.md). GIF stores only unsigned integer centiseconds.
The exporter uses a nominal **60 Hz** for the retained EGA 320×200 graphics
pipeline and rounds each frame independently to the nearest centisecond:

```text
gifDelayCentiseconds = (delayRetraces * 100 + 30) / 60
```

Integer arithmetic makes the output deterministic. A source delay of zero is
encoded as GIF delay zero; the exporter does not invent a pause. GIF viewers
are permitted to impose their own minimum display delay, so the five one-frame
still files can be reported or displayed as longer than their encoded timing.
For example, the local FFmpeg build reports 0.10 seconds for those zero-delay
single-frame GIFs. The export result retains both total source retraces and the
converted centiseconds so callers can distinguish source data from adapter
policy.

The 60 Hz choice is a renderer policy, not a byte stored in ANM. It is isolated
in `AnimationGifTiming` so a later port can offer a different playback clock
without changing the verified container/timing decoder.

## Encoding

`AnimationGifEncoder` is dependency-free and writes:

- an 88×88 GIF89a logical screen;
- the same explicit standard 16-colour EGA palette as PNG export;
- one complete image for every accumulated ANM frame;
- per-frame graphics control delays;
- a `NETSCAPE2.0` repeat count of zero, meaning loop indefinitely.

The encoder emits literal LZW codes and frequent clear codes. This deliberately
favours a short, auditable implementation over maximum compression: the full
original corpus is only about 1.04 MB. All frames are complete replacements,
so GIF disposal/compositing cannot expose the ANM XOR delta representation.

## Verification

The synthetic harness passes **120 assertions**. It checks timing conversion,
GIF signature and geometry, infinite-loop metadata, per-frame delays, invalid
inputs, overwrite protection, batch collision preflight, and all-22 batch
production.

The ignored original installation produced 22 GIFs containing all 198 decoded
frames, totalling 1,036,926 bytes. FFprobe independently decoded every output
as 88×88 video and counted the expected number of frames in each file. A visual
check of `O0.gif` shows a correctly oriented cockpit frame and palette.
Generated GIFs remain under ignored `artifacts/`; no copyrighted external asset
or derivative output is committed.
