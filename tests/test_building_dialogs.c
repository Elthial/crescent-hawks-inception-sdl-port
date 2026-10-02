#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*Only caller boundaries are substituted. Every action enters the full native
 * dispatcher, with real EXE tables, record storage and original formatters.*/
static unsigned balances,heals,keys,waits,lines,advances,marches,retraces,animations,missions;
static unsigned grants,zeroTransactions,assigned,called[48],drawn,loaded,endingStage;
static uint16_t choice,yes,healingTier,mission,crew;
static uint32_t entered;
static uint8_t randomValue;
static uint16_t lastGrant;
static char output[16384];
static size_t outputLength;
static unsigned coverage[48];
static uint16_t lastDrawRow;
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"Building action mismatch line%u\n",line); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
uint8_t SDLBackend_EgaWriteMode;
void SDLBackend_CopyEgaLatchByte(uint16_t s,uint16_t so,uint16_t d,uint16_t doff)
{ (void)s; (void)so; (void)d; (void)doff; check(0); }
void Display_Text_From_Memory(uint8_t *text)
{
    size_t bytes=strlen((char *)text); check(outputLength+bytes<sizeof output);
    memcpy(output+outputLength,text,bytes+1); outputLength+=bytes;
    for(size_t byte=0;byte<bytes;++byte) if(text[byte]=='\r') ++TextRow;
}
void Display_Text_CBill_Balance(void) { ++balances; }
void Display_Text_4FA0_Value(void) { ++lines; }
void Display_Text_Shop_Cannot_Afford_Text(void) { ++called[0]; }
void Draw_EGA_Text_To_Screen(uint8_t *text,uint16_t x,uint16_t y,uint16_t foreground,uint16_t background)
{
    (void)y; check(background==0 && foreground<=15);
    if(!strcmp((char *)text,"Stock account:") || !strcmp((char *)text,"C-bill account:")) check(x==TextPanelLeft);
    else check(x==(uint16_t)(39-strlen((char *)text)));
    ++drawn; lastDrawRow=y;
}
void Draw_Top_Graphic_Sidebar(void) { }
uint16_t Display_Menu_Choices_And_Check(uint16_t menu) { check(menu==7 || menu==23); return choice; }
uint32_t Prompt_For_Unsigned_Decimal(void) { return entered; }
void Display_No_Stock_Transaction_Text(void) { ++zeroTransactions; }
void Distribute_Purchased_Armour(uint16_t type,uint16_t value) { check(type>=1 && type<=5 && value==ArmourTypeDurability[type]); ++called[9]; }
void Distribute_Weapon_To_Party(uint16_t weapon) { ++grants; lastGrant=weapon; }
void Talk_To_Building_Occupants(void) { ++called[12]; }
void Mechlube_Repair_Mech(void) { ++called[18]; }
void Mechlube_Modify_Mech(void) { ++called[19]; }
void Mechlube_Upgrade_Mech(void) { ++called[20]; }
void Heal_Characters(uint16_t tier) { ++heals; healingTier=tier; }
void Wait_For_50Hz_Then_Check_Input(void) { ++waits; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return 0; }
uint16_t Prompt_Yes_No(uint16_t defaultYes) { check(defaultYes==1); return yes; }
void Recruit_Rex_And_Start_KuritaParty_Ambush(void) { ++called[30]; }
void Prepare_Rental_Locust_For_Arena(void) { ++called[33]; }
void Prepare_Party_Mech_For_Arena(void) { ++called[34]; }
void Arena_Select_And_Apply_Combat_Map_Patch(void) { ++called[35]; }
void Arena_Remove_And_Randomize_Combat_Map_Patch(void) { ++called[1]; }
void Restore_Party_After_Arena_Combat(void) { ++called[2]; }
void Assign_Pilot_and_rider_to_Mechs(uint16_t slot) { ++assigned; crew=slot; }
void Run_Jailbreak_Mission_And_Award_Stinger(void) { ++called[40]; }
void Mechlube_Buy_Ammo(void) { ++called[44]; }
void Map_Construct_Nine_Regions(uint16_t region) { check(region==0x89); ++called[3]; }
void DOS_Load_Map_Files(uint16_t slot,uint16_t map) { check(slot<9 && map==0xFF80); ++loaded; }
void Map_NineGrid_Parent(void) { ++called[4]; }
void Starport_MapPatch_SaveApply_Or_Restore(uint16_t restore) { check(restore==0); ++called[5]; }
uint8_t Rand_0x00_to_0xFF(void) { return randomValue; }
uint8_t Advance_Combatant_Animation_Stream(uint16_t actor) { check(actor==0); ++advances; return 77; }
void Update_Animated_Map_Tiles(void) { ++marches; }
void PosXY_OffsetGrid(uint16_t x,uint16_t y) { check(x>=0xC3C && x<0xC40 && y==0xC04F); }
void Copy_Data_To_GraphicsMemory(void) { }
void Draw_Infantry_And_Mechs(void) { }
void EGA_DrawBox_Wrapper(void) { }
void Wait_For_N_Vertical_Retraces(uint16_t count) { check(count==3); ++retraces; }
void Display_Animation_Scene(uint16_t scene,uint16_t restore) { check(scene==0 && restore==0); ++animations; }
void Mech_Mission(uint16_t selectedMission) { ++missions; mission=selectedMission; }
void Select_Game_Disk_And_Drive(uint16_t disk) { check(disk==2 && endingStage++==0); }
uint16_t Load_File_To_Memory(const uint8_t *file,uint8_t *destination)
{
    if(endingStage==1) { check(!strcmp((char *)file,"ENDMECH.CMP") && destination==GraphicsSceneWorkspace && GraphicsCompatibilityFlag==0); }
    else check(endingStage==6 && !strcmp((char *)file,"STARLEAG.ICN") && destination==GraphicsFileWorkspace && GraphicsCompatibilityFlag==1);
    ++endingStage; return 0;
}
void Decompress_File_Into_Memory(uint8_t *source,uint8_t *destination)
{
    if(endingStage==2) check(source==GraphicsSceneWorkspace && destination==GraphicsFileWorkspace);
    else check(endingStage==7 && source==GraphicsFileWorkspace && destination==GraphicsSceneWorkspace);
    ++endingStage;
}
void DrawCall_Image_To_VGA_Memory(uint8_t *buffer,uint16_t segment) { check(endingStage++==3 && buffer==GraphicsFileWorkspace && segment==0xA800); }
void Set_Palette_registers(const uint8_t *palette)
{
    if(endingStage==4) { check(palette==EndingEgaPalette); ++endingStage; }
    else check(endingStage==9 && palette==DefaultEgaPalette);
}
void Draw_GraphicsFile_In_Memory(uint8_t *buffer,uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{ check(endingStage++==5 && buffer==GraphicsFileWorkspace && x==0 && y==0 && width==40 && height==200); }
void Graphics_Set_Screen_To_Black(void) { check(endingStage++==8); }
void Draw_Health_and_C_Bills_Sidebar(uint16_t refresh) { check(refresh==1 && endingStage==9); }
static void reset(void)
{
    memset(OriginalSavedState.bytes,0,sizeof OriginalSavedState.bytes);
    for(unsigned member=0;member<PartySize;++member) Characters[member].name=Character_Dead;
    memset(called,0,sizeof called); memset(output,0,sizeof output);
    balances=heals=keys=waits=lines=advances=marches=retraces=animations=missions=grants=zeroTransactions=assigned=drawn=loaded=endingStage=0;
    outputLength=0; choice=0; yes=1; entered=0; randomValue=0; GraphicsAdapter=2; MainCharactersAlive=1;
    StockHoldingCount=0; CpuTimingCalibration=0; DisableInput=0;
}
static void runAction(uint16_t action)
{ if(action>=1 && action<=47) ++coverage[action]; Citadel_Building_Dialogs(action); }
#define Citadel_Building_Dialogs runAction
static void testMoney(void)
{
    const uint32_t values[]={0,1,74,75,124,125,500,65535,65536,0x7FFFFFFF,0x80000000,0xFFFFFFFF};
    for(unsigned stock=0;stock<3;++stock) for(unsigned a=0;a<12;++a) for(unsigned b=0;b<12;++b) {
        reset(); choice=(uint16_t)stock; CBills=values[a]; StockBalances[stock]=values[b]; entered=values[b];
        uint32_t cash=CBills,holding=StockBalances[stock],amount=entered<cash?entered:cash;
        Citadel_Building_Dialogs(CitadelAction_InvestStock);
        check(CBills==cash-amount && StockBalances[stock]==holding+amount);
        check(balances==(unsigned)(amount!=0) && zeroTransactions==(unsigned)(cash!=0 && amount==0));
        reset(); choice=(uint16_t)stock; StockHoldingCount=1; CBills=values[a]; StockBalances[stock]=values[b]; entered=values[a];
        cash=CBills; holding=StockBalances[stock]; amount=entered<holding?entered:holding;
        Citadel_Building_Dialogs(CitadelAction_SellStock);
        check(CBills==cash+amount && StockBalances[stock]==holding-amount);
        check(balances==(unsigned)(amount!=0) && zeroTransactions==(unsigned)(holding!=0 && amount==0));
    }
    for(unsigned type=1;type<=5;++type) for(unsigned paid=0;paid<2;++paid) {
        reset(); MenuControls[14].selection=(uint16_t)type; BldInteractionFlag=0;
        CBills=ArmourPurchaseCost[type-1]-(paid?0u:1u);
        Citadel_Building_Dialogs(CitadelAction_BuyArmour);
        check(ShopPaymentSuccessful==paid && called[9]==paid && balances==paid && CBills==(paid?0:ArmourPurchaseCost[type-1]-1));
    }
    for(unsigned weapon=1;weapon<=10;++weapon) for(unsigned paid=0;paid<2;++paid) {
        reset(); unsigned menu=weapon<=6?16:weapon<=9?17:18,offset=weapon<=6?0:weapon<=9?6:9;
        MenuControls[menu].selection=(uint16_t)(weapon-offset); WeaponShopCategoryOffset=(uint8_t)offset;
        CBills=InfantryWeaponPurchaseCost[weapon-1]-(paid?0u:1u); Citadel_Building_Dialogs(CitadelAction_BuyWeapon);
        check(ShopPaymentSuccessful==paid && grants==paid && balances==paid && (!paid || lastGrant==weapon));
    }
}
static void testTrainingAndHealing(void)
{
    for(unsigned skill=0;skill<3;++skill) for(unsigned level=0;level<=4;++level) for(unsigned paid=0;paid<2;++paid) {
        reset(); SelectedSchoolSkill=(uint8_t)skill; uint8_t *value=(uint8_t *)&Characters[0]+4+skill; *value=(uint8_t)level;
        uint32_t price=level*125+75; CBills=price-(paid?0u:1u); SchoolTrainingPurchased=0xA5;
        Citadel_Building_Dialogs(CitadelAction_BuySchoolTraining);
        unsigned succeeds=paid && level!=4;
        check(*value==level+succeeds && SchoolTrainingPurchased==succeeds && balances==succeeds);
        check(CBills==price-(paid?0u:1u)-(succeeds?price:0));
    }
    for(unsigned action=15;action<=21;action+=6) for(unsigned member=0;member<8;++member) for(unsigned old=0;old<256;++old) {
        reset(); SelectedPartyMemberSlot=(uint8_t)member; CBills=500;
        uint8_t *skill=action==15?&Characters[member].skillTech:&Characters[member].skillMedical; *skill=(uint8_t)old;
        Citadel_Building_Dialogs((uint16_t)action);
        check(*skill==(uint8_t)(old+1) && CBills==0 && balances==1 && SelectedPartyMemberSlot==1);
        check(Characters[member].trainingFlags==(action==15?1:2));
    }
    for(unsigned tier=0;tier<8;++tier) for(unsigned paid=0;paid<2;++paid) {
        reset(); SelectedMedicalServiceTier=(uint8_t)tier; CBills=tier?(uint32_t)MedicalServiceFee[tier]-(paid?0u:1u):0;
        uint32_t before=CBills; Citadel_Building_Dialogs(CitadelAction_TreatParty);
        check(heals==(unsigned)(tier==0 || paid) && balances==(unsigned)(tier!=0));
        check(!heals || healingTier==tier);
        check(CBills==(tier==0?before:paid?0:before+25)); check(keys==(unsigned)(tier!=0 && !paid) && waits==keys);
    }
    for(unsigned value=0;value<256;++value) {
        reset(); Characters[0].health=(uint8_t)value; Characters[0].trainingFlags=0xA5;
        Citadel_Building_Dialogs(CitadelAction_DeductJasonHealth);
        check(Characters[0].health==((int8_t)value>5?(uint8_t)(value-4):value) && Characters[0].trainingFlags==0xA5);
    }
}
static void testArmour(void)
{
    for(unsigned type=1;type<=5;++type) for(unsigned remaining=0;remaining<=ArmourTypeDurability[type];++remaining) for(unsigned funds=0;funds<=4;++funds) {
        reset(); Characters[2].armourType=(uint8_t)type; Characters[2].armourValue=(uint8_t)remaining; SelectedPartyMemberSlot=2;
        unsigned deficit=ArmourTypeDurability[type]-remaining,rate=ArmourRepairCostPerPoint[type-1];
        unsigned cash=funds==0?0:funds==1?rate-1:funds==2?rate:funds==3?rate*deficit:rate*deficit/2;
        CBills=cash; unsigned points=cash/rate; if(points>deficit) points=deficit;
        Citadel_Building_Dialogs(CitadelAction_RepairArmour);
        check(Characters[2].armourValue==remaining+points && CBills==cash-points*rate && balances==points && keys==1);
    }
}
static void testStory(void)
{
    for(unsigned selected=0;selected<4;++selected) for(unsigned calibration=0;calibration<3;++calibration) {
        reset(); SelectedTrainingMech=(uint8_t)selected; CpuTimingCalibration=(uint16_t)(calibration==0?8:calibration==1?9:0xFFFF);
        PersistentState.bytes[0]=0x80; Citadel_Building_Dialogs(CitadelAction_StartTraining);
        check(Mechs[0].name[0]==MechRefs[selected==0?0:selected==2?4:1].name[0]);
        check(CrescentHawkMapPositionX==0xC40 && CrescentHawkMapPositionY==0xC04F && advances==5 && marches==24 && animations==1);
        check(retraces==(calibration==1?24u:0u) && missions==1 && mission==0xFF80 && CombatantSpriteFrame[0]==77);
    }
    for(unsigned destroyed=0;destroyed<2;++destroyed) {
        reset(); KuritaDestroyedCitadel=(uint8_t)destroyed;
        for(unsigned effect=0;effect<64;++effect) { MapEffectSpriteIndex[effect]=(uint8_t)(effect+0x70); MapEffectPackedPage[effect]=0x8A; MapEffectPositionXLow[effect]=1; MapEffectPositionYLow[effect]=2; }
        Citadel_Building_Dialogs(CitadelAction_LeaveTraining);
        for(unsigned effect=0;effect<64;++effect) {
            uint8_t original=(uint8_t)(effect+0x70); unsigned cleared=!destroyed && (original&0x7E)==0x7C;
            check(MapEffectSpriteIndex[effect]==(cleared?0:original) && MapEffectPositionYLow[effect]==2);
        }
    }
    for(unsigned alive=0;alive<256;++alive) {
        reset(); Characters[0].name=0;
        for(unsigned member=1;member<8;++member) Characters[member].name=(uint8_t)(alive&(1u<<member)?member:255);
        unsigned count=0; for(unsigned member=1;member<8;++member) count+=(Characters[member].name!=255);
        Citadel_Building_Dialogs(CitadelAction_CountCompanions); check(SelectedPartyMemberSlot==count);
        choice=(uint16_t)count; Citadel_Building_Dialogs(CitadelAction_SelectPartyMember);
        unsigned last=0; for(unsigned member=1;member<8;++member) if(Characters[member].name!=255) last=member;
        check(SelectedPartyMemberSlot==last && MenuControls[23].optionCount==count+1);
    }
    for(unsigned saved=0;saved<16;++saved) {
        reset(); ArenaEscapeAllowed=1; CombatantActive[0]=0; CombatantPackedY[4]=0x8066; MainCharactersAlive=0;
        memset(MapFileByWorldRegion,128,WorldRegionCount); memset(ArenaMechRecordBackup,0xA5,125); memset(Mechs,0xEE,8*125);
        for(unsigned mech=0;mech<4;++mech) StoredPartyMechNameInitial[mech]=(uint8_t)(saved&(1u<<mech)?'L'+mech:255);
        uint8_t names[4]; memcpy(names,StoredPartyMechNameInitial,4);
        for(unsigned member=1;member<8;++member) SavedPartyNameId[member]=(uint8_t)member;
        Citadel_Building_Dialogs(CitadelAction_RunArena);
        check(CrescentHawkMapPositionX==0x970 && CrescentHawkMapPositionY==0x8066 && ArenaRentalMechMode==0);
        check(PersistentState.fields.requiredMainCharacterDead==1 && called[1]==0 && called[2]==0 && loaded==9);
        unsigned vacancy=0; if(saved) for(unsigned mech=1;mech<4;++mech) if(names[mech]==255) { vacancy=mech; break; }
        if(saved) {
            check(((uint8_t *)&Mechs[vacancy])[1]==0xA5 && Mechs[vacancy].name[0]==names[0]);
            if(vacancy) check(Mechs[0].name[0]==0xEE);
        } else check(Mechs[0].name[0]==0xEE);
        for(unsigned mech=0;mech<4;++mech) check(StoredPartyMechNameInitial[mech]==255 && Mechs[mech].pilotId==255 && Mechs[mech].riderId==255);
        for(unsigned member=0;member<8;++member) check(Characters[member].mechAssignment==8);
    }
    reset(); Citadel_Building_Dialogs(CitadelAction_RunArena); check(called[1]==1 && called[2]==1 && CrescentHawkMapPositionX==0xA39 && CrescentHawkMapPositionY==0x805D);
    reset(); Citadel_Building_Dialogs(CitadelAction_ShowEnding); check(endingStage==9 && waits==1 && keys==1);
    reset(); CrescentHawkMapPositionX=0xABCD; CrescentHawkMapPositionY=0xFFFF;
    Citadel_Building_Dialogs(CitadelAction_StageAllNpcs);
    check(CrescentHawkMapPositionY==0);
    for(unsigned npc=0;npc<8;++npc) check(RoamingMapNpcs[npc].currentPositionX==0xABCD && RoamingMapNpcs[npc].currentPositionY==0xFFFF &&
        RoamingMapNpcs[npc].movementDelay==npc && RoamingMapNpcs[npc].waypointPair==0x77 && CombatantPackedY[16+npc]==0xFFFF);
    reset(); MapInteractablePositionX[7]=0x1234; MapInteractablePositionY[7]=0x5678; MapInteractablePositionX[0]=9; MapInteractablePositionY[0]=10;
    Citadel_Building_Dialogs(CitadelAction_StageFirstNpc); check(RoamingMapNpcs[0].currentPositionX==0x1234 && RoamingMapNpcs[0].destinationY==10 && RoamingMapNpcs[0].waypointPair==0x70);
}
static void testRemainingActions(void)
{
    const uint16_t delegated[]={CitadelAction_TalkToOccupants,CitadelAction_RepairMech,CitadelAction_ModifyMech,
        CitadelAction_UpgradeMech,CitadelAction_RecruitRex,CitadelAction_RentArenaLocust,CitadelAction_StageOwnedArenaMech,CitadelAction_BuyMechAmmo};
    for(unsigned entry=0;entry<sizeof delegated/sizeof delegated[0];++entry) {
        reset(); Citadel_Building_Dialogs(delegated[entry]); check(called[delegated[entry]]==1);
    }
    for(unsigned value=0;value<256;++value) {
        reset(); Mechs[0].name[0]=(uint8_t)value; Citadel_Building_Dialogs(CitadelAction_TrainingDebrief);
        check(!strcmp(output,value=='L'?"Locust, the results are hardly surprising. ":value=='W'?
            "Wasp, the results are all the more impressive. ":"Chameleon, the results are all the more impressive. "));
        reset(); Characters[0].skillBowsAndBlade=Characters[0].skillPistol=Characters[0].skillRifle=(uint8_t)value;
        Citadel_Building_Dialogs(CitadelAction_ShowTranscript); check(drawn==3);
        reset(); SelectedPartyMemberSlot=3; Characters[3].trainingFlags=(uint8_t)value;
        Citadel_Building_Dialogs(CitadelAction_QueryTechTraining); check(SelectedSpecialistTrainingMask==(value&1));
        Citadel_Building_Dialogs(CitadelAction_QueryMedicalTraining); check(SelectedSpecialistTrainingMask==(value&2));
    }
    for(unsigned holdings=0;holdings<8;++holdings) {
        reset(); CBills=0x12345678; TextRow=257;
        for(unsigned company=0;company<3;++company) StockBalances[company]=holdings&(1u<<company)?65536:0;
        unsigned count=0; for(unsigned bit=0;bit<3;++bit) count+=(holdings>>bit)&1;
        Citadel_Building_Dialogs(CitadelAction_ShowAccounts);
        check(StockHoldingCount==count && drawn==count+1 && lastDrawRow==(count?257+count:1));
        check(count || !strcmp(output,"\rNone"));
    }
    for(unsigned body=0;body<256;++body) for(unsigned health=0;health<256;++health) {
        /*No clearing large fixture per tuple: only the eight read records matter.*/
        for(unsigned member=1;member<8;++member) Characters[member].name=255;
        Characters[0].name=0; Characters[0].body=(uint8_t)body; Characters[0].health=(uint8_t)health;
        Citadel_Building_Dialogs(CitadelAction_QueryInjuries);
        check(PartyHasInjuredMember==((int8_t)body*10!=(int8_t)health));
    }
    for(unsigned visible=0;visible<16;++visible) {
        reset(); unsigned count=0;
        for(unsigned mech=0;mech<4;++mech) { Mechs[mech].name[0]=(uint8_t)(visible&(1u<<mech)?'L'+mech:255); count+=(Mechs[mech].name[0]!=255); }
        Citadel_Building_Dialogs(CitadelAction_CountVisibleMechs); check(NumberOfActiveLanceMechs==count);
        Citadel_Building_Dialogs(CitadelAction_HidePartyMechs);
        for(unsigned mech=0;mech<4;++mech) check(Mechs[mech].name[0]==255 && StoredPartyMechNameInitial[mech]==(visible&(1u<<mech)?'L'+mech:255));
        Citadel_Building_Dialogs(CitadelAction_CountStoredMechs); check(NumberOfActiveLanceMechs==count);
        Citadel_Building_Dialogs(CitadelAction_RestorePartyMechs); check(assigned==1 && crew==0);
        for(unsigned mech=0;mech<4;++mech) check(Mechs[mech].name[0]==StoredPartyMechNameInitial[mech] && CombatantSpriteFamilyOffset[mech]==(Mechs[mech].name[0]=='L'?0:146));
        Citadel_Building_Dialogs(CitadelAction_RebuildMechSprites); check(assigned==2);
    }
    reset(); Citadel_Building_Dialogs(CitadelAction_GrantMedkit); Citadel_Building_Dialogs(CitadelAction_GrantSurgeryKit);
    check(PurchasedMedkit==1 && PurchasedFieldSurgeryKit==1);
    reset(); SelectedPartyMemberSlot=2; Characters[2].armourType=5;
    Citadel_Building_Dialogs(CitadelAction_QueryArmourType); check(SelectedPartyArmourType==5);
    reset(); TraitorCharacterId=3; Characters[3].name=4;
    Citadel_Building_Dialogs(CitadelAction_ShowTraitorName); check(!strcmp(output,(char *)CharacterNames[4]));
    reset(); NextRecruitNameId=7; Citadel_Building_Dialogs(CitadelAction_ShowRecruitName); check(!strcmp(output,(char *)CharacterNames[7]));
    reset(); Citadel_Building_Dialogs(CitadelAction_KillJason); check(!MainCharactersAlive && Characters[0].name==255 && !PersistentState.fields.requiredMainCharacterDead);
    reset(); Citadel_Building_Dialogs(CitadelAction_ResetCrewAssignments); check(assigned==1 && crew==1);
    reset(); MainCharactersAlive=0; Citadel_Building_Dialogs(CitadelAction_Jailbreak); check(called[40]==1 && PersistentState.fields.requiredMainCharacterDead==1);
    for(unsigned body=0;body<256;++body) for(unsigned coin=0;coin<2;++coin) {
        reset(); SelectedPartyMemberSlot=2; randomValue=(uint8_t)coin;
        for(unsigned member=0;member<8;++member) { Characters[member].name=(uint8_t)member; Characters[member].body=(uint8_t)body; Characters[member].health=100; }
        Citadel_Building_Dialogs(CitadelAction_GrantLasers); check(grants==3 && lastGrant==13-coin);
        int cap=(6+(int)coin)*(int8_t)body;
        for(unsigned member=0;member<8;++member) check(Characters[member].health==(100>cap?(uint8_t)cap:100));
    }
    reset(); Characters[0].name=0; Characters[0].weapon=1;
    Citadel_Building_Dialogs(CitadelAction_GrantLasers); check(Characters[0].weapon==13 && grants==0);
    reset(); Citadel_Building_Dialogs(CitadelAction_ShowCash); check(!strcmp(output,"0\006\017."));
    reset(); for(unsigned action=0;action<65536;++action) if(action==0 || action>47) Citadel_Building_Dialogs((uint16_t)action);
    check(outputLength==0 && balances==0 && drawn==0);
}
int main(void)
{
    testMoney(); testTrainingAndHealing(); testArmour(); testStory(); testRemainingActions();
    for(unsigned action=1;action<=47;++action) check(coverage[action]!=0);
    puts("Original building dispatcher scenarios passed"); return 0;
}
