#include "game.h"
#include "backend.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t background[PackedGraphicsOutputBytes];
static uint8_t mech[2][4+24*3*4],infantry[4+8*4];
void Draw_Persistent_Map_Effects(void) {} /*Isolated effects, already separately tested*/
static void check(int condition) { if(!condition) { fputs("Combat compositor pixels mismatch\n",stderr); exit(1); } }
static void solid(uint8_t *sprite,unsigned height,unsigned columns,unsigned colour) {
    memset(sprite,0,4+height*columns*4); sprite[1]=(uint8_t)(height-1); sprite[2]=(uint8_t)columns;
    for(unsigned y=0;y<height;++y) for(unsigned x=0;x<columns;++x) for(unsigned plane=0;plane<4;++plane)
        sprite[4+(y*columns+x)*4+plane]=(colour&(1u<<plane))?255:0;
}
static unsigned pixel(unsigned x,unsigned y) {
    unsigned colour=0;
    for(uint8_t plane=0;plane<4;++plane)
        if(SDLBackend_ReadEgaPlaneByte(MapViewportSegment,(uint16_t)(y*40+x/8),plane)&(0x80>>(x%8))) colour|=1u<<plane;
    return colour;
}
int main(void) {
    memset(CombatantPackedX,255,sizeof CombatantPackedX); memset(CombatantPackedY,255,sizeof CombatantPackedY);
    memset(CombatantSpriteFrame,0,sizeof CombatantSpriteFrame); memset(CombatantSpriteFamilyOffset,0,sizeof CombatantSpriteFamilyOffset);
    CrescentHawkMapPositionX=0x0220; CrescentHawkMapPositionY=0x3020; CachedMapOriginIndex=0; DrawJailMissionParkedMechs=FALSE;
    CombatantPackedX[0]=0x0220; CombatantPackedY[0]=0x3022; CombatantSpriteFrame[0]=1;
    CombatantPackedX[12]=0x0221; CombatantPackedY[12]=0x3020; CombatantSpriteFrame[12]=2;
    CombatantPackedX[4]=0x0220; CombatantPackedY[4]=0x3020; CombatantSpriteFrame[4]=3;
    solid(mech[0],24,3,4); solid(mech[1],24,3,7); solid(infantry,8,1,3);
    CombatSpritePointers[1]=mech[0]; CombatSpritePointers[2]=mech[1]; CombatSpritePointers[3]=infantry;
    for(unsigned clipped=0;clipped<2;++clipped) {
        memset(background,0x99,sizeof background); memset(CombatMap,0x2F,sizeof CombatMap);
        InsideStarLeagueCache=(uint8_t)!clipped;
        SDLBackend_EgaRasterOperation=EgaRaster_Replace; SDLBackend_EgaMapMask=15;
        SDLBackend_EgaRotateCount=SDLBackend_EgaEnableSetReset=SDLBackend_EgaSetReset=0;
        DrawCall_Image_To_VGA_Memory(background,MapViewportSegment);
        Draw_Menu_MultiSelect();
        for(unsigned y=0;y<200;++y) for(unsigned x=0;x<320;++x) {
            unsigned expected=9;
            if(x>=208 && x<216 && y>=96 && y<96+(clipped?4u:8u)) expected=3; /*Infantry first*/
            if(x>=208 && x<232 && y>=80 && y<80+(clipped?8u:24u)) expected=7; /*Enemy behind*/
            if(x>=200 && x<224 && y>=96 && y<96+(clipped?8u:24u)) expected=4; /*Lower anchor last*/
            check(pixel(x,y)==expected);
        }
        check(mech[0][1]==23 && mech[1][1]==23 && infantry[1]==7);
    }
    puts("Original combat compositor SDL pixels passed"); return 0;
}
