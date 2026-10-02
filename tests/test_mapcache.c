/* Sol: Actual original cache producer/near helper and terrain path.
 * Synthetic descriptor fixtures, not procedural-generation/gameplay proof. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Map cache check %u failed\n",checks); exit(1); }
}
int main(void)
{
    uint32_t hash=UINT32_C(2166136261);
    for (unsigned index=0;index<0x1560;++index) /* Original tested0C1D..217C span; overview now extends storage. */
        hash=(hash^MapTemplateData[index])*UINT32_C(16777619);
    check(hash==UINT32_C(0x80E63AFF)); /* Exact expanded EXE-owned data. */
    /* Every normal EXE block, high-bit descriptor copies its native bytes
     * directly and deliberately ignores the nonzero reflection phase. */
    for (unsigned selector=0;selector<82;++selector) {
        memset(MapAdjacencyCache,0,sizeof MapAdjacencyCache);
        memset(MapDescriptorCache,0,sizeof MapDescriptorCache);
        MapAdjacencyCache[257]=(uint8_t)selector; MapDescriptorCache[257]=0x80;
        uint16_t destination=0;
        check(PosXY_GridSegment(1,0,&destination)==(selector|0x80));
        for (unsigned row=0;row<8;++row) for (unsigned column=0;column<8;++column)
            check(CombatMap[row*24+column]==MapTemplateData[selector*64+row*8+column]);
    }
    uint8_t old=MapVerticalReflection[0];
    MapHorizontalReflection[32]=37;
    check(MapVerticalReflection[0]==37); /* Overlap is real, not separate tables. */
    MapVerticalReflection[0]=old;
    uint8_t pattern[64];
    for (unsigned index=0;index<64;++index) {
        pattern[index]=(uint8_t)(index%13==0?0x45:index%16);
        MapTemplateData[index]=pattern[index];
        MapTemplateData[MapSpecialBlockOffset+index]=(uint8_t)(0x80+index);
    }
    static const uint8_t horizontal[16]={0,1,8,9,4,5,12,13,2,3,10,11,6,7,14,15};
    static const uint8_t vertical[16]={0,4,2,6,1,5,3,7,8,12,10,14,9,13,11,15};
    for (unsigned terrain=0;terrain<256;++terrain) for (unsigned phase=0;phase<4;++phase) {
        memset(MapAdjacencyCache,0,sizeof MapAdjacencyCache);
        memset(MapDescriptorCache,0,sizeof MapDescriptorCache);
        MapDescriptorCache[256+phase]=(uint8_t)terrain;
        memset(CombatMap,0xCC,sizeof CombatMap);
        uint16_t destination=0;
        check(PosXY_GridSegment((uint8_t)phase,0,&destination)==terrain && destination==8);
        check(MapRenderSelector==0 && MapRenderTerrain==terrain && MapBlockReflection==phase);
        for (unsigned row=0;row<8;++row) for (unsigned column=0;column<24;++column) {
            if (column>=8) { check(CombatMap[row*24+column]==0xCC); continue; }
            uint8_t expected;
            if (terrain==0x10) expected=(uint8_t)(0x80+row*8+column);
            else {
                unsigned sourceRow=terrain<128 && (phase&1)?7-row:row;
                unsigned sourceColumn=terrain<128 && (phase&2)?7-column:column;
                expected=pattern[sourceRow*8+sourceColumn];
                if (terrain<128 && expected<0x40) {
                    if (phase&1) expected=vertical[expected];
                    if (phase&2) expected=horizontal[expected];
                    unsigned base=terrain==0?0:(terrain<16 || terrain>=65?48:terrain-16);
                    expected=(uint8_t)((expected%16)+base);
                }
            }
            check(CombatMap[row*24+column]==expected);
        }
        for (unsigned index=192;index<MapCacheTileCount;++index) check(CombatMap[index]==0xCC);
    }
    /* BlockFF/8 edges and corners choose the corresponding neighbour grid.
     * Selector0, direct-copy high-bit terrain keeps fixtures independent. */
    const uint8_t coordinates[3]={255,0,8};
    for (unsigned row=0;row<3;++row) for (unsigned column=0;column<3;++column) {
        memset(MapDescriptorCache,0,sizeof MapDescriptorCache);
        unsigned grid=row*3+column;
        unsigned localRow=row==0?7:0, localColumn=column==0?7:0;
        unsigned index=grid*64+localRow*8+localColumn;
        MapDescriptorCache[index]=0x80;
        uint16_t destination=0;
        check(PosXY_GridSegment(coordinates[column],coordinates[row],&destination)==0x80);
    }
    /* Real nine-block producer, metadata and real combat path linked together. */
    memset(MapAdjacencyCache,0,sizeof MapAdjacencyCache);
    memset(MapDescriptorCache,0x80,sizeof MapDescriptorCache);
    memset(MapTemplateData,5,64);
    CrescentHawkMapPositionX=0x0248; CrescentHawkMapPositionY=0x3048;
    PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
    check(CachedMapPositionX==0x0248 && CachedMapPositionY==0x3048 && LocalMapBlockX==4 && LocalMapBlockY==4);
    for (unsigned index=0;index<MapCacheTileCount;++index) check(CombatMap[index]==5);
    for (unsigned index=0;index<MapNeighbourhoodCount;++index) check(LocalTerrainFlags[index]==0x80);
    ArenaRentalMechMode=0; BlockingTileCodeThreshold=85;
    check(Combat_Check_Terrain_Path(0,12,0x024B,0x3048)==1);
    memset(MapTemplateData,85,64);
    check(Combat_Check_Terrain_Path(0,12,0x024B,0x3048)==0);
    check(CrescentHawkMapPositionX==0x0248 && CrescentHawkMapPositionY==0x3048);
    printf("Original map tile cache: %u checks passed\n",checks); return 0;
}
