/* Original compositor and packed-offset method; only draw/effect boundaries mocked. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks,draws,effects;
static uint8_t sprites[CombatSpriteCount][4];
typedef struct RecordedDraw { unsigned sprite,height; int16_t x,y; } RecordedDraw;
static RecordedDraw recorded[40];
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Compositor check %u failed\n",checks); exit(1); }
}
void Draw_Persistent_Map_Effects(void) { check(draws==0); ++effects; }
void DrawCall_EGA_CharacterPos(EgaMemoryAddress destination,uint8_t *sprite,int16_t x,int16_t y)
{
    check(destination.segment==MapViewportSegment && destination.offset==0);
    check(draws<40);
    unsigned id=0;
    while (id<CombatSpriteCount && sprite!=sprites[id]) ++id;
    check(id<CombatSpriteCount);
    recorded[draws++]=(RecordedDraw){id,sprite[1],x,y};
}
/* Other methods sharing the original packed-offset object must not be called. */
void Map_Move_North(void) { check(0); }
void Map_Move_South(void) { check(0); }
void Map_Move_West(void) { check(0); }
void Map_Move_East(void) { check(0); }
static void prepare(void)
{
    memset(Characters,255,sizeof Characters);
    memset(Mechs,255,sizeof Mechs);
    memset(CombatMap,0xF6,sizeof CombatMap);
    memset(CombatantActive,0x77,sizeof CombatantActive);
    memset(TerrainOverlapRows,0x55,sizeof TerrainOverlapRows);
    memset(CombatantSpriteFrame,0,sizeof CombatantSpriteFrame);
    memset(CombatantSpriteFamilyOffset,0,sizeof CombatantSpriteFamilyOffset);
    memset(CombatantPackedX,0xEE,sizeof CombatantPackedX);
    memset(CombatantPackedY,0xEE,sizeof CombatantPackedY);
    for (unsigned id=0;id<8;++id) RoamingMapNpcs[id].movementDelay=1;
    for (unsigned id=0;id<CombatSpriteCount;++id) {
        sprites[id][1]=31; CombatSpritePointers[id]=sprites[id];
    }
    InsideStarLeagueCache=0; DrawJailMissionParkedMechs=0;
    CrescentHawkMapPositionX=0x0220; CrescentHawkMapPositionY=0x3020;
    CachedMapOriginIndex=0;
    draws=effects=0;
}
int main(void)
{
    prepare();
    Characters[1].name=1; Characters[1].mechAssignment=Character_OnFoot;
    Characters[5].name=2; Characters[5].mechAssignment=Character_OnFoot;
    Characters[2].name=3; Characters[2].mechAssignment=0x80; /* native signed JL excludes */
    Characters[3].name=4; Characters[3].mechAssignment=0xFF;
    Characters[4].name=5; Characters[4].mechAssignment=0; /* aboard a mech */
    Mechs[2].name[0]='L'; Mechs[3].name[0]='C';
    CombatantSpriteFrame[5]=10; CombatantSpriteFrame[9]=11;
    CombatantSpriteFamilyOffset[5]=20; CombatantSpriteFamilyOffset[9]=20;
    CombatantSpriteFrame[2]=12; CombatantSpriteFrame[3]=13;
    CombatMap[150]=0x23; CombatMap[126]=0x06; CombatMap[198]=0x2F;
    Draw_Infantry_And_Mechs();
    check(effects==1 && draws==4 && OnFootPartyMemberCount==2);
    check(recorded[0].sprite==30 && recorded[0].x==208 && recorded[0].y==96 && recorded[0].height==27);
    check(recorded[1].sprite==31 && recorded[1].y==88 && recorded[1].height==29);
    check(recorded[2].sprite==12 && recorded[2].x==200 && recorded[2].y==64 && recorded[2].height==31);
    check(recorded[3].sprite==13 && recorded[3].y==112 && recorded[3].height==15);
    check(TerrainOverlapRows[5]==4 && TerrainOverlapRows[9]==2 && TerrainOverlapRows[2]==0 && TerrainOverlapRows[3]==16);
    check(CombatantPackedX[4]==0x0220 && CombatantPackedY[4]==0x3020);
    check(CombatantPackedY[5]==0x301F && CombatantPackedY[0]==0x301E && CombatantPackedY[1]==0x3024);
    for(unsigned id=0;id<12;++id) check(CombatantActive[id]==(id<2 || id==4 || id==5));
    check(CombatantActive[16]==0x7777 && TerrainOverlapRows[4]==0x55);
    check(CrescentHawkMapPositionX==0x0220 && CrescentHawkMapPositionY==0x3020);
    for(unsigned id=0;id<CombatSpriteCount;++id) check(sprites[id][1]==31);

    /* Same-page NPC visibility, delay gating and original draw order. */
    prepare();
    CombatantPackedX[16]=0x0220; CombatantPackedY[16]=0x3020;
    RoamingMapNpcs[0].movementDelay=0; CombatantSpriteFrame[16]=50;
    CombatMap[150]=0x23;
    Draw_Infantry_And_Mechs();
    check(draws==1 && recorded[0].sprite==50 && recorded[0].x==208 && recorded[0].y==96 && recorded[0].height==27);
    check(TerrainOverlapRows[16]==0x55 && CombatantActive[16]==0x7777);
    CombatantPackedX[16]=0x0200; draws=0;
    Draw_Infantry_And_Mechs(); check(draws==0);

    /* Raw packed-word edge fixture projects encodedX=128 to localX=0.
     * This exercises native SAR(-13)/2=-7, not ordinary valid world movement. */
    prepare();
    CrescentHawkMapPositionX=0x02FF; CrescentHawkMapPositionY=0x300C;
    CachedMapOriginIndex=100;
    CombatantPackedX[16]=0x0365;
    CombatantPackedY[16]=0x3000; RoamingMapNpcs[0].movementDelay=0;
    CombatantSpriteFrame[16]=51;
    CombatMap[94]=0x29; /* origin100 + floor(-13/2) + odd-X correction1 */
    Draw_Infantry_And_Mechs();
    check(draws==1 && recorded[0].x==0 && recorded[0].y==0 && recorded[0].height==27);
    InsideStarLeagueCache=1; draws=effects=0;
    Draw_Infantry_And_Mechs(); check(draws==1 && effects==0 && recorded[0].height==31);

    /* All four camera parities select the original mask/offset tables. */
    for(unsigned parity=0;parity<4;++parity) {
        prepare();
        CrescentHawkMapPositionX+=(uint16_t)(parity&1);
        CrescentHawkMapPositionY+=(uint16_t)(parity>>1);
        Characters[0].name=0; Characters[0].mechAssignment=Character_OnFoot;
        Mechs[0].name[0]='L'; CombatantSpriteFrame[0]=60;
        unsigned infantryCell=150+(parity&1);
        unsigned mechCell=126+(parity&1);
        CombatMap[infantryCell]=(uint8_t)(0x20|PartyInfantryOcclusionMask[0][parity]);
        CombatMap[mechCell]=(uint8_t)(0x20|(FriendlyMechOcclusionMask[0]^((parity&2)?5:0)));
        Draw_Infantry_And_Mechs();
        check(draws==2 && recorded[0].height==27 && recorded[1].height==23);
        check(sprites[0][1]==31 && sprites[60][1]==31);
    }
    /* Packed formation offsets carry across local127 without moving the anchor. */
    prepare();
    CrescentHawkMapPositionX=0x027F; CrescentHawkMapPositionY=0x307F;
    for(unsigned id=0;id<4;++id) {
        Characters[id].name=(uint8_t)id; Characters[id].mechAssignment=Character_OnFoot;
    }
    Draw_Infantry_And_Mechs();
    check(CombatantPackedX[7]==0x0300 && CombatantPackedY[6]==0x4000);
    check(CrescentHawkMapPositionX==0x027F && CrescentHawkMapPositionY==0x307F);

    /* Jail overlay draws four fixed generic humanoids without terrain edits. */
    prepare();
    DrawJailMissionParkedMechs=1;
    CrescentHawkMapPositionX=0x0D19; CrescentHawkMapPositionY=0x702C;
    Draw_Infantry_And_Mechs();
    check(draws==4);
    for(unsigned id=0;id<LanceSize;++id)
        check(recorded[id].sprite==MECH_Sprite_COMMANDO && recorded[id].x==(20+(int)id*4)*8 && recorded[id].y==96);
    printf("Original exploration compositor: %u checks\n",checks);
    return 0;
}
