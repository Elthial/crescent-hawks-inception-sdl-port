# Phase 3R — legacy extractor retirement

## Review boundary

The maintained bounded APIs now cover every output formerly produced by the
interactive extractor: CMP/ICN images, MTP maps, MECHSHAP sprites, and ANM
animations. This block removes only that superseded extraction graph and its
Windows-only image dependency. It does not remove unrelated reverse-engineering
notes or the legacy save/data classes.

## Removed implementation

- the interactive `AssetExtractor` entry point;
- unbounded `RunLengthEncoding` decoders;
- legacy `AnimationFile`, `InceptionImageFile`, `MAP`, `MapFile`, and `STARMAP`
  wrappers;
- `EGA`, `Sprites`, `Palette`, and `PaletteColour` GDI+ renderers;
- the extractor-only `ImagePurpose` and `MapFormat` enums;
- the `System.Drawing.Common` package reference;
- the legacy `log4net` startup/error wrapper and its final package dependency.

`EGA_Animations.cs` remains because it contains code-oriented reconstruction
notes rather than a live extraction path. `SpriteEnum.cs` remains as a useful
public ID reference. The old `SaveFile` class and its `AssetFile` base remain
outside this graphics-only cleanup boundary.

Running InceptionTools without arguments now prints command help instead of
prompting for a directory and writing a large implicit `Assets` tree. Extraction
is explicit, scriptable, collision-safe, and directed by output arguments.

## Maintained replacements

| Legacy responsibility | Replacement |
|---|---|
| CMP/ICN decompression and image output | `CompressedImageDecoder`, `CompressedGraphicsExporter` |
| map parsing and drawing | `MtpMapRecord`, `MtpMapExporter` |
| MECHSHAP sprite extraction | `MechShapeSpriteCatalog`, `MechSpriteSheetExporter` |
| ANM decompression and output | `AnimationFrameDecoder`, `AnimationFrameExporter` |
| EGA PNG writing | `EgaIndexedPngEncoder` |

## Verification

After removal, InceptionTools itself has no third-party package dependencies,
builds with **zero warnings and zero errors**, and the dependency-free harness
passes **178 assertions**. The previously verified real-file results remain:

- all twelve CMP/ICN files export successfully;
- all fifteen MTP outputs match legacy rendered pixels exactly;
- all 22 ANM files / 198 frames export successfully;
- all 376 MECHSHAP source sprites remain represented.

The two tracked README display images, `InceptionTools/MAP1.png` and
`InceptionTools/MECHSHAP.png`, are intentionally retained by project-owner
direction. No generated artifact directory is tracked.
