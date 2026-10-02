#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef CHI_TEST_HEAP_BOUNDARY
#include <SDL3/SDL.h>
#endif

static unsigned messages,inputs;
static void check(int condition){if(!condition){fputs("Original allocation contract mismatch\n",stderr);exit(1);}}
/* Sol: only presentation/input are isolated in the SDL integration case.
 * Headless case additionally isolates the WORD heap entry to force failures. */
void Draw_EGA_Text_To_Screen(uint8_t *text,uint16_t column,uint16_t row,uint16_t foreground,uint16_t background)
{
    check(!strcmp((char *)text,"Alloc: Null pointer return!"));
    check(column==0 && row==10 && foreground==EGA_BrightWhite && background==EGA_Black);
    ++messages;
}
uint16_t Keyboard_Get_ASCII_Hex_Input(void){++inputs;return 13;}
#ifdef CHI_TEST_HEAP_BOUNDARY
static uint16_t observedCount;
static unsigned allocations;
static int forceFailure;
static uint8_t buffer[65536];
uint8_t *Runtime_Allocate_Word_Buffer(uint16_t byteCount)
{
    observedCount=byteCount;++allocations;
    return forceFailure?NULL:buffer;
}
#endif
int main(void)
{
    const uint32_t requests[]={0,1,292,2112,32768,65535,0xFFFF8000u,0xFFFFFFFFu,0x80000124u};
    for(unsigned index=0;index<sizeof requests/sizeof requests[0];++index){
        uint8_t *allocation=Allocate_Far_Buffer(requests[index]);
        check(allocation!=NULL);
#ifdef CHI_TEST_HEAP_BOUNDARY
        check(allocation==buffer && observedCount==(uint16_t)requests[index]);
#else
        size_t bytes=(uint16_t)requests[index];
        if(bytes!=0){memset(allocation,0xA5,bytes);check(allocation[bytes-1]==0xA5);}
        SDL_free(allocation);
#endif
    }
    check(messages==0 && inputs==0);
#ifdef CHI_TEST_HEAP_BOUNDARY
    forceFailure=1;
    check(Allocate_Far_Buffer(292)==NULL);
    check(observedCount==292 && messages==1 && inputs==1);
    check(allocations==sizeof requests/sizeof requests[0]+1);
#endif
    puts("Original allocation low-WORD and diagnostic contracts passed");
    return 0;
}
