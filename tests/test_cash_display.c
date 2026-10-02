#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned phase;
static uint16_t savedLayout;
static char expected[24];
static void verify(int ok,unsigned line){if(!ok){fprintf(stderr,"Cash display mismatch line%u\n",line);exit(1);}}
#define check(x) verify(!!(x),__LINE__)
void Menu_Memory_Variables(uint16_t layout){
    if(phase==0){check(layout==3);phase=1;}else{check(phase==4 && layout==savedLayout);phase=5;}
    CurrentMenuLayoutIndex=layout;
}
void Set_Text_Colour_Bright_Green(void){check(phase==2);TextColour=EGA_BrightGreen;phase=3;}
void Display_Text_From_Memory(uint8_t *text){
    if(phase==1){check(TextRow==9 && TextColumn==0 && !strcmp((char *)text,"\006\017C-Bills:\r"));phase=2;}
    else{check(phase==3 && text==DynamicString && !strcmp((char *)text,expected));phase=4;}
}
int main(void){
    const uint32_t values[]={0,1,9,10,99,100,999999999,1000000000,INT32_MAX,UINT32_C(2147483648),UINT32_MAX};
    for(unsigned entry=0;entry<sizeof values/sizeof values[0];++entry){
        CBills=values[entry];savedLayout=(uint16_t)(entry+20);CurrentMenuLayoutIndex=savedLayout;
        phase=0;TextColour=EGA_BrightWhite;TextRow=100;TextColumn=100;
        int64_t signedCash=values[entry]<=INT32_MAX?values[entry]:(int64_t)values[entry]-INT64_C(4294967296);
        (void)snprintf(expected,sizeof expected,"%-10lld",(long long)signedCash);
        Display_Text_CBill_Balance();
        check(phase==5 && CurrentMenuLayoutIndex==savedLayout && TextColour==EGA_BrightWhite);
    }
    puts("Original cash formatting, padding and colour restoration preserved.");return 0;
}
