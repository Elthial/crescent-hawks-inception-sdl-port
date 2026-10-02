#include "game.h"

/* Sol: Original0800:4621..46A6, retained EGA adapter path. The original
 * CGA-only00D1 returns unchanged for EGA; no fake replacement is needed.
 * Disk selection and compatibility writes precede the real file/decode calls. */
void Load_And_Draw_BTTLTECH_ICN(void)
{
    GraphicsCompatibilityFlag=TRUE;
    Select_Game_Disk_And_Drive(GameDisk_Second);
    Load_File_To_Memory((const uint8_t *)"BTTLTECH.ICN",GraphicsFileWorkspace);
    Decompress_File_Into_Memory(GraphicsFileWorkspace,GraphicsSceneWorkspace);
    DrawCall_Image_To_VGA_Memory(GraphicsSceneWorkspace,EgaTilesetBufferSegment);
    TilesetId=Tileset_BattleTech;
}
