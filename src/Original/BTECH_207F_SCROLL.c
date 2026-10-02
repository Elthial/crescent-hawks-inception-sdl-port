#include "game.h"

/* Sol: Original207F:16E3..17C4. Middle->right BEFORE left->middle,
 * with32 forward WORDs per block, preserving the retained columns. */
void Map_Move_West(void)
{
    uint8_t local=(uint8_t)((uint8_t)CrescentHawkMapPositionX-1);
    uint8_t page=(uint8_t)(CrescentHawkMapPositionX>>8);
    if (!(local&PackedPositionLocalCarryBit)) {
        CrescentHawkMapPositionX=(uint16_t)((page<<8)|local); return;
    }
    if (page==0) return;
    CrescentHawkMapPositionX=(uint16_t)(((uint8_t)(page-1)<<8)|PackedPositionLocalMask);
    for (uint16_t row=0;row<MapNeighbourhoodWidth;++row) {
        uint16_t base=row*MapNeighbourhoodWidth*MapBlockTileCount;
        for (uint16_t index=0;index<MapBlockTileCount;++index)
            MapDescriptorCache[base+2*MapBlockTileCount+index]=MapDescriptorCache[base+MapBlockTileCount+index];
        for (uint16_t index=0;index<MapBlockTileCount;++index)
            MapDescriptorCache[base+MapBlockTileCount+index]=MapDescriptorCache[base+index];
    }
    uint16_t region=(uint8_t)((CrescentHawkMapPositionX|CrescentHawkMapPositionY)>>8);
    uint16_t vertex=(uint16_t)(offsetof(MapStaticStorage,worldVertices)+region-MapRegionPreviousRowAndColumn);
    for (uint16_t row=0;row<MapNeighbourhoodWidth;++row) {
        Map_Copy_Vertex_Corners((uint16_t)(vertex+row*MapRegionRowStride),MapConstructionLattice);
        Map_Build_Procedural_Block(MapDescriptorCache+row*MapNeighbourhoodWidth*MapBlockTileCount,MapConstructionLattice);
    }
    Map_Rebuild_Adjacency();
    for (uint16_t entry=0;entry<MapNeighbourhoodWidth;++entry) {
        PendingMapGridSlot[entry]=(uint8_t)(entry*MapNeighbourhoodWidth);
        PendingMapRegionIndex[entry]=(uint8_t)(region-MapRegionPreviousRowAndColumn+entry*MapRegionRowStride);
    }
}

/* Sol: Original207F:17C5..1885.64 forward WORDs per cache row transfer
 * middle/right->left/middle. Expose column2, then rebuild all adjacency. */
void Map_Move_East(void)
{
    uint8_t local=(uint8_t)((uint8_t)CrescentHawkMapPositionX+1);
    uint8_t page=(uint8_t)(CrescentHawkMapPositionX>>8);
    if (!(local&PackedPositionLocalCarryBit)) {
        CrescentHawkMapPositionX=(uint16_t)((page<<8)|local); return;
    }
    if (page==0x0F) return; /* Last coarse world column. */
    CrescentHawkMapPositionX=(uint16_t)((uint8_t)(page+1)<<8);
    for (uint16_t row=0;row<MapNeighbourhoodWidth;++row) {
        uint16_t base=row*MapNeighbourhoodWidth*MapBlockTileCount;
        for (uint16_t index=0;index<2*MapBlockTileCount;++index)
            MapDescriptorCache[base+index]=MapDescriptorCache[base+MapBlockTileCount+index];
    }
    uint16_t region=(uint8_t)((CrescentHawkMapPositionX|CrescentHawkMapPositionY)>>8);
    uint16_t vertex=(uint16_t)(offsetof(MapStaticStorage,worldVertices)+region-(MapRegionRowStride-1));
    for (uint16_t row=0;row<MapNeighbourhoodWidth;++row) {
        Map_Copy_Vertex_Corners((uint16_t)(vertex+row*MapRegionRowStride),MapConstructionLattice);
        Map_Build_Procedural_Block(MapDescriptorCache+(row*MapNeighbourhoodWidth+2)*MapBlockTileCount,MapConstructionLattice);
    }
    Map_Rebuild_Adjacency();
    for (uint16_t entry=0;entry<MapNeighbourhoodWidth;++entry) {
        PendingMapGridSlot[entry]=(uint8_t)(entry*MapNeighbourhoodWidth+2);
        PendingMapRegionIndex[entry]=(uint8_t)(region-(MapRegionRowStride-1)+entry*MapRegionRowStride);
    }
}

