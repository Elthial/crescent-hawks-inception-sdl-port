#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint16_t keys[32],savedColumn,savedRow;
static unsigned keyCount,keyIndex,stage;
static uint16_t expectedLeft,expectedTop,expectedRight;
static char displayed[32][8];
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"Decimal entry mismatch line%u\n",line); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
void Display_Text_From_Memory(uint8_t *text)
{
    check(stage++%3==0 && keyIndex<keyCount && text==DynamicString);
    check(TextColumn==savedColumn && TextRow==savedRow);
    check(TextColour==(GraphicsAdapter==0?1:2));
    size_t length=strlen((char *)text); check(length<=7);
    memcpy(displayed[keyIndex],text,length+1);
    TextColumn=(uint16_t)(TextColumn+length);
    expectedLeft=(uint16_t)((TextPanelLeft+TextColumn)*8);
    expectedTop=(uint16_t)((TextPanelTop+TextRow)*8);
    expectedRight=(uint16_t)((TextPanelLeft+TextPanelWidth)*8-1);
}
void Draw_Horizontal_EGA_Line(uint16_t left,uint16_t top,uint16_t right,uint16_t bottom,uint16_t colour)
{
    check(stage++%3==1 && left==expectedLeft && top==expectedTop && right==expectedRight);
    check(bottom==(uint16_t)(expectedTop+7) && colour==0);
}
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { check(stage++%3==2 && keyIndex<keyCount); return keys[keyIndex++]; }
static void run(const uint16_t *sequence,unsigned count,uint32_t expected,const char *final)
{
    memcpy(keys,sequence,count*sizeof *sequence); keyCount=count; keyIndex=stage=0;
    savedColumn=TextColumn=0xFFFA; savedRow=TextRow=0xFFFF;
    TextPanelLeft=0x8005; TextPanelTop=0xFFF9; TextPanelWidth=37;
    memset(DynamicString,0xA5,DynamicTextScratchBytes);
    check(Prompt_For_Unsigned_Decimal()==expected);
    check(stage==count*3 && keyIndex==count && TextColour==15);
    check(!strcmp((char *)DynamicString,final) && DynamicString[8]==0xA5);
    check(!strcmp(displayed[0],"0"));
}
int main(void)
{
    for(unsigned key=0;key<65536;++key) {
        uint16_t sequence[2]={(uint16_t)key,13};
        char final[2]={'0',0}; uint32_t expected=0;
        if(key>='0' && key<='9') { final[0]=(char)key; expected=key-'0'; }
        if(key==8) final[0]=0;
        GraphicsAdapter=(uint16_t)key;
        run(sequence,key==13?1:2,expected,final);
    }
    const uint16_t sevenDigits[]={'9','9','9','9','9','9','9','0',13};
    run(sevenDigits,9,9999999,"9999999"); check(!strcmp(displayed[8],"9999999"));
    const uint16_t editing[]={'0','0','1','2',8,'3',27,'4',8,8,8,13};
    run(editing,12,0,"");
    check(!strcmp(displayed[3],"1") && !strcmp(displayed[6],"13"));
    check(!strcmp(displayed[7],"0") && !strcmp(displayed[8],"4"));
    const uint16_t replace[]={'1','2','3','4','5','6','7','8',8,'9',13};
    run(replace,11,1234569,"1234569");
    const uint16_t reset[]={'1','2','3','4','5','6','7',27,'0','5',13};
    run(reset,11,5,"5");
    return 0;
}
