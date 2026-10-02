#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"Overview tile mismatch line%u\n",line); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
int main(void)
{
    uint8_t expected[32];
    for(unsigned tile=0;tile<256;++tile) for(unsigned selector=0;selector<256;++selector) {
        memset(MapDescriptorCache,tile,MapDescriptorCacheBytes); memset(MapAdjacencyCache,selector,MapDescriptorCacheBytes);
        memset(GraphicsFileWorkspace,0xA5,GraphicsWorkspaceBytes);
        Overhead_Map_Build_Centre_Block(0x150);
        unsigned translated=tile>=144?tile:tile==16?64:((tile>=32?tile-16:tile)|selector);
        for(unsigned index=0;index<960;++index) {
            unsigned relative=index-0x150;
            unsigned drawn=index>=0x150 && relative/40<8 && relative%40<8;
            check(GraphicsFileWorkspace[index]==(drawn?translated:0xA5));
        }
    }
    for(unsigned tile=144;tile<256;++tile) for(unsigned seed=0;seed<256;++seed) {
        unsigned source=0x4000+(tile-144)*64;
        for(unsigned colour=0;colour<256;++colour) OverviewTileColours[colour]=(uint8_t)(colour*17+seed);
        for(unsigned pixel=0;pixel<64;++pixel) GraphicsFileWorkspace[source+pixel]=(uint8_t)(seed+pixel*13);
        for(unsigned byte=0;byte<32;++byte) {
            uint8_t first=OverviewTileColours[(uint8_t)(seed+byte*26)];
            uint8_t second=OverviewTileColours[(uint8_t)(seed+byte*26+13)];
            expected[byte]=(uint8_t)((first*16)|second);
        }
        GraphicsFileWorkspace[0x3FDF]=0xBE;
        Pack_Dynamic_Overhead_Tile((uint16_t)tile);
        check(!memcmp(GraphicsFileWorkspace+0x3FE0,expected,32) && GraphicsFileWorkspace[0x3FDF]==0xBE);
    }
    for(unsigned adapter=0;adapter<4;++adapter) {
        GraphicsAdapter=(uint16_t)adapter;
        for(unsigned colour=0;colour<256;++colour) OverviewTileColours[colour]=(uint8_t)(colour&15);
        for(unsigned pixel=0;pixel<64;++pixel) GraphicsFileWorkspace[0x4000+pixel]=(uint8_t)pixel;
        Build_Dynamic_Overhead_Tile(144);
        for(unsigned byte=0;byte<32;++byte) {
            unsigned oracle=0;
            if(adapter==2) {
                unsigned plane=byte%4,row=byte/4;
                for(unsigned column=0;column<8;++column) oracle|=(((row*8+column)&(1u<<plane))?1u:0u)<<(7-column);
            } else oracle=((byte*2&15)<<4)|((byte*2+1)&15);
            check(GraphicsFileWorkspace[0x3FE0+byte]==oracle);
        }
    }
    return 0;
}