/* Sol: Original207F:158C..163A. Native north crossing uses backward192
 * WORDs followed by CLD; explicit backward bytes retain intended overlap. */
void Map_Move_North(void)
{
    uint8_t local=(uint8_t)((uint8_t)CrescentHawkMapPositionY-1);
    uint8_t page=(uint8_t)(CrescentHawkMapPositionY>>8);
    if (!(local&PackedPositionLocalCarryBit)) {
        CrescentHawkMapPositionY=(uint16_t)((page<<8)|local); return;
    }
    if (page==0) return;
    CrescentHawkMapPositionY=(uint16_t)(((uint8_t)(page-MapRegionRowStride)<<8)|PackedPositionLocalMask);
    for (int index=2*MapNeighbourhoodWidth*MapBlockTileCount-1;index>=0;--index)
        MapDescriptorCache[MapNeighbourhoodWidth*MapBlockTileCount+index]=MapDescriptorCache[index];
    uint16_t region=(uint8_t)((CrescentHawkMapPositionX|CrescentHawkMapPositionY)>>8);
    uint16_t vertex=(uint16_t)(offsetof(MapStaticStorage,worldVertices)+region-MapRegionPreviousRowAndColumn);
    for (uint16_t column=0;column<MapNeighbourhoodWidth;++column) {
        Map_Copy_Vertex_Corners((uint16_t)(vertex+column),MapConstructionLattice);
        Map_Build_Procedural_Block(MapDescriptorCache+column*MapBlockTileCount,MapConstructionLattice);
    }
    Map_Rebuild_Adjacency();
    for (uint16_t entry=0;entry<MapNeighbourhoodWidth;++entry) {
        PendingMapGridSlot[entry]=(uint8_t)entry;
        PendingMapRegionIndex[entry]=(uint8_t)(region-MapRegionPreviousRowAndColumn+entry);
    }
}

/* Sol: Original207F:163B..16E2. Forward192 WORDs, then expose bottom row. */
void Map_Move_South(void)
{
    uint8_t local=(uint8_t)((uint8_t)CrescentHawkMapPositionY+1);
    uint8_t page=(uint8_t)(CrescentHawkMapPositionY>>8);
    if (!(local&PackedPositionLocalCarryBit)) {
        CrescentHawkMapPositionY=(uint16_t)((page<<8)|local); return;
    }
    if (page==0xF0) return; /* Last coarse world row. */
    CrescentHawkMapPositionY=(uint16_t)((uint8_t)(page+MapRegionRowStride)<<8);
    for (uint16_t index=0;index<2*MapNeighbourhoodWidth*MapBlockTileCount;++index)
        MapDescriptorCache[index]=MapDescriptorCache[MapNeighbourhoodWidth*MapBlockTileCount+index];
    uint16_t region=(uint8_t)((CrescentHawkMapPositionX|CrescentHawkMapPositionY)>>8);
    uint16_t vertex=(uint16_t)(offsetof(MapStaticStorage,worldVertices)+region+MapRegionRowStride-1);
    for (uint16_t column=0;column<MapNeighbourhoodWidth;++column) {
        Map_Copy_Vertex_Corners((uint16_t)(vertex+column),MapConstructionLattice);
        Map_Build_Procedural_Block(MapDescriptorCache+(2*MapNeighbourhoodWidth+column)*MapBlockTileCount,MapConstructionLattice);
    }
    Map_Rebuild_Adjacency();
    for (uint16_t entry=0;entry<MapNeighbourhoodWidth;++entry) {
        PendingMapGridSlot[entry]=(uint8_t)(2*MapNeighbourhoodWidth+entry);
        PendingMapRegionIndex[entry]=(uint8_t)(region+MapRegionRowStride-1+entry);
    }
}
