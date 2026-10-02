#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t rolls[2];
static unsigned randomIndex,effects,messages;
static uint16_t effectX,effectY;
static char text[64];
static void checkLine(int value,unsigned line) { if(!value) { fprintf(stderr,"Damage support mismatch line%u\n",line); exit(1); } }
#define check(value) checkLine(!!(value),__LINE__)
/* Original dice, fire, hit text, string copy/append, verbosity and step clear
 * execute. RNG and effect/display boundaries are explicit test-only adapters. */
uint8_t Rand_0x00_to_0xFF(void) { check(randomIndex<2); return rolls[randomIndex++]; }
void Register_Persistent_Map_Effect(uint16_t sprite,uint16_t x,uint16_t y) {
    check(sprite==0x7C); ++effects; effectX=x; effectY=y;
}
void Menu_Memory_Variables(uint16_t panel) { (void)panel; check(0); }
void Display_Text_From_Memory(uint8_t *message) {
    size_t length=strlen((char *)message); check(length<sizeof text);
    memcpy(text,message,length+1); ++messages;
}
int main(void) {
    for(unsigned first=1;first<=6;++first) for(unsigned second=1;second<=6;++second) {
        rolls[0]=(uint8_t)(first-1); rolls[1]=(uint8_t)(second-1); randomIndex=effects=0;
        CombatantPackedX[23]=0x027F; CombatantPackedY[23]=0x30FF;
        Combat_Random_CreateFire(23,1,2);
        check(randomIndex==2 && effects==(unsigned)(first+second<5 || first+second>9));
        if(effects) check(effectX==0x0200 && effectY==0x3101);
        check(CombatantPackedX[23]==0x027F && CombatantPackedY[23]==0x30FF);
    }
    for(unsigned raw=0;raw<65536;++raw) {
        rolls[0]=rolls[1]=0; randomIndex=effects=0;
        CombatantPackedX[0]=(uint16_t)raw; CombatantPackedY[0]=(uint16_t)raw;
        Combat_Random_CreateFire(0,UINT16_MAX,UINT16_MAX);
        unsigned result=(raw+65535)%65536;
        check(effects==1 && effectX==(result&128?result&0x0F7F:result) && effectY==(result&128?result&0xF07F:result));
    }
    const char *expected[11]={"Left Torso.","Left Leg.","Left Arm.","Left Rear Torso.","Right Torso.","Right Leg.","Right Arm.","Right Rear Torso.","Center Torso.","Head.","Rear Center Torso."};
    for(unsigned mode=0;mode<3;++mode) for(unsigned location=0;location<11;++location) {
        messages=0; text[0]=0; DynamicString[0]='?'; DynamicString[1]=0;
        CombatMessageVerbosity=(uint16_t)mode;
        Combat_DisplayText_ArmourHitLocation((uint16_t)CombatArmourLocationOffsets[location]);
        check(!strcmp((char *)DynamicString,expected[location]));
        check(messages==(unsigned)(mode!=0));
        if(mode) check(!strcmp(text,expected[location]));
    }
    for(unsigned raw=0;raw<65536;++raw) {
        if(raw>=17 && raw<=27) continue;
        messages=0; DynamicString[0]='?'; DynamicString[1]=0;
        Combat_DisplayText_ArmourHitLocation((uint16_t)raw);
        check(!messages && DynamicString[0]=='?' && !DynamicString[1]);
    }
    /* Signed key comparison and first duplicate entry are native contracts. */
    int8_t saved=CombatArmourLocationOffsets[0]; CombatArmourLocationOffsets[0]=-2;
    CombatMessageVerbosity=1; messages=0; Combat_DisplayText_ArmourHitLocation(65534);
    check(messages==1 && !strcmp(text,"Left Torso.")); CombatArmourLocationOffsets[0]=saved;
    int8_t secondKey=CombatArmourLocationOffsets[1]; CombatArmourLocationOffsets[1]=saved;
    messages=0; Combat_DisplayText_ArmourHitLocation((uint16_t)saved);
    check(messages==1 && !strcmp(text,"Left Torso.")); CombatArmourLocationOffsets[1]=secondKey;
    memset(CombatMovementOrders,0x95,sizeof CombatMovementOrders); memset(CombatMovementPlanBytes,0xA5,sizeof CombatMovementPlanBytes);
    Combat_Clear_Movement_Plans();
    for(unsigned byte=0;byte<sizeof CombatMovementPlanBytes;++byte) check(!CombatMovementPlanBytes[byte]);
    for(unsigned byte=0;byte<sizeof CombatMovementOrders;++byte) check(CombatMovementOrders[byte]==0x95);
    puts("Original hit text, missed-shot fire and step-clear checks passed"); return 0;
}
