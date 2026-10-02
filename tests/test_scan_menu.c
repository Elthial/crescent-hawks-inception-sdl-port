#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint16_t choice,lastActor,lastSide,lastDetail;
static unsigned browses,keys,prompts,sidebars;
static char text[512];
static void check(int condition) { if(!condition) { fputs("Scan menu mismatch\n",stderr); exit(1); } }
void Draw_Top_Graphic_Sidebar(void) { ++sidebars; }
void Display_Text_From_Memory(uint8_t *value) {
    size_t used=strlen(text),length=strlen((const char *)value)+1;
    check(used+length<=sizeof text); memcpy(text+used,value,length);
}
uint16_t Display_Menu_Choices_And_Check(uint16_t menu) {
    check(menu==3 && CurrentMenuLayoutIndex==3);
    check(MenuControls[menu].baseRow==1 && MenuControls[menu].optionCount==3 && MenuControls[menu].selection==0);
    return choice;
}
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return ' '; }
void Prompt_And_Wait_For_Key(void) { ++prompts; }
void Combat_Browse_Scan_Targets(uint16_t actor,uint16_t side,uint16_t detail) {
    ++browses; lastActor=actor; lastSide=side; lastDetail=detail;
}
static void run(uint16_t actor,uint16_t sideChoice,unsigned expectedBrowse,uint16_t detail,unsigned expectedKeys,unsigned expectedPrompts) {
    choice=sideChoice; browses=keys=prompts=sidebars=0; text[0]=0;
    Scan_Enemies(actor);
    check(browses==expectedBrowse && keys==expectedKeys && prompts==expectedPrompts);
    if(expectedBrowse) check(lastActor==actor && lastSide==(sideChoice==1?12:0) && lastDetail==detail);
    check(MenuControls[3].baseRow==0 && MenuControls[3].selection==(actor<4?6:4));
    check(strstr(text,"\x06\x0FScan...\rFriends\rEnemies\rCancel")!=NULL);
}
int main(void) {
    for(unsigned id=0;id<AllCombatantCount;++id) { CombatantActive[id]=0; CombatantPackedX[id]=CombatantPosition_Unused; }
    for(uint16_t actor=0;actor<12;++actor) {
        run(actor,0,1,1,0,0); run(actor,2,0,1,0,0); run(actor,3,0,1,0,0);
        run(actor,0x8000,1,1,0,0); run(actor,0xFFFF,1,1,0,0);
    }
    for(uint16_t actor=0;actor<4;++actor) {
        Mechs[actor].sensorHits=0; run(actor,1,1,1,0,0);
        Mechs[actor].sensorHits=1; run(actor,1,1,1,0,0);
        Mechs[actor].sensorHits=2; run(actor,1,1,0,1,0);
        check(strstr(text,"Because your sensors are destroyed")!=NULL && sidebars==4);
        Mechs[actor].sensorHits=3; run(actor,1,1,1,0,0); /* EXACT2, not>=2. */
    }
    for(uint16_t actor=4;actor<12;++actor) {
        run(actor,1,0,0,0,1); check(strstr(text,"There are no enemy humans")!=NULL);
        for(unsigned enemy=12;enemy<16;++enemy) {
            CombatantActive[enemy]=0x8000; run(actor,1,0,0,1,1);
            check(strstr(text,"You can only see and describe enemy humans.")!=NULL);
            CombatantActive[enemy]=0;
        }
        for(unsigned enemy=16;enemy<24;++enemy) {
            CombatantActive[enemy]=1; CombatantPackedX[enemy]=0xFFFF;
            run(actor,1,0,0,0,1);
            CombatantPackedX[enemy]=0x00FF; CombatantPackedY[enemy]=0xFFFF;
            run(actor,1,1,0,0,0); /* XFFFF only; Y is not tested by parent. */
            CombatantActive[enemy]=0; run(actor,1,0,0,0,1);
            CombatantPackedX[enemy]=0xFFFF;
        }
    }
    puts("Original scan permissions, sensor equality, signed choices and FFFF sentinel verified.");
    return 0;
}
