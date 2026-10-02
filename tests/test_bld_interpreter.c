#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned calls,balance,keys,sidebars,waits,scenes,texts;
static uint16_t argument,secondArgument,randomValue,answer,selection;
static unsigned entryMode,loads,disks,cacheEntries,healthRefreshes,entryActions;
static uint8_t partyCount;
static uint16_t entryScene;
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"BLD mismatch line%u\n",line); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
/* Interpreter and native WORD readers are real. Rendering, input and action
 * methods are explicit boundaries, not fake implementations in production. */
void Play_Sound_If_Enabled(uint16_t id) { ++calls; argument=id; }
void Recruit_Crescent_Hawk_Agent(uint16_t id) { ++calls; argument=id; }
void Citadel_Building_Dialogs(uint16_t id) {
    ++calls; argument=id;
    if(entryMode) { check(id==CitadelDialog_CountOtherPartyMembers); ++entryActions; SelectedPartyMemberSlot=partyCount; }
}
void Draw_Menu_Border(uint16_t id) { ++calls; argument=id; }
void Menu_Memory_Variables(uint16_t id) { ++calls; argument=id; }
uint16_t Display_Menu_Choices_And_Check(uint16_t id) { ++calls; argument=id; return selection; }
void Display_Animation_Scene(uint16_t id,uint16_t mode) { ++scenes; argument=id; entryScene=id; secondArgument=mode; }
void Display_Text_CBill_Balance(void) { ++balance; }
uint8_t Rand_0x00_to_0xFF(void) { return (uint8_t)randomValue; }
uint16_t Prompt_Yes_No(uint16_t initial) { check(initial==TRUE); return answer; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return 0; }
void Draw_Top_Graphic_Sidebar(void) { ++sidebars; }
void Wait_For_50Hz_Then_Check_Input(void) { ++waits; }
void Display_Text_From_Memory(uint8_t *text) {
    check(!strcmp((char *)text,entryMode?"When you get outside, you decide to use your new ":"original text")); ++texts;
}
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text) {
    check(entryMode && !strcmp((char *)text,"holoviewer to watch your father's holodisk."));
}
void Draw_Health_and_C_Bills_Sidebar(uint16_t refresh) { check(entryMode && refresh==TRUE); ++healthRefreshes; }
void Draw_STARLEAG_ICN_AND_Game_Logic(void) { check(entryMode); ++cacheEntries; }
/* Loader shares the readers' original source object but is not called here. */
void Select_Game_Disk_And_Drive(uint16_t disk) { check(entryMode && (disk==1 || disk==2)); ++disks; }
uint16_t Load_File_To_Memory(const uint8_t *name,uint8_t *memory) {
    check(entryMode && name==BldFileNameById[BldFileIndex] && memory==BTStatsOrBldMemory);
    memset(memory,(int)((((unsigned)Bld_Exit^(unsigned)BldDecodeXor)-BldDecodeAddition)&UINT8_MAX),BldFixedDecodeSpan);
    ++loads; return 1;
}
/* Branch fixture writes distinct state bytes on fallthrough and target. */
static void branchTest(uint8_t opcode,const uint8_t *operands,unsigned count,int taken) {
    uint8_t code[40]={0}; code[0]=opcode; if(count) memcpy(code+1,operands,count);
    code[1+count]=20; code[2+count]=0;
    code[3+count]=Bld_SetStateByte; code[4+count]=99; code[5+count]=1; code[6+count]=Bld_Exit;
    code[20]=Bld_SetStateByte; code[21]=99; code[22]=2; code[23]=Bld_Exit;
    PersistentState.bytes[99]=0; Execute_Bld_Bytecode(code); check(PersistentState.bytes[99]==(taken?2:1));
}
int main(void) {
    const uint8_t callOpcodes[]={Bld_PlaySound,Bld_Recruit,Bld_CallAction,Bld_DrawBorder,Bld_ApplyLayout};
    for(unsigned op=0;op<sizeof callOpcodes;++op) for(unsigned byte=0;byte<256;++byte) {
        uint8_t code[]={callOpcodes[op],(uint8_t)byte,Bld_Exit}; calls=0;
        Execute_Bld_Bytecode(code); check(calls==1 && argument==(uint16_t)(int16_t)(int8_t)byte);
    }
    for(unsigned byte=0;byte<256;++byte) {
        uint8_t code[]={Bld_SetTextLayout,(uint8_t)byte,(uint8_t)(255-byte),Bld_ConditionalScene,(uint8_t)byte,128,Bld_Exit};
        scenes=0; DisableInput=0; Execute_Bld_Bytecode(code);
        check(TextColumn==(uint16_t)(int16_t)(int8_t)byte && TextRow==(uint16_t)(int16_t)(int8_t)(255-byte));
        check(scenes==1 && argument==TextColumn && secondArgument==0xFF80);
        scenes=0; DisableInput=1; Execute_Bld_Bytecode(code); check(scenes==0);
    }
    for(unsigned amount=0;amount<65536;++amount) {
        uint32_t nativeAmount=(uint32_t)(int32_t)(int16_t)amount;
        uint8_t code[]={Bld_AddCBills,(uint8_t)amount,(uint8_t)(amount>>8),Bld_Exit};
        CBills=17; Execute_Bld_Bytecode(code); check(CBills==17+nativeAmount);
        code[0]=Bld_SubtractCBills; CBills=17; Execute_Bld_Bytecode(code); check(CBills==(17>=nativeAmount?17-nativeAmount:0));
        uint8_t operand[]={(uint8_t)amount,(uint8_t)(amount>>8)};
        CBills=17; branchTest(Bld_BranchIfCBills,operand,2,17>=nativeAmount);
    }
    uint8_t position[]={Bld_SetMapPosition,0x34,0x12,0xCD,0xAB,Bld_Exit};
    Execute_Bld_Bytecode(position); check(CrescentHawkMapPositionX==0x1234 && CrescentHawkMapPositionY==0xABCD);
    uint8_t x[]={0x34,0x12}; branchTest(Bld_BranchIfXEquals,x,2,1); x[0]=0; branchTest(Bld_BranchIfXEquals,x,2,0);
    for(unsigned value=0;value<256;++value) {
        uint8_t mask=(uint8_t)value; randomValue=0xA5; branchTest(Bld_BranchIfRandomMask,&mask,1,(0xA5&value)!=0);
        PurchasedMedkit=(uint8_t)value; PurchasedFieldSurgeryKit=(uint8_t)value;
        branchTest(Bld_BranchIfMedkit,NULL,0,value!=0); branchTest(Bld_BranchIfSurgeryKit,NULL,0,value!=0);
        answer=(uint16_t)value; branchTest(Bld_YesNoBranch,NULL,0,value!=0);
        PersistentState.bytes[0]=(uint8_t)value; uint8_t index=0; branchTest(Bld_BranchIfStateNonzero,&index,1,value!=0);
        memset(Characters,255,sizeof Characters); Characters[0].name=0; Characters[0].skillMedical=(uint8_t)value;
        uint8_t skill[]={6,0}; branchTest(Bld_BranchIfPartySkill,skill,2,(int8_t)value>=0);
    }
    branchTest(Bld_Branch,NULL,0,1);
    uint8_t code[]={Bld_SetStateByte,128,250,Bld_AddStateByte,128,10,Bld_TimedWait,Bld_WaitForKey,Bld_RedrawSidebar,
        Bld_DisplayText,'o','r','i','g','i','n','a','l',' ','t','e','x','t',0,Bld_Exit};
    Execute_Bld_Bytecode(code); check(WorldMapState.bytes[3048-128]==4 && waits==1 && keys==1 && sidebars==1 && texts==1);
    uint8_t table[]={Bld_MenuBranchTable,128,8,0,12,0,Bld_Exit,0,Bld_SetStateByte,98,1,Bld_Exit,Bld_SetStateByte,98,2,Bld_Exit};
    for(selection=0;selection<2;++selection) { Execute_Bld_Bytecode(table); check(argument==0xFF80 && PersistentState.bytes[98]==selection+1); }
    uint8_t stateTable[]={Bld_BranchStateTable,0,8,0,12,0,Bld_Exit,0,Bld_SetStateByte,98,1,Bld_Exit,Bld_SetStateByte,98,2,Bld_Exit};
    for(unsigned value=0;value<2;++value) { PersistentState.bytes[0]=(uint8_t)value; Execute_Bld_Bytecode(stateTable); check(PersistentState.bytes[98]==value+1); }
    uint8_t negativeTable[256]={Bld_BranchStateTable,0};
    negativeTable[243]=Bld_SetStateByte; negativeTable[244]=98; negativeTable[245]=77; negativeTable[246]=Bld_Exit;
    PersistentState.bytes[0]=255; Execute_Bld_Bytecode(negativeTable);
    check(PersistentState.bytes[98]==77); /* CBW(-1)*2 selects bytes BEFORE table. */
    selection=0x8001; Execute_Bld_Bytecode(table); check(PersistentState.bytes[98]==2);
    for(unsigned member=0;member<PartySize;++member) {
        memset(Characters,255,sizeof Characters); Characters[member].name=0; Characters[member].skillMedical=4;
        uint8_t skill[]={6,4}; branchTest(Bld_BranchIfPartySkill,skill,2,1);
        Characters[member].name=Character_Dead; branchTest(Bld_BranchIfPartySkill,skill,2,0);
    }
    for(unsigned opcode=0;opcode<Bld_PlaySound;++opcode) { uint8_t noop[]={(uint8_t)opcode,Bld_Exit}; Execute_Bld_Bytecode(noop); }
    entryMode=1; partyCount=0;
    for(unsigned script=0;script<BldFileCount;++script) {
        AlternativeBldByBuildingId[0]=(uint8_t)script;
        loads=disks=entryActions=healthRefreshes=cacheEntries=scenes=0;
        PersistentState.fields.entranceBldState=0; HasHoloviewer=0;
        Interact_with_BLD(0);
        check(loads==1 && disks==(script<2 || script>=17?3u:2u) && entryActions==1 && healthRefreshes==1);
        check(BldFileIndex==script && EnteredBuildingId==0 && BldInteractionFlag==TRUE && BTStatsAssetLoaded==FALSE);
        check(scenes==(unsigned)(EnterBuildingAnimations[script]!=0));
        if(scenes) check(entryScene==EnterBuildingAnimations[script] && secondArgument==BuildingEntryAnimationCallerRedraw);
    }
    AlternativeBldByBuildingId[0]=Bld_Garage;
    for(unsigned flags=0;flags<8;++flags) {
        HasViewedHolodisk=(uint8_t)(flags&1); HasHoloviewer=(uint8_t)(flags&2); partyCount=(uint8_t)(flags&4);
        loads=entryActions=healthRefreshes=0; ViewedHolodisk=0;
        Interact_with_BLD(0); unsigned chained=(flags==6);
        check(loads==1+chained && entryActions==1+chained && healthRefreshes==1+chained);
        check(EnteredBuildingId==(chained?Bld_Viewdisk:0) && ViewedHolodisk==(chained?12:0));
    }
    AlternativeBldByBuildingId[0]=Bld_BarracksReturn; partyCount=255; HasViewedHolodisk=255; HasHoloviewer=0;
    loads=0; Interact_with_BLD(0); check(loads==2 && ViewedHolodisk==12 && EnteredBuildingId==18);
    AlternativeBldByBuildingId[0]=Bld_CacheEntrance; partyCount=0;
    for(unsigned flag=0;flag<256;++flag) {
        memset(MapFogOfWar,0xA5,MapFogOfWarBytes); PersistentState.fields.entranceBldState=(uint8_t)flag; cacheEntries=0;
        Interact_with_BLD(0); check(cacheEntries==(unsigned)(flag!=0));
        for(unsigned i=0;i<MapFogOfWarBytes;++i)
            check(MapFogOfWar[i]==((flag && i%16==12 && i/16>=96 && i/16<104)?0:0xA5));
    }
    loads=0; Interact_with_BLD(Bld_Viewdisk); check(loads==1 && BldFileIndex==18 && EnteredBuildingId==18);
    AlternativeBldByBuildingId[0]=0;
    for(unsigned byte=0;byte<256;++byte) {
        EnterBuildingAnimations[0]=(uint8_t)byte; scenes=0;
        Interact_with_BLD(0); check(scenes==(unsigned)(byte!=0));
        if(scenes) check(entryScene==(uint16_t)(int16_t)(int8_t)byte && secondArgument==1);
    }
    EnterBuildingAnimations[0]=0;
    return 0;
}
