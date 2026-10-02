#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned moves,origins,offsets,loads,rebuilds,attempts,arriveOn;
static uint16_t expectedTarget,loadedSlots[9],loadedMaps[9];
static void verify(int condition,unsigned line) { if(!condition) { fprintf(stderr,"Encounter mismatch line%u\n",line); exit(1); } }
#define check(condition) verify(!!(condition),__LINE__)
/* Original three routines; cache, disk and movement-probe boundaries are
 * scripted, not gameplay validation or replacement production methods. */
void Move_Map_View_To_Packed_Position(uint16_t x,uint16_t y) { ++moves; CrescentHawkMapPositionX=x; CrescentHawkMapPositionY=y; }
void PosXY_OffsetGrid(uint16_t x,uint16_t y) { check(x==CrescentHawkMapPositionX && y==CrescentHawkMapPositionY); ++offsets; }
void Update_Cached_Map_Origin(void) { ++origins; }
void Map_NineGrid_Parent(void) { ++rebuilds; }
void DOS_Load_Map_Files(uint16_t slot,uint16_t map) { check(loads<9); loadedSlots[loads]=slot; loadedMaps[loads++]=map; }
void Movement_Select_Next_Step(uint16_t actor,uint16_t x,uint16_t y,uint16_t screenX,uint16_t screenY,uint16_t render) {
    check(actor==MovementProbeFlag && render==FALSE);
    check(x==CombatantPackedX[expectedTarget] && y==CombatantPackedY[expectedTarget]);
    check(screenX==(uint16_t)(CombatPreviewOriginColumn-attempts) && screenY==CombatPreviewOriginRow+attempts);
    ++attempts; CombatChosenStepX=0xFFFF; CombatChosenStepY=1;
    if(attempts==arriveOn) { MovementActorPositionX=x; MovementActorPositionY=y; }
}
static void prepare(void) {
    memset(CombatantActive,0,sizeof CombatantActive);
    Characters[Character_Jason].mechAssignment=Character_OnFoot;
    CombatantPackedX[4]=0x0220; CombatantPackedY[4]=0x3020;
    CombatantPackedX[12]=0x0230; CombatantPackedY[12]=0x3030;
    CombatantPackedX[13]=0x0221; CombatantPackedY[13]=0x3020;
    CrescentHawkMapPositionX=0x0210; CrescentHawkMapPositionY=0x3010;
    moves=origins=offsets=attempts=0; expectedTarget=12; arriveOn=1;
}
static void assess(unsigned expectedResult,unsigned expectedAttempts) {
    check(Combat_Assess_FirstEnemy_Reachability()==expectedResult);
    check(attempts==expectedAttempts && moves==2 && offsets==2 && origins==2);
    check(CrescentHawkMapPositionX==0x0210 && CrescentHawkMapPositionY==0x3010);
}
int main(void) {
    prepare(); assess(FALSE,0); check(CharacterMovementPointsRemaining==30 && MovementActorPositionX==0x0220);
    prepare(); CombatantActive[12]=2; CombatantActive[13]=1; assess(TRUE,1);
    check(CharacterMovementPointsRemaining==0 && MovementActorPositionX==0x0230);
    prepare(); CombatantActive[13]=1; expectedTarget=13; assess(TRUE,1);
    prepare(); CombatantActive[12]=1; CombatantPackedX[12]=0x0220; CombatantPackedY[12]=0x3020;
    assess(FALSE,0); check(CharacterMovementPointsRemaining==30);
    for(unsigned arrival=1;arrival<=31;++arrival) {
        prepare(); CombatantActive[12]=1; arriveOn=arrival;
        assess(arrival<=30,arrival<=30?arrival:30); check(CharacterMovementPointsRemaining==0);
    }
    prepare(); Characters[0].mechAssignment=2; CombatantPackedX[2]=0x0440; CombatantPackedY[2]=0x5040;
    assess(FALSE,0); check(MovementActorPositionX==0x0440 && MovementActorPositionY==0x5040);
    memset(MapDescriptorCache,0x55,MapDescriptorCacheBytes);
    memset(MapFileByWorldRegion,0,sizeof MapFileByWorldRegion);
    CrescentHawkMapPositionX=0x0220; CrescentHawkMapPositionY=0x3020;
    /* Center region32: top-left21,22,23 /31,32,33 /41,42,43. */
    for(unsigned row=0;row<3;++row) for(unsigned column=0;column<3;++column)
        MapFileByWorldRegion[0x21+row*16+column]=(uint8_t)(0x80+row*3+column);
    MapDescriptorCache[4*64]=MapFileFirstBlockDescriptor;
    loads=rebuilds=0; Combat_Load_9Grid_Map(); check(loads==8 && rebuilds==1);
    for(unsigned i=0;i<8;++i) { unsigned slot=i<4?i:i+1; check(loadedSlots[i]==slot && loadedMaps[i]==(uint16_t)(0xFF80+slot)); }
    check(MapDescriptorCache[0]==0x55); /* Loader boundary does not write; routine doesn't clear. */
    CrescentHawkMapPositionX=CrescentHawkMapPositionY=0;
    memset(MapFileByWorldRegion,1,sizeof MapFileByWorldRegion); memset(MapDescriptorCache,0,MapDescriptorCacheBytes);
    loads=rebuilds=0; Combat_Load_9Grid_Map(); check(loads==5 && rebuilds==1);
    check(loadedSlots[0]==4 && loadedSlots[4]==8); /* Linear world-edge crossing retained. */
    puts("Original encounter map/probe/restore checks passed"); return 0;
}
