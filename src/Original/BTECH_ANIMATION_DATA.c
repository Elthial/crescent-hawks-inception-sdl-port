#include "game.h"

/* Distinct original311A..3159 tables, not aliases of equal328A path tables. */
int8_t MovementDirectionSearchOffsets[2*CompassDirectionCount]=
    {7,-6,5,-4,3,-2,1,0,-7,6,-5,4,-3,2,-1,0};
int16_t MovementDirectionStepX[CompassDirectionCount]={0,1,1,1,0,-1,-1,-1};
int16_t MovementDirectionStepY[CompassDirectionCount]={-1,-1,0,1,1,1,0,-1};
int16_t MovementPackedBoundaryX[CompassDirectionCount]={0,128,128,128,0,-128,-128,-128};
int16_t MovementPackedBoundaryY[CompassDirectionCount]={-3968,-3968,0,3968,3968,3968,0,-3968};
uint16_t MovementSearchBankTimer=100,MovementSearchBank; /*DS315A/315C*/
int16_t CombatChosenStepX,CombatChosenStepY;
uint16_t CombatDestinationBlocked;
uint8_t AnimatedMapTileFrames[AnimatedMapTileBytes]; /*3092:D582..E481*/

/* EXE-owned original2FE8:0270..02EF walk bytecode. FD facing, four frame
 * bytes, FE5 looping back to the first frame, not to FD. Mutable originals. */
uint8_t WalkAnimationStreams[2*CompassDirectionCount*WalkAnimationStreamBytes]={
    253,0,0,1,2,3,254,5, 253,1,4,5,6,7,254,5,
    253,2,6,7,4,5,254,5, 253,3,5,6,7,4,254,5,
    253,4,8,9,10,11,254,5, 253,5,12,13,14,15,254,5,
    253,6,15,12,13,14,254,5, 253,7,14,15,12,13,254,5,
    253,0,24,25,26,27,254,5, 253,1,20,21,22,23,254,5,
    253,2,20,21,22,23,254,5, 253,3,20,21,22,23,254,5,
    253,4,16,17,18,19,254,5, 253,5,28,29,30,31,254,5,
    253,6,28,29,30,31,254,5, 253,7,28,29,30,31,254,5
};
#define WALK_ADDRESS(offset) {WalkAnimationStreams,WalkAnimationStreamNativeOffset,offset}
/* DS3EDB initial FAR cursors: south-facing mechs0290, infantry02D0. */
AnimationCursor CombatantAnimationCursors[CombatantAnimationCursorCount]={
    WALK_ADDRESS(0x290),WALK_ADDRESS(0x290),WALK_ADDRESS(0x290),WALK_ADDRESS(0x290),
    WALK_ADDRESS(0x2D0),WALK_ADDRESS(0x2D0),WALK_ADDRESS(0x2D0),WALK_ADDRESS(0x2D0),
    WALK_ADDRESS(0x2D0),WALK_ADDRESS(0x2D0),WALK_ADDRESS(0x2D0),WALK_ADDRESS(0x2D0),
    WALK_ADDRESS(0x290),WALK_ADDRESS(0x290),WALK_ADDRESS(0x290),WALK_ADDRESS(0x290),
    WALK_ADDRESS(0x2D0),WALK_ADDRESS(0x2D0),WALK_ADDRESS(0x2D0),WALK_ADDRESS(0x2D0),
    WALK_ADDRESS(0x2D0),WALK_ADDRESS(0x2D0),WALK_ADDRESS(0x2D0),WALK_ADDRESS(0x2D0),
    WALK_ADDRESS(0x290)
};
AnimationCursor FriendlyMechWalkAnimationByDirection[CompassDirectionCount]={
    WALK_ADDRESS(0x270),WALK_ADDRESS(0x278),WALK_ADDRESS(0x280),WALK_ADDRESS(0x288),
    WALK_ADDRESS(0x290),WALK_ADDRESS(0x298),WALK_ADDRESS(0x2A0),WALK_ADDRESS(0x2A8)};
AnimationCursor FriendlyInfantryWalkAnimationByDirection[CompassDirectionCount]={
    WALK_ADDRESS(0x2B0),WALK_ADDRESS(0x2B8),WALK_ADDRESS(0x2C0),WALK_ADDRESS(0x2C8),
    WALK_ADDRESS(0x2D0),WALK_ADDRESS(0x2D8),WALK_ADDRESS(0x2E0),WALK_ADDRESS(0x2E8)};
#undef WALK_ADDRESS
uint16_t AnimatedMapTileFrame; /*DS3EDB:5800*/
uint8_t AnimatedMapTileId[AnimatedMapTileCount]={87,88,89,90,91,105,106,107,111,113}; /*04B0*/
uint8_t CombatantMovementDirection[AllCombatantCount]; /*3092:3920*/
uint8_t CombatantVisibleOnScreen[AllCombatantCount]; /*3092:42F6; NPC view aliases first16*/
uint16_t MovementActorPositionX,MovementActorPositionY; /*3092:E486/E488*/
uint16_t FixedTonePitDivisor; /*3092:3984*/
