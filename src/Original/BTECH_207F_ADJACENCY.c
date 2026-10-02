#include "game.h"

/* Sol: Original207F:1DA8..1DBA simply calls1886; no new descriptor build. */
void Map_NineGrid_Parent(void)
{
    Map_Rebuild_Adjacency();
}

/* Sol: Original207F:104E..11BA. Nine row-major blocks, then native1886.
 * Static-storage indices preserve legal region0..255 border reads into the
 * adjoining seed/template bytes, not invented clamping or wraparound maps. */
void Map_Construct_Nine_Regions(uint16_t centreRegion)
{
    uint16_t firstVertex=(uint16_t)(offsetof(MapStaticStorage,worldVertices)+centreRegion-MapRegionPreviousRowAndColumn);
    for (uint16_t row=0;row<MapNeighbourhoodWidth;++row)
        for (uint16_t column=0;column<MapNeighbourhoodWidth;++column) {
            Map_Copy_Vertex_Corners((uint16_t)(firstVertex+row*MapRegionRowStride+column),MapConstructionLattice);
            Map_Build_Procedural_Block(MapDescriptorCache+(row*MapNeighbourhoodWidth+column)*MapBlockTileCount,MapConstructionLattice);
        }
    Map_Rebuild_Adjacency();
}

/* Sol: OriginalNEAR207F:18D8..18EE. Four ordered BYTE load/store pairs. */
void Map_Copy_Vertex_Corners(uint16_t sourceIndex,uint8_t *lattice)
{
    uint8_t *vertices=(uint8_t *)&MapStaticData;
    lattice[0]=vertices[sourceIndex];
    lattice[MapCornerTopRight]=vertices[sourceIndex+1];
    lattice[MapCornerBottomLeft]=vertices[sourceIndex+MapRegionRowStride];
    lattice[MapCornerBottomRight]=vertices[sourceIndex+MapRegionRowStride+1];
}

/* Sol: OriginalNEAR207F:1886..18D7. Each call reloads both SI/DI offsets. */
void Map_Rebuild_Adjacency(void)
{
    for (uint16_t slot=0;slot<MapNeighbourhoodCount;++slot)
        Map_Build_Adjacency_Block((uint16_t)(offsetof(MapCacheStorage,descriptors)+slot*MapBlockTileCount),
            (uint16_t)(offsetof(MapCacheStorage,adjacency)+slot*MapBlockTileCount));
}

/* Sol: OriginalNEAR207F:11BB..12B9. Source/destination are indices into the
 * checked BYTE storage, modelling native SI/DI without research CPU globals.
 * Outer descriptor reads reach adjoining arrays, NOT invented border tiles.
 * Special80h computes equality only on top/bottom interior columns. */
void Map_Build_Adjacency_Block(uint16_t descriptorIndex,uint16_t adjacencyIndex)
{
    uint8_t *memory=(uint8_t *)&MapCache;
    for (uint16_t row=0;row<MapBlockHeight;++row)
        for (uint16_t column=0;column<MapBlockWidth;++column) {
            uint16_t index=(uint16_t)(descriptorIndex+row*MapBlockWidth+column);
            uint8_t value=memory[index],flags=0;
            if (column==0) flags=Map_Adjacency_Left_Edge(index);
            else if (column==MapBlockWidth-1) flags=Map_Adjacency_Right_Edge(index);
            else if (value&PackedPositionLocalCarryBit) flags=(uint8_t)(value-PackedPositionLocalCarryBit);
            uint16_t computeEquality=!(value&PackedPositionLocalCarryBit) ||
                (value==PackedPositionLocalCarryBit && (row==0 || row==MapBlockHeight-1) && column!=0 && column!=MapBlockWidth-1);
            if (computeEquality) {
                if (column!=0 && column!=MapBlockWidth-1) {
                    if (value==memory[index-1]) flags|=MapAdjacency_West;
                    if (value==memory[index+1]) flags|=MapAdjacency_East;
                }
                if (row==0) flags=Map_Adjacency_Top_Edge(index,value,flags);
                else if (row==MapBlockHeight-1) flags=Map_Adjacency_Bottom_Edge(index,value,flags);
                else {
                    if (value==memory[index+MapBlockWidth]) flags|=MapAdjacency_South;
                    if (value==memory[index-MapBlockWidth]) ++flags;
                }
            }
            memory[adjacencyIndex+row*MapBlockWidth+column]=flags;
        }
}
uint8_t Map_Adjacency_Left_Edge(uint16_t index) /* 207F:12BA..12D8 */
{
    uint8_t *memory=(uint8_t *)&MapCache;
    uint8_t value=memory[index],flags=0;
    if (value&PackedPositionLocalCarryBit) return (uint8_t)(value-PackedPositionLocalCarryBit);
    if (value==memory[index-MapAdjacentBlockColumnDelta]) flags|=MapAdjacency_West;
    if (value==memory[index+1]) flags|=MapAdjacency_East;
    return flags;
}
uint8_t Map_Adjacency_Right_Edge(uint16_t index) /* 207F:12D9..12F1, shared12D3 exit */
{
    uint8_t *memory=(uint8_t *)&MapCache;
    uint8_t value=memory[index],flags=0;
    if (value&PackedPositionLocalCarryBit) return (uint8_t)(value-PackedPositionLocalCarryBit);
    if (value==memory[index-1]) flags|=MapAdjacency_West;
    if (value==memory[index+MapAdjacentBlockColumnDelta]) flags|=MapAdjacency_East;
    return flags;
}
uint8_t Map_Adjacency_Top_Edge(uint16_t index,uint8_t value,uint8_t flags) /* 207F:12F2..1302 */
{
    uint8_t *memory=(uint8_t *)&MapCache;
    if (value==memory[index-MapAdjacentBlockRowDelta]) ++flags;
    if (value==memory[index+MapBlockWidth]) flags|=MapAdjacency_South;
    return flags;
}
uint8_t Map_Adjacency_Bottom_Edge(uint16_t index,uint8_t value,uint8_t flags) /* 207F:1303..1313 */
{
    uint8_t *memory=(uint8_t *)&MapCache;
    if (value==memory[index-MapBlockWidth]) ++flags;
    if (value==memory[index+MapAdjacentBlockRowDelta]) flags|=MapAdjacency_South;
    return flags;
}
