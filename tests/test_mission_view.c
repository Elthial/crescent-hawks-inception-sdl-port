#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned stage;
static uint16_t expectedX,expectedY;
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"Mission view mismatch line%u\n",line); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
/* Real original body; redraw boundaries inspect camera state and call order. */
void PosXY_OffsetGrid(uint16_t x,uint16_t y) {
    check(stage++==0 && x==expectedX && y==expectedY && CrescentHawkMapPositionX==x && CrescentHawkMapPositionY==y);
}
void Copy_Data_To_GraphicsMemory(void) { check(stage++==1); }
void Draw_Infantry_And_Mechs(void) { check(stage++==2); }
void EGA_DrawBox_Wrapper(void) { check(stage++==3); }
int main(void)
{
    uint16_t originalX[AllCombatantCount],originalY[AllCombatantCount];
    Character originalCharacters[CharacterRecordCount];
    for(unsigned actor=0;actor<AllCombatantCount;++actor) {
        CombatantPackedX[actor]=(uint16_t)(0x0300+actor*17);
        CombatantPackedY[actor]=(uint16_t)(0xA000+actor*19);
    }
    memset(Characters,0x5A,sizeof Characters);
    for(unsigned assignment=0;assignment<256;++assignment) for(unsigned y=0;y<65536;++y) {
        Characters[0].mechAssignment=(uint8_t)assignment;
        CombatantPackedY[0]=(uint16_t)y; CombatantPackedY[4]=(uint16_t)(y^0xFFFF);
        CombatantPackedX[0]=(uint16_t)(y*7); CombatantPackedX[4]=(uint16_t)(y*13);
        memcpy(originalX,CombatantPackedX,sizeof originalX); memcpy(originalY,CombatantPackedY,sizeof originalY);
        memcpy(originalCharacters,Characters,sizeof originalCharacters);
        if(assignment==8) { expectedX=originalX[4]; expectedY=originalY[4]; }
        else {
            /* Independent packed-page oracle: bit7 means next map row;
             * bits0..6 survive, row/page increment wraps at WORD width. */
            unsigned shifted=(y+2)&0xFFFF;
            expectedX=originalX[0];
            expectedY=(uint16_t)((shifted&0x80)?((shifted&0xFF7F)+0x1000):shifted);
        }
        stage=0; Restore_Mission_Map_View_After_Combat(); check(stage==4);
        check(!memcmp(originalX,CombatantPackedX,sizeof originalX) && !memcmp(originalY,CombatantPackedY,sizeof originalY));
        check(!memcmp(originalCharacters,Characters,sizeof originalCharacters));
    }
    return 0;
}
