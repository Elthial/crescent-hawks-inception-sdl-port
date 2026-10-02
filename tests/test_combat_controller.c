#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned rounds,menus,armourSalvage,mechSalvage,loot,healing,restores,randomCalls;
static uint16_t menuResult,accept,reachable,generated,die,expectedRules,roundLimit=1;
static uint8_t randomValue;
static char displayed[16000];
static void verify(int ok,unsigned line) {
    if (!ok) { fprintf(stderr,"Combat controller mismatch line%u rounds%u\n",line,rounds); exit(1); }
}
#define check(x) verify(!!(x),__LINE__)
/* This executes the whole original parent. Callee boundaries are controlled
 * to validate orchestration, not claimed as a full combat simulation. */
#define VOID_BOUNDARY(name) void name(void) {}
VOID_BOUNDARY(Draw_Top_Graphic_Sidebar)
VOID_BOUNDARY(Draw_Message_Box)
VOID_BOUNDARY(Drain_Pending_Keyboard_Input)
VOID_BOUNDARY(Prompt_And_Wait_For_Key)
VOID_BOUNDARY(Wait_For_50Hz_Then_Check_Input)
VOID_BOUNDARY(Copy_Data_To_GraphicsMemory)
VOID_BOUNDARY(Draw_Menu_MultiSelect)
VOID_BOUNDARY(EGA_DrawBox_Wrapper)
VOID_BOUNDARY(Update_Cached_Map_Origin)
VOID_BOUNDARY(Map_NineGrid_Parent)
VOID_BOUNDARY(Combat_Load_9Grid_Map)
void Menu_Memory_Variables(uint16_t layout) { check(layout<MenuPanelLayoutCount); }
void Draw_Menu_Border(uint16_t style) { check(style<BorderStyleCount); }
void Draw_Health_and_C_Bills_Sidebar(uint16_t refresh) { check(refresh==1); }
void Display_Text_From_Memory(uint8_t *text) {
    size_t used=strlen(displayed),length=strlen((char *)text);
    check(used+length<sizeof displayed); memcpy(displayed+used,text,length+1);
}
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text) { Display_Text_From_Memory(text); }
void Display_Text_Dynamic_Value(uint16_t value) { (void)value; }
void Display_Plural_Suffix(void) { Display_Text_From_Memory((uint8_t *)"s"); }
void Display_Sentence_Period(void) { Display_Text_From_Memory((uint8_t *)"."); }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { return 0; }
uint16_t Pending_Input(void) { return 0; }
uint16_t Prompt_Yes_No(uint16_t defaultYes) { check(defaultYes==1); return accept; }
uint16_t Return_Bool_Allow_Computer_Control_Dialog(void) { return 0; }
uint16_t SettingsMenu_CombatMessages(void) { return 2; }
uint16_t SettingsMenu_SeeCombatGraphics(void) { return 1; }
uint8_t Rand_0x00_to_0xFF(void) { ++randomCalls; return randomValue; }
void Offset_Packed_Position(int16_t x,int16_t y) {
    CrescentHawkMapPositionX=(uint16_t)(CrescentHawkMapPositionX+x);
    CrescentHawkMapPositionY=(uint16_t)(CrescentHawkMapPositionY+y);
}
void Move_Map_View_To_Packed_Position(uint16_t x,uint16_t y) { Combat_Move_Position(x,y); }
void PosXY_OffsetGrid(uint16_t x,uint16_t y) { (void)x; (void)y; }
void Combat_Character_Pos_Grid(uint16_t x,uint16_t y) { ++restores; Combat_Move_Position(x,y); }
void Generate_Random_Encounter_Enemies(void) {
    if (generated) { CombatantActive[12]=1; CombatantPackedX[12]=0x052A; CombatantPackedY[12]=0x603A; Mechs[4].name[0]='L'; }
}
uint16_t Combat_Assess_FirstEnemy_Reachability(void) { return reachable; }
uint16_t Combat_UI_Menu_Logic(void) { ++menus; check(menus<5); return menuResult; }
void Combat_Plan_Computer_Side(uint16_t first) { check(first==0 || first==12); }
void Combat_Computer_Control(uint16_t actor,uint16_t preview) { check(actor<24 && preview==0); }
void Combat_Mech_Movement(uint16_t mech,uint16_t mode) { (void)mech; (void)mode; }
void Combat_Infantry_Movement(uint16_t actor) { (void)actor; }
void Combat_Calculate_Movement(uint16_t actor) { (void)actor; }
uint16_t Combat_Packed_Distance_From_Map_Position(uint16_t x,uint16_t y) { (void)x; (void)y; return 0; }
void Combat_Mechanics(uint16_t prearranged) {
    check(prearranged==expectedRules); ++rounds; check(rounds<=roundLimit);
    if (rounds==roundLimit) {
        for (unsigned actor=12;actor<24;++actor) CombatantActive[actor]=0;
        CombatantCasualtyFlags[12]=1;
        if (die) MainCharactersAlive=0;
    }
}
void Register_Persistent_Map_Effect(uint16_t sprite,uint16_t x,uint16_t y) { (void)sprite; (void)x; (void)y; }
void Display_Animation_Scene(uint16_t animation,uint16_t mode) { (void)animation; (void)mode; }
void DOS_Load_Map_Files(uint16_t slot,uint16_t map) { check(slot==4 && map==11); }
void Salvage_Armour_Dialog(void) { ++armourSalvage; }
void Salvage_Mechs_Dialog(void) { ++mechSalvage; }
void Loot_Enemy_Soldiers_Dialog(void) { ++loot; }
void Heal_Characters(uint16_t tier) { check(tier==MedicalService_UsePartyMedicAndEquipment); ++healing; }
static void prepare(void) {
    rounds=menus=armourSalvage=mechSalvage=loot=healing=restores=randomCalls=0;
    menuResult=die=expectedRules=0; accept=reachable=generated=1; randomValue=1; roundLimit=1;
    displayed[0]=0;
    memset(Characters,0,sizeof Characters); for (unsigned i=0;i<16;++i) Characters[i].name=255;
    memset(Mechs,0,sizeof WorldMapState.fields.mechs); for (unsigned i=0;i<8;++i) Mechs[i].name[0]=255;
    memset(PersistentState.bytes,0,sizeof PersistentState.bytes);
    for (unsigned actor=0;actor<24;++actor) {
        CombatantActive[actor]=0; CombatantPackedX[actor]=0x0100+(uint16_t)actor;
        CombatantPackedY[actor]=0x7000+(uint16_t)actor;
        CombatantCasualtyFlags[actor]=0; CombatantAnimationSelector[actor]=0x55;
    }
    for (unsigned i=0;i<8;++i) {
        RoamingMapNpcs[i].currentPositionX=0x0200+(uint16_t)i;
        RoamingMapNpcs[i].currentPositionY=0x6000+(uint16_t)i;
    }
    Characters[0].name=0; Characters[0].body=8; Characters[0].health=80;
    Characters[0].mechAssignment=8; Characters[0].skillTech=1;
    Mechs[0].name[0]='L'; CrescentHawkMapPositionX=0x0520; CrescentHawkMapPositionY=0x6030;
    memset(LocalTerrainFlags,0,MapNeighbourhoodCount);
    memset(CombatMovementOrders,0,sizeof CombatMovementOrders);
    MainCharactersAlive=1; KuritaAttackFlag=TraitorWarning=DisableComputerControl=0;
    EnemyPersonnelFlightPossible=CombatComputerControl=0;
}
static void checkExit(unsigned roaming) {
    check(EnemyTargetId==12);
    for (unsigned i=0;i<8;++i) {
        check(CombatantPackedX[16+i]==(roaming?0x0110+i:0x0200+i));
        check(CombatantPackedY[16+i]==(roaming?0x7010+i:0x6000+i));
        check(CombatantAnimationSelector[16+i]==255 && CombatantAnimationSelector[i]==255);
        check(CombatantSpriteFrame[16+i]==16);
    }
    for (unsigned i=8;i<12;++i) check(CombatantAnimationSelector[i]==0x55);
}
int main(void) {
    prepare(); generated=0; Combat_Run_Encounter(0);
    check(!rounds && restores==1 && !loot); checkExit(1);
    prepare(); reachable=0; Combat_Run_Encounter(0);
    check(!rounds && restores==1 && !loot); checkExit(1);
    prepare(); accept=0; Combat_Run_Encounter(0);
    check(!rounds && randomCalls==1 && !loot && strstr(displayed,"eluded"));
    check(strstr(displayed,"Attacking force:\r") && strstr(displayed," and\r")); checkExit(1);
    prepare(); accept=0; randomValue=0; Combat_Run_Encounter(0);
    check(rounds==1 && randomCalls==1 && loot==1 && armourSalvage==1 && strstr(displayed,"did not evade")); checkExit(1);
    for (unsigned limit=1;limit<=3;++limit) {
        prepare(); roundLimit=(uint16_t)limit; Combat_Run_Encounter(0);
        check(rounds==limit && menus==limit && loot==1 && armourSalvage==1 && !healing);
        check(CombatMessageVerbosity==2 && CombatDisplayGraphics==1); checkExit(1);
    }
    prepare(); Characters[0].health=79; Combat_Run_Encounter(0); check(healing==1 && PartyHealthRecoveryTimer==0);
    /* BUG-017: a dead member with nonzero Tech still qualifies after CBW. */
    prepare(); Characters[0].skillTech=0; Characters[1].skillTech=1;
    Combat_Run_Encounter(0); check(armourSalvage==1);
    prepare(); Characters[0].skillPiloting=1; Combat_Run_Encounter(0);
    check(mechSalvage==1 && loot==1);
    for (unsigned slot=0;slot<9;++slot) {
        prepare(); Characters[0].skillPiloting=1; LocalTerrainFlags[slot]=0x80;
        Combat_Run_Encounter(0); check(!mechSalvage && loot==1);
    }
    /* On-foot betrayal clears the traitor, but leaves the remaining party. */
    prepare(); Characters[1]=Characters[0]; Characters[1].name=2;
    TraitorCharacterId=1; TraitorInParty=1; TraitorWarning=1;
    Combat_Run_Encounter(0);
    check(Characters[1].name==255 && !TraitorInParty && TraitorBattleProbability==127);
    check(!CombatantActive[5] && CombatantPackedX[5]==65535 && CombatantPackedY[5]==65535);
    check(strstr(displayed,"interrogation") && Characters[0].name==0);
    prepare(); expectedRules=1; CombatantActive[12]=1; CombatantPackedX[12]=0x052A; CombatantPackedY[12]=0x603A;
    Combat_Run_Encounter(1);
    check(rounds==1 && !loot && strstr(displayed,"return to base")); checkExit(0);
    prepare(); expectedRules=1; CombatantActive[12]=1; CombatantPackedX[12]=0x052A; CombatantPackedY[12]=0x603A;
    Combat_Run_Encounter(2);
    check(rounds==1 && ArenaWon==1 && !loot && strstr(displayed,"Congratulations")); checkExit(0);
    prepare(); die=1; Combat_Run_Encounter(0); check(rounds==1 && !loot && !healing); checkExit(1);
    prepare(); menuResult=1; Combat_Run_Encounter(0);
    check(!rounds && randomCalls==1 && !loot && strstr(displayed,"eluded")); checkExit(1);
    puts("Whole original encounter controller: consent, rounds, outcomes, salvage gating and common exits passed.");
    return 0;
}
