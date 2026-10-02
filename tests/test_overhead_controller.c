#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint16_t partyX,partyY,expectedX,expectedY,command;
static unsigned passes,stage,swaps,mapCalls,loads;
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"Overhead mismatch line%u\n",line); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
void OverHead_Map_Function(void) { check(OverheadMapActive==(swaps++==0?1:0)); }
uint16_t Overhead_Map_Draw(uint16_t x,uint16_t y)
{
    check(OverheadMapActive==1 && x==partyX && y==partyY && passes<2);
    check(CrescentHawkMapPositionX==(passes?expectedX:partyX));
    check(CrescentHawkMapPositionY==(passes?expectedY:partyY));
    if(InsideStarLeagueCache) check(MapFogOfWar[0x66C]==0x7F);
    CrescentHawkMapPositionX=x; CrescentHawkMapPositionY=y;
    return passes++==0?command:' ';
}
void Map_Construct_Nine_Regions(uint16_t centre) { check(centre==(uint8_t)((partyX|partyY)>>8)); ++mapCalls; }
void DOS_Load_Map_Files(uint16_t cell,uint16_t map) { check(cell<9 && map==0xFF80); ++loads; }
void Map_NineGrid_Parent(void) { for(unsigned i=0;i<3;++i) check(PendingMapGridSlot[i]==255); }
void PosXY_OffsetGrid(uint16_t x,uint16_t y) { check(stage++==0 && x==partyX && y==partyY); }
void Copy_Data_To_GraphicsMemory(void) { check(stage++==1); }
void Draw_Infantry_And_Mechs(void) { check(stage++==2); }
void EGA_DrawBox_Wrapper(void) { check(stage++==3); }
void Draw_Health_and_C_Bills_Sidebar(uint16_t redraw) { check(stage++==4 && redraw==1); }
void Draw_Top_Graphic_Sidebar(void) { check(stage++==5); }
static void run(unsigned flags)
{
    CrescentHawkMapPositionX=partyX; CrescentHawkMapPositionY=partyY;
    InsideStarLeagueCache=(uint8_t)(flags&1); KuritaDestroyedCitadel=(uint8_t)(flags&2);
    MapFogOfWar[0x66C]=255; memset(PendingMapGridSlot,0xA5,3);
    passes=stage=swaps=mapCalls=loads=0;
    Show_Overhead_Map();
    check(stage==6 && passes==((flags&1)||!(flags&2)||command==' '?1u:2u));
    check(swaps==(flags&1?2u:0u) && mapCalls==(flags&1?0u:1u));
    check(CrescentHawkMapPositionX==partyX && CrescentHawkMapPositionY==partyY && OverheadMapActive==0);
    check(MapFogOfWar[0x66C]==(flags&1?127:255));
}
int main(void)
{
    const uint16_t directions[8]={Command_MoveNorth,Command_MoveNorthEast,Command_MoveEast,Command_MoveSouthEast,
        Command_MoveSouth,Command_MoveSouthWest,Command_MoveWest,Command_MoveNorthWest};
    memset(MapFileByWorldRegion,128,WorldRegionCount);
    for(unsigned direction=0;direction<8;++direction) for(unsigned value=0;value<65536;++value) {
        partyX=partyY=(uint16_t)value; command=directions[direction]; expectedX=partyX; expectedY=partyY;
        if((direction==0 || direction==1 || direction==7) && value>=0x2000) expectedY=(uint16_t)(value-0x2000);
        if(direction>=3 && direction<=5 && value<0xE000) expectedY=(uint16_t)(value+0x2000);
        if(direction>=5 && value>=0x300) expectedX=(uint16_t)(value-0x200);
        if(direction>=1 && direction<=3 && value<0xD00) expectedX=(uint16_t)(value+0x200);
        run(2);
    }
    for(unsigned flags=0;flags<4;++flags) for(unsigned key=0;key<256;++key) {
        partyX=0xC35; partyY=0xC047; command=(uint16_t)key;
        /* Single screen modes cannot reach a second render; test unknown keys
         * and Space in scrolling mode without movement-oracle duplication. */
        if(flags==2 && key!=' ') continue;
        expectedX=partyX; expectedY=partyY; run(flags);
    }
    return 0;
}
