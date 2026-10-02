#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Sol: Real original formatting and abs entries. libc formatting is only
 * an independent test oracle for their normal decimal/hex contracts. */
static unsigned checks, draws;
static uint8_t rendered[DynamicTextScratchBytes];
static uint8_t *expectedTextPointer = DynamicString;
static void check(int condition)
{
    ++checks; if (!condition) { fprintf(stderr,"Numeric check %u failed\n",checks); exit(1); }
}
void Display_Text_From_Memory(uint8_t *text)
{
    check(text == expectedTextPointer);
    check(strlen((char *)text) < sizeof rendered);
    memcpy(rendered,text,strlen((char *)text)+1); ++draws;
}
int main(void)
{
    uint8_t buffer[40]; char expected[40];
    for (uint32_t value = 0; value <= UINT16_MAX; ++value) {
        memset(buffer,0xCC,sizeof buffer);
        check(snprintf(expected,sizeof expected,"%d",(int)(int16_t)value)>0);
        check(ASM_Text_Formatting((uint16_t)value,buffer+1,10) == buffer+1);
        check(!strcmp((char *)buffer+1,expected) && buffer[0] == 0xCC);
        check(buffer[strlen(expected)+2] == 0xCC);
        check(snprintf(expected,sizeof expected,"%x",(unsigned)value)>0);
        ASM_Text_Formatting((uint16_t)value,buffer+1,16);
        check(!strcmp((char *)buffer+1,expected));
        int16_t signedValue = (int16_t)value;
        uint16_t magnitude = signedValue < 0 ? (uint16_t)(0-value) : (uint16_t)value;
        check((uint16_t)Native_Abs_Word(signedValue) == magnitude);
        Display_Text_Dynamic_Value((uint16_t)value);
        snprintf(expected,sizeof expected,"%d",(int)signedValue);
        check(!strcmp((char *)rendered,expected));
    }
    check(draws == 65536);
    const uint32_t boundaries[] = {0,1,32767,32768,65535,65536,UINT32_C(0x7FFFFFFF),
        UINT32_C(0x80000000),UINT32_C(0x80000001),UINT32_MAX};
    uint32_t state = 1;
    for (unsigned i = 0; i < 1034; ++i) {
        uint32_t value;
        if (i < sizeof boundaries / sizeof boundaries[0]) value = boundaries[i];
        else { state = state * UINT32_C(1664525) + UINT32_C(1013904223); value = state; }
        check(snprintf(expected,sizeof expected,"%ld",(long)(int32_t)value)>0);
        check(CBill_Text_Formatting(value,buffer,10) == buffer);
        check(!strcmp((char *)buffer,expected));
        snprintf(expected,sizeof expected,"%lx",(unsigned long)value);
        CBill_Text_Formatting(value,buffer,16); check(!strcmp((char *)buffer,expected));
    }
    CBill_Text_Formatting(UINT32_MAX,buffer,2);
    check(!strcmp((char *)buffer,"11111111111111111111111111111111"));
    CBill_Text_Formatting(35,buffer,36); check(!strcmp((char *)buffer,"z"));
    CBill_Text_Formatting(36,buffer,36); check(!strcmp((char *)buffer,"10"));
    CBill_Text_Formatting(255,buffer,256); check(buffer[0] == '/' && buffer[1] == 0);
    CBill_Text_Formatting(99,buffer,100); check(buffer[0] == 186 && buffer[1] == 0);
    CBill_Text_Formatting(65535,buffer,65535); check(!strcmp((char *)buffer,"10"));
    CBill_Text_Formatting(0,buffer,1); check(!strcmp((char *)buffer,"0"));
    expectedTextPointer = buffer; memcpy(buffer,"Positioned text",16);
    Display_Text_At(buffer,UINT16_MAX,UINT16_C(0x8000));
    check(TextColumn == UINT16_MAX && TextRow == UINT16_C(0x8000) && TextColour == EGA_BrightWhite);
    check(!strcmp((char *)rendered,"Positioned text"));
    Set_Text_Colour_Bright_Green(); check(TextColour == EGA_BrightGreen);
    printf("Original numeric routines: %u checks passed\n",checks); return 0;
}
