#include "game.h"

/* Original0800:3D40..3FAD. Sol: Scroll a two-region-step overview after the
 * Citadel attack; earlier story and Cache views consume one key only. Keep
 * the party marker fixed, restore original coordinates/cache and exploration
 * drawing afterwards. Cache colour-table swaps bracket the entire display. */
void Show_Overhead_Map(void)
{
    OverheadMapActive=TRUE;
    uint16_t partyX=CrescentHawkMapPositionX,partyY=CrescentHawkMapPositionY;
    uint16_t viewX=partyX,viewY=partyY;
    if(InsideStarLeagueCache!=FALSE) OverHead_Map_Function();
    uint16_t exitMap=FALSE;
    do {
        if(InsideStarLeagueCache!=FALSE || KuritaDestroyedCitadel==FALSE) {
            exitMap=TRUE;
            if(InsideStarLeagueCache!=FALSE) MapFogOfWar[CacheOverviewFogCell]&=CacheOverviewFogClearMask;
        }
        uint16_t command=Keyboard_Convert_To_MoveCommands(Overhead_Map_Draw(partyX,partyY));
        if(command==' ') exitMap=TRUE;
        else {
            CrescentHawkMapPositionX=viewX; CrescentHawkMapPositionY=viewY;
            if((command==Command_MoveNorth || command==Command_MoveNorthEast || command==Command_MoveNorthWest) && CrescentHawkMapPositionY>=OverheadNorthMinimumY)
                CrescentHawkMapPositionY=(uint16_t)(CrescentHawkMapPositionY-OverheadVerticalStep);
            if((command==Command_MoveSouth || command==Command_MoveSouthEast || command==Command_MoveSouthWest) && CrescentHawkMapPositionY<OverheadSouthMaximumY)
                CrescentHawkMapPositionY=(uint16_t)(CrescentHawkMapPositionY+OverheadVerticalStep);
            if((command==Command_MoveWest || command==Command_MoveNorthWest || command==Command_MoveSouthWest) && CrescentHawkMapPositionX>=OverheadWestMinimumX)
                CrescentHawkMapPositionX=(uint16_t)(CrescentHawkMapPositionX-OverheadHorizontalStep);
            if((command==Command_MoveEast || command==Command_MoveNorthEast || command==Command_MoveSouthEast) && CrescentHawkMapPositionX<OverheadEastMaximumX)
                CrescentHawkMapPositionX=(uint16_t)(CrescentHawkMapPositionX+OverheadHorizontalStep);
            viewX=CrescentHawkMapPositionX; viewY=CrescentHawkMapPositionY;
        }
    } while(exitMap==FALSE);
    CrescentHawkMapPositionX=partyX; CrescentHawkMapPositionY=partyY;
    if(InsideStarLeagueCache==FALSE) {
        uint16_t centre=(uint8_t)((partyX|partyY)>>8);
        Map_Construct_Nine_Regions(centre);
        int16_t region=(int16_t)(centre-MapRegionPreviousRowAndColumn);
        for(uint16_t row=0;row<MapNeighbourhoodWidth;++row) {
            for(uint16_t column=0;column<MapNeighbourhoodWidth;++column) {
                int16_t neighbour=(int16_t)(uint16_t)(region+column);
                if(neighbour>=0 && neighbour<WorldRegionCount && MapFileByWorldRegion[neighbour]!=0)
                    DOS_Load_Map_Files(row*MapNeighbourhoodWidth+column,(uint16_t)(int16_t)(int8_t)MapFileByWorldRegion[neighbour]);
            }
            region=(int16_t)(uint16_t)(region+MapRegionRowStride);
        }
        for(uint16_t pending=0;pending<MapNeighbourhoodWidth;++pending) PendingMapGridSlot[pending]=UINT8_MAX;
        Map_NineGrid_Parent();
    }
    PosXY_OffsetGrid(partyX,partyY); Copy_Data_To_GraphicsMemory();
    Draw_Infantry_And_Mechs(); EGA_DrawBox_Wrapper();
    Draw_Health_and_C_Bills_Sidebar(TRUE); Draw_Top_Graphic_Sidebar();
    OverheadMapActive=FALSE;
    if(InsideStarLeagueCache!=FALSE) OverHead_Map_Function();
}
