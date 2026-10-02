/* Original movement step + direction + occupancy bodies. Only unused UI/
 * audio adapters are test doubles; planning must not crush infantry. */
#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
static void checkAtLine(int condition,unsigned line) {
    ++checks;
    if(!condition) { fprintf(stderr,"Movement-step check%u failed line%u\n",checks,line); exit(1); }
}
#define check(condition) checkAtLine((condition),__LINE__)
void Menu_Memory_Variables(uint16_t layout) { (void)layout; check(0); }
void Draw_Top_Graphic_Sidebar(void) { check(0); }
void Display_Text_From_Memory(uint8_t *text) { (void)text; check(0); }
void Play_Sound_If_Enabled(uint16_t sound) { (void)sound; check(0); }
void Register_Persistent_Map_Effect(uint16_t id,uint16_t x,uint16_t y) { (void)id; (void)x; (void)y; check(0); }
void Wait_For_N_Vertical_Retraces(uint16_t count) { (void)count; check(0); }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { check(0); return 0; }
void Map_Move_North(void) { check(0); }
void Map_Move_South(void) { check(0); }
void Map_Move_West(void) { check(0); }
void Map_Move_East(void) { check(0); }
void EGA_Upload_Animated_Tile(uint8_t *source,uint16_t destination) {
    (void)source; (void)destination; check(0);
}
static void prepare(void) {
    memset(CombatMap,0,sizeof CombatMap);
    memset(CombatantActive,0,sizeof CombatantActive);
    memset(CombatantPackedX,255,sizeof CombatantPackedX);
    memset(CombatantPackedY,255,sizeof CombatantPackedY);
    memset(CombatantMovementDirection,2,sizeof CombatantMovementDirection);
    CrescentHawkMapPositionX=MovementActorPositionX=0x0220;
    CrescentHawkMapPositionY=MovementActorPositionY=0x3020;
    CachedMapOriginIndex=0; BlockingTileCodeThreshold=0x55;
    CombatChosenStepX=123; CombatChosenStepY=-456; CombatDestinationBlocked=0;
    MovementSearchBank=0; MovementSearchBankTimer=100;
}
static void step(uint16_t actor,uint16_t targetX,uint16_t targetY,uint16_t screenX,uint16_t screenY,uint16_t alternate) {
    Movement_Select_Next_Step(actor,targetX,targetY,screenX,screenY,alternate);
    check(CrescentHawkMapPositionX==0x0220 && CrescentHawkMapPositionY==0x3020);
}
int main(void) {
    prepare(); step(16,0x0224,0x3020,26,12,FALSE);
    check(MovementActorPositionX==0x0221 && MovementActorPositionY==0x3020);
    check(CombatChosenStepX==1 && CombatChosenStepY==0 && CombatantMovementDirection[16]==2);
    check(CombatantPackedX[16]==0xFFFF); /* Caller publishes working result. */
    prepare(); CombatMap[150]=0xF6; step(16,0x0224,0x3020,26,12,FALSE);
    check(CombatChosenStepX==1 && CombatChosenStepY==0); /* Infantry needs primary151 only. */
    prepare(); CombatMap[150]=0xF6; step(0,0x0224,0x3020,26,12,FALSE);
    check(CombatChosenStepX==1 && CombatChosenStepY==-1 && CombatantMovementDirection[0]==1);
    prepare(); memset(CombatMap,0xF6,sizeof CombatMap);
    CombatDestinationBlocked=0x1234; step(16,0x0224,0x3020,26,12,FALSE);
    check(CombatChosenStepX==0 && CombatChosenStepY==0 && CombatantMovementDirection[16]==2);
    check(MovementActorPositionX==0x0220 && MovementActorPositionY==0x3020 && CombatDestinationBlocked==0x1234);

    prepare(); CombatantPackedX[4]=0x0221; CombatantPackedY[4]=0x3020;
    step(16,0x0221,0x3020,26,12,FALSE);
    check(CombatChosenStepX==1 && CombatChosenStepY==1 && CombatantMovementDirection[16]==3);
    check(CombatDestinationBlocked==TRUE && !CombatantActive[4]); /* Original exact-position check ignores active. */
    prepare(); CombatantActive[16]=TRUE; CombatantPackedX[16]=0x0221; CombatantPackedY[16]=0x3020;
    step(0,0x0224,0x3020,26,12,FALSE);
    check(CombatChosenStepX==1 && CombatChosenStepY==0 && CombatantActive[16]); /* Planning disables crush. */
    prepare(); CombatantActive[12]=TRUE; CombatantPackedX[12]=0x0221; CombatantPackedY[12]=0x3020;
    step(MovementProbeFlag,0x0224,0x3020,26,12,FALSE);
    check(CombatChosenStepX==1 && CombatChosenStepY==0 && CombatantMovementDirection[0]==2);
    prepare(); CombatMap[150]=0xF6;
    step(MovementProbeFlag,0x0224,0x3020,26,12,FALSE);
    check(CombatChosenStepY==-1); /* Probe still checks mech footprint. */

    prepare(); MovementSearchBankTimer=1;
    step(16,0x0220,0x3020,26,12,TRUE);
    check(CombatChosenStepX==0 && CombatChosenStepY==0);
    check(MovementSearchBank==8 && MovementSearchBankTimer==30);
    MovementSearchBankTimer=1; step(16,0x0220,0x3020,26,12,TRUE);
    check(MovementSearchBank==0 && MovementSearchBankTimer==30);
    MovementSearchBank=0xAB08; step(16,0x0220,0x3020,26,12,FALSE);
    check(MovementSearchBank==0 && MovementSearchBankTimer==30);
    MovementSearchBankTimer=0; step(16,0x0220,0x3020,26,12,TRUE);
    check(MovementSearchBankTimer==0xFFFF && MovementSearchBank==0);

    /* Bank0 retries East then Southeast; bank8 retries East then Northeast. */
    for(unsigned bank=0;bank<2;++bank) {
        prepare(); CombatMap[151]=0xF6;
        MovementSearchBank=(uint16_t)(bank*CompassDirectionCount);
        step(16,0x0224,0x3020,26,12,TRUE);
        check(CombatChosenStepX==1 && CombatChosenStepY==-1);
        /* Bank0's second directionSE shares primary151 and is rejected before NE. */
        check(MovementSearchBank==bank*CompassDirectionCount);
    }
    /* Turn only one octant toward desired facing before candidate search. */
    prepare(); CombatantMovementDirection[16]=0;
    step(16,0x0224,0x3020,26,12,FALSE);
    check(CombatantMovementDirection[16]==1 && CombatChosenStepY==-1);

    /* Packed local127 carries into X/Y page nibbles; camera unchanged. */
    prepare(); MovementActorPositionX=0x027F;
    step(16,0x0302,0x3020,26,12,FALSE);
    check(MovementActorPositionX==0x0300 && CombatChosenStepX==1);
    prepare(); MovementActorPositionY=0x307F; CombatantMovementDirection[16]=4;
    step(16,0x0220,0x4002,26,12,FALSE);
    check(MovementActorPositionY==0x4000 && CombatChosenStepY==1);

    /* ScreenX0+east1 -> relative-12, signed terrain address wraps before
     * adding origin100, giving cell94 rather than huge logical-shift result. */
    prepare(); CachedMapOriginIndex=100; CombatMap[94]=0xF6;
    step(16,0x0224,0x3020,0,0,FALSE);
    check(CombatChosenStepY==-1); /* Retry NE uses signed Y floor at another cell. */
    prepare(); CachedMapOriginIndex=100; CombatMap[109]=0xF6;
    step(16,0x0224,0x3020,0x7FFF,(uint16_t)(int16_t)-1364,FALSE);
    check(CombatChosenStepX==1 && CombatChosenStepY==-1); /* Wrapped SUB before SAR, not promoted subtraction. */
    prepare(); BlockingTileCodeThreshold=0xFFFF;
    step(16,0x0224,0x3020,26,12,FALSE);
    check(CombatChosenStepX==0 && CombatChosenStepY==0); /* First threshold comparison is signed. */

    /* Integrated headless roaming sequence: real NPC -> step -> direction ->
     * occupancy -> frame interpreter, no movement/animation stand-in. */
    prepare();
    memset(RoamingMapNpcs,0,sizeof RoamingMapNpcs);
    memset(CombatantAnimationDirection,255,sizeof CombatantAnimationDirection);
    for(unsigned slot=1;slot<MapCharacterCount;++slot) RoamingMapNpcs[slot].movementDelay=255;
    HoldRickAtlasUntilLoungeConversation=0;
    CombatantPackedX[16]=0x0220; CombatantPackedY[16]=0x3020;
    RoamingMapNpcs[0].destinationX=0x0225; RoamingMapNpcs[0].destinationY=0x3020;
    Update_Roaming_Map_Npcs();
    check(CombatantPackedX[16]==0x0221 && CombatantPackedY[16]==0x3020);
    check(CombatantSpriteFrame[16]==20 && CombatantAnimationDirection[16]==2 && RoamingNpcVisibleOnScreen[0]);
    Update_Roaming_Map_Npcs();
    check(CombatantPackedX[16]==0x0222 && CombatantSpriteFrame[16]==21);
    CombatantPackedX[4]=0x0223; CombatantPackedY[4]=0x3020;
    Update_Roaming_Map_Npcs();
    check(CombatantPackedX[16]==0x0223 && CombatantPackedY[16]==0x3021);
    check(CombatantMovementDirection[16]==3 && CombatantAnimationDirection[16]==3 && CombatantSpriteFrame[16]==20);
    check(CrescentHawkMapPositionX==0x0220 && CrescentHawkMapPositionY==0x3020);
    printf("Original movement-step, direction, occupancy and roaming sequence: %u checks\n",checks);
    return 0;
}
