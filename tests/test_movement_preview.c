#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned markers,glyphs,recenters,rebuilds,presentations,plans;
static uint16_t actor,lastColumn,lastRow;
/* Uncalled settings entries share the native infantry-budget object. */
void Menu_Memory_Variables(uint16_t panel) { (void)panel; }
void Draw_Top_Graphic_Sidebar(void) {}
void Display_Text_From_Memory(uint8_t *text) { (void)text; }
uint16_t Display_Menu_Choices_And_Check(uint16_t panel) { (void)panel; return 0; }
uint16_t Prompt_Yes_No(uint16_t choice) { return choice; }
static void check(int condition) { if(!condition) { fputs("Movement preview mismatch\n",stderr); exit(1); } }
void Move_Map_View_To_Packed_Position(uint16_t x,uint16_t y) {
    check(x==0x1234 && y==0x5678); ++recenters;
}
void PosXY_OffsetGrid(uint16_t x,uint16_t y) { check(x==CrescentHawkMapPositionX && y==CrescentHawkMapPositionY); }
void Copy_Data_To_GraphicsMemory(void) { ++rebuilds; }
void Draw_Menu_MultiSelect(void) { ++rebuilds; }
void EGA_DrawBox_Wrapper(void) { ++presentations; }
uint8_t DrawCall_Combat_Menu(uint16_t x,uint16_t y,uint16_t width,uint16_t colour) {
    unsigned mech=actor<4 || (actor>=12 && actor<16);
    check(x==(mech?25:26) && y==(mech?10+markers:12) && width==(mech?3:1) && colour==EGA_BrightWhite);
    ++markers; return 3;
}
void Combat_Calculate_Movement(uint16_t id) {
    check(id==actor && CharacterMovementPointsRemaining==(actor<4?5:6)); ++plans;
}
void Draw_EGA_Text_To_Screen(uint8_t *text,uint16_t x,uint16_t y,uint16_t fg,uint16_t bg) {
    check(text[1]==0 && fg==EGA_BrightWhite && bg==CombatPreviewBackgroundBlack);
    if(actor==0) {
        static const uint8_t glyph[]={2,7,5};
        static const uint16_t column[]={27,26,26},row[]={11,11,12};
        check(text[0]==glyph[glyphs] && x==column[glyphs] && y==row[glyphs]);
    } else { check(text[0]==3 && x==27+glyphs && y==12); }
    lastColumn=x; lastRow=y; ++glyphs;
}
static void reset(uint16_t id) {
    actor=id; markers=glyphs=recenters=rebuilds=presentations=plans=0;
    if(id<LanceSize) {
        Mechs[id].walkMove=5; Mechs[id].currentActuators[0]=15; Mechs[id].currentActuators[1]=15;
        MechHeatLevel[id]=0;
    }
    CombatantPackedX[id]=0x1234; CombatantPackedY[id]=0x5678;
    memset(CombatMovementPlanBytes,CombatMovementPlanEnd,sizeof CombatMovementPlanBytes);
    MovementPreviewEndpointColumn=99; MovementPreviewEndpointRow=88;
}
int main(void) {
    reset(0); CombatMovementOrders[0]=0xFF;
    const uint8_t steps[]={1,0xFF,0xFF,0,0,1,CombatMovementPlanEnd};
    memcpy(CombatMovementPlanBytes,steps,sizeof steps);
    Combat_Render_Movement_Preview(0,0xFFFF);
    check(markers==3 && glyphs==3 && plans==1 && recenters==1 && rebuilds==2 && presentations==1);
    check(MovementPreviewEndpointColumn==26 && MovementPreviewEndpointRow==12);
    reset(4); Characters[0].dexterity=8; Characters[0].armourType=ArmourType_None;
    for(unsigned step=0;step<CombatMovementPlanBytesPerUnit;step+=2) {
        CombatMovementPlanBytes[4*CombatMovementPlanBytesPerUnit+step]=1;
        CombatMovementPlanBytes[4*CombatMovementPlanBytesPerUnit+step+1]=0;
    }
    CombatMovementPlanBytes[5*CombatMovementPlanBytesPerUnit]=1; /*Next row probe, not a thirteenth step*/
    Combat_Render_Movement_Preview(4,FALSE);
    check(markers==1 && glyphs==12 && plans==1 && recenters==0 && CharacterMovementPointsRemaining==6);
    check(MovementPreviewEndpointColumn==lastColumn && MovementPreviewEndpointRow==lastRow);
    reset(3); CombatMovementOrders[3*CombatMovementOrderBytes]=0;
    Combat_Render_Movement_Preview(3,FALSE);
    check(markers==3 && glyphs==0 && MovementPreviewEndpointColumn==26 && MovementPreviewEndpointRow==12);
    for(uint16_t id=12;id<24;++id) {
        reset(id); Combat_Render_Movement_Preview(id,FALSE);
        check(markers==(id<16?3u:1u) && !glyphs && !plans && presentations==1);
        check(MovementPreviewEndpointColumn==99 && MovementPreviewEndpointRow==88);
    }
    puts("Original movement preview passed"); return 0;
}
