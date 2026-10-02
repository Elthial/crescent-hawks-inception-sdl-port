#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks,uploads,positions;
static uint16_t resultX,resultY,lastActor,lastScreenX,lastScreenY;
static uint8_t randomValue;
static void checkAtLine(int condition,unsigned line) {
    ++checks;
    if(!condition) { fprintf(stderr,"Walking/NPC check%u failed line%u\n",checks,line); exit(1); }
}
#define check(condition) checkAtLine((condition),__LINE__)
/* Other routines in the packed-offset object are not part of these paths. */
void Map_Move_North(void) { check(0); }
void Map_Move_South(void) { check(0); }
void Map_Move_West(void) { check(0); }
void Map_Move_East(void) { check(0); }
void EGA_Upload_Animated_Tile(uint8_t *source,uint16_t destination) {
    unsigned tile=uploads%AnimatedMapTileCount;
    check(source==AnimatedMapTileFrames+tile*AnimatedMapFrameCount*AnimatedMapFrameBytes
        +AnimatedMapTileFrame*AnimatedMapFrameBytes);
    check(destination==(uint16_t)((uint16_t)(int16_t)(int8_t)AnimatedMapTileId[tile]*AnimatedMapTilePlaneBytes));
    ++uploads;
}
/* Isolated planner boundary; actual integrated planner is tested separately
 * in test_movement_step.c. Not a gameplay implementation. */
