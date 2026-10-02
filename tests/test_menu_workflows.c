#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char events[32];
static unsigned eventCount,assignmentCalls,sidebarCalls;
static int refreshing;
static void check(int condition) { if(!condition) { fputs("Original menu workflow mismatch\n",stderr); exit(1); } }
static void event(char value) { check(eventCount+1<sizeof events); events[eventCount++]=value; events[eventCount]=0; }
void Assign_Pilot_and_rider_to_Mechs(uint16_t mode) { check(mode==AssignmentMode_SynchronizeExisting); ++assignmentCalls; event('A'); }
void Draw_Health_and_C_Bills_Sidebar(uint16_t refresh) { check(refresh==TRUE); event('H'); }
void Draw_Top_Graphic_Sidebar(void) {
    if(refreshing) check(CurrentMenuLayoutIndex==(sidebarCalls?RefreshPartyPanel:RefreshLowerPanel));
    ++sidebarCalls; event('S');
}
void Draw_Message_Box(void) { event('M'); }
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text) {
    check(strcmp((const char *)text,"It would be dangerous to dismount so close to a populated area.")==0); event('T');
}
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { event('K'); return ' '; }
void PosXY_OffsetGrid(uint16_t x,uint16_t y) { check(x==0xA472 && y==0x6668); event('P'); }
void Copy_Data_To_GraphicsMemory(void) { event('C'); }
void Draw_Menu_MultiSelect(void) { event('V'); }
void Draw_Infantry_And_Mechs(void) { event('I'); }
void EGA_DrawBox_Wrapper(void) { event('E'); }
void Draw_Menu_Border(uint16_t style) {
    check(CurrentMenuLayoutIndex==style);
    check(style==(sidebarCalls==1?RefreshLowerPanel:RefreshPartyPanel)); event('B');
}
static void reset(void) { eventCount=assignmentCalls=sidebarCalls=0; events[0]=0; }
int main(void)
{
    memset(LocalTerrainFlags,0x7F,MapNeighbourhoodCount);
    reset(); Menu_Assign_Pilots(); check(strcmp(events,"AHS")==0 && assignmentCalls==1);
    for(unsigned neighbour=0;neighbour<MapNeighbourhoodCount;++neighbour) {
        LocalTerrainFlags[neighbour]=MapTile_BlockDismounting;
        reset(); Menu_Assign_Pilots(); check(strcmp(events,"MTK")==0 && !assignmentCalls && !sidebarCalls);
        LocalTerrainFlags[neighbour]=0x7F;
    }
    memset(LocalTerrainFlags,0xFF,MapNeighbourhoodCount);
    reset(); Menu_Assign_Pilots(); check(strcmp(events,"MTK")==0);
    CrescentHawkMapPositionX=0xA472; CrescentHawkMapPositionY=0x6668;
    refreshing=TRUE;
    static const uint16_t modes[]={FALSE,TRUE,0x8000,0xFFFF};
    for(unsigned mode=0;mode<sizeof modes/sizeof modes[0];++mode) {
        reset(); Menu_Draw_MultiSelect(modes[mode]);
        check(strcmp(events,modes[mode]?"PCVESBSB":"PCIESBSB")==0);
        check(CurrentMenuLayoutIndex==RefreshPartyPanel && sidebarCalls==2);
        check(TextPanelTop==13 && TextPanelHeight==11);
    }
    puts("Original nine-cell dismount gating and planning refresh order verified.");
    return 0;
}
