#include "game.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t source[65536],destination[65536],inPlace[32000];
static void matches(uint16_t bytes)
{
    for (unsigned group=0;group<bytes/4u;++group)
        for (unsigned plane=0;plane<4;++plane) {
            uint8_t expected=0;
            for (unsigned pixel=0;pixel<8;++pixel) {
                uint8_t pair=source[group*4+pixel/2];
                uint8_t colour=(uint8_t)((pixel&1)?pair&15:pair>>4);
                if (colour&(1<<plane)) expected|=(uint8_t)(0x80>>pixel);
            }
            assert(destination[group*4+plane]==expected);
        }
}
int main(void)
{
    for (unsigned value=0;value<256;++value) {
        for (unsigned byte=0;byte<4;++byte) source[byte]=(uint8_t)(value+byte*37);
        memset(destination,0xCC,8);
        VGA_Inline_ASM_Loop(source,destination,3);
        matches(4);
        assert(destination[4]==0xCC && destination[5]==0xCC);
    }
    for (unsigned i=0;i<sizeof source;++i) source[i]=(uint8_t)(i*17+i/157);
    VGA_Inline_ASM_Loop(source,destination,16000); matches(32000);
    memcpy(inPlace,source,sizeof inPlace);
    VGA_Inline_ASM_Loop(inPlace,inPlace,16000);
    assert(!memcmp(inPlace,destination,sizeof inPlace));
    /* Count0/1 both execute65536 groups and traverse relative64KiB four times. */
    VGA_Inline_ASM_Loop(source,destination,0);
    for (unsigned group=0;group<16384;++group)
        for (unsigned plane=0;plane<4;++plane) {
            uint8_t expected=0;
            for (unsigned pixel=0;pixel<8;++pixel) {
                uint8_t pair=source[group*4+pixel/2];
                uint8_t colour=(uint8_t)((pixel&1)?pair&15:pair>>4);
                if (colour&(1<<plane)) expected|=(uint8_t)(0x80>>pixel);
            }
            assert(destination[group*4+plane]==expected);
        }
    memset(destination,0xCC,sizeof destination);
    VGA_Inline_ASM_Loop(source,destination,1);
    matches(32000);
    puts("Original packed-plane conversion, in-place ordering, odd count and underflow passed");
    return 0;
}
