#include "game.h"

/* Original207F:1F04..1F50. Sol: Reduce centre descriptor/adjacency block to
 * eight overview rows. Dynamic descriptors pass through;10h has its own tile
 * and bypasses adjacency. Caller supplies an8x8 destination within workspace. */
void Overhead_Map_Build_Centre_Block(uint16_t destination)
{
    uint16_t source=MapBlockTileCount*MapCacheSlot_Centre;
    for(uint16_t row=0;row<MapBlockWidth;++row) {
        for(uint16_t column=0;column<MapBlockWidth;++column) {
            uint8_t tile=MapDescriptorCache[source];
            if(tile<OverviewDynamicFirstTile) {
                if(tile==OverviewDedicatedTerrainCategory) tile=OverviewDedicatedTerrainTile;
                else {
                    if(tile>=OverviewTerrainRemapThreshold) tile=(uint8_t)(tile-OverviewTerrainCategoryStep);
                    tile|=MapAdjacencyCache[source];
                }
            }
            GraphicsFileWorkspace[destination++]=tile; ++source;
        }
        destination=(uint16_t)(destination+OverviewBufferColumns-MapBlockWidth);
    }
}

/* Original207F:1F51..1F9B. Sol: Two tile-colour XLAT loads per packed BYTE.
 * Preserve WORD subtract/swap/shifts/AH addition, not a generalized formula.
 * Shipped dynamic BYTE IDs90..FF and valid scratch extent are the contract. */
void Pack_Dynamic_Overhead_Tile(uint16_t tileId)
{
    uint16_t index=(uint16_t)(tileId-OverviewDynamicFirstTile);
    uint16_t swapped=(uint16_t)((index<<8)|(index>>8));
    uint16_t source=swapped>>2;
    source=(uint16_t)((source&0xFF)|((uint16_t)(uint8_t)((source>>8)+OverviewScratchHighByte)<<8));
    for(uint16_t byte=0;byte<OverviewPackedTileBytes;++byte) {
        uint8_t first=OverviewTileColours[GraphicsFileWorkspace[source++]];
        uint8_t second=OverviewTileColours[GraphicsFileWorkspace[source++]];
        GraphicsFileWorkspace[OverviewPackedTileOffset+byte]=(uint8_t)((first<<4)|second);
    }
}

/* Original0800:45C2..4620, maintained EGA-only path.16 WORDs means32 bytes. */
void Build_Dynamic_Overhead_Tile(uint16_t tileId)
{
    Pack_Dynamic_Overhead_Tile(tileId);
    if(GraphicsAdapter==GraphicsAdapter_Ega)
        VGA_Inline_ASM_Loop(GraphicsFileWorkspace+OverviewPackedTileOffset,
            GraphicsFileWorkspace+OverviewPackedTileOffset,OverviewPackedTileBytes/sizeof(uint16_t));
}
