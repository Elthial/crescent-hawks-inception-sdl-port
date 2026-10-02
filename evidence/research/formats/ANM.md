# ANM animation files

## Inventory and container

The local installation contains 22 files, `O0.ANM` through `O21.ANM`. Every
file length is a multiple of 128. The animation image width is 88 pixels and the
decoded packed-frame buffer is `0x0F20` bytes, matching 88×88 four-bit pixels.

| Offset | Size | Confidence | Meaning |
|---:|---:|:---:|---|
| `0x00` | `0x20` | P | Playback-control bytes indexed by frame number; zero terminates ordinary playback. The 32-byte extent is established from all local files rather than an explicit bound in the loop. |
| `0x20` | `0x12` | V | Timing lookup indexed by `playbackControl - 0x41`. |
| `0x32` | `0x01` | V | Timing scale byte. |
| `0x33` | remainder | P | Compressed XOR-delta frame stream. |

The first two bytes are control/header values, not a size. The former
`AnimationFile.Size = BitConverter.ToInt16(bytes, 0)` interpretation was
therefore incorrect. Phase 3F replaces it with the actual file length while
retaining the complete header and compressed stream.

## Header observations

The playback code indexes the first header region by frame number and stops when
the selected byte is zero. The first zero within its 32 bytes therefore gives
the ordinary sequence length for the current files:

| File | Entries | File | Entries |
|---|---:|---|---:|
| O0 | 18 | O11 | 6 |
| O1 | 14 | O12 | 11 |
| O2 | 29 | O13 | 3 |
| O3 | 12 | O14 | 9 |
| O4 | 13 | O15 | 6 |
| O5 | 8 | O16 | 7 |
| O6 | 31 | O17 | 1 |
| O7 | 6 | O18 | 1 |
| O8 | 3 | O19 | 1 |
| O9 | 12 | O20 | 1 |
| O10 | 5 | O21 | 1 |

Playback values are in the `0x41..0x52` range and select one of the 18 timing
bytes at `+0x20..+0x31`. They are not themselves frame offsets. O6 uses entries `0..30`
and terminates at the final playback-control byte, `+0x1F`. All 22 files also
contain the timing scale at `+0x32`.

After drawing a frame, `0800:1BBE` calculates the wait count as
`signedLowWord(3 * signedByte(table[control - 0x41]) * signedByte(scale)) >> 2`
and passes it to the vertical-retrace wait at `1F3D:0006`. Thus the file defines
exact retrace counts rather than milliseconds. The active video-mode refresh
rate is an external presentation property.

## Frame compression

Clean assembly at `207F:23EC` establishes the following grammar. Unlike the
CMP/ICN extended-repeat form, the two count bytes in an ANM zero-control token
are stored **big-endian**: the decoder loads a little-endian word and then
executes `XCHG AL,AH`.

| Signed control byte | Following data | Verified output |
|---:|---|---|
| `+N` (`1..127`) | `N` literal bytes | XOR each literal into the next destination byte. |
| `-N` (`-1..-128`) | one byte `V` | XOR `V` into the next `N` destination bytes. |
| `0` | big-endian word `N`, then byte `V` | XOR `V` into the next `N` destination bytes. |

Each call writes exactly `0x0F20` packed bytes. The routine returns the number
of compressed bytes consumed; the caller advances its stream pointer by that
amount while retaining the destination buffer for the next frame. Thus each
frame describes changes from the accumulated prior frame.

Phase 3G's `AnimationFrameDecoder` implements these semantics with explicit
cursor accounting and malformed-input diagnostics. All advertised frames in
all 22 local files decode exactly. A short, uniform-value remainder follows the
last frame in every file and reaches the 128-byte file boundary; it remains
uninterpreted rather than being silently discarded as known padding.

## Pixel layout and EGA transpose

The accumulated `0x0F20`-byte frame contains two 4-bit palette indices per
byte, high nibble first and low nibble second. It therefore expands directly to
`0x1E40` bytes, or 88×88 pixels. This is not inferred solely from the byte
count: `0800:1AFD` sends the packed buffer through `207F:0572`, whose bit-rotate
loop transposes each group of eight chunky pixels into four EGA plane bytes.
`207F:1E37` then writes those plane bytes across 11 byte-columns and 88 rows.

Phase 3H's `AnimationPixelDecoder` exposes the pre-hardware representation as
one byte per palette index. Rendering, RGB palette selection, timing, GIF
export, and playback remain separate later steps.

Phase 3I adds single-frame indexed PNG export using the standard EGA RGB
mapping. The ANM container does not carry RGB palette data, so this is an
explicit renderer default rather than a property attributed to the file.

Phase 3J adds whole-sequence PNG export. It decodes the source only once and
writes each accumulated XOR frame in playback order with deterministic numbered
filenames. No duration is assigned at this stage.

Phase 3K exposes the verified timing table, scale byte, and exact per-frame
retrace counts.

Phase 3L adds a looping GIF adapter. It uses a documented nominal 60 Hz EGA
playback rate and nearest-centisecond rounding while retaining the exact source
retrace totals in its result. Zero-retrace waits are encoded as zero; a viewer
may still impose its own minimum delay. The ANM file itself contains neither a
refresh-rate field nor RGB palette values.

## Code references

- `Btech/BTECH_0800.c`, `0800:1AFD`: decodes, draws, advances the compressed
  pointer, looks up the header timing value, and increments frame number.
- `Btech/BTECH_0800.c`, `0800:48B7`: `Display_Animation_Scene` builds and loads
  `O{id}.ANM`, applies the outtake-frequency policy, and loops until the next
  header control entry is zero. O8 is replayed seven times. See the
  [animation-scene controller](../phase4/BTECH_0800_ANIMATION_SCENE_CONTROLLER.md).
- `Btech/BTECH_11B8.c`, `11B8:16B2`: the failed-start path loads `O0.ANM`
  directly and plays frames 0 through 7, stopping when the next control byte
  reaches `49h`.
- `InceptionTools/Animation/AnimationFrameDecoder.cs`: verified bounded frame
  and accumulated-sequence decoder.
- `InceptionTools/Animation/AnimationPixelDecoder.cs`: verified high/low-nibble
  expansion to 88×88 palette indices.
- `InceptionTools/Animation/AnimationPngEncoder.cs`: portable indexed PNG
  encoding with the standard EGA palette.
- `InceptionTools/Animation/AnimationFrameExporter.cs`: bounded single-frame
  selection and overwrite-safe file export.
- `InceptionTools/Animation/AnimationTimingDecoder.cs`: verified timing-table
  lookup and retrace-count arithmetic.
- `InceptionTools/Animation/AnimationGifTiming.cs`: explicit 60 Hz to GIF
  centisecond adapter policy.
- `InceptionTools/Animation/AnimationGifEncoder.cs`: portable looping GIF89a
  writer.
- `InceptionTools/FileTypes/AnimationFile.cs`: legacy metadata adapter, corrected in Phase 3F.
- `InceptionTools/Records/AnmFileRecord.cs`: bounded, lossless container model.
- `InceptionTools/Inspection/AnimationInspector.cs`: read-only text/JSON view.
