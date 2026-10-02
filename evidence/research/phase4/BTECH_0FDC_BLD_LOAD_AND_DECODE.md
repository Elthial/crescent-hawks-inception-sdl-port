# `BTECH_0FDC` indexed BLD load and decode

## Review boundary

This block covers `Load_And_Decode_Indexed_BLD_1D30`, `0FDC:1D30-1DC1`, the
final routine in `BTECH_0FDC.c`. It is called when entering a building and when
mission teardown must restore a BLD script displaced from the shared buffer by
`BTSTATS.CMP`.

## Logical disk selection

The routine always calls `Select_Game_Disk_And_Drive_28CC(1)` first. It then
calls the same helper with logical Disk 2 when:

```text
BldFileId < 0x0002 || BldFileId >= 0x0011
```

The resulting original-media grouping is:

- Game Disk 1: IDs `0x02..0x10`, COMSTAR.BLD through THEATER.BLD;
- Disk 2: IDs `0x00..0x01` and `0x11..0x19`, TRAINING/CITADEL and FROB through
  WINSCENE.BLD.

`28CC` stores the requested logical disk and selects DOS drive A or B according
to installation settings. It does not show the disk-insertion prompt. On a
hard-disk installation, it records the logical disk number but performs no DOS
drive switch.

## Filename lookup and payload destination

The BLD ID is preserved as a WORD at `3092:3FF8`. The static table beginning at
`3EDB:4EC2` contains 26 native 16:16 far pointers, not 26 WORD offsets. Indexing
it with `BldFileId * 4` produces the filename passed to `Load_File_To_Memory`.

The destination is the shared byte buffer at `3092:00A0`. The lower-level file
loader consumes the two-byte stored payload length, then reads precisely that
many following bytes into `00A0`; the length word itself is not copied there.

## Fixed-span in-place decode

The executable transforms each byte as:

```text
decoded = ((stored + 0x29) & 0xFF) XOR 0xE9
```

Addition occurs before XOR. The loop counter and bound are WORDs, and the exact
bound is hexadecimal `0x2328`—9000 decimal bytes—covering runtime addresses
`3092:00A0..23C7`.

Every shipped BLD payload is shorter than that span. The original therefore
also transforms stale bytes beyond the just-loaded payload. This is retained as
documented `QUIRK-001`; safe tooling and a new runtime should normally decode
only the validated stored payload length.

After decoding, WORD `3092:4594` is cleared to record that BLD data, rather than
`BTSTATS.CMP`, now occupies the shared buffer.

## Corrections to the former pseudo-C

- Restored both original disk-selection calls and their exact ID bounds.
- Corrected the filename table from WORD entries to 16:16 far pointers.
- Replaced ambiguous decimal `9000` with named exact bound `0x2328`.
- Parenthesized the addition-before-XOR transform and made byte wrap explicit.
- Identified `3092:00A0` as the destination address, not a file offset.
- Renamed the routine to include both loading and decoding.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:1D30-1DBD`;
- `BTech-Reko-expanded/BTECH.reko/BTECH_1F3D.asm`, `1F3D:063B`, for the
  length-prefixed read;
- filename-pointer bytes at `3EDB:4EC2` and target strings at `3EDB:478A`;
- all 26 ignored `chinception/*.BLD` files, whose stored lengths equal file
  length minus two;
- successful InceptionTools disassembly of the shipped `ARENA.BLD` using the
  same transform and payload origin.

The disk conditions, pointer width, lookup stride, destination, transform,
fixed span, and cache flag are directly verified. No Astra review is required.
