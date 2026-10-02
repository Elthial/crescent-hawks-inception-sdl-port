/* Native main-loop body; deterministic adapters for its original callees.
 * Not end-to-end gameplay validation. Finance, timers and fog writes are real. */
#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks,ticks,targetTicks,waits,moves,draws,npcs,encounters,pauses,balances;
static unsigned inputs,inputMode,messageAfterDraw,loss,replayAnswer,reloads,sidebar;
static uint16_t command,randomValues[4];
static unsigned randomIndex;
static void checkAtLine(int condition,unsigned line) {
    ++checks;
    if (!condition) { fprintf(stderr,"Main loop check %u failed at line%u\n",checks,line); exit(1); }
}
#define check(condition) checkAtLine((condition),__LINE__)
uint16_t Pending_Input(void) { return (uint16_t)(inputMode==2 || (inputMode==1 && inputs==0)); }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++inputs; return command; }
void Drain_Pending_Keyboard_Input(void) { }
uint16_t Keyboard_Convert_To_MoveCommands(uint16_t key) { return key; }
void Wait_For_N_Vertical_Retraces(uint16_t count) { check(count==1); ++waits; }
void Menu_Memory_Variables(uint16_t layout) { check(layout==4); }
void Character_Movement_On_Map(uint16_t key) { check(key==command); ++moves; }
void Draw_Infantry_And_Mechs(void) {
    ++draws;
    if(messageAfterDraw && moves) { MessageBoxOpen=TRUE; messageAfterDraw=0; }
}
void Update_Friendly_Movement_Animations(uint16_t key) { check(key==command && !MessageBoxOpen); }
void Game_Pause_Menu(void) { ++pauses; }
void Display_Animation_Scene(uint16_t id,uint16_t mode) {
    check(id==AnimationO15_LyranBlastDoor && mode==AnimationPlayback_RestoreGameView);
    check(DisableInput==0x1234 && !ExitMainLoop);
}
void Starport_MapPatch_SaveApply_Or_Restore(uint16_t restore) { check(restore==TRUE); ++reloads; }
uint8_t Rand_0x00_to_0xFF(void) { check(randomIndex<4); return (uint8_t)randomValues[randomIndex++]; }
void Combat_Run_Encounter(uint16_t mode) { check(mode==CombatEncounter_Roaming); ++encounters; }
void Display_Text_CBill_Balance(void) { ++balances; }
void Update_Animated_Map_Tiles(void) {
    ++ticks; randomIndex=0;
    if(ticks==targetTicks) ExitMainLoop=TRUE;
    if(loss) MainCharactersAlive=FALSE;
}
void Update_Roaming_Map_Npcs(void) { ++npcs; }
void PosXY_OffsetGrid(uint16_t x,uint16_t y) { check(x==CrescentHawkMapPositionX && y==CrescentHawkMapPositionY); }
void Copy_Data_To_GraphicsMemory(void) { }
void EGA_DrawBox_Wrapper(void) { }
void Draw_Message_Box(void) { }
void Display_Text_From_Memory(uint8_t *text) { check(text!=NULL); }
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text) {
    check(strcmp((char *)text," has died. You cannot finish the game without him.")==0);
}
void Draw_Top_Graphic_Sidebar(void) { ++sidebar; }
uint16_t Prompt_Yes_No(uint16_t mode) { check(mode==TRUE); return (uint16_t)replayAnswer; }
void Load_Game_Map_Data(void) { ++reloads; MainCharactersAlive=TRUE; }
static void prepare(void) {
    memset(WorldMapState.bytes,0,sizeof WorldMapState.bytes);
    memset(Characters,0,sizeof Characters);
    memset(StockBalances,0,sizeof StockBalances);
    ticks=waits=moves=draws=npcs=encounters=pauses=balances=inputs=messageAfterDraw=loss=reloads=sidebar=0;
    randomIndex=0; memset(randomValues,0,sizeof randomValues);
    targetTicks=1; inputMode=0; replayAnswer=0;
    MainCharactersAlive=TRUE; TransmittedCacheFound=FALSE;
    ExplorationStepsPerInput=1; MessageBoxOpen=0; NpcUpdatePhase=0;
    CBills=20; ComstarFinanceCountdown=1;
    PersistentState.fields.allowanceLow=50;
    command=Command_MoveNorth;
    CrescentHawkMapPositionX=0x0200; CrescentHawkMapPositionY=0x3000;
}
static void run(void) { Main_Game_Loop(0x1234); }
int main(void) {
    prepare(); targetTicks=3; ComstarFinanceCountdown=100; PartyHealthRecoveryTimer=5;
    TrainingMissionAndQuizCooldown=2; SchoolTrainingCooldown=1;
    UnclassifiedWorldCountdownA=3; UnclassifiedWorldCountdownB=4;
    run();
    check(ticks==3 && waits==23 && moves==0 && npcs==1);
    check(PartyHealthRecoveryTimer==2 && TrainingMissionAndQuizCooldown==0 && SchoolTrainingCooldown==0);
    check(UnclassifiedWorldCountdownA==0 && UnclassifiedWorldCountdownB==1 && ComstarFinanceCountdown==97);

    prepare(); inputMode=2; targetTicks=3; ComstarFinanceCountdown=100;
    ExplorationStepsPerInput=4; run();
    check(waits==0 && inputs==3 && moves==12 && draws==15);
    check(MapFogOfWar[386]==224 && MapFogOfWar[370]==224 && MapFogOfWar[402]==224);
    prepare(); inputMode=1; messageAfterDraw=1; ExplorationStepsPerInput=4; run();
    check(moves==1 && draws==2 && !MessageBoxOpen);
    prepare(); inputMode=1; ExplorationStepsPerInput=0xFFFF; run();
    check(moves==0 && draws==1); /* signed setting -1 performs no movement */
    prepare(); inputMode=1; command=Command_Pause; run();
    check(moves==0 && pauses==1 && draws==2);

    prepare(); inputMode=1; HasMapper=TRUE; run();
    for(unsigned row=0;row<8;++row) check(MapFogOfWar[386+row*16]==255);
    prepare(); inputMode=1; HasMapper=TRUE; InsideStarLeagueCache=TRUE; run();
    check(MapFogOfWar[386]==224 && MapFogOfWar[498]==0);

    /* Original adjacent fog writes: northern partial subrow hits last mech;
     * southern partial subrow hits real persistent script bytes. */
    prepare(); inputMode=1; CrescentHawkMapPositionX=0; CrescentHawkMapPositionY=1; run();
    check(WorldMapState.bytes[984]==224 && MapFogOfWar[0]==224 && MapFogOfWar[16]==224);
    prepare(); inputMode=1; CrescentHawkMapPositionX=0x0F00; CrescentHawkMapPositionY=0xF070; run();
    check(MapFogOfWar[2047]==224 && PersistentState.bytes[15]==224);
    prepare(); inputMode=1; CrescentHawkMapPositionX=0; CrescentHawkMapPositionY=0; run();
    check(WorldMapState.bytes[984]==0);
    prepare(); inputMode=1; CrescentHawkMapPositionY=0xF07F; run();
    check(PersistentState.bytes[0]==0); /* native unsigned last-Y guard */

    prepare(); StarportCountdown=1; PersistentState.fields.countdownLow=0;
    PersistentState.fields.countdownHigh=1; run();
    check(PersistentState.fields.countdownLow==255 && PersistentState.fields.countdownHigh==0 && StarportCountdown==255);
    prepare(); StarportCountdown=1; PersistentState.fields.countdownLow=1;
    CrescentHawkMapPositionX=0x0800; CrescentHawkMapPositionY=0x6000; run();
    check(StarportCountdown==0 && reloads==1);

    prepare(); KuritaDestroyedCitadel=1; TraitorBattleProbability=31; randomValues[0]=32; run();
    check(encounters==1);
    prepare(); KuritaDestroyedCitadel=1; TraitorBattleProbability=31; randomValues[0]=1; run();
    check(encounters==0);
    prepare(); KuritaDestroyedCitadel=1; InsideStarLeagueCache=TRUE; run(); check(encounters==0);

    prepare(); ComstarFinanceCountdown=0; run();
    check(CBills==35 && ComstarFinanceCountdown==255 && balances==1);
    prepare(); ComstarFinanceCountdown=0; CBills=50; StockBalances[0]=100; run();
    check(CBills==50 && StockBalances[0]==100 && balances==1); /* frozen wealth uses prepayment sum */
    prepare(); ComstarFinanceCountdown=0; CBills=0x80000000; StockBalances[0]=110; run();
    check(CBills==0x8000000F && StockBalances[0]==96); /* signed high-WORD wealth comparison */
    prepare(); ComstarFinanceCountdown=0; KuritaDestroyedCitadel=1;
    StockBalances[0]=110; StockBalances[1]=900; StockBalances[2]=9000;
    randomValues[0]=1; randomValues[1]=0; randomValues[2]=7; randomValues[3]=1;
    run();
    check(CBills==20 && balances==0 && StockBalances[0]==96 && StockBalances[1]==1000 && StockBalances[2]==4687);
    prepare(); PersistentState.fields.allowanceLow=0x34; PersistentState.fields.allowanceHigh=0xF2;
    check(Get_CBill_Allowance_Wealth_Limit()==0xF234);
    prepare(); loss=1; run(); check(sidebar==1 && inputs==1 && ExitMainLoop);
    prepare(); loss=1; replayAnswer=1; run(); check(reloads==1 && MainCharactersAlive);
    prepare(); TransmittedCacheFound=TRUE; run(); check(sidebar==0 && inputs==0 && ExitMainLoop);
    printf("Original exploration loop: %u checks\n",checks);
    return 0;
}
