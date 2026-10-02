#include "game.h"
#include "backend.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t packed[32000],allocation[65538];
static uint32_t allocationRequest;
static unsigned allocations;
/* Pending original05BC allocation boundary only. No production substitute. */
uint8_t *Allocate_Far_Buffer(uint32_t bytes)
{
    allocationRequest=bytes; ++allocations;
    memset(allocation,0xCC,sizeof allocation);
    return allocation+1;
}
static void matches(uint8_t *tiles,uint16_t firstColumn,uint16_t firstRow,uint16_t count)
{
    uint16_t column=firstColumn,row=firstRow;
    for (uint16_t tile=0;tile<count;++tile) {
        for (uint16_t y=0;y<8;++y)
            for (uint8_t plane=0;plane<4;++plane) {
                uint8_t expected=0;
                for (uint16_t x=0;x<8;++x) {
                    unsigned pixel=(row*8+y)*320+column*8+x;
                    uint8_t colour=(uint8_t)((pixel&1)?(packed[pixel/2]&15):(packed[pixel/2]>>4));
                    if (colour&(1<<plane)) expected|=(uint8_t)(0x80>>x);
                }
                assert(tiles[tile*32+y*4+plane]==expected);
            }
        if (++column==40) { column=0; ++row; }
    }
    assert(allocation[0]==0xCC && tiles[count*32]==0xCC);
}
int main(void)
{
    for (unsigned i=0;i<sizeof packed;++i) packed[i]=(uint8_t)(i*17+i/157);
    DrawCall_Image_To_VGA_Memory(packed,EgaSceneStagingSegment);
    /* EGA ignores the display argument: NULL still captures real A800 bytes. */
    uint8_t *border=Create_TileSet_Array(NULL,0,0,8);
    assert(border==allocation+1 && allocationRequest==256 && allocations==1);
    matches(border,0,0,8);
    BorderTileset=border;
    Menu_Memory_Variables(4);
    Draw_Menu_Border(4);
    for (uint16_t y=0;y<8;++y)
        for (uint8_t plane=0;plane<4;++plane)
            assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,(uint16_t)(y*40),plane)==border[y*4+plane]);
    uint8_t *tinyland=Create_TileSet_Array(NULL,0,0,66);
    assert(tinyland==allocation+1 && allocationRequest==2112 && allocations==2);
    matches(tinyland,0,0,66);
    uint8_t *crossing=Create_TileSet_Array(NULL,39,2,3);
    assert(allocationRequest==96 && allocations==3); matches(crossing,39,2,3);
    assert(SDLBackend_EgaReadMapSelect==3 && SDLBackend_EgaBitMask==0 && SDLBackend_EgaWriteMode==2);
    assert(Create_TileSet_Array(NULL,0,0,0)==allocation+1 && allocationRequest==0 && allocations==4);
    assert(allocation[1]==0xCC);
    /* Negative signed tile count skips capture, but wrapped/CWD request survives. */
    assert(Create_TileSet_Array(NULL,0,0,65535)==allocation+1 && allocationRequest==UINT32_C(0xFFFFFFE0));
    assert(allocations==5 && allocation[1]==0xCC);
    puts("Original tileset allocation requests, A800 capture and border rendering passed");
    return 0;
}
