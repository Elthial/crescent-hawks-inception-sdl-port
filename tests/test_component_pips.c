#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef CHI_TEST_SDL
#include "backend.h"
#endif

static unsigned calls;
static uint16_t firstColumn,firstRow,healthy;
static void check(int condition) { if(!condition) { fputs("Component pip mismatch\n",stderr); exit(1); } }
#ifndef CHI_TEST_SDL
void Draw_Horizontal_EGA_Line(uint16_t left,uint16_t top,uint16_t right,uint16_t bottom,uint16_t colour) {
    unsigned index=calls++;
    unsigned column=firstColumn+(index<5?index:index-5);
    unsigned row=firstRow+(index>=5?1:0);
    check(left==(uint16_t)(column*8+2) && right==(uint16_t)(column*8+4));
    check(top==(uint16_t)(row*8+3) && bottom==(uint16_t)(row*8+5));
    check(colour==(index>=healthy?EGA_Red:EGA_Green));
}
#else
static unsigned pixel(unsigned x,unsigned y) {
    unsigned colour=0;
    for(uint8_t plane=0;plane<4;++plane)
        if(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,(uint16_t)(y*40+x/8),plane)&(0x80>>(x%8))) colour|=1u<<plane;
    return colour;
}
#endif
static void run(uint16_t column,uint16_t row,uint16_t total,uint16_t healthyCount) {
    firstColumn=column; firstRow=row; healthy=healthyCount; calls=0;
#ifdef CHI_TEST_SDL
    SDLBackend_ClearEgaScreen();
#endif
    Draw_Component_Status_Pips(column,row,total,healthyCount);
    unsigned expectedCount=(int16_t)total>0?total:0;
#ifndef CHI_TEST_SDL
    check(calls==expectedCount);
#else
    for(unsigned y=0;y<40;++y) for(unsigned x=0;x<320;++x) {
        unsigned expected=0;
        for(unsigned pip=0;pip<expectedCount;++pip) {
            unsigned pipX=(column+(pip<5?pip:pip-5))*8+2;
            unsigned pipY=(row+(pip>=5?1:0))*8+3;
            if(x>=pipX && x<=pipX+2 && y>=pipY && y<=pipY+2)
                expected=pip>=healthyCount?EGA_Red:EGA_Green;
        }
        check(pixel(x,y)==expected);
    }
#endif
}
int main(void) {
    for(uint16_t total=0;total<=12;++total)
        for(uint16_t healthyCount=0;healthyCount<=13;++healthyCount) run(4,1,total,healthyCount);
    run(4,1,0x8000,0); run(4,1,0xFFFF,0); run(4,1,10,0xFFFF);
#ifndef CHI_TEST_SDL
    run(0xFFFF,0xFFFF,10,6); /* Native shift/add WORD truncation. */
    run(4,1,0x7FFF,0x7FFE); /* Largest positive signed total. */
#endif
    puts("Original component pips: signed totals, colour transition and one-time wrap verified.");
    return 0;
}
