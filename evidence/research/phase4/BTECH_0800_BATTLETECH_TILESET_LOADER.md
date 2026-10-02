# `BTECH_0800` BattleTech tileset loader

## Review boundary

This light block covers the complete routine at `0800:4621-46A6`, retained as
`Load_And_Draw_BTTLTECH_ICN`. The next routine begins at `0800:46A7`.

## Workflow

The routine restores the normal exploration tileset:

1. It calls `207F:00D1(2FE8:0150)`. That helper does work only for graphics
   adapter 0, where it rebuilds two 256-byte CGA colour-conversion tables from
   the supplied 32-byte palette table. It returns immediately in EGA mode.
2. It sets `GraphicsCompatibilityFlag_4FBC` at `3EDB:4FBC` to one.
3. It selects original game disk 2 and loads `BTTLTECH.ICN` into `246C:244B`.
4. It decompresses the file into the reusable buffer at `3092:4614`.
5. For graphics-adapter value 2, it transfers the result to the off-screen EGA
   tile buffer at segment `A400`.
6. It records tileset ID 0 (`TileSet_BattleTech`) at `3092:3988`.

The maintained pseudocode represents the state changes, asset load,
decompression, and EGA transfer. It documents but does not restore the
CGA-only table builder, matching the project's EGA-only target.

## Porting notes

- `BTTLTECH.ICN` remains an external copyrighted asset required from the
  original game installation.
- A portable renderer can decode the ICN directly into host-side pixels; the
  DOS `A400` transfer is hardware layout, not part of the asset format.
- The inherited `VGA_Buffer` name means segment `A400` here, but the executable
  reaches it specifically through the EGA/VGA adapter-2 branch.
- No Astra review is needed: the control flow, segment addresses, disk number,
  filename and adapter test are explicit in the clean assembly.
