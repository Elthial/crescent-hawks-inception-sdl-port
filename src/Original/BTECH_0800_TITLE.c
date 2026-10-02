#include "game.h"

/* Sol: Original0800:46A7..476C, complete raw ASM retained EGA path.
 * Title reverses the tileset buffers: encoded3092:4614 -> decoded246C:244B.
 * Native CGA-only00D1 is a no-op on retained EGA. No invented present call. */
void Load_And_Draw_BTTITLE_CMP(void)
{
    Set_Palette_registers(DefaultEgaPalette);
    Select_Game_Disk_And_Drive(GameDisk_Second);
    Load_File_To_Memory((const uint8_t *)"BTTITLE.CMP",GraphicsSceneWorkspace);
    GraphicsCompatibilityFlag=FALSE;
    Decompress_File_Into_Memory(GraphicsSceneWorkspace,GraphicsFileWorkspace);
    DrawCall_Image_To_VGA_Memory(GraphicsFileWorkspace,EgaSceneStagingSegment);
    Draw_GraphicsFile_In_Memory(GraphicsFileWorkspace,0,0,EgaFramebufferRowBytes,EgaScreenHeight);
    TilesetId=Tileset_None;
}
