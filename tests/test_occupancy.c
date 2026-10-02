/* Sol: Original occupancy, packed movement, abs, verbosity and speed bodies.
 * Test-only drawing/sound/effect/key/retrace adapters, not gameplay proof. */
#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks, menus, draws, sounds, effects, waits, keyReads, messages;
static uint16_t waitCount, expectedX, expectedY;
static uint8_t *rendered[8];
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Occupancy check %u failed\n",checks); exit(1); }
}
void Menu_Memory_Variables(uint16_t layout) { check(layout==4); ++menus; }
void Draw_Top_Graphic_Sidebar(void) { check(menus==draws+1); ++draws; }
void Display_Text_From_Memory(uint8_t *text)
{
    check(messages<8); rendered[messages++]=text;
}
void Play_Sound_If_Enabled(uint16_t id) { check(id==Sound_SquishedByMech); ++sounds; }
void Register_Persistent_Map_Effect(uint16_t id,uint16_t x,uint16_t y)
{
    check(id==Sprite_Impact_Small && x==expectedX && y==expectedY); ++effects;
}
void Wait_For_N_Vertical_Retraces(uint16_t count) { ++waits; waitCount=count; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keyReads; return 0; }
static void prepare(void)
{
    memset(Characters,0,sizeof Characters);
    memset(CombatantActive,0,sizeof CombatantActive);
    memset(CombatantSpriteFrame,17,sizeof CombatantSpriteFrame);
    memset(CombatantSpriteFamilyOffset,34,sizeof CombatantSpriteFamilyOffset);
    for (unsigned id=0;id<AllCombatantCount;++id) {
        CombatantPackedX[id]=CombatantPackedY[id]=0xFFFF;
        CombatantCasualtyFlags[id]=0x1234;
    }
    CrescentHawkMapPositionX=0x020A; CrescentHawkMapPositionY=0x300A;
    expectedX=CrescentHawkMapPositionX; expectedY=CrescentHawkMapPositionY;
    menus=draws=sounds=effects=waits=keyReads=messages=0;
    MainCharactersAlive=1; CombatSpeedSetting=2; CombatMessageVerbosity=2;
}
static uint16_t occupancy(uint16_t id,uint16_t crush)
{
    uint16_t blocked=Combat_Check_Occupancy_And_Crush(id,123,-456,crush);
    check(CrescentHawkMapPositionX==expectedX && CrescentHawkMapPositionY==expectedY);
    return blocked;
}
int main(void)
{
    check(CombatSpeedSetting==2 && MainCharactersAlive==1);
    for (uint16_t id=0;id<AllCombatantCount;++id) {
        prepare(); check(occupancy(id,1)==0 && sounds==0);
    }
    prepare(); CombatantPackedX[5]=expectedX; CombatantPackedY[5]=expectedY;
    check(occupancy(4,0)==1); /* Inactive infantry still blocks infantry. */
    prepare(); CombatantPackedX[4]=expectedX; CombatantPackedY[4]=expectedY;
    check(occupancy(4,0)==0); /* Actor itself is excluded. */
    prepare(); CombatantPackedX[23]=expectedX; CombatantPackedY[23]=expectedY;
    check(occupancy(4,0)==1); /* Enemy exact-position alias16..23. */
    for (int delta=-2;delta<=2;++delta) {
        prepare(); CombatantPackedX[12]=(uint16_t)(expectedX+delta);
        CombatantPackedY[12]=expectedY;
        check(occupancy(4,0)==(delta>=-1 && delta<=1)); /* Inactive Mech still blocks infantry. */
        check(occupancy(0,0)==0); /* But inactive Mech does not block Mech. */
        CombatantActive[12]=1;
        check(occupancy(0,0)==1); /* Mech centres<3 apart overlap. */
    }
    prepare(); CombatantActive[12]=1; CombatantPackedX[12]=expectedX+3;
    CombatantPackedY[12]=expectedY; check(occupancy(0,0)==0);
    prepare(); CombatantActive[4]=1; CombatantPackedX[4]=expectedX;
    CombatantPackedY[4]=expectedY; check(occupancy(0,1)==1 && effects==0);
    prepare(); CombatantActive[16]=1; CombatantPackedX[16]=expectedX;
    CombatantPackedY[16]=expectedY; Characters[8].name=2; Characters[8].health=100;
    check(occupancy(0,0)==0 && Characters[8].health==100); /* Preview never crushes. */
    check(occupancy(0,1)==0 && Characters[8].health==0 && Characters[8].name==255);
    check(CombatantActive[16]==0 && CombatantSpriteFrame[16]==Sprite_Impact_Small);
    check(CombatantSpriteFamilyOffset[16]==0 && CombatantCasualtyFlags[16]==0x1234);
    check(CombatantPackedX[16]==expectedX && CombatantPackedY[16]==expectedY);
    check(MainCharactersAlive && menus==1 && draws==1 && sounds==1 && effects==1);
    check(waits==1 && waitCount==24 && keyReads==0);
    check(messages==2 && !strcmp((char *)rendered[0],"Enemy human") &&
        !strcmp((char *)rendered[1]," is squashed underfoot."));
    for (uint16_t record=0;record<PartySize;++record) {
        prepare(); uint16_t id=(uint16_t)(record+Friendly_Infantry_Combatant_Range_First);
        CombatantActive[id]=1; CombatantPackedX[id]=expectedX; CombatantPackedY[id]=expectedY;
        Characters[record].name=(uint8_t)record; Characters[record].health=100;
        check(occupancy(12,1)==0 && Characters[record].health==0);
        check(MainCharactersAlive==(record>=2) && waits==2);
        check(messages==2 && rendered[0]==CharacterNames[record]);
    }
    prepare(); CombatSpeedSetting=5; CombatMessageVerbosity=0;
    CombatantActive[16]=1; CombatantPackedX[16]=expectedX; CombatantPackedY[16]=expectedY;
    check(occupancy(0,1)==0 && waits==0 && keyReads==0 && messages==0 && effects==1);
    /* Actual speed method signed gate and low-WORD multiplication. */
    for (unsigned setting=0;setting<=UINT16_MAX;++setting) {
        CombatSpeedSetting=(uint16_t)setting; waits=keyReads=0;
        GameSpeed_RateControl();
        if ((int16_t)setting<5) check(waits==1 && keyReads==0 && waitCount==(uint16_t)(setting*12));
        else check(waits==0 && keyReads==1);
    }
    CrescentHawkMapPositionX=0x0D00; CrescentHawkMapPositionY=0x7000;
    Offset_Packed_Position(-1,-1);
    check(CrescentHawkMapPositionX==0x0C7F && CrescentHawkMapPositionY==0x607F);
    Offset_Packed_Position(1,1);
    check(CrescentHawkMapPositionX==0x0D00 && CrescentHawkMapPositionY==0x7000);
    prepare(); expectedX=0x0300; CrescentHawkMapPositionX=expectedX;
    CombatantPackedX[0]=0x027F; CombatantPackedY[0]=expectedY;
    check(occupancy(4,0)==1); /* Footprint crosses packed region boundary. */
    printf("Combat occupancy/crushing: %u checks passed\n",checks); return 0;
}
