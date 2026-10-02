#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned boxes,keys,pauses,sounds,frames,retraces,stage;
static unsigned expectedDoor,animated;
static char text[256];
static uint8_t expectedTiles[MapFileMaximumTileBytes];
static void verify(int condition,unsigned line) { if(!condition) { fprintf(stderr,"Cache door mismatch line%u\n",line); exit(1); } }
#define check(condition) verify(!!(condition),__LINE__)
/* Original complete door routine, tables and shared storage. Drawing/input
 * boundaries check order and complete tile writes, without exporting art. */
void Draw_Message_Box(void) { ++boxes; }
void Display_Text_From_Memory(uint8_t *message) {
    size_t used=strlen(text),added=strlen((char *)message); check(used+added<sizeof text);
    memcpy(text+used,message,added+1);
}
void Wait_For_50Hz_Then_Check_Input(void) { ++pauses; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return 0; }
void Play_Sound_If_Enabled(uint16_t sound) { check(sound==Sound_CacheDoor && !frames); ++sounds; }
void PosXY_OffsetGrid(uint16_t x,uint16_t y) {
    check(x==0x0240 && y==0x3050 && stage++==0 && CacheDoorOpened[expectedDoor]==TRUE);
    unsigned localX=(unsigned)CacheDoorPositionX[expectedDoor],localY=(unsigned)CacheDoorPositionY[expectedDoor];
    unsigned base=(localY/16)*512+((localY%16)/2)*8+(localX/16)*64+(localX%16)/2;
    unsigned column=(localX%16)/2,offset=0,frame=animated?frames:2;
    for(unsigned tile=0;tile<4;++tile) {
        expectedTiles[base+offset]=CacheDoorAnimationTiles[frame*4+tile];
        ++offset; if(++column==8) offset+=56;
    }
    check(!memcmp(expectedTiles,MapFileTiles,sizeof expectedTiles));
}
void Copy_Data_To_GraphicsMemory(void) { check(stage++==1); }
void Draw_Infantry_And_Mechs(void) { check(stage++==2); }
void EGA_DrawBox_Wrapper(void) { check(stage++==3); stage=0; ++frames; }
void Wait_For_N_Vertical_Retraces(uint16_t count) { check(count==20 && stage==0); ++retraces; }
static void prepare(unsigned door,unsigned animate) {
    expectedDoor=door; animated=animate; boxes=keys=pauses=sounds=frames=retraces=stage=0; text[0]=0;
    memset(CacheDoorOpened,0,CacheDoorCount); memset(CacheSecurityCodeUsed,0,sizeof CacheSecurityCodeUsed);
    for(unsigned colour=0;colour<3;++colour) SelectedCacheCodeByColour[colour]=(uint8_t)(CacheDoorRequiredCode[colour][door]-1);
    for(unsigned i=0;i<sizeof expectedTiles;++i) expectedTiles[i]=(uint8_t)(i*7+3);
    memcpy(MapFileTiles,expectedTiles,sizeof expectedTiles);
    CrescentHawkMapPositionX=0x0240; CrescentHawkMapPositionY=0x3050; MessageBoxOpen=0x7777;
}
static void lookup(int16_t argument) {
    StarLeague_Key_Codes((uint16_t)(0x0700+CacheDoorPositionX[expectedDoor]-1),(uint16_t)(0x9000+CacheDoorPositionY[expectedDoor]),argument);
}
static void consumed(unsigned door) {
    for(unsigned code=0;code<CacheSecurityCodeCount;++code) {
        unsigned used=door!=CacheDoorEntrance &&
            (code==(unsigned)(CacheDoorRequiredCode[0][door]-1) || code==(unsigned)(CacheDoorRequiredCode[1][door]-1) || code==(unsigned)(CacheDoorRequiredCode[2][door]-1));
        check(CacheSecurityCodeUsed[code]==used);
    }
}
int main(void) {
    for(unsigned door=0;door<CacheDoorCount;++door) {
        prepare(door,TRUE); lookup(-1);
        check(frames==3 && retraces==3 && sounds==1 && !boxes && !keys && !pauses && MessageBoxOpen==0x7777);
        consumed(door);
        if(door==CacheDoorEntrance) for(unsigned colour=0;colour<3;++colour) check(SelectedCacheCodeByColour[colour]==255);
        else for(unsigned colour=0;colour<3;++colour) check(SelectedCacheCodeByColour[colour]==CacheDoorRequiredCode[colour][door]-1);
        for(unsigned other=0;other<CacheDoorCount;++other) check(CacheDoorOpened[other]==(other==door));
        /* Door consumption does not independently prevent opening it again. */
        frames=retraces=sounds=0; lookup(-1); check(frames==3 && sounds==1);
        prepare(door,FALSE); memset(SelectedCacheCodeByColour,0xFF,3);
        StarLeague_Key_Codes(0xFFFF,0xFFFF,(int16_t)door);
        check(frames==1 && !retraces && !sounds && !boxes && !keys); consumed(door);
        prepare(door,FALSE); lookup(-2); check(frames==1 && !retraces && !sounds); consumed(door);
        prepare(door,FALSE); lookup(-32768); check(frames==1 && !retraces && !sounds);
    }
    for(unsigned door=0;door<CacheDoorEntrance;++door) for(unsigned mismatch=1;mismatch<8;++mismatch) {
        prepare(door,TRUE);
        for(unsigned colour=0;colour<3;++colour) if(mismatch&(1u<<colour)) SelectedCacheCodeByColour[colour]=0xFF;
        lookup(-1); check(boxes==1 && keys==1 && pauses==1 && !frames && !sounds && MessageBoxOpen==TRUE);
        check(!!strstr(text,"Incorrect RED code.\r")==!!(mismatch&1));
        check(!!strstr(text,"Incorrect BLUE code.\r")==!!(mismatch&2));
        check(!!strstr(text,"Incorrect YELLOW code.")==!!(mismatch&4));
        for(unsigned code=0;code<CacheSecurityCodeCount;++code) check(!CacheSecurityCodeUsed[code]);
        check(!memcmp(expectedTiles,MapFileTiles,sizeof expectedTiles));
    }
    prepare(0,TRUE); StarLeague_Key_Codes(0,0,-1); check(!boxes && !frames && !sounds);
    /* First matching door must be rejected, not fall through to a duplicate. */
    int8_t savedX=CacheDoorPositionX[1],savedY=CacheDoorPositionY[1];
    CacheDoorPositionX[1]=CacheDoorPositionX[0]; CacheDoorPositionY[1]=CacheDoorPositionY[0];
    prepare(0,TRUE); for(unsigned colour=0;colour<3;++colour) SelectedCacheCodeByColour[colour]=(uint8_t)(CacheDoorRequiredCode[colour][1]-1);
    lookup(-1); check(boxes==1 && !frames && !CacheDoorOpened[1]);
    CacheDoorPositionX[1]=savedX; CacheDoorPositionY[1]=savedY;
    puts("Original cache door lookup, consumption, animation and replay checks passed"); return 0;
}
