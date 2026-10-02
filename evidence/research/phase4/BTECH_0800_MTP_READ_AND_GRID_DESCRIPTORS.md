# `BTECH_0800` MTP read and grid descriptors

## Reviewed block

- Address range: `0800:2EF0-30BA`.
- Parent routine: `DOS_Load_Map_Files`.
- Previous stage: filename, tileset, and logical-disk selection.
- Review boundary: map-specific post-load state begins at `0800:30BB`.

This stage opens a standard `MAP1.MTP` through `MAP14.MTP`, reads its fixed
metadata and tile payload, and marks the map's constituent 8×8-tile blocks in
one slot of the cached 3×3 world-map neighborhood.

## Open failure and disk retry

`Get_FileHandle` returns the 16-bit value `0xFFFF` when the open fails. The old
annotated C instead compared the result with `0x0001`, treating a valid handle
as the error case and allowing `0xFFFF` into the read calls.

On failure the original calls:

```text
Request_Game_Disk_2913(RequestedGameDiskNumber_014E)
```

and retries the open. The request therefore uses whichever logical disk the
preceding setup selected for this map. The retry is unbounded: the prompt waits
for any key, ignores which key was returned, and the loader tries the file
again.

The maintained `FileHandle` is now a signed 16-bit value so `-1` represents the
original `0xFFFF` sentinel without inheriting Reko's 32-bit `int` assumption.

## Five-byte map header

The first five reads target stack bytes, not globals in segment `3EDB`:

| File offset | Local meaning | Runtime use |
|---:|---|---|
| `0x000` | Reserved/unknown | Read, never used by this function. |
| `0x001` | Descriptor-grid X offset | Added directly to the destination pointer. |
| `0x002` | Descriptor-grid Y offset | Multiplied by 8 and added to the destination pointer. |
| `0x003` | Width in tiles | Shifted right three bits to obtain block columns. |
| `0x004` | Height in tiles | Shifted right three bits to obtain block rows. |

All original standard MTP files currently available in `chinception` contain
`00 00 00` in the first three positions. The offset behaviour is still real
code and should remain in a general loader rather than being hard-coded away.

## Fixed reads

After the five header bytes, the DOS routine performs these reads in order:

| File range | Bytes | Runtime destination |
|---|---:|---|
| `0x005-0x084` | `0x80` | `246C:A461`, eight 16-byte character names. |
| `0x085-0x184` | `0x100` | `246C:A561`, sixteen 16-byte building names. |
| `0x185-0x1A4` | `0x20` | `3092:4564`, sixteen 16-bit interaction X values. |
| `0x1A5-0x1C4` | `0x20` | `3092:4596`, sixteen 16-bit interaction Y values. |
| `0x1C5-0x1E4` | `0x20` | `3092:39B4`, sixteen 16-bit character/map X values. |
| `0x1E5-0x204` | `0x20` | `3092:39D4`, sixteen 16-bit character/map Y values. |
| `0x205-0x214` | `0x10` | `3092:4602`, alternate-BLD lookup bytes. |
| `0x215-0x21C` | `0x08` | `3092:3768`, roaming-NPC waypoint/link bytes. |
| `0x21D onward` | up to `0x1000` | `246C:101D`, byte tile IDs. |

The last DOS call always requests the maximum 64×64 payload (`0x1000` bytes).
For 32×32 and 8×8 maps DOS reaches EOF after `0x400` or `0x40` bytes. The game
does not check the returned byte count; the header dimensions constrain later
use to the data actually present.

This proves several corrections to `BTECH.h`:

- `MapTile` is a byte array, not a word array;
- the character-name area is `8 * 16` bytes;
- the alternate-BLD array contains `0x10` bytes; and
- each cached neighborhood slot occupies `0x40`, not decimal 40, bytes.

## The 3×3 cache contains block descriptors

`3EDB:0170` is nine consecutive 16:16 far pointers. They point to the nine
`0x40`-byte slots at:

```text
246C:0564  246C:05A4  246C:05E4
246C:0624  246C:0664  246C:06A4
246C:06E4  246C:0724  246C:0764
```

The loader selects pointer `MapGridSlot`, applies the header's `Y * 8 + X`
offset, and writes sequential byte IDs starting with `0x90`. It emits one ID
for every 8×8-tile block, with an eight-byte destination stride:

```text
blockRows    = mapHeight / 8
blockColumns = mapWidth / 8

for blockRow:
    for blockColumn:
        grid[blockRow * 8 + blockColumn] = nextId++
```

Consequently a 64×64 map fills all 64 descriptors (`0x90-0xCF`), a 32×32 map
fills a 4×4 area (`0x90-0x9F`), and an 8×8 map writes only descriptor `0x90`.
The fine tile IDs themselves remain in the payload buffer at `246C:101D`.

This also explains the combat loader's test: it dereferences the selected far
pointer and treats first descriptor `0x90` as “slot already loaded.” The old C
compared a byte of the pointer table itself.

## Porting notes

- Represent DOS open failure explicitly rather than treating handle 1 as a
  special value.
- A modern loader should validate exact payload length even though DOS relied
  on EOF and ignored the read count.
- Preserve header placement fields for format fidelity despite all known files
  using zero.
- Keep raw tile payloads separate from the nine 8×8 descriptor grids.
- Bounds-check `MapGridSlot` to `0..8` in the C# implementation.
- The descriptor construction confirms 8×8 block organization but does not by
  itself settle the direction of the within-block tile-order transform.

No Astra review is requested. File offsets, read sizes, pointer-table stride,
descriptor values, and loops are explicit in the clean assembly.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:2EF0-30BA`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_183B.asm`, `183B:2886-28A6`.
- `Btech/BTECH_0800.c`, `DOS_Load_Map_Files`.
- `Btech/BTECH_183B.c`, `Combat_Load_9Grid_Map_2835`.
- `chinception/MAP1.MTP` through `MAP14.MTP`, first-five-byte and size checks
  only; copyrighted files remain ignored and undistributed.
