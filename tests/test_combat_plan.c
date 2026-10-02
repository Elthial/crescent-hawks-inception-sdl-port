#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned glyphs,markers;
static unsigned uiTest,keyCount,keyIndex,questions,messageWaits,planningRedraws;
static uint16_t uiKeys[64],yesAnswer;
static char uiText[8192];
static void checkAtLine(int condition,unsigned line) {
    if(!condition) { fprintf(stderr,"Combat plan mismatch line%u\n",line); exit(1); }
}
#define check(condition) checkAtLine((condition),__LINE__)
/* Unused UI/audio entries share the original occupancy/budget objects. */
void Menu_Memory_Variables(uint16_t panel) { check(uiTest && panel==4); }
void Draw_Top_Graphic_Sidebar(void) { check(uiTest); }
void Display_Text_From_Memory(uint8_t *text) {
    size_t used=strlen(uiText),length=strlen((char *)text);
    check(uiTest && used+length<sizeof uiText);
    memcpy(uiText+used,text,length+1);
}
void Prompt_And_Wait_For_Key(void) { check(uiTest); ++messageWaits; }
uint16_t Display_Menu_Choices_And_Check(uint16_t panel) { (void)panel; check(0); return 0; }
uint16_t Prompt_Yes_No(uint16_t choice) { check(uiTest && choice==TRUE); ++questions; return yesAnswer; }
void Play_Sound_If_Enabled(uint16_t sound) { (void)sound; check(0); }
void Register_Persistent_Map_Effect(uint16_t id,uint16_t x,uint16_t y) { (void)id; (void)x; (void)y; check(0); }
void Wait_For_N_Vertical_Retraces(uint16_t count) { (void)count; check(0); }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { check(uiTest && keyIndex<keyCount); return uiKeys[keyIndex++]; }
void Map_Move_North(void) { check(0); }
void Map_Move_South(void) { check(0); }
void Map_Move_West(void) { check(0); }
void Map_Move_East(void) { check(0); }
void EGA_Upload_Animated_Tile(uint8_t *source,uint16_t destination) { (void)source; (void)destination; check(0); }
/* Preview adapter boundary. Real budget, plan, direction, step, occupancy. */
void PosXY_OffsetGrid(uint16_t x,uint16_t y) { check(uiTest && x==0x0220 && y==0x3020); }
void DOS_Load_Map_Files(uint16_t slot,uint16_t map) { (void)slot; (void)map; check(0); }
void Map_NineGrid_Parent(void) { check(0); }
void Copy_Data_To_GraphicsMemory(void) { check(uiTest); }
void Draw_Menu_MultiSelect(void) { check(uiTest && CombatantActionState[4]!=0); ++planningRedraws; }
void EGA_DrawBox_Wrapper(void) {}
uint8_t DrawCall_Combat_Menu(uint16_t x,uint16_t y,uint16_t width,uint16_t colour) {
    if(uiTest) check(width>=1 && width<=3 && (colour==EGA_BrightWhite || colour==CombatDestinationCursorYellow));
    else check(x==26 && y==12 && width==1 && colour==EGA_BrightWhite);
    ++markers; return 3;
}
void Draw_EGA_Text_To_Screen(uint8_t *text,uint16_t x,uint16_t y,uint16_t fg,uint16_t bg) {
    if(uiTest) check(!text[1] && fg==15 && bg==0);
    else check(text[0]==3 && !text[1] && x==27+glyphs && y==12 && fg==15 && bg==0);
    ++glyphs;
}
static void prepare(uint16_t id,uint16_t points,uint8_t mode,uint8_t targetX) {
    memset(CombatMap,0,sizeof CombatMap);
    memset(CombatantActive,0,sizeof CombatantActive);
    memset(CombatantPackedX,255,sizeof CombatantPackedX);
    memset(CombatantPackedY,255,sizeof CombatantPackedY);
    memset(CombatantMovementDirection,2,sizeof CombatantMovementDirection);
    memset(CombatantActionState,0,sizeof CombatantActionState);
    memset(CombatMovementOrders,255,sizeof CombatMovementOrders);
    memset(CombatMovementPlanBytes,77,sizeof CombatMovementPlanBytes);
    uint16_t start=(uint16_t)(id*CombatMovementOrderBytes);
    CombatMovementOrders[start]=mode; CombatMovementOrders[start+1]=0x32;
    CombatMovementOrders[start+2]=targetX; CombatMovementOrders[start+3]=0x20;
    CrescentHawkMapPositionX=0x0220; CrescentHawkMapPositionY=0x3020;
    CachedMapOriginIndex=0; BlockingTileCodeThreshold=0x55;
    CharacterMovementPointsRemaining=points; CombatDestinationBlocked=99;
    glyphs=markers=0;
}
static unsigned pairs(uint16_t id) {
    unsigned start=id*CombatMovementPlanBytesPerUnit,count=0;
    while(count<12 && CombatMovementPlanBytes[start+count*2]!=CombatMovementPlanEnd) ++count;
    return count;
}
static void east(uint16_t id,unsigned expected) {
    check(pairs(id)==expected);
    for(unsigned step=0;step<expected;++step) {
        check(CombatMovementPlanBytes[id*24+step*2]==1);
        check(CombatMovementPlanBytes[id*24+step*2+1]==0);
    }
    check(CombatantPackedX[id]==0xFFFF && CombatantPackedY[id]==0xFFFF);
    check(CrescentHawkMapPositionX==0x0220 && CrescentHawkMapPositionY==0x3020);
}
static void prepareUI(uint16_t id) {
    prepare(id,6,0xFF,0x20); uiTest=1; keyCount=keyIndex=questions=messageWaits=planningRedraws=0;
    yesAnswer=TRUE; uiText[0]=0;
    MovementPreviewEndpointColumn=26; MovementPreviewEndpointRow=12;
    Characters[0].dexterity=8; Characters[0].armourType=0;
    Mechs[0].walkMove=5; Mechs[0].jumpMove=3; Mechs[0].currentActuators[0]=Mechs[0].currentActuators[1]=15;
    MechHeatLevel[0]=0; CombatantPackedX[id]=0x0220; CombatantPackedY[id]=0x3020;
}
static void key(uint16_t command) { check(keyCount<64); uiKeys[keyCount++]=command; }
static void planUI(uint16_t id,uint16_t mode) {
    Combat_Select_Movement_Plan_For_Turn(id,mode); check(keyIndex==keyCount);
    check(CrescentHawkMapPositionX==0x0220 && CrescentHawkMapPositionY==0x3020);
}
static void order(uint16_t id,uint16_t byte,uint8_t mode,uint8_t x) {
    unsigned start=id*48+byte; CombatMovementOrders[start]=mode; CombatMovementOrders[start+1]=0x32;
    CombatMovementOrders[start+2]=x; CombatMovementOrders[start+3]=0x20;
}
int main(void) {
    prepare(4,6,0,0x24); CombatantMovementDirection[4]=6;
    Combat_Calculate_Movement(4); east(4,4);
    check(CharacterMovementPointsRemaining==2 && CombatantMovementDirection[4]==6);
    check(MovementActorPositionX==0x0224 && MovementActorPositionY==0x3020 && !CombatDestinationBlocked);
    check(CombatMovementPlanBytes[3*24]==77 && CombatMovementPlanBytes[5*24]==77);
    prepare(4,6,0,0x26); memset(CombatMap,0x11,sizeof CombatMap);
    Combat_Calculate_Movement(4); east(4,3); check(!CharacterMovementPointsRemaining);
    prepare(4,5,0,0x26); memset(CombatMap,0x29,sizeof CombatMap);
    Combat_Calculate_Movement(4); east(4,2); check(!CharacterMovementPointsRemaining); /*Last step costs3 with2 left*/
    prepare(4,3,2,0x26); memset(CombatMap,0x29,sizeof CombatMap);
    Combat_Calculate_Movement(4); east(4,3); check(!CharacterMovementPointsRemaining);
    prepare(4,4,0,0x20); Combat_Calculate_Movement(4);
    east(4,0); check(CharacterMovementPointsRemaining==4);
    prepare(4,6,0,0x24); CombatMovementOrders[4*48]=0xFF;
    Combat_Calculate_Movement(4); east(4,0); check(CharacterMovementPointsRemaining==6 && CombatDestinationBlocked==99);
    prepare(16,12,0,0x30); Combat_Calculate_Movement(16); east(16,12);
    check(CombatMovementPlanBytes[17*24]==77 && !CharacterMovementPointsRemaining);
    prepare(16,13,0,0x30); Combat_Calculate_Movement(16);
    check(CombatMovementPlanBytes[17*24]==1 && CombatMovementPlanBytes[17*24+1]==0);
    check(CombatMovementPlanBytes[17*24+2]==77); /*Native oversized budget spills into next row, no invented12-step guard*/
    prepare(4,3,0,0x24); memset(CombatMap,0xF6,sizeof CombatMap);
    Combat_Calculate_Movement(4); check(pairs(4)==3 && MovementActorPositionX==0x0220);
    for(unsigned byte=0;byte<6;++byte) check(CombatMovementPlanBytes[4*24+byte]==0); /*Failed searches still consume MP*/
    prepare(4,6,0,0x21); CombatantPackedX[5]=0x0221; CombatantPackedY[5]=0x3020;
    Combat_Calculate_Movement(4); check(pairs(4)==1 && CombatDestinationBlocked && CharacterMovementPointsRemaining==5);
    prepare(4,6,0,0x24);
    CombatMovementOrders[4*48+4]=0; CombatMovementOrders[4*48+5]=0x32;
    CombatMovementOrders[4*48+6]=0x26; CombatMovementOrders[4*48+7]=0x20;
    Combat_Calculate_Movement(4); east(4,6); check(!CharacterMovementPointsRemaining);
    for(uint16_t oddX=0;oddX<2;++oddX) for(uint16_t oddY=0;oddY<2;++oddY)
        for(uint8_t quadrant=1;quadrant<=8;quadrant*=2) {
            prepare(4,4,0,(uint8_t)(0x21+oddX));
            CrescentHawkMapPositionX+=oddX; CrescentHawkMapPositionY+=oddY;
            CombatMovementOrders[4*48+3]=(uint8_t)(0x21+oddY);
            memset(CombatMap,0x20|quadrant,sizeof CombatMap);
            Combat_Calculate_Movement(4);
            unsigned mask=6u^(oddX?10u:0u)^(oddY?5u:0u);
            check(pairs(4)==1 && CharacterMovementPointsRemaining==((mask&quadrant)?1:3));
            check(CombatMovementPlanBytes[4*24]==1 && CombatMovementPlanBytes[4*24+1]==1);
        }
    prepare(4,6,0,0x24); Characters[0].dexterity=8; Characters[0].armourType=0;
    Combat_Render_Movement_Preview(4,FALSE);
    east(4,4); check(markers==1 && glyphs==4 && MovementPreviewEndpointColumn==30 && MovementPreviewEndpointRow==12);
    prepareUI(4); key('d'); key('d'); key('d'); key(' '); planUI(4,0);
    check(CombatMovementOrders[4*48]==0 && CombatMovementOrders[4*48+2]==0x23 && CombatMovementOrders[4*48+4]==0xFF);
    check(pairs(4)==3 && CharacterMovementPointsRemaining==3 && strstr(uiText,"You have 6 movement points left.") && MovementPreviewEndpointColumn==29);
    prepareUI(4); key('d'); key('a'); key(13); planUI(4,0);
    check(CombatMovementOrders[4*48+2]==0x20 && pairs(4)==0 && CharacterMovementPointsRemaining==6);
    prepareUI(4); order(4,0,0,0x21); MovementPreviewEndpointColumn=27;
    key('d'); key('a'); key(' '); planUI(4,0);
    check(CombatMovementOrders[4*48+2]==0x21 && CombatMovementOrders[4*48+4]==0xFF && pairs(4)==1);
    prepareUI(4); for(unsigned i=0;i<20;++i) key('d'); key(' '); planUI(4,0);
    check(MovementPreviewEndpointColumn==39 && CombatMovementOrders[4*48+2]==0x2D && pairs(4)==6);
    prepareUI(0); order(0,0,1,0x21); yesAnswer=FALSE; planUI(0,0);
    check(questions==1 && CombatMovementOrders[0]==1 && strstr(uiText,"running.\rDo you wish to walk instead?")!=NULL);
    prepareUI(0); order(0,0,1,0x21); yesAnswer=2; key(13); key('d'); key(13); planUI(0,0);
    check(questions==1 && CombatMovementOrders[0]==0 && CombatMovementOrders[2]==0x21);
    check(strstr(uiText,"deleted.\rPress a key")!=NULL);
    prepareUI(4); CombatantActionState[4]=2; order(4,0,0,0x24); key(' '); planUI(4,0);
    check(planningRedraws==1 && !CombatantActionState[4] && CombatMovementOrders[4*48]==0xFF);
    prepareUI(0); Mechs[0].jumpMove=0; order(0,0,0,0x21); planUI(0,2);
    check(!questions && messageWaits==1 && strstr(uiText,"no jump jets.")!=NULL && CombatMovementOrders[0]==0);
    prepareUI(0); MechHeatLevel[0]=30; planUI(0,0);
    check(messageWaits==1 && CurrentMechHeatMovementPenalty==6 && strstr(uiText,"overheated and shut down.")!=NULL);
    prepareUI(0); Mechs[0].currentActuators[0]=Mechs[0].currentActuators[1]=0; MechHeatLevel[0]=10;
    planUI(0,0); check(strstr(uiText,"inhibited by leg damage.\r")!=NULL && !CurrentMechLegDamage && !CurrentMechHeatMovementPenalty);
    prepareUI(0); Mechs[0].walkMove=1; MechHeatLevel[0]=10; planUI(0,0);
    check(strstr(uiText,"inhibited by this 'Mech's heat level.")!=NULL && !CurrentMechHeatMovementPenalty);
    prepareUI(0); order(0,0,0,0x25); planUI(0,0);
    check(strstr(uiText,"can't walk any further this turn.\rYou might move farther")!=NULL && messageWaits==1);
    prepareUI(0); order(0,0,1,0x28); planUI(0,1);
    check(strstr(uiText,"can't run any further")!=NULL && strstr(uiText,"might move farther")==NULL);
    prepareUI(4); order(4,0,0,0x26); CurrentMechLegDamage=CurrentMechHeatMovementPenalty=99; planUI(4,0);
    check(strstr(uiText,"can't move any further")!=NULL && !CurrentMechLegDamage && !CurrentMechHeatMovementPenalty);
    prepareUI(4); for(uint16_t byte=0;byte<48;byte+=4) order(4,byte,0,0x20);
    key('d'); key(' '); planUI(4,0);
    check(CombatMovementOrders[5*48]==0 && CombatMovementOrders[5*48+1]==0x32 && CombatMovementOrders[5*48+2]==0x21);
    check(CombatMovementOrders[5*48+4]==0xFF); /*Native BUG-018 full row overflow preserved*/
    /* Whole AI into actual terrain clearance, budgets, path builder, direction,
     * step and occupancy. Cache rebuilding remains the existing boundary. */
    prepareUI(0); memset(Mechs[0].criticalSlots,0,sizeof Mechs[0].criticalSlots);
    Mechs[0].criticalSlots[0]=Mech_Small_Laser; Mechs[0].currentAmmo[0]=255;
    CombatantActive[12]=1; CombatantPackedX[12]=0x0228; CombatantPackedY[12]=0x3020;
    ArenaRentalMechMode=TraitorInParty=0;
    Combat_Computer_Control(0,0);
    check(CombatWeaponTarget[0]==12 && CombatWeaponTarget[11]==12 && CombatantActionState[0]==1);
    check(pairs(0)==0 && CombatMovementOrders[0]==255);
    memset(Mechs[0].criticalSlots,0,sizeof Mechs[0].criticalSlots);
    Combat_Computer_Control(0,0);
    check(CombatMovementOrders[0]==MovementMode_Run && CombatMovementOrders[2]==0x28);
    check(pairs(0)>0 && CrescentHawkMapPositionX==0x0220 && CrescentHawkMapPositionY==0x3020);
    /* Encounter probe with real step/direction/terrain handling. Camera/cache
     * rebuild is the same boundary used above, not a scripted movement probe. */
    prepareUI(4); Characters[0].mechAssignment=Character_OnFoot;
    CombatantActive[12]=1; CombatantPackedX[12]=0x0228; CombatantPackedY[12]=0x3020;
    CombatDestinationBlocked=0;
    check(Combat_Assess_FirstEnemy_Reachability()==TRUE);
    check(MovementActorPositionX==0x0228 && MovementActorPositionY==0x3020 && CharacterMovementPointsRemaining==0);
    check(CrescentHawkMapPositionX==0x0220 && CrescentHawkMapPositionY==0x3020);
    prepareUI(4); Characters[0].mechAssignment=Character_OnFoot;
    CombatantActive[12]=1; CombatantPackedX[12]=0x0228; CombatantPackedY[12]=0x3020;
    memset(CombatMap,BlockingTileCodeThreshold,sizeof CombatMap); CombatDestinationBlocked=0;
    check(Combat_Assess_FirstEnemy_Reachability()==FALSE);
    check(MovementActorPositionX==0x0220 && CharacterMovementPointsRemaining==0 && !CombatDestinationBlocked);
    /* Impassable terrain rejects candidates without setting the occupancy
     * blocked latch; probe80 skips occupancy altogether. */
    puts("Original combat plan/preview integration passed"); return 0;
}
