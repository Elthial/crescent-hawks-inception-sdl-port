#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned mission,updates,inputs,generations,combats,restores,failures,startups,warnings,keys,npcUpdates,reloads;
static unsigned returnUpdate,jailDeath,repeatFirst,disabledDuringCombat;
static unsigned idleMode;
static uint16_t deploymentMechs,deploymentInfantry;
static uint8_t randomValue;
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"Mission mismatch line%u mission%u updates%u\n",line,mission,updates); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
/* Actual whole mission controller; generation, combat and world/UI routines
 * are explicit boundaries. Fixtures drive objectives, not real gameplay. */
void Mission_GenerateEnemies(uint16_t mechs,uint16_t infantry) { ++generations; deploymentMechs=mechs; deploymentInfantry=infantry; }
void Combat_Run_Encounter(uint16_t mode) {
    ++combats; disabledDuringCombat=DisableComputerControl;
    if(mission==2) {
        Mech *enemy=&Mechs[4]; check(enemy->currentActuators[0]==0 && enemy->currentActuators[1]==0 && enemy->walkMove==0 && enemy->jumpMove==0);
        for(unsigned i=0;i<10;++i) check(enemy->currentAmmo[i]==0);
    }
    check(mode==(mission==8?2:mission==9?3:1));
    if(mission==9) { check(updates==81); if(jailDeath) MainCharactersAlive=0; }
}
void Restore_Mission_Map_View_After_Combat(void) { ++restores; CrescentHawkMapPositionX=0x0C3C; CrescentHawkMapPositionY=0xC04F; }
void Character_Movement_On_Map(uint16_t command) {
    if(command==0) return;
    check(command==1);
    if(mission==0) {
        CrescentHawkMapPositionX=inputs==returnUpdate?0x0C3C:0x0C78;
        CrescentHawkMapPositionY=inputs==returnUpdate?0xC04F:0xC07C;
    } else if(mission==1) {
        CrescentHawkMapPositionX=inputs==returnUpdate?0x0C3C:(uint16_t)(0x0C00+2*TrainingRubbleTargetX[randomValue&7]);
        CrescentHawkMapPositionY=inputs==returnUpdate?0xC04F:(uint16_t)(0xC000+2*(TrainingRubbleTargetY[randomValue&7]+1));
    } else if(mission==9 && !jailDeath) {
        unsigned attempt=repeatFirst?(inputs<=2?0:inputs-2):inputs-1;
        CrescentHawkMapPositionX=(uint16_t)(0x0D14+attempt*4); CrescentHawkMapPositionY=0x702D;
    }
}
uint16_t Pending_Input(void) { check(++inputs<1000); return idleMode?FALSE:TRUE; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return 1; }
uint16_t Keyboard_Convert_To_MoveCommands(uint16_t key) { check(key==1); return 1; }
void Drain_Pending_Keyboard_Input(void) {}
void Update_Friendly_Movement_Animations(uint16_t command) { check(command==1); }
void Update_Animated_Map_Tiles(void) {
    ++updates;
    if(idleMode) { CrescentHawkMapPositionX=updates==1?0x0C78:0x0C3C; CrescentHawkMapPositionY=updates==1?0xC07C:0xC04F; }
}
void Update_Roaming_Map_Npcs(void) { ++npcUpdates; }
void PosXY_OffsetGrid(uint16_t x,uint16_t y) { check(x==CrescentHawkMapPositionX && y==CrescentHawkMapPositionY); }
void Copy_Data_To_GraphicsMemory(void) {}
void Draw_Infantry_And_Mechs(void) {}
void EGA_DrawBox_Wrapper(void) {}
void Draw_Message_Box(void) {}
void Display_Text_From_Memory(uint8_t *text) { if(!strcmp((char *)text,"Don't come back here until you complete your mission!")) ++warnings; }
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text) { check(text!=NULL); }
void Wait_For_50Hz_Then_Check_Input(void) {}
void Wait_For_N_Vertical_Retraces(uint16_t count) { check(count==1); }
void Menu_Memory_Variables(uint16_t layout) { check(layout==6); }
void Draw_Top_Graphic_Sidebar(void) {}
void Draw_Menu_Border(uint16_t border) { check(border==0); }
void Play_Failed_Mech_Startup_Scene(void) { ++failures; }
void Display_Animation_Scene(uint16_t id,uint16_t mode) { check(id==0 && mode==0); ++startups; }
uint8_t Rand_0x00_to_0xFF(void) { return randomValue; }
void Load_And_Decode_Indexed_BLD(uint16_t id) { check(id==BldFileIndex); ++reloads; BTStatsAssetLoaded=FALSE; }
static void prepare(unsigned number) {
    mission=number; updates=inputs=generations=combats=restores=failures=startups=warnings=keys=npcUpdates=reloads=0;
    returnUpdate=2; jailDeath=repeatFirst=idleMode=0; KuritaAttackFlag=DisableComputerControl=MissionNpcUpdatePhase=0;
    BTStatsAssetLoaded=0; FriendlyPersonnelWithdrawal=0; MainCharactersAlive=1;
    KuritaDestroyedCitadel=0; TrainingMechSurvivedKuritaAttack=0;
    memset(Mechs,0x5A,sizeof WorldMapState.fields.mechs); Mechs[0].name[0]='W';
    memset(MapFileTiles,0xA5,MapFileMaximumTileBytes);
    Characters[0].skillGunnery=Characters[0].skillPiloting=0; TerrainOverlapRows[0]=0;
    CrescentHawkMapPositionX=0x0C3C; CrescentHawkMapPositionY=0xC04F;
}
int main(void) {
    for(unsigned chassis=0;chassis<2;++chassis) for(unsigned ticks=213;ticks<=267;++ticks) {
        prepare(0); randomValue=0; returnUpdate=ticks; Mechs[0].name[0]=chassis?'L':'W';
        Mech_Mission(0); check(updates==ticks && warnings==1);
        check(TrainingMissionPassed==(ticks-(chassis?50u:0u)<215)); check(MapFileTiles[0x0FED]==0xA5);
    }
    for(unsigned target=0;target<8;++target) for(unsigned chassis=0;chassis<2;++chassis) {
        prepare(1); randomValue=(uint8_t)target; Mechs[0].name[0]=chassis?'L':'W';
        Mech_Mission(1); check(TrainingMissionPassed==!chassis && Characters[0].skillPiloting==1);
        for(unsigned tile=0;tile<MapFileMaximumTileBytes;++tile) check(MapFileTiles[tile]==0xA5);
    }
    for(unsigned rows=0;rows<256;++rows) {
        prepare(0); returnUpdate=220; TerrainOverlapRows[0]=(uint8_t)rows;
        Mech_Mission(0);
        int signedRows=rows<128?(int)rows:(int)rows-256;
        int delay=signedRows>=0?signedRows/8:-((-signedRows+7)/8);
        int16_t timer=(int16_t)(uint16_t)(220*(1+delay));
        check(TrainingMissionPassed==(timer<215));
    }
    prepare(0); idleMode=1; Mech_Mission(0);
    check(updates==2 && inputs==12 && npcUpdates==1 && TrainingMissionPassed==TRUE);
    for(unsigned number=2;number<=8;++number) for(unsigned dead=0;dead<2;++dead) for(unsigned withdrawn=0;withdrawn<2;++withdrawn) {
        prepare(number); randomValue=0; if(dead) Mechs[0].name[0]=255; FriendlyPersonnelWithdrawal=(uint16_t)withdrawn;
        BTStatsAssetLoaded=TRUE; BldFileIndex=7; Mech_Mission((uint16_t)number);
        check(generations==1 && combats==1 && restores==(number==8?0u:1u) && reloads==1 && DrawJailMissionParkedMechs==FALSE);
        check(TrainingMissionPassed==(number<8 && !dead && !withdrawn));
        if(number==4) check(disabledDuringCombat==TRUE && DisableComputerControl==FALSE);
        if(number==7) check(KuritaDestroyedCitadel==TRUE && TrainingMechSurvivedKuritaAttack==!dead && KuritaAttackFlag==FALSE);
        if(number==8) check(deploymentMechs==0x81 && deploymentInfantry==0);
    }
    for(unsigned skill=0;skill<256;++skill) {
        prepare(5); randomValue=1; Characters[0].skillGunnery=Characters[0].skillPiloting=(uint8_t)skill;
        FriendlyPersonnelWithdrawal=TRUE; Mech_Mission(5);
        uint8_t expected=(uint8_t)(skill+((int8_t)skill<4));
        check(Characters[0].skillGunnery==expected && Characters[0].skillPiloting==expected && TrainingMissionPassed==FALSE);
        prepare(1); Characters[0].skillPiloting=(uint8_t)skill; randomValue=0;
        Mech_Mission(1); check(Characters[0].skillPiloting==(uint8_t)(skill+((int8_t)skill<3)));
    }
    prepare(6); randomValue=1; Mech_Mission(6);
    check(KuritaDestroyedCitadel==TRUE && TrainingMechSurvivedKuritaAttack==TRUE && KuritaAttackFlag==FALSE && updates==0);
    for(unsigned tile=0;tile<MapFileMaximumTileBytes;++tile) check(MapFileTiles[tile]==(tile>=0x0FF0?0x40:0xA5));
    for(unsigned repeat=0;repeat<2;++repeat) {
        prepare(9); repeatFirst=repeat; CrescentHawkMapPositionX=0x0D00; CrescentHawkMapPositionY=0x7000;
        Mech_Mission(9); check(failures==2 && startups==1 && updates==3+repeat && combats==0 && TrainingMissionPassed==FALSE);
    }
    prepare(9); jailDeath=1; CrescentHawkMapPositionX=0x0D00; CrescentHawkMapPositionY=0x7000;
    Mech_Mission(9); check(updates==81 && combats==1 && deploymentMechs==0 && deploymentInfantry==0x88 && MainCharactersAlive==0);
    return 0;
}
