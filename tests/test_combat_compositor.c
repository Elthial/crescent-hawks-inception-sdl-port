#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t sprites[CombatSpriteCount][4];
static unsigned effects,calls;
static struct { unsigned sprite; int16_t x,y; uint8_t height; } drawn[32];
static void checkAtLine(int condition,unsigned line) {
    if(!condition) { fprintf(stderr,"Combat compositor mismatch line%u\n",line); exit(1); }
}
#define check(condition) checkAtLine((condition),__LINE__)
void Draw_Persistent_Map_Effects(void) { check(!calls); ++effects; }
void DrawCall_EGA_CharacterPos(EgaMemoryAddress destination,uint8_t *sprite,int16_t x,int16_t y) {
    check(effects==1 && calls<32 && destination.offset==0 && destination.segment==MapViewportSegment);
    unsigned id=0;
    while(id<CombatSpriteCount && sprite!=sprites[id]) ++id;
    check(id<CombatSpriteCount && sprite==sprites[id]);
    drawn[calls].sprite=id; drawn[calls].x=x; drawn[calls].y=y;
    drawn[calls++].height=sprite[SpriteHeightMinusOneByte];
}
static void reset(void) {
    memset(CombatMap,0,sizeof CombatMap);
    memset(CombatantPackedX,255,sizeof CombatantPackedX);
    memset(CombatantPackedY,255,sizeof CombatantPackedY);
    memset(CombatantActive,0,sizeof CombatantActive);
    memset(CombatantVisibleOnScreen,0x55,sizeof CombatantVisibleOnScreen);
    memset(TerrainOverlapRows,0x55,sizeof TerrainOverlapRows);
    memset(MapTileUnderCombatant,0x55,sizeof MapTileUnderCombatant);
    memset(CombatMechYParityAdjustment,0x55,sizeof CombatMechYParityAdjustment);
    memset(CombatMechPreviousMapRowTile,0x55,sizeof CombatMechPreviousMapRowTile);
    for(unsigned id=0;id<CombatSpriteCount;++id) {
        CombatSpritePointers[id]=sprites[id]; sprites[id][SpriteHeightMinusOneByte]=31;
    }
    for(unsigned id=0;id<AllCombatantCount;++id) { CombatantSpriteFrame[id]=(uint8_t)id; CombatantSpriteFamilyOffset[id]=0; }
    CrescentHawkMapPositionX=0x0220; CrescentHawkMapPositionY=0x3020;
    CachedMapOriginIndex=0; InsideStarLeagueCache=FALSE; DrawJailMissionParkedMechs=FALSE;
    effects=calls=0;
}
static void unit(uint16_t id,int16_t dx,int16_t dy) {
    CombatantPackedX[id]=(uint16_t)(CrescentHawkMapPositionX+dx);
    CombatantPackedY[id]=(uint16_t)(CrescentHawkMapPositionY+dy);
}
static void draw(void) {
    Draw_Menu_MultiSelect(); check(effects==1);
    for(unsigned id=0;id<CombatSpriteCount;++id) check(sprites[id][SpriteHeightMinusOneByte]==31);
}
int main(void) {
    reset(); draw(); check(!calls);
    for(unsigned id=0;id<AllCombatantCount;++id) check(!CombatantVisibleOnScreen[id]);
    reset(); memset(CombatMap,0x2F,sizeof CombatMap);
    unit(4,0,0); unit(16,1,1); unit(0,0,2); unit(12,1,-2);
    draw(); check(calls==4);
    check(drawn[0].sprite==4 && drawn[0].x==208 && drawn[0].y==96 && drawn[0].height==27);
    check(drawn[1].sprite==16 && drawn[1].x==216 && drawn[1].y==104 && drawn[1].height==27);
    check(drawn[2].sprite==12 && drawn[2].x==208 && drawn[2].y==64 && drawn[2].height==15);
    check(drawn[3].sprite==0 && drawn[3].x==200 && drawn[3].y==96 && drawn[3].height==15);
    check(MapTileUnderCombatant[0]==0x2F && CombatMechPreviousMapRowTile[0]==0x2F && TerrainOverlapRows[0]==16);
    check(CombatantScreenPixelX[0]==208 && CombatantScreenPixelY[0]==112);
    check(CombatantVisibleOnScreen[16] && RoamingNpcVisibleOnScreen[0]);
    check(sizeof RoamingNpcVisibleOnScreen==MapCharacterCount && &RoamingNpcVisibleOnScreen[0]==CombatantVisibleOnScreen);
    check(!CombatantActive[0] && !CombatantActive[16]); /*No added active filter*/
    reset(); unit(0,0,-1); CombatMap[126]=0x14; CombatMap[102]=0x33;
    draw(); check(calls==1 && CombatMechYParityAdjustment[0]==1 && MapTileUnderCombatant[0]==0x14);
    check(CombatMechPreviousMapRowTile[0]==0x33 && TerrainOverlapRows[0]==8 && drawn[0].height==23);
    reset(); memset(CombatMap,0x2F,sizeof CombatMap); InsideStarLeagueCache=TRUE; unit(4,0,0); unit(0,0,0);
    draw(); check(calls==2 && drawn[0].height==31 && drawn[1].height==31 && !TerrainOverlapRows[0]);
    reset(); unit(4,0,0); unit(0,0,0);
    CombatantSpriteFrame[4]=200; CombatantSpriteFamilyOffset[4]=100;
    CombatantSpriteFrame[0]=200; CombatantSpriteFamilyOffset[0]=100;
    draw(); check(calls==2 && drawn[0].sprite==300 && drawn[1].sprite==44); /*WORD infantry/BYTE mech sum*/
    reset(); for(unsigned side=0;side<2;++side) for(unsigned slot=0;slot<4;++slot)
        unit((uint16_t)(side*12+slot),(int16_t)slot,(int16_t)(7-(side*4+slot)));
    draw(); check(calls==8);
    for(unsigned i=0;i<8;++i) {
        unsigned original=7-i,id=original<4?original:original+8;
        check(drawn[i].sprite==id && drawn[i].y==80+(int16_t)i*8);
        check(drawn[i].x==200+(int16_t)(original%4)*8);
    }
    reset(); unit(4,-13,0); unit(5,-14,0); unit(0,-15,14); unit(1,-16,14); unit(12,0,15);
    draw(); check(calls==2 && CombatantVisibleOnScreen[4] && !CombatantVisibleOnScreen[5]);
    check(CombatantVisibleOnScreen[0] && !CombatantVisibleOnScreen[1] && !CombatantVisibleOnScreen[12]);
    check(CombatMechYParityAdjustment[1]==0 && CombatMechPreviousMapRowTile[12]==0);
    reset(); CrescentHawkMapPositionX=0x027F; CombatantPackedX[4]=0x0300; CombatantPackedY[4]=0x3020;
    draw(); check(calls==1 && drawn[0].x==216 && CombatantVisibleOnScreen[4]); /*Neighbour page projects by128*/
    reset(); DrawJailMissionParkedMechs=TRUE; CrescentHawkMapPositionX=JailParkedMechFirstX; CrescentHawkMapPositionY=JailParkedMechY;
    draw(); check(calls==4);
    for(unsigned i=0;i<4;++i) check(drawn[i].sprite==MECH_Sprite_COMMANDO && drawn[i].x==208+(int16_t)i*32 && drawn[i].y==96);
    puts("Original combat compositor passed"); return 0;
}