void Movement_Select_Next_Step(uint16_t actor,uint16_t x,uint16_t y,uint16_t screenX,uint16_t screenY,uint16_t alternate) {
    ++positions; lastActor=actor; lastScreenX=screenX; lastScreenY=screenY;
    check(x==RoamingMapNpcs[actor-16].destinationX && y==RoamingMapNpcs[actor-16].destinationY && alternate==TRUE);
    MovementActorPositionX=resultX; MovementActorPositionY=resultY;
    CombatantMovementDirection[actor]=2;
}
uint8_t Rand_0x00_to_0xFF(void) { return randomValue; }
static void prepareNpcs(void) {
    memset(RoamingMapNpcs,0,sizeof RoamingMapNpcs);
    memset(CombatantPackedX,0,sizeof CombatantPackedX); memset(CombatantPackedY,0,sizeof CombatantPackedY);
    memset(CombatantAnimationDirection,255,sizeof CombatantAnimationDirection);
    memset(CombatantSpriteFrame,0x55,sizeof CombatantSpriteFrame);
    memset(RoamingNpcVisibleOnScreen,0x55,sizeof RoamingNpcVisibleOnScreen);
    for(unsigned slot=0;slot<MapCharacterCount;++slot) RoamingMapNpcs[slot].movementDelay=2;
    CrescentHawkMapPositionX=0x0220; CrescentHawkMapPositionY=0x3020;
    HoldRickAtlasUntilLoungeConversation=0; positions=0; randomValue=7;
}
int main(void) {
    /* All16 original direction streams, including repeated FE5 cycles. */
    for(unsigned family=0;family<2;++family)
        for(unsigned direction=0;direction<CompassDirectionCount;++direction) {
            AnimationCursor start=family?FriendlyInfantryWalkAnimationByDirection[direction]:FriendlyMechWalkAnimationByDirection[direction];
            CombatantAnimationCursors[23]=start;
            for(unsigned frame=0;frame<12;++frame) {
                check(Advance_Combatant_Animation_Stream(23)==start.data[
                    start.offset-start.dataOffset+2+frame%4]);
                check(CombatantAnimationDirection[23]==direction);
            }
        }
    FixedTonePitDivisor=0xABCD;
    CombatantAnimationCursors[24]=FriendlyMechWalkAnimationByDirection[4];
    check(Advance_Combatant_Animation_Stream(24)==8 && FixedTonePitDivisor==0xAB04);
    uint8_t wrapped[]={0xFC,0xFD,3,10,0xFE,0xFF,20};
    CombatantAnimationCursors[0]=(AnimationCursor){wrapped,0xFFFE,0xFFFE};
    check(Advance_Combatant_Animation_Stream(0)==10 && CombatantAnimationDirection[0]==3);
    check(CombatantAnimationCursors[0].offset==2);
    /* FE's operand is NOT consumed; signed-1 moves forward to20. */
    check(Advance_Combatant_Animation_Stream(0)==20 && CombatantAnimationCursors[0].offset==5);

    memset(Mechs,255,sizeof Mechs); Mechs[1].name[0]='L';
    memset(CombatantAnimationDirection,255,sizeof CombatantAnimationDirection);
    memset(CombatantSpriteFrame,0x55,sizeof CombatantSpriteFrame);
    Update_Friendly_Movement_Animations(Command_MoveNorth);
    check(CombatantSpriteFrame[1]==0 && CombatantSpriteFrame[0]==0x55);
    for(unsigned actor=4;actor<12;++actor) check(CombatantSpriteFrame[actor]==24 && CombatantAnimationDirection[actor]==0);
    Update_Friendly_Movement_Animations(Command_MoveNorth);
    check(CombatantSpriteFrame[1]==1 && CombatantSpriteFrame[4]==25);
    Update_Friendly_Movement_Animations(Command_MoveEast);
    check(CombatantSpriteFrame[1]==6 && CombatantSpriteFrame[4]==20);
    AnimationCursor before=CombatantAnimationCursors[4];
    Update_Friendly_Movement_Animations(Command_Pause);
    check(CombatantAnimationCursors[4].offset==before.offset);

    TilesetId=Tileset_Destruct; AnimatedMapTileFrame=2; uploads=0;
    Update_Animated_Map_Tiles(); check(uploads==0 && AnimatedMapTileFrame==2);
    TilesetId=Tileset_BattleTech; AnimatedMapTileFrame=0;
    for(unsigned call=0;call<4;++call) { Update_Animated_Map_Tiles(); check(AnimatedMapTileFrame==(call+1)%3); }
    check(uploads==40);
    uint8_t original=AnimatedMapTileId[0]; AnimatedMapTileId[0]=0x80;
    AnimatedMapTileFrame=0xFFFF; Update_Animated_Map_Tiles();
    check(AnimatedMapTileFrame==0); AnimatedMapTileId[0]=original;

    prepareNpcs(); RoamingMapNpcs[0].movementDelay=1; HoldRickAtlasUntilLoungeConversation=1;
    Update_Roaming_Map_Npcs();
    check(RoamingMapNpcs[0].movementDelay==254 && positions==0 && !RoamingNpcVisibleOnScreen[0]);
    prepareNpcs(); RoamingMapNpcs[1].movementDelay=1; RoamingMapNpcs[1].waypointPair=0x34;
    MapInteractablePositionX[3]=0x0222; MapInteractablePositionY[3]=0x3022;
    Update_Roaming_Map_Npcs();
    check(CombatantPackedX[17]==0x0222 && CombatantPackedY[17]==0x3022 && positions==0);
    check(CrescentHawkMapPositionX==0x0220 && CrescentHawkMapPositionY==0x3020);
    prepareNpcs(); RoamingMapNpcs[0].movementDelay=0;
    CombatantPackedX[16]=0x0221; CombatantPackedY[16]=0x3022;
    RoamingMapNpcs[0].destinationX=0x0225; RoamingMapNpcs[0].destinationY=0x3025;
    resultX=0x0222; resultY=0x3022;
    Update_Roaming_Map_Npcs();
    check(positions==1 && lastActor==16 && lastScreenX==27 && lastScreenY==14);
    check(CombatantPackedX[16]==resultX && CombatantPackedY[16]==resultY && RoamingNpcVisibleOnScreen[0]);
    check(CombatantSpriteFrame[16]==20 && CombatantAnimationDirection[16]==2);
    check(CrescentHawkMapPositionX==0x0220 && CrescentHawkMapPositionY==0x3020);

    /* Strict boundary rejection and camera scratch restoration. */
    static const uint16_t edgeX[]={0x0210,0x023F,0x0220,0x0220};
    static const uint16_t edgeY[]={0x3020,0x3020,0x3010,0x303F};
    for(unsigned edge=0;edge<4;++edge) {
        prepareNpcs(); RoamingMapNpcs[0].movementDelay=0;
        CombatantPackedX[16]=edgeX[edge]; CombatantPackedY[16]=edgeY[edge];
        Update_Roaming_Map_Npcs();
        check(positions==0 && !RoamingNpcVisibleOnScreen[0]);
        check(CrescentHawkMapPositionX==0x0220 && CrescentHawkMapPositionY==0x3020);
    }
    prepareNpcs(); CrescentHawkMapPositionX=0x027F; CrescentHawkMapPositionY=0x307F;
    RoamingMapNpcs[0].movementDelay=0; CombatantPackedX[16]=0x0300; CombatantPackedY[16]=0x4000;
    resultX=0x0301; resultY=0x4000; Update_Roaming_Map_Npcs();
    check(positions==1 && lastScreenX==27 && lastScreenY==13);
    check(CrescentHawkMapPositionX==0x027F && CrescentHawkMapPositionY==0x307F);

    /* Arrival clears world positions and moves low waypoint to high nibble. */
    prepareNpcs(); RoamingMapNpcs[0].movementDelay=0;
    CombatantPackedX[16]=0x0221; CombatantPackedY[16]=0x3022;
    RoamingMapNpcs[0].destinationX=resultX=0x0222; RoamingMapNpcs[0].destinationY=resultY=0x3022;
    RoamingMapNpcs[0].waypointPair=0x34; RoamingNpcWaypointLink[4]=6;
    MapInteractablePositionX[4]=resultX; MapInteractablePositionY[4]=resultY;
    MapInteractablePositionX[6]=0x0228; MapInteractablePositionY[6]=0x3028;
    randomValue=255; Update_Roaming_Map_Npcs();
    check(RoamingMapNpcs[0].movementDelay==31 && RoamingMapNpcs[0].waypointPair==0x46);
    check(CombatantPackedX[16]==0 && CombatantPackedY[16]==0 && RoamingNpcVisibleOnScreen[0]);
    check(RoamingMapNpcs[0].destinationX==0x0228 && RoamingMapNpcs[0].destinationY==0x3028);
    RoamingMapNpcs[0].movementDelay=1; Update_Roaming_Map_Npcs();
    check(CombatantPackedX[16]==0x0222 && CombatantPackedY[16]==0x3022);
    /* Preserve zero-delay arrival: no delayed branch remains to respawn it. */
    RoamingMapNpcs[0].movementDelay=0;
    RoamingMapNpcs[0].destinationX=resultX; RoamingMapNpcs[0].destinationY=resultY;
    randomValue=0; positions=0; Update_Roaming_Map_Npcs();
    check(positions==1 && RoamingMapNpcs[0].movementDelay==0 && CombatantPackedX[16]==0);
    Update_Roaming_Map_Npcs();
    check(positions==1 && !RoamingNpcVisibleOnScreen[0] && CombatantPackedX[16]==0);
    /* Sol: exercise all152 original finite fire/kick/personnel streams using
     * the REAL0800:1732 interpreter. FF waits forever and must not be called.
     * Source cursor tables are copied, never advanced in place. */
    AnimationCursor *attackTables[]={LocustFireAnimationStreams,CommandoFireAnimationStreams,
        LocustKickAnimationStreams,CommandoKickAnimationStreams,PersonnelAttackAnimationStreams};
    const size_t tableEntries[]={CompassDirectionCount,CompassDirectionCount,
        CompassDirectionCount,CompassDirectionCount,PersonnelAttackWeaponCount*CompassDirectionCount};
    unsigned attackFrames=0;
    for(unsigned table=0;table<5;++table) for(size_t entry=0;entry<tableEntries[table];++entry) {
        AnimationCursor initialAttackCursor=attackTables[table][entry];
        CombatantAnimationCursors[0]=initialAttackCursor;
        unsigned frames=0;
        while(CombatantAnimationCursors[0].data[(uint16_t)(CombatantAnimationCursors[0].offset-initialAttackCursor.dataOffset)]!=AnimationToken_Wait) {
            unsigned index=(uint16_t)(CombatantAnimationCursors[0].offset-initialAttackCursor.dataOffset);
            check(index<CombatAttackBytecodeBytes && frames<8);
            uint8_t expected=AttackAnimationBytecode[index];
            check(expected<AnimationToken_SetDirection);
            check(Advance_Combatant_Animation_Stream(0)==expected);
            ++frames; ++attackFrames;
        }
        check(frames>=1);
        check(attackTables[table][entry].offset==initialAttackCursor.offset);
    }
    /* Missile six-frame and target four-frame FE loops have no end token. */
    for(unsigned direction=0;direction<CompassDirectionCount;++direction) {
        AnimationCursor stream=MissileAnimationStreams[direction];
        CombatantAnimationCursors[EffectAnimationCursorId]=stream;
        for(unsigned tick=0;tick<60;++tick)
            check(Advance_Combatant_Animation_Stream(EffectAnimationCursorId)==
                AttackAnimationBytecode[stream.offset-stream.dataOffset+tick%6]);
    }
    CombatantAnimationCursors[0]=CombatTargetImpactStream;
    for(unsigned tick=0;tick<40;++tick)
        check(Advance_Combatant_Animation_Stream(0)==TargetImpactBytecode[tick%4]);
    CombatantAnimationCursors[EffectAnimationCursorId]=CombatProjectileImpactStream;
    for(unsigned tick=0;tick<3;++tick)
        check(Advance_Combatant_Animation_Stream(EffectAnimationCursorId)==ProjectileImpactBytecode[tick]);
    check(CombatantAnimationCursors[EffectAnimationCursorId].offset==0x41DB);
    check(LocustFireAnimationStreams[1].offset==LocustFireAnimationStreams[2].offset);
    uint16_t sharedIndex=(uint16_t)(LocustFireAnimationStreams[1].offset-CombatAttackBytecodeNativeOffset);
    uint8_t originalFrame=AttackAnimationBytecode[sharedIndex];
    AttackAnimationBytecode[sharedIndex]=119;
    CombatantAnimationCursors[0]=LocustFireAnimationStreams[2];
    check(Advance_Combatant_Animation_Stream(0)==119);
    AttackAnimationBytecode[sharedIndex]=originalFrame;
    printf("Original walking/tile/NPC routines and152 attack streams (%u frames): %u checks\n",attackFrames,checks);
    return 0;
}
