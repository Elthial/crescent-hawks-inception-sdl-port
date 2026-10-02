# Original file formats

This folder is the single destination for file-format documentation produced by
this project. The descriptions below are an initial normalized inventory, not a
claim that every imported detail has been independently verified.

Original samples live only in the ignored `chinception/` reference installation.
They may be inspected locally but must not be copied into this folder or any
other tracked path. The future replacement executable will load these files in
their original formats.

## Format inventory

| Format | Purpose | Current confidence | Existing implementation/evidence |
|---|---|:---:|---|
| `.CMP` | Compressed EGA images/sheets | P | Verified bounded formats 1/2 decoder and portable indexed PNG exporters. |
| `.ICN` | Tile/icon graphics | P | Verified bounded decoder, 16×4000 tile-strip export, and portable map consumption. |
| `.MTP` | Local and star maps | P | Bounded standard/star records and portable PNG/JSON export; tile-order selection remains probable. |
| `.ANM` | 88-pixel-wide delta animations | P | Bounded retained-XOR decoding, timing, PNG sequence, and looping GIF export. |
| `.BLD` | Encrypted building dialogue and bytecode | V | Length-prefixed payload, decode, interpreter origin, and 28 decoded opcodes verified against clean assembly. |
| Save (no extension) | Serialized game state | P | Annotated contiguous arrays and old InceptionTools save parser. |
| `DEMOFILE` (no extension) | Attractor/self-play instruction stream | V | Loaded only after no intro input; exact `0x03FF`-byte read and byte-wise consumption in clean assembly. |
| `.SIF` | Intro-song sound data | P | Headerless four-channel note frames, rest/pitch conversion, and PC-speaker/Tandy timer consumers recovered; exact Tandy timbre remains open. |
| Palette data | EGA colour mapping | H | Portable standard EGA table plus three preserved legacy file overrides; executable origin still to normalize. |

Detailed references:

- [Original installation manifest](ORIGINAL_INSTALLATION.md)
- [CMP/ICN graphics and RLE](GRAPHICS.md)
- [MTP maps](MAPS.md)
- [ANM animations](ANM.md)
- [BLD building scripts](BLD.md)
- [BLD decoded opcode reference](BLD_OPCODES.md)
- [Save files](SAVE.md)
- [Demo input stream and SIF sound data](DEMO_AND_SOUND.md)

## Shared compressed-image header

Original samples and InceptionTools establish this prefix:

| Offset | Size | Confidence | Meaning |
|---:|---:|:---:|---|
| `0x00` | 2 | V | Little-endian stored length following this word; equals file length minus 2. |
| `0x02` | 1 | V | Compression format ID. |
| `0x03` | variable | V | Compressed payload. |

Imported research describes format `0x01` as row-oriented and `0x02` as
column-oriented RLE. The exact run rules and planar conversion will be rewritten
here after comparison with the annotated decompressor and original samples.

## MTP maps

The original loader reads this standard-map prefix:

| Relative position | Size | Confidence | Meaning |
|---:|---:|:---:|---|
| `0x000` | 1 | V | Reserved byte, read but unused by this loader. |
| `0x001` | 1 | V | X offset in the selected 8×8 block-descriptor grid. |
| `0x002` | 1 | V | Y offset in that descriptor grid. |
| `0x003` | 1 | V | Width in tiles. |
| `0x004` | 1 | V | Height in tiles. |
| `0x005` | `0x80` | V | Eight fixed 16-byte map-character names. |
| `0x085` | `0x100` | V | Sixteen fixed 16-byte building names. |
| `0x185` | `0x20` × 4 | V | Four 16-entry word coordinate/interaction arrays. |
| `0x205` | `0x10` | V | Alternate BLD-file lookup array. |
| `0x215` | `0x08` | V | Roaming-NPC waypoint/link array. |
| `0x21D` | width × height | V | Byte tile IDs organized around 8×8-tile blocks. |

`MAP15.MTP` is treated specially as a 32×24 star map with no normal metadata
prefix. See `MAPS.md` for exact destinations, file-size groups, and the
remaining within-block ordering question.

## ANM animations

Current findings:

- Width is 88 pixels. **P**
- Compressed data begins at `file+0x33`. **P**
- Bytes `0x00..0x32` form a frame/control header. The first two bytes are not a
  generic size word; the current InceptionTools `Size` property is wrong. **V**
- Imported research describes cumulative XOR-delta frames and a 0x0F20-byte
  nibble buffer for an 88×88 image. **H** until reproduced locally against the
  original files.
- Imported notes identify `O0.ANM` through `O21.ANM`; earlier subsets listing
  fewer files should not be treated as a complete inventory. **P**

## BLD building scripts

BLD files consist of a two-byte stored-length prefix followed by one encrypted
script payload. The generic loader consumes the length word, places the payload
at `3092:00A0`, decodes it from its first byte, and passes that same address to
the interpreter. Thus script offset zero is file offset `0x02`; `0xA0` is not a
file offset. See `BLD.md` and `BLD_OPCODES.md` for the verified transformation,
address domains, and complete decoded dispatch table.

## Save file

The beginning of the save is strongly constrained by its contiguous records:

| Offset | Size | Confidence | Meaning |
|---:|---:|:---:|---|
| `0x0000` | 1 | P | Header/state byte; `0x0C` in all six local saves. |
| `0x0001` | `8 × 0x11` | V | Player infantry records. |
| `0x0089` | `8 × 0x11` | V | Enemy infantry records. |
| `0x0111` | `4 × 0x7D` | V | Player lance mech records. |
| `0x0305` | `4 × 0x7D` | V | Enemy mech records. |
| `0x04F9` | `0x800` | P | Bit-packed world-map visibility data. |
| `0x0F45` | 2 | V | Party map position X. |
| `0x0F47` | 2 | V | Party map position Y. |
| other later fields | variable | H | Missions, inventory, finance, and other state. |

All six local slots are exactly `0x0F49` bytes. The old InceptionTools parser
starts its final four-byte position read at `0x0F44`, one byte too early, and
leaves the last byte unread. See `SAVE.md`.

## Next format-documentation tasks

1. Trace the original RLE decoder and write exact run semantics.
2. Create a sample manifest containing filename, size, header, format ID, and
   hash without checking in the samples.
3. Reproduce ANM frame extraction locally.
4. Reconstruct the complete save layout from load/save copy operations.
5. Recover the internal `WWOODBT.SIF` event stream and exact `0x48` demo token
   semantics when the sound/input workflows reach controlled annotation review.
