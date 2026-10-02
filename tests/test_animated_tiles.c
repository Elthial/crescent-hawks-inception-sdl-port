#include "game.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned step;
void Select_Game_Disk_And_Drive(uint16_t disk)
{
    ++step; assert((step==1 && disk==GameDisk_Second) || (step==5 && disk==GameDisk_First));
}
uint16_t Load_File_To_Memory(const uint8_t *name,uint8_t *memory)
{
    assert(++step==2 && !strcmp((const char *)name,"ANIMATE.ICN"));
    assert(memory==GraphicsFileWorkspace && GraphicsCompatibilityFlag==TRUE);
    return 0;
}
void Decompress_File_Into_Memory(uint8_t *source,uint8_t *destination)
{
    assert(++step==3 && source==GraphicsFileWorkspace && destination==GraphicsSceneWorkspace);
    memset(destination,0xCC,GraphicsWorkspaceBytes);
    for (unsigned i=0;i<AnimatedMapTileBytes;i+=4) {
        uint8_t *p=destination+AnimatedMapTileHeaderBytes+i;
        p[0]=0x12; p[1]=0x34; p[2]=0x56; p[3]=0x78;
    }
}
void Load_And_Draw_BTTLTECH_ICN(void)
{
    assert(++step==4);
    const uint8_t planes[]={0xAA,0x66,0x1E,0x01};
    for (unsigned i=0;i<AnimatedMapTileBytes;++i) assert(AnimatedMapTileFrames[i]==planes[i%4]);
    for (unsigned i=0;i<AnimatedMapTileHeaderBytes;++i) assert(GraphicsSceneWorkspace[i]==0xCC);
    assert(GraphicsSceneWorkspace[AnimatedMapTileHeaderBytes+AnimatedMapTileBytes]==0xCC);
    memset(GraphicsSceneWorkspace,0xDD,GraphicsWorkspaceBytes); /* base tiles overwrite workspace */
}
int main(void)
{
    Load_And_Draw_ANIMATE_ICN();
    assert(step==5 && AnimatedMapTileBytes==3840);
    assert(AnimatedMapTileFrames[AnimatedMapTileBytes-1]==1);
    puts("Original animation header skip, all three tile frames, WORD conversion/copy and disk restoration passed");
    return 0;
}
