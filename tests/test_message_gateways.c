#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned step,soundCalls,waitCalls,textCalls;
static uint16_t expectedSound;
static uint8_t *expectedText;
static void verify(int ok,unsigned line) {
    if (!ok) { fprintf(stderr,"Original message/sound gateway mismatch line%u\n",line); exit(1); }
}
#define check(x) verify(!!(x),__LINE__)
void Menu_Memory_Variables(uint16_t layout) { check(step++==0 && layout==7); }
void Draw_Top_Graphic_Sidebar(void) { check(step++==1); }
void Draw_Menu_Border(uint16_t border) { check(step++==2 && border==0); }
void Display_Text_From_Memory(uint8_t *text) {
    check(!waitCalls); ++textCalls;
    if (expectedText) check(text==expectedText);
    else check(!strcmp((char *)text,"Come back when you can afford it."));
}
void Wait_For_50Hz_Then_Check_Input(void) { check(textCalls==1); ++waitCalls; }
void Sound_Setup(uint16_t soundId) { check(soundId==expectedSound); ++soundCalls; }
int main(void) {
    Draw_Message_Box(); check(step==3 && !waitCalls && !textCalls);
    uint8_t text[]="Native\r\x06\x0F text";
    expectedText=text; Display_Text_From_Memory_ScreenRetrace_KeyboardInput(text);
    check(textCalls==1 && waitCalls==1);
    expectedText=NULL; textCalls=waitCalls=0;
    Display_Text_Shop_Cannot_Afford_Text(); check(textCalls==1 && !waitCalls);
    static const uint16_t settings[4]={0,1,256,32768};
    for (unsigned i=0;i<4;++i) for (unsigned id=0;id<65536;++id) {
        soundCalls=0; SoundEffectsEnabled=settings[i]; expectedSound=(uint16_t)id;
        Play_Sound_If_Enabled((uint16_t)id); check(soundCalls==(unsigned)(i!=0));
    }
    puts("Original message presentation/order, affordability punctuation and all WORD sound IDs passed.");
    return 0;
}
