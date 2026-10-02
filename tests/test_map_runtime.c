#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void check(int ok) { if(!ok) { fputs("Native map storage mismatch\n",stderr); exit(1); } }
int main(void)
{
    /*Literal native addresses are independent of the declarations' offsets.
     * No external map/art data is embedded: only synthetic patterns are used.*/
    check((uint8_t *)&MapCache==MapRuntime.bytes);
    check(CombatMap==MapRuntime.bytes+0x07AD-0x02D3);
    check((uint8_t *)&CachedMapOriginIndex==MapRuntime.bytes+0x09ED-0x02D3);
    check((uint8_t *)&CachedMapOriginColumn==MapRuntime.bytes+0x09EF-0x02D3);
    check((uint8_t *)&CachedMapOriginRowOffset==MapRuntime.bytes+0x09F1-0x02D3);
    check(PendingMapGridSlot==MapRuntime.bytes+0x09F3-0x02D3);
    check(PendingMapRegionIndex==MapRuntime.bytes+0x09F6-0x02D3);
    check((uint8_t *)&MapConstructionSeedIndex==MapRuntime.bytes+0x09F9-0x02D3);
    check(MapConstructionSeeds==MapRuntime.bytes+0x09FB-0x02D3);
    check(MapWorldVertices==MapRuntime.bytes+0x0B0B-0x02D3);
    check(MapTemplateData==MapRuntime.bytes+0x0C1D-0x02D3);
    check(MapFileTiles==MapRuntime.bytes+0x101D-0x02D3);
    check(GraphicsFileWorkspace==MapRuntime.bytes+0x244B-0x02D3);
    check((uint8_t *)&CrescentHawkMapPositionX==MapRuntime.bytes+0xA44B-0x02D3);
    check((uint8_t *)&CrescentHawkMapPositionY==MapRuntime.bytes+0xA44D-0x02D3);
    for(unsigned pattern=0;pattern<256;++pattern) {
        for(unsigned byte=0;byte<MapRuntimeBytes;++byte) MapRuntime.bytes[byte]=(uint8_t)(byte*73+pattern);
        uint8_t *cache=(uint8_t *)&MapCache,*staticData=(uint8_t *)&MapStaticData;
        for(unsigned byte=0;byte<sizeof(MapCache);++byte) check(cache[byte]==(uint8_t)(byte*73+pattern));
        for(unsigned byte=0;byte<sizeof(MapStaticData);++byte) check(staticData[byte]==(uint8_t)((byte+0x728)*73+pattern));
        for(unsigned byte=0;byte<GraphicsWorkspaceBytes;++byte) check(GraphicsFileWorkspace[byte]==(uint8_t)((byte+0x2178)*73+pattern));
        /*Defined whole-object BYTE access reproduces pre-check reads at -1
         * and -10 without an invalid subscript into the576-byte tile member.*/
        check(MapRuntime.bytes[0x07AC-0x02D3]==LocalTerrainFlags[8]);
        check(MapRuntime.bytes[0x07A3-0x02D3]==MapDescriptorCache[575]);
        CachedMapOriginIndex=0x1234; CachedMapOriginColumn=0x5678; CachedMapOriginRowOffset=0x9ABC;
        check(MapRuntime.bytes[0x09ED - 0x02D3]==0x34 && MapRuntime.bytes[0x09EE - 0x02D3]==0x12);
        check(MapRuntime.bytes[0x09EF-0x02D3]==0x78 && MapRuntime.bytes[0x09F0-0x02D3]==0x56);
        check(MapRuntime.bytes[0x09F1-0x02D3]==0xBC && MapRuntime.bytes[0x09F2-0x02D3]==0x9A);
        PendingMapGridSlot[2]=0xDC; PendingMapRegionIndex[0]=0xBA; MapConstructionSeedIndex=0xFEDC;
        check(MapRuntime.bytes[0x09F5-0x02D3]==0xDC && MapRuntime.bytes[0x09F6-0x02D3]==0xBA);
        check(MapRuntime.bytes[0x09F9-0x02D3]==0xDC && MapRuntime.bytes[0x09FA-0x02D3]==0xFE);
        MapRuntime.bytes[0x244A-0x02D3]=0x13; MapRuntime.bytes[0x244B-0x02D3]=0x57;
        check(staticData[sizeof(MapStaticData)-1]==0x13 && GraphicsFileWorkspace[0]==0x57);
        CrescentHawkMapPositionX=0x1357; CrescentHawkMapPositionY=0x2468;
        check(MapRuntime.bytes[0xA44B-0x02D3]==0x57 && MapRuntime.bytes[0xA44D-0x02D3]==0x68);
    }
    return 0;
}
