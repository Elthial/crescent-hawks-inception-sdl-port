#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Sol: Real adjacency bodies. Independent neighbour-coordinate oracle uses
 * a second complete cache image, preserving outside reads and write order. */
static unsigned checks;
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Adjacency check %u failed\n",checks); exit(1); }
}
int main(void)
{
    uint8_t expected[sizeof(MapCacheStorage)];
    for (unsigned trial=0;trial<288;++trial) {
        uint8_t *actual=(uint8_t *)&MapCache;
        for (unsigned index=0;index<sizeof MapCache;++index)
            actual[index]=(uint8_t)(trial<256?trial:(index*37+trial*19+(index/11)*53));
        memcpy(expected,actual,sizeof expected);
        for (unsigned block=0;block<9;++block)
            for (unsigned row=0;row<8;++row) for (unsigned column=0;column<8;++column) {
                unsigned index=(unsigned)offsetof(MapCacheStorage,descriptors)+block*64+row*8+column;
                uint8_t value=expected[index],flags;
                if (value>=128 && !(value==128 && (row==0 || row==7) && column>0 && column<7))
                    flags=(uint8_t)(value-128);
                else {
                    unsigned west=column==0?index-57:index-1;
                    unsigned east=column==7?index+57:index+1;
                    unsigned north=row==0?index-136:index-8;
                    unsigned south=row==7?index+136:index+8;
                    flags=(uint8_t)((value==expected[west]?8:0)|(value==expected[east]?2:0)|
                        (value==expected[north]?1:0)|(value==expected[south]?4:0));
                }
                expected[offsetof(MapCacheStorage,adjacency)+block*64+row*8+column]=flags;
            }
        Map_Rebuild_Adjacency();
        check(!memcmp(actual,expected,sizeof expected));
    }
    /* Native vertical helpers INC the full supplied BYTE, not OR north1.
     * Exercise all flags/values, including carry and FF=>0 wrap. */
    uint16_t index=(uint16_t)(offsetof(MapCacheStorage,descriptors)+4*64);
    uint8_t *memory=(uint8_t *)&MapCache;
    for (unsigned value=0;value<256;++value) for (unsigned flags=0;flags<256;++flags) {
        memory[index-136]=memory[index+8]=memory[index-8]=memory[index+136]=(uint8_t)value;
        uint8_t result=(uint8_t)(((flags+1)&255)|4);
        check(Map_Adjacency_Top_Edge(index,(uint8_t)value,(uint8_t)flags)==result);
        check(Map_Adjacency_Bottom_Edge(index,(uint8_t)value,(uint8_t)flags)==result);
    }
    memset(&MapCache,0,sizeof MapCache);
    MapDescriptorCache[1]=128; MapDescriptorCache[0]=128; MapDescriptorCache[2]=128;
    MapDescriptorCache[9]=128;
    MapAdjacencyCache[441]=128; /* Descriptor index1-136 reaches adjacency storage. */
    Map_Build_Adjacency_Block((uint16_t)offsetof(MapCacheStorage,descriptors),
        (uint16_t)offsetof(MapCacheStorage,adjacency));
    check(MapAdjacencyCache[0]==0 && MapAdjacencyCache[1]==15); /* Corner exception absent. */
    /* Full region parent including reads into adjacent seed/template data. */
    uint8_t *staticBytes=(uint8_t *)&MapStaticData;
    for (uint16_t region=0;region<256;++region) {
        memset(&MapCache,0xCC,sizeof MapCache);
        Map_Construct_Nine_Regions(region);
        unsigned first=(unsigned)offsetof(MapStaticStorage,worldVertices)+region-17;
        for (unsigned row=0;row<3;++row) for (unsigned column=0;column<3;++column)
            check(MapDescriptorCache[(row*3+column)*64]==(staticBytes[first+row*16+column]&0xF0));
        unsigned finalCorner=first+2*16+2;
        check(MapConstructionLattice[0]==staticBytes[finalCorner] &&
            MapConstructionLattice[8]==staticBytes[finalCorner+1] &&
            MapConstructionLattice[72]==staticBytes[finalCorner+16] &&
            MapConstructionLattice[80]==staticBytes[finalCorner+17]);
    }
    uint8_t retained[384];
    memset(&MapCache,0x12,sizeof MapCache);
    for (unsigned cell=0;cell<576;++cell) MapDescriptorCache[cell]=(uint8_t)(cell/64+128);
    memcpy(retained,MapDescriptorCache,384);
    CrescentHawkMapPositionX=0x0540; CrescentHawkMapPositionY=0x5000;
    Map_Move_North();
    check(CrescentHawkMapPositionY==0x407F && !memcmp(MapDescriptorCache+192,retained,384));
    for (unsigned entry=0;entry<3;++entry)
        check(PendingMapGridSlot[entry]==entry && PendingMapRegionIndex[entry]==0x34+entry);
    memcpy(retained,MapDescriptorCache+192,384);
    Map_Move_South();
    check(CrescentHawkMapPositionY==0x5000 && !memcmp(MapDescriptorCache,retained,384));
    for (unsigned entry=0;entry<3;++entry)
        check(PendingMapGridSlot[entry]==6+entry && PendingMapRegionIndex[entry]==0x64+entry);
    MapCacheStorage saved=MapCache;
    CrescentHawkMapPositionY=0; Map_Move_North();
    check(CrescentHawkMapPositionY==0 && !memcmp(&saved,&MapCache,sizeof saved));
    CrescentHawkMapPositionY=0xF07F; Map_Move_South();
    check(CrescentHawkMapPositionY==0xF07F && !memcmp(&saved,&MapCache,sizeof saved));
    CrescentHawkMapPositionY=0x5060; Map_Move_North(); Map_Move_South();
    check(CrescentHawkMapPositionY==0x5060 && !memcmp(&saved,&MapCache,sizeof saved));
    uint8_t columns[576];
    for (unsigned cell=0;cell<576;++cell) MapDescriptorCache[cell]=(uint8_t)(cell/64+128);
    memcpy(columns,MapDescriptorCache,576);
    CrescentHawkMapPositionX=0x0500; CrescentHawkMapPositionY=0x5040;
    Map_Move_West();
    check(CrescentHawkMapPositionX==0x047F);
    for (unsigned row=0;row<3;++row) {
        check(!memcmp(MapDescriptorCache+row*192+64,columns+row*192,128));
        check(PendingMapGridSlot[row]==row*3 && PendingMapRegionIndex[row]==0x43+row*16);
    }
    memcpy(columns,MapDescriptorCache,576); Map_Move_East();
    check(CrescentHawkMapPositionX==0x0500);
    for (unsigned row=0;row<3;++row) {
        check(!memcmp(MapDescriptorCache+row*192,columns+row*192+64,128));
        check(PendingMapGridSlot[row]==row*3+2 && PendingMapRegionIndex[row]==0x46+row*16);
    }
    saved=MapCache;
    CrescentHawkMapPositionX=0; Map_Move_West();
    check(CrescentHawkMapPositionX==0 && !memcmp(&saved,&MapCache,sizeof saved));
    CrescentHawkMapPositionX=0x0F7F; Map_Move_East();
    check(CrescentHawkMapPositionX==0x0F7F && !memcmp(&saved,&MapCache,sizeof saved));
    CrescentHawkMapPositionX=0x0560; Map_Move_West(); Map_Move_East();
    check(CrescentHawkMapPositionX==0x0560 && !memcmp(&saved,&MapCache,sizeof saved));
    /* Absolute targets traverse Y before X: final queue is the X crossing. */
    CrescentHawkMapPositionX=0x057F; CrescentHawkMapPositionY=0x507F;
    Move_Map_View_To_Packed_Position(0x0601,0x6001);
    check(CrescentHawkMapPositionX==0x0601 && CrescentHawkMapPositionY==0x6001);
    check(PendingMapGridSlot[0]==2 && PendingMapRegionIndex[0]==0x57);
    Move_Map_View_By_Signed_Delta(-3,-3);
    check(CrescentHawkMapPositionX==0x057E && CrescentHawkMapPositionY==0x507E);
    check(PendingMapGridSlot[0]==0 && PendingMapRegionIndex[0]==0x44);
    CrescentHawkMapPositionX=0; CrescentHawkMapPositionY=0;
    Move_Map_View_By_Signed_Delta(-2,-2);
    check(CrescentHawkMapPositionX==0 && CrescentHawkMapPositionY==0);
    printf("Original adjacency/scrolling: %u checks passed\n",checks); return 0;
}
