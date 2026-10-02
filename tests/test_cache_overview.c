#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"Overview mismatch line%u\n",line); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
int main(void)
{
    uint8_t saved[MapTemplateDataBytes];
    uint32_t hash=UINT32_C(2166136261);
    for(unsigned i=0;i<3*CacheOverviewColourSwapCount;++i)
        hash=(hash^OverviewTileColours[i])*UINT32_C(16777619);
    check(hash==UINT32_C(0x53bc240a)); /* Exact expanded EXE215D..244A bytes. */
    /* Native offsets share storage, including the first32 bytes already
     * present in the map templates and six XLAT-domain bytes in cache data. */
    check(OverviewTileColours==MapTemplateData+(0x215D-0x0C1D));
    check(CacheOverviewTileColours==OverviewTileColours+250);
    check(MapRoomOverviewTileColours==CacheOverviewTileColours+250);
    for(unsigned flag=0;flag<256;++flag) {
        for(unsigned i=0;i<sizeof saved;++i) saved[i]=MapTemplateData[i]=(uint8_t)(i*19+flag);
        CacheMapRoomLoaded=(uint8_t)flag;
        OverHead_Map_Function();
        unsigned alternate=flag?MapRoomOverviewColoursOffset:CacheOverviewColoursOffset;
        for(unsigned i=0;i<sizeof saved;++i) {
            unsigned source=i;
            if(i>=OverviewColoursOffset && i<OverviewColoursOffset+250) source=alternate+i-OverviewColoursOffset;
            else if(i>=alternate && i<alternate+250) source=OverviewColoursOffset+i-alternate;
            check(MapTemplateData[i]==saved[source]);
        }
        OverHead_Map_Function(); check(!memcmp(MapTemplateData,saved,sizeof saved));
    }
    return 0;
}
