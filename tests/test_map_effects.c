#include "game.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned draws;
static int16_t lastX,lastY;
static uint8_t sprites[256][1];
void DrawCall_EGA_CharacterPos(EgaMemoryAddress destination,uint8_t *sprite,int16_t x,int16_t y)
{
    assert(destination.segment==0xAC00 && destination.offset==0);
    assert(sprite==sprites[0x7C] || sprite==sprites[0x7D] || sprite==sprites[0x7E]);
    lastX=x; lastY=y; ++draws;
}
int main(void)
{
    for (unsigned i=0;i<256;++i) CombatSpritePointers[i]=sprites[i];
    CrescentHawkMapPositionX=0x0C45; CrescentHawkMapPositionY=0xC019;
    memset(MapEffectPackedPage,0,sizeof MapEffectPackedPage);
    memset(MapEffectPositionXLow,0,sizeof MapEffectPositionXLow);
    memset(MapEffectPositionYLow,0,sizeof MapEffectPositionYLow);
    MapEffectPackedPage[0]=0xCC; MapEffectPositionXLow[0]=0x45; MapEffectPositionYLow[0]=0x19;
    MapEffectSpriteIndex[0]=0x7C;
    Draw_Persistent_Map_Effects();
    assert(draws==1 && lastX==208 && lastY==96 && MapEffectSpriteIndex[0]==0x7D);
    Draw_Persistent_Map_Effects(); assert(draws==2 && MapEffectSpriteIndex[0]==0x7C);
    MapEffectPositionXLow[0]=0; /*same-page outside viewport: don't advance fire*/
    Draw_Persistent_Map_Effects(); assert(draws==2 && MapEffectSpriteIndex[0]==0x7C);
    /* Neighbour X page: broad signed carry projection wraps through local7F. */
    CrescentHawkMapPositionX=0x0100; CrescentHawkMapPositionY=0x1010;
    MapEffectPackedPage[0]=0x10; MapEffectPositionXLow[0]=0x7F; MapEffectPositionYLow[0]=0x10;
    Draw_Persistent_Map_Effects();
    assert(draws==3 && lastX==200 && lastY==96 && MapEffectSpriteIndex[0]==0x7D);
    MapEffectSpriteIndex[0]=0x7E;
    Draw_Persistent_Map_Effects(); assert(draws==4 && MapEffectSpriteIndex[0]==0x7E);
    MapEffectPositionYLow[0]=0x50; /*same Y page, offscreen*/
    Draw_Persistent_Map_Effects(); assert(draws==4);
    puts("Original map-effect projection, neighbour-page signed bounds and visible-only fire advancement passed");
    return 0;
}
