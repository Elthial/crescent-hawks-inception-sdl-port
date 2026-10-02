#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t rolls[256];
static unsigned count,cursor,diceCalls,anchorCalls,originCalls;
static uint16_t originColumn=10;
static void verify(int ok,unsigned line) {
    if (!ok) { fprintf(stderr,"Random encounter mismatch line%u RNG%u/%u\n",line,cursor,count); exit(1); }
}
#define check(x) verify(!!(x),__LINE__)
uint8_t Rand_0x00_to_0xFF(void) { check(cursor<count); return rolls[cursor++]; }
uint16_t Roll2D6(void) { ++diceCalls; return (uint16_t)(diceCalls+5); }
void PosXY_OffsetGrid(uint16_t x,uint16_t y) {
    check(x==CrescentHawkMapPositionX && y==CrescentHawkMapPositionY); ++anchorCalls;
}
void Update_Cached_Map_Origin(void) {
    ++originCalls; CachedMapOriginColumn=originColumn; CachedMapOriginRowOffset=240; CachedMapOriginIndex=(uint16_t)(240+originColumn);
}
static void push(unsigned value) { check(count<sizeof rolls); rolls[count++]=(uint8_t)value; }
static void prepare(unsigned negativeX,unsigned negativeY) {
    cursor=count=diceCalls=anchorCalls=originCalls=0;
    originColumn=10;
    memset(Characters,0xA5,sizeof Characters);
    memset(Mechs,0xA5,sizeof WorldMapState.fields.mechs);
    memset(MapRuntime.bytes,40,sizeof MapRuntime.bytes);
    CrescentHawkMapPositionX=0x0520; CrescentHawkMapPositionY=0x6030;
    BlockingTileCodeThreshold=64;
    for (unsigned actor=0;actor<AllCombatantCount;++actor) {
        CombatantActive[actor]=0x1234;
        CombatantPackedX[actor]=0x1234; CombatantPackedY[actor]=0x1234;
        CombatantSpriteFrame[actor]=0xA5; CombatantSpriteFamilyOffset[actor]=0xA5;
    }
    push(0); push(negativeX!=0); push(0); push(negativeY!=0);
}
static void finish(void) {
    Generate_Random_Encounter_Enemies();
    check(cursor==count && anchorCalls==1 && originCalls==1);
    for (unsigned actor=0;actor<12;++actor)
        check(CombatantActive[actor]==0x1234 && CombatantPackedX[actor]==0x1234 && CombatantPackedY[actor]==0x1234);
}
int main(void) {
    static const uint8_t weapons[22]={10,12,9,7,0,8,1,3,5,7,7,8,6,4,3,2,0,0,8,9,13,11};
    prepare(0,0);
    for (unsigned i=0;i<12;++i) push(0);
    finish(); check(diceCalls==0);
    for (unsigned actor=12;actor<24;++actor)
        check(!CombatantActive[actor] && CombatantPackedX[actor]==65535 && CombatantPackedY[actor]==65535);
    for (unsigned i=8;i<16;++i) check(Characters[i].name==255 && Characters[i].body==0xA5);
    for (unsigned i=4;i<8;++i) check(Mechs[i].name[0]==255 && Mechs[i].name[1]==0xA5);
    for (unsigned sum=0;sum<22;++sum) for (unsigned signs=0;signs<4;++signs) {
        prepare(signs&1,signs&2); push(1);
        unsigned remaining=sum;
        for (unsigned i=0;i<7;++i) { unsigned value=remaining>3?3:remaining; push(value); remaining-=value; }
        for (unsigned i=0;i<7;++i) push(i);
        push(7);
        for (unsigned i=0;i<11;++i) push(0);
        finish();
        Character *person=&Characters[8];
        check(person->name==1 && person->mechAssignment==8 && person->weapon==weapons[sum]);
        check(person->body==6 && person->health==60 && person->dexterity==7 && person->charisma==0xA5);
        check(person->armourType==3 && person->armourValue==17 && diceCalls==4);
        for (unsigned i=0;i<7;++i) check(((uint8_t *)person)[4+i]==(i&3));
        check(CombatantActive[16]==1 && CombatantSpriteFrame[16]==16);
        check(CombatantPackedX[16]==(signs&1?0x0516:0x052A));
        check(CombatantPackedY[16]==(signs&2?0x6026:0x603A));
    }
    for (unsigned type=0;type<3;++type) for (unsigned signs=0;signs<4;++signs) {
        prepare(signs&1,signs&2);
        for (unsigned i=0;i<8;++i) push(0);
        push(1); push(type);
        for (unsigned i=0;i<3;++i) push(0);
        finish(); check(diceCalls==0);
        check(!memcmp(&Mechs[4],&MechRefs[type],sizeof(Mech)));
        check(CombatantActive[12]==1 && CombatantSpriteFrame[12]==0);
        check(CombatantSpriteFamilyOffset[12]==(type?146:0));
        check(CombatantPackedX[12]==(signs&1?0x0517:0x052B));
        check(CombatantPackedY[12]==(signs&2?0x6026:0x603A));
    }
    /* The chance roll is consumed even when the paired friendly Mech is absent. */
    prepare(0,0);
    for (unsigned i=0;i<8;++i) push(0);
    for (unsigned i=0;i<4;++i) { Mechs[i].name[0]=255; push(1); }
    finish(); check(diceCalls==0);
    for (unsigned i=4;i<8;++i) check(Mechs[i].name[0]==255);
    /* Primary525 and adjacent524 are initially blocked. The native search
     * advances one unit to526/527, not a fresh random spawn. */
    prepare(0,0); CombatMap[525]=0; CombatMap[524]=0;
    for (unsigned i=0;i<8;++i) push(0);
    push(1); push(0); for (unsigned i=0;i<3;++i) push(0);
    finish();
    check(CombatantActive[12]==1 && CombatantPackedX[12]==0x052C);
    /* BUG-005: anchor column23 accepts neighbouring cell in the next row.
     * All other tiles blocked makes an accidental "fix" observable. */
    prepare(0,0); originColumn=11; CrescentHawkMapPositionX=0x0521;
    memset(CombatMap,0,MapCacheTileCount); CombatMap[527]=40; CombatMap[528]=40;
    for (unsigned i=0;i<8;++i) push(0);
    push(1); push(0); for (unsigned i=0;i<3;++i) push(0);
    finish();
    check(CombatantActive[12]==1 && CombatantPackedX[12]==0x052C);
    /* A passable anchor outside the horizontal cache is discarded AFTER
     * the terrain search, without retrying or regenerating a Mech. */
    prepare(0,0); originColumn=12;
    for (unsigned i=0;i<8;++i) push(0);
    push(1); push(0); for (unsigned i=0;i<3;++i) push(0);
    finish();
    check(Mechs[4].name[0]==255 && !CombatantActive[12] && CombatantPackedX[12]==65535);
    puts("Whole native random encounter: RNG order, infantry rolls, templates and signed origins passed.");
    return 0;
}
