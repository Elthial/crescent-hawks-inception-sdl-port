#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static char text[4096];
static unsigned choiceCount,choiceIndex,previews,waits,lines;
static uint16_t choices[8],previewIds[8];
static void checkLine(int value,unsigned line) {
    if(!value) { fprintf(stderr,"Target mismatch line%u\n",line); exit(1); }
}
#define check(value) checkLine((value),__LINE__)
/* Test-only UI boundaries; original kick, target picker, distance, range and
 * EXE-owned weapon records execute unchanged. */
void Menu_Memory_Variables(uint16_t panel) { check(panel==3 || panel==4); }
void Draw_Menu_Border(uint16_t panel) { check(panel==3); }
void Draw_Top_Graphic_Sidebar(void) {}
void Set_Text_Colour_Bright_Green(void) { TextColour=10; }
void Display_Text_From_Memory(uint8_t *message) {
    size_t used=strlen(text),length=strlen((char *)message);
    check(used+length<sizeof text); memcpy(text+used,message,length+1);
}
void Draw_Horizontal_EGA_Line(uint16_t x,uint16_t y,uint16_t right,uint16_t bottom,uint16_t colour) {
    check(x==8 && right==0x57 && (y==0x48 || y==0x50) && (bottom==0x4F || bottom==0x57) && colour==0); ++lines;
}
void Draw_EGA_Text_To_Screen(uint8_t *message,uint16_t x,uint16_t y,uint16_t foreground,uint16_t background) {
    check(x==1 && y==10 && foreground==15 && background==0);
    Display_Text_From_Memory(message);
}
void Combat_Render_Movement_Preview(uint16_t id,uint16_t recenter) {
    check(recenter==TRUE && previews<8); previewIds[previews++]=id;
}
void Move_Map_View_To_Packed_Position(uint16_t x,uint16_t y) { CrescentHawkMapPositionX=x; CrescentHawkMapPositionY=y; }
uint16_t Display_Menu_Choices_And_Check(uint16_t panel) {
    check(panel==3 && choiceIndex<choiceCount && CombatMessageMenuOptionCount==3); return choices[choiceIndex++];
}
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++waits; return 13; }
static void prepare(void) {
    memset(CombatantActive,0,sizeof CombatantActive);
    memset(CombatWeaponTarget,0xFF,sizeof CombatWeaponTarget);
    memset(CombatantPackedX,0,sizeof CombatantPackedX); memset(CombatantPackedY,0,sizeof CombatantPackedY);
    text[0]=0; choiceCount=choiceIndex=previews=waits=lines=0;
    EnemyTargetId=12; CrescentHawkMapPositionX=CrescentHawkMapPositionY=0;
    CombatantPackedX[0]=CombatantPackedY[0]=0;
}
static uint16_t packX(unsigned x) { return (uint16_t)((x/128)*256+(x%128)); }
static uint16_t packY(unsigned y) { return (uint16_t)((y/128)*4096+(y%128)); }
int main(void) {
    prepare();
    /* Across all pages/local cells, compare to independent uncompressed
     * coordinate arithmetic, including ignored carry and unrelated bits. */
    for(unsigned axis=0;axis<2048;++axis) {
        CrescentHawkMapPositionX=packX(axis); CrescentHawkMapPositionY=packY(2047-axis);
        for(unsigned target=0;target<2048;target+=31) {
            unsigned dx=axis>target?axis-target:target-axis;
            unsigned dy=(2047-axis)>target?(2047-axis)-target:target-(2047-axis);
            check(Combat_Packed_Distance_From_Map_Position(packX(target)|0xF080,packY(target)|0x0F80)==(dx>dy?dx/2+dy:dy/2+dx));
        }
    }
    prepare();
    for(unsigned weapon=0;weapon<WeaponRecordCount;++weapon) for(unsigned distance=0;distance<100;++distance) {
        CombatantPackedX[16]=packX(distance*2); CombatantPackedY[16]=0;
        Weapon *record=&WeaponStats[weapon];
        unsigned shortRange=record->rangeBracket>>5,mediumRange=record->rangeBracket&31;
        if(record->attackCountOrClusterColumn<128 && weapon!=32) { shortRange*=3; mediumRange*=3; }
        unsigned expected=distance<shortRange?0:distance<mediumRange?1:distance<record->maximumRange?2:3;
        check(Combat_Calculate_RangeBracket(16,(uint16_t)weapon)==expected);
    }
    /* Same physical point is corrected only for a Mech beyond distance3. */
    CombatantPackedX[12]=8; CombatantPackedY[12]=2;
    CombatantPackedX[16]=8; CombatantPackedY[16]=2;
    check(Combat_Calculate_RangeBracket(12,7)==RangeBracket_Medium);
    check(Combat_Calculate_RangeBracket(16,7)==RangeBracket_Long);
    /* Crossing packed page boundaries retains the native masks. */
    CrescentHawkMapPositionX=0x007F; CrescentHawkMapPositionY=0x007F;
    CombatantPackedX[12]=0x0100; CombatantPackedY[12]=0x1005;
    check(Combat_Calculate_RangeBracket(12,7)==RangeBracket_Short);
    prepare(); CombatantActive[12]=1; memcpy(Mechs[4].name,"Enemy Locust",sizeof "Enemy Locust");
    choices[choiceCount++]=0; Combat_Select_Weapon_Target_UI(0,32,11);
    check(previews==1 && previewIds[0]==12 && CombatWeaponTarget[11]==12 && EnemyTargetId==12);
    check(strstr(text,"\r\rTarget:\r\rRange:\006\017") && strstr(text,"Enemy Locust") && strstr(text,"Short  "));
    prepare(); EnemyTargetId=23; CombatantActive[23]=1; CombatantActive[13]=1;
    Characters[15].weapon=7; choices[choiceCount++]=1; choices[choiceCount++]=2;
    CombatWeaponTarget[11]=23; Combat_Select_Weapon_Target_UI(0,32,11);
    check(previews==2 && previewIds[0]==23 && previewIds[1]==13 && EnemyTargetId==13 && CombatWeaponTarget[11]==255);
    check(strstr(text,"Weapon:\006\017") && strstr(text,"Human      ") && strstr(text,"Pistol"));
    prepare(); CombatantActive[16]=1; EnemyTargetId=16; Characters[8].weapon=0; CombatantPackedX[16]=100;
    choices[choiceCount++]=0; Combat_Select_Weapon_Target_UI(0,32,11);
    check(CombatWeaponTarget[11]==16 && strstr(text,"OUT  ")); /*OUT still accepts*/
    prepare(); CombatantActive[12]=1; CombatWeaponTarget[11]=17; choices[choiceCount++]=9;
    Combat_Select_Weapon_Target_UI(0,32,11); check(CombatWeaponTarget[11]==17 && previews==1);
    for(unsigned left=0;left<16;++left) for(unsigned right=0;right<16;++right) {
        prepare(); Mechs[0].currentActuators[0]=(uint8_t)left; Mechs[0].currentActuators[1]=(uint8_t)right;
        CombatantActive[12]=1; choices[choiceCount++]=0; Combat_Kick_Target(0);
        check(CombatMessageMenuDefaultOption==4);
        if((left&8) && (right&8)) check(previews==1 && waits==0 && CombatWeaponTarget[11]==12 && strstr(text,"Choose the enemy to kick:\r\rTarget:"));
        else check(previews==0 && waits==1 && CombatWeaponTarget[11]==255 && strstr(text,"This 'Mech can't kick because it has damaged hips. Press a key."));
    }
    puts("Original distance, range, target selection and kick checks passed"); return 0;
}
