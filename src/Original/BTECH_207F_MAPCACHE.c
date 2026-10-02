#include "game.h"

/* Sol: Original207F:1314..13D8. The native unrolled nine calls are expressed
 * as three rows/columns with identical coordinates, output bands and metadata.
 * Local destinationIndex represents the saved DI, not a research CPU global. */
void PosXY_OffsetGrid(uint16_t packedX,uint16_t packedY)
{
    CachedMapPositionX=packedX; CachedMapPositionY=packedY;
    LocalMapBlockX=(uint8_t)packedX>>4; LocalMapBlockY=(uint8_t)packedY>>4;
    uint16_t destinationIndex=0;
    for (uint16_t row=0;row<MapNeighbourhoodWidth;++row) {
        for (uint16_t column=0;column<MapNeighbourhoodWidth;++column)
            LocalTerrainFlags[row*MapNeighbourhoodWidth+column]=PosXY_GridSegment(
                (uint8_t)(LocalMapBlockX+column-1),
                (uint8_t)(LocalMapBlockY+row-1),&destinationIndex);
        if (row!=MapNeighbourhoodWidth-1)
            destinationIndex+=(MapBlockHeight-1)*MapCacheWidth;
    }
}

/* Sol: OriginalNEAR207F:13D9..158B. AL/BL inputs become explicit BYTE
 * parameters; in/out DI becomes a cache-relative WORD index reference.
 * Intended cache/template RAM views, not a replacement generic decoder. */
uint8_t PosXY_GridSegment(uint8_t blockX,uint8_t blockY,uint16_t *destinationIndex)
{
    uint8_t phase=(uint8_t)(blockX+blockY);
    MapBlockReflection=phase&(MapReflectionHorizontal|MapReflectionVertical);
    MapBlockPhase=(uint8_t)(phase<<2)&0x10; /* Native AH phase bit, not used by tile loop. */
    /* Centre grid begins at slot4. BYTE row*8 wrap and FF/8 neighbour
     * redirections reproduce SI424h/39Ch/4E4h without a raw memory helper. */
    uint16_t adjacencyIndex=(uint16_t)(4*MapBlockTileCount+(uint8_t)(blockY*MapBlockWidth));
    if (blockY==MapBlockBeforeFirst) adjacencyIndex=1*MapBlockTileCount+(MapBlockHeight-1)*MapBlockWidth;
    if (blockY==MapBlockColumnEnd) adjacencyIndex=7*MapBlockTileCount;
    uint8_t localX=blockX;
    if (blockX==MapBlockBeforeFirst) { localX=MapBlockWidth-1; adjacencyIndex-=MapBlockTileCount; }
    if (blockX==MapBlockColumnEnd) { localX=0; adjacencyIndex+=MapBlockTileCount; }
    adjacencyIndex+=localX;
    uint8_t selector=MapAdjacencyCache[adjacencyIndex];
    uint8_t terrain=MapDescriptorCache[adjacencyIndex];
    MapRenderSelector=selector; MapRenderTerrain=terrain;
    uint16_t templateIndex=(uint16_t)(selector*MapBlockTileCount);
    uint8_t terrainBase=terrain;
    if (!(terrain&PackedPositionLocalCarryBit)) {
        if (terrain==MapTerrainSpecialDescriptor) terrainBase=MapTerrainSpecialTileBase;
        else {
            if (terrain!=0) {
                terrainBase=(uint8_t)(terrain-MapTerrainCategoryStride);
                if (terrainBase>MapTerrainLastCategoryBase) terrainBase=MapTerrainLastCategoryBase;
            }
            uint8_t reflection=MapBlockReflection;
            if (reflection!=0) {
                uint8_t reflectedSelector=selector;
                if (reflection&MapReflectionVertical) reflectedSelector=MapVerticalReflection[reflectedSelector];
                if (reflection&MapReflectionHorizontal) reflectedSelector=MapHorizontalReflection[reflectedSelector];
                uint16_t sourceIndex=(uint16_t)(reflectedSelector*MapBlockTileCount);
                for (uint16_t row=0;row<MapBlockHeight;++row)
                    for (uint16_t column=0;column<MapBlockWidth;++column) {
                        uint8_t tile=MapTemplateData[sourceIndex++];
                        if (tile<MapTemplateFixedTileFirst) {
                            if (reflection&MapReflectionVertical) tile=MapVerticalReflection[tile];
                            if (reflection&MapReflectionHorizontal) tile=MapHorizontalReflection[tile];
                        }
                        uint16_t outputRow=(reflection&MapReflectionVertical)?MapBlockHeight-1-row:row;
                        uint16_t outputColumn=(reflection&MapReflectionHorizontal)?MapBlockWidth-1-column:column;
                        MapReflectedBlock[outputRow*MapBlockWidth+outputColumn]=tile;
                    }
                templateIndex=MapReflectedBlockOffset;
            }
        }
    }
    if (terrainBase==MapTerrainSpecialTileBase) templateIndex=MapSpecialBlockOffset;
    /* Native CLD followed by forward MOVSW/LODSB/STOSB. C has no CPU DF;
     * all these accesses are explicitly forward. No inherited host DF. */
    uint16_t outputIndex=*destinationIndex;
    for (uint16_t row=0;row<MapBlockHeight;++row) {
        for (uint16_t column=0;column<MapBlockWidth;++column) {
            uint8_t tile=MapTemplateData[templateIndex++];
            if (!(terrainBase&PackedPositionLocalCarryBit) && terrainBase!=MapTerrainSpecialTileBase && tile<MapTemplateFixedTileFirst)
                tile=(uint8_t)((tile&MapTemplateVariantMask)+terrainBase);
            CombatMap[outputIndex++]=tile;
        }
        outputIndex+=MapCacheWidth-MapBlockWidth;
    }
    *destinationIndex=(uint16_t)(outputIndex-(MapBlockHeight*MapCacheWidth-MapBlockWidth));
    return MapRenderSelector|MapRenderTerrain;
}
