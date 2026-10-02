#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Sol: Fixed expected lattices from the independent recursive/carry-factored
 * model in Verify-MapConstructionTranscriptions.ps1, using original EXE seeds.
 * They are synthetic branch witnesses, not captured emulator/gameplay data. */
static const struct Fixture { uint8_t corners[4], lattice[81]; uint16_t index; } fixtures[]={
    {{0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 35},
    {{32,32,32,32}, {32,26,24,22,24,22,24,26,32,26,27,24,25,22,25,24,27,26,24,24,28,24,24,24,28,24,24,22,25,24,27,26,27,24,25,22,24,22,24,26,32,26,24,22,24,22,25,24,27,26,27,24,25,22,24,24,28,24,24,24,28,24,24,26,27,24,25,22,25,24,27,26,32,26,24,22,24,22,24,26,32}, 99},
    {{128,128,128,128}, {128,122,120,118,120,118,120,122,128,122,123,120,121,118,121,120,123,122,120,120,124,120,120,120,124,120,120,118,121,120,123,122,123,120,121,118,120,118,120,122,128,122,120,118,120,118,121,120,123,122,123,120,121,118,120,120,124,120,120,120,124,120,120,122,123,120,121,118,121,120,123,122,128,122,120,118,120,118,120,122,128}, 35},
    {{255,255,255,255}, {255,0,123,59,0,59,123,0,255,0,157,123,93,59,93,123,157,0,123,123,127,123,123,123,127,123,123,59,93,123,157,0,157,123,93,59,0,59,123,0,255,0,123,59,0,59,93,123,157,0,157,123,93,59,123,123,127,123,123,123,127,123,123,0,157,123,93,59,93,123,157,0,255,0,123,59,0,59,123,0,255}, 33},
    {{10,60,100,140}, {10,10,14,18,27,31,39,47,60,15,22,25,32,35,44,49,58,64,24,30,40,42,48,54,64,66,72,33,42,47,55,60,67,70,77,80,47,50,58,65,77,76,80,84,92,56,64,69,77,81,88,90,97,100,69,74,84,85,90,95,105,106,112,82,88,91,97,99,107,111,119,124,100,99,102,105,112,115,122,0,140}, 235},
    {{0,127,128,255}, {0,9,23,37,55,69,87,105,127,10,26,39,56,69,76,80,87,91,24,39,59,71,87,80,77,66,59,38,56,71,90,105,87,66,48,27,56,69,87,105,127,91,59,27,0,70,77,80,87,91,85,75,69,59,88,80,77,66,59,75,95,107,123,106,88,66,49,27,69,107,149,0,128,92,60,28,0,59,123,0,255}, 161},
    {{20,50,80,110}, {20,17,19,21,27,28,34,40,50,21,26,26,31,32,39,41,48,51,27,30,38,38,42,45,53,53,57,32,39,41,48,51,56,56,61,62,42,43,49,55,65,62,64,66,72,47,54,56,63,66,71,71,76,77,57,60,68,68,72,75,83,83,87,66,71,71,76,77,84,86,93,96,80,77,79,81,87,88,94,100,110}, 195},
    {{80,8,40,112}, {80,65,54,43,36,25,18,11,8,69,63,53,47,38,34,26,22,15,62,57,57,48,44,39,39,30,26,55,55,52,53,50,48,43,42,37,52,50,52,54,60,54,52,50,52,45,50,51,56,58,61,60,63,63,42,46,55,55,60,64,73,73,78,39,46,50,58,62,71,77,87,93,40,43,50,57,68,75,86,97,112}, 155},
};
static unsigned checks;
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Construction check %u failed\n",checks); exit(1); }
}
int main(void)
{
    for (unsigned trial=0;trial<sizeof fixtures/sizeof fixtures[0];++trial) {
        uint8_t lattice[83], output[66];
        memset(lattice,0xCC,sizeof lattice); memset(output,0xDD,sizeof output);
        memset(MapSubdivisionStack,0xEE,sizeof MapSubdivisionStack);
        uint8_t *grid=lattice+1;
        grid[0]=fixtures[trial].corners[0]; grid[8]=fixtures[trial].corners[1];
        grid[72]=fixtures[trial].corners[2]; grid[80]=fixtures[trial].corners[3];
        MapConstructionSeedIndex=0xABCD; /* Parent must reseed BOTH bytes. */
        Map_Build_Procedural_Block(output+1,grid);
        check(!memcmp(grid,fixtures[trial].lattice,81));
        check(MapConstructionSeedIndex==fixtures[trial].index);
        check(lattice[0]==0xCC && lattice[82]==0xCC && output[0]==0xDD && output[65]==0xDD);
        for (unsigned row=0;row<8;++row) for (unsigned column=0;column<8;++column)
            check(output[1+row*8+column]==(fixtures[trial].lattice[row*9+column]&0xF0));
        for (unsigned index=40;index<MapSubdivisionStackBytes;++index) check(MapSubdivisionStack[index]==0xEE);
    }
    uint8_t lattice[81]; memset(lattice,42,sizeof lattice);
    MapConstructionSeedIndex=255;
    MapSubdivisionStack[0]=0; MapSubdivisionStack[1]=8;
    uint16_t stackTop=2;
    Map_Subdivide_Horizontal_Edge(lattice,&stackTop);
    check(stackTop==0 && MapConstructionSeedIndex==255);
    MapSubdivisionStack[0]=0; MapSubdivisionStack[1]=72; stackTop=2;
    Map_Subdivide_Vertical_Edge(lattice,&stackTop);
    check(stackTop==0 && MapConstructionSeedIndex==255);
    uint8_t oldSeed=MapConstructionSeeds[255]; MapConstructionSeeds[255]=3;
    lattice[0]=100; lattice[2]=100; lattice[1]=255;
    MapSubdivisionStack[0]=0; MapSubdivisionStack[1]=2; stackTop=2;
    Map_Subdivide_Horizontal_Edge(lattice,&stackTop);
    check(lattice[1]==101 && MapConstructionSeedIndex==0 && stackTop==0);
    MapConstructionSeedIndex=255;
    lattice[0]=100; lattice[18]=100; lattice[9]=255;
    MapSubdivisionStack[0]=0; MapSubdivisionStack[1]=18; stackTop=2;
    Map_Subdivide_Vertical_Edge(lattice,&stackTop);
    check(lattice[9]==101 && MapConstructionSeedIndex==0 && stackTop==0);
    MapConstructionSeeds[255]=oldSeed;
    /* Procedural output drives actual adjacency, tile-cache and path methods. */
    uint8_t descriptor[64]; memset(lattice,0,sizeof lattice);
    lattice[0]=lattice[8]=lattice[72]=lattice[80]=32;
    Map_Build_Procedural_Block(descriptor,lattice);
    memset(MapAdjacencyCache,0,sizeof MapAdjacencyCache);
    for (unsigned slot=0;slot<9;++slot) memcpy(MapDescriptorCache+slot*64,descriptor,64);
    Map_Rebuild_Adjacency();
    CrescentHawkMapPositionX=0x0248; CrescentHawkMapPositionY=0x3048;
    PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
    check(CachedMapPositionX==0x0248 && CachedMapPositionY==0x3048);
    ArenaRentalMechMode=0; BlockingTileCodeThreshold=255;
    check(Combat_Check_Terrain_Path(0,12,0x024B,0x3048)==1);
    BlockingTileCodeThreshold=0;
    check(Combat_Check_Terrain_Path(0,12,0x024B,0x3048)==0);
    printf("Original procedural block: %u checks passed\n",checks); return 0;
}
