# `BTECH_0800` title-screen loader

## Review boundary

This block covers `0800:46A7-476C`, retained as
`Load_And_Draw_BTTITLE_CMP`. The music routine begins at `0800:476D`.

## Workflow

The routine selects the default full-screen palette, loads the original
disk-2 `BTTITLE.CMP`, decompresses it as a 320x200 packed image, draws it, and
marks the map tileset as unavailable:

1. Adapter 2 uses the sixteen EGA palette bytes at `2FE8:0000`; adapter 3 uses
   sixteen MCGA colour words at `2FE8:0010`.
2. Disk 2 is selected. The original then invokes a CGA-only conversion-table
   setup with `2FE8:0210`; this is a no-op for the maintained EGA path.
3. The compressed file is loaded at `3092:4614`, `GraphicsCompatibilityFlag_4FBC` is cleared,
   and the image is decompressed at `246C:244B`.
4. Adapter 2 converts/transfers the packed image into segment `A800`.
5. The common draw copies the complete 40-by-200 screen area.
6. `3092:3988` receives the word sentinel `0xFFFF`, meaning no map tileset is
   resident. The old `TileSet_None` value of `0x00FF` was incorrect.

`BTTITLE.CMP` remains a required external original-game asset. Its verified
format-2 decoder and portable exporter are documented in
`docs/formats/GRAPHICS.md`; no embedded copy is introduced here.

The assembly is direct and this block does not need Astra confirmation.
