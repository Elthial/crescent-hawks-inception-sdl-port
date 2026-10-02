# `BTECH_0800` map-loader setup

## Reviewed block

- Address range: `0800:2DA8-2EEF`.
- Maintained name: `DOS_Load_Map_Files`.
- Review boundary: the actual `.MTP` open and read begins at `0800:2EF0`.

This first stage selects map-specific collision rules, constructs the map
filename, ensures the correct tile artwork is loaded, and selects the logical
game disk containing the requested map.

## Arguments are 16-bit words

The clean assembly reads both arguments as words:

| Stack location | Maintained name | Meaning |
|---|---|---|
| `[BP+6]` | `MapGridSlot` | Destination slot in the cached 3-by-3 map grid. |
| `[BP+8]` | `MapNumber` | Numeric map-file identifier. |

The previous `unsigned char` declarations discarded the original calling
convention. The file identifier may currently fit in a byte, but it was passed
and consumed as a 16-bit C argument.

## Blocking tile-code threshold

`3EDB:0150` has been renamed from `UnknownMapVariable` to
`BlockingTileCodeThreshold_0150`. Movement and combat-map tests consistently
treat tile codes at or above this value as blocking.

The loader initializes it as follows:

| Map | Threshold | Additional state |
|---|---:|---|
| Ordinary map | `0x55` | `InsideStarLeagueCache = false` |
| Star League Cache (`0x0E`) | `0x21` | `InsideStarLeagueCache = true` |

Other code assigns `0x8B` for a separate scene/mode. The rename expresses the
verified comparison role without claiming that every tile category above the
threshold has already been catalogued.

Confidence: **high**, from repeated `>= 3EDB:0150` collision tests and the
map-dependent assignments.

## Exact `.MTP` filename construction

The executable-owned strings are:

- `3EDB:05A5`: `MAP`
- `3EDB:05A9`: `.MTP`

The helper sequence copies `MAP`, writes `MapNumber` in radix 10 starting at
the fourth byte, and appends `.MTP`. The result at `3092:0012` is therefore:

```text
MAP<decimal map number>.MTP
```

The old comments described the fragments as merely “likely”; the segment data
makes both literals certain.

## Tile-art selection

The routine first requests logical disk 2 because the tile artwork resides
there. If map `0x0B` (the destroyed Citadel) is requested while another tileset
is resident, it loads and decompresses `DESTRUCT.ICN`, transfers it into the
off-screen EGA tile buffer at segment `A400`, and records tileset 1.

For any map other than `0x0B`, a nonstandard resident tileset is replaced by
`BTTLTECH.ICN` through `Load_And_Draw_BTTLTECH_ICN`.

The original destroyed-Citadel branch also:

- calls `207F:00D1` with `2FE8:0150`, apparently rebuilding the original
  adapter colour-translation tables; and
- sets `GraphicsCompatibilityFlag_4FBC` at `3EDB:4FBC`.

The flag assignment is represented in the annotated source. The removed
adapter helper is recorded as a `Sol:` TODO rather than recreated from an
unverified high-level interpretation. The original screen transfer is guarded
by graphics-adapter value 2; the maintained codebase intentionally retains
only the EGA pipeline, so the transfer is direct.

## Map disk selection

After tile-art handling, the loader requests logical disk 1, except for these
map numbers which select disk 2 before the file open:

- Citadel, map 1;
- destroyed Citadel, map `0x0B`;
- Star League Cache and later maps, `MapNumber >= 0x0E`.

This is original removable-media routing. A hard-disk installation records the
logical request but does not switch the DOS default drive.

## Porting notes

- Keep logical disk identity separate from host filesystem paths.
- Treat the 3-by-3 cache slot and map identifier as 16-bit call arguments.
- Preserve the tileset residency check so artwork is not decompressed for every
  neighboring map load.
- Name the `A400` destination as an off-screen EGA tile buffer in the C# port;
  the inherited `VGA_Buffer` macro is misleading.
- Review `207F:00D1` with the graphics-support block before deciding whether an
  EGA-only replacement needs any equivalent setup.

No Astra review is requested for this block. The control flow, strings,
arguments, disk numbers, and tile assets are directly visible in the clean
assembly and data segment.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:2DA8-2EEF`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_3EDB.asm`, strings at
  `3EDB:05A5-05BA`.
- `Btech/BTECH_0800.c`, `DOS_Load_Map_Files`.
- `Btech/BTECH_0800.c`, `Map_Interactables_Building_Or_Items` collision tests.
- `Btech/BTECH_0DAB.c` and `Btech/BTECH_1631.c`, combat movement tests.
