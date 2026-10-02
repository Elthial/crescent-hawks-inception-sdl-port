#include "game.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned step;
void Set_Palette_registers(const uint8_t *palette)
{ assert(++step==1 && palette==DefaultEgaPalette && TilesetId==Tileset_Destruct); }
void Select_Game_Disk_And_Drive(uint16_t disk)
{ assert(++step==2 && disk==GameDisk_Second); }
uint16_t Load_File_To_Memory(const uint8_t *filename,uint8_t *memory)
{
    assert(++step==3 && strcmp((const char *)filename,"BTTITLE.CMP")==0);
    assert(memory==GraphicsSceneWorkspace && GraphicsCompatibilityFlag==TRUE);
    return TRUE;
}
void Decompress_File_Into_Memory(uint8_t *compressed,uint8_t *decoded)
{
    assert(++step==4 && compressed==GraphicsSceneWorkspace && decoded==GraphicsFileWorkspace);
    assert(GraphicsCompatibilityFlag==FALSE);
}
void DrawCall_Image_To_VGA_Memory(uint8_t *image,uint16_t segment)
{ assert(++step==5 && image==GraphicsFileWorkspace && segment==EgaSceneStagingSegment); }
void Draw_GraphicsFile_In_Memory(uint8_t *fileBuffer,uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{
    assert(++step==6 && fileBuffer==GraphicsFileWorkspace);
    assert(x==0 && y==0 && width==40 && height==200 && TilesetId==Tileset_Destruct);
}
int main(void)
{
    TilesetId=Tileset_Destruct; GraphicsCompatibilityFlag=TRUE;
    Load_And_Draw_BTTITLE_CMP();
    assert(step==6 && TilesetId==Tileset_None && GraphicsCompatibilityFlag==FALSE);
    puts("Original title workflow: native buffer swap, palette/disk/draw order and state passed.");
    return 0;
}
