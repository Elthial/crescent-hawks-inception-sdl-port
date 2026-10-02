#include "game.h"


/* Sol: Original0800:320B..32B2 retained EGA path, checked against full ASM.
 * ANIMATE's128-byte header is not sprite pixels. The following3840 bytes
 * contain three alternatives for ten16x16 map tiles. 0780 is a WORD count,
 * not a byte count; original annotation's shorter array would truncate it.
 * Missing disk selection/compatibility stores in annotations are restored. */
void Load_And_Draw_ANIMATE_ICN(void)
{
    Select_Game_Disk_And_Drive(GameDisk_Second);
    GraphicsCompatibilityFlag=TRUE;
    Load_File_To_Memory((const uint8_t *)"ANIMATE.ICN",GraphicsFileWorkspace);
    Decompress_File_Into_Memory(GraphicsFileWorkspace,GraphicsSceneWorkspace);
    uint8_t *pixels=GraphicsSceneWorkspace+AnimatedMapTileHeaderBytes;
    VGA_Inline_ASM_Loop(pixels,pixels,AnimatedMapTileBytes/2);
    /* Inline original207F:0A76 REP MOVSW. Preceding0572 exits with DF clear;
     * this caller therefore copies forward. Two-byte read precedes writes. */
    for (uint16_t offset=0;offset<AnimatedMapTileBytes;offset+=2) {
        uint8_t low=pixels[offset],high=pixels[offset+1];
        AnimatedMapTileFrames[offset]=low;
        AnimatedMapTileFrames[offset+1]=high;
    }
    Load_And_Draw_BTTLTECH_ICN();
    Select_Game_Disk_And_Drive(GameDisk_First);
}
