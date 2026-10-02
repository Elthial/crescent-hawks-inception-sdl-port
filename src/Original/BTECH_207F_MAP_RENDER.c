#include "game.h"
#include "../SDL/backend.h"

/* Sol: Original207F:18EF..1AA7 retained EGA composition, full ASM checked.
 * Native CLD is represented by explicit forward accesses in all child loops.
 * No CPU FLAGS emulation or research framebuffer helper is copied. */
void Copy_Data_To_GraphicsMemory(void)
{
    SDLBackend_EgaWriteMode=2;
    SDLBackend_EgaBitMask=0;
	
	
	MapTileDestinationWidth = MapTileByteWidth; //16 pixels/8
	MapFullBandGap = MapTileHeight*EgaFramebufferRowBytes-MapViewportByteWidth; //640 (16*40) minus27 byte-columns written
	MapHalfBandGap = MapTileHalfHeight*EgaFramebufferRowBytes-MapViewportByteWidth; //320 (8*40) minus27
	uint16_t destinationOffset = MapViewportLeftByte; //104px X;27 byte-columns to right screen edge
	uint16_t originRow = (((uint8_t)CrescentHawkMapPositionY >> 1) & MapCacheLocalBlockMask) + MapCacheOriginMargin;
	uint16_t originColumn = (((uint8_t)CrescentHawkMapPositionX >> 1) & MapCacheLocalBlockMask) + MapCacheOriginMargin;
	CachedMapOriginIndex = originRow * MapCacheWidth + originColumn;
	uint16_t cacheOffset = CachedMapOriginIndex; // address, NOT contents of first cell
	uint16_t oddX = CrescentHawkMapPositionX & 1;
	uint16_t oddY = CrescentHawkMapPositionY & 1;
	
	MapCopyHalfHeight = 0;
	if (oddY)
	{
		if (oddX)
		{
			MapCopyLeftHalf = 0; // left viewport edge uses tile's RIGHT half
			MapCopyHalfHeight = 1;
			MapCopyBottomHalf = 1;
			Graphic_Memory_To_Destination(CombatMap[cacheOffset++], destinationOffset, MapViewportSegment);
			++destinationOffset;
		}
		for (uint16_t tileColumn = 0; tileColumn < MapViewportFullTileColumns; ++tileColumn)
		{
			Graphic_Memory_Lower_Half_To_Destination(CombatMap[cacheOffset++], destinationOffset, 0xAC00); // bottom8 rows of tile
			destinationOffset += MapTileByteWidth;
		}
		if (!oddX)
		{
			MapCopyHalfHeight = 1;
			MapCopyLeftHalf = 1;
			MapCopyBottomHalf = 1;
			Graphic_Memory_To_Destination(CombatMap[cacheOffset++], destinationOffset, 0xAC00);
			++destinationOffset;
		}
		cacheOffset += MapCacheWidth-(MapViewportFullTileColumns+1); //24-byte cache row minus14 consumed cells
		destinationOffset += MapHalfBandGap;
	}
	MapCopyHalfHeight = 0;
	MapCopyLeftHalf = 0;
	MapCopyBottomHalf = 0;
	MapFullBandsRemaining = MapViewportFullBands; //192px middle plus8px edge band =200px
	do
	{
		if (oddX)
		{
			MapCopyLeftHalf = 0;
			Graphic_Memory_To_Destination(CombatMap[cacheOffset++], destinationOffset, 0xAC00);
			++destinationOffset;
		}
		for (uint16_t tileColumn = 0; tileColumn < MapViewportFullTileColumns; ++tileColumn)
		{
			EGA_Copy_Tile_Full_Width_To_Destination(CombatMap[cacheOffset++], destinationOffset, 0xAC00);
			destinationOffset += MapTileByteWidth;
		}
		if (!oddX)
		{
			MapCopyLeftHalf = 1;
			Graphic_Memory_To_Destination(CombatMap[cacheOffset++], destinationOffset, 0xAC00);
			++destinationOffset;
		}
		cacheOffset += MapCacheWidth-(MapViewportFullTileColumns+1);
		destinationOffset += MapFullBandGap;
	} while (--MapFullBandsRemaining != 0);
	if (!oddY)
	{
		MapCopyHalfHeight = 1; //bottom viewport edge uses TOP8 rows
		if (oddX)
		{
			MapCopyLeftHalf = 0;
			Graphic_Memory_To_Destination(CombatMap[cacheOffset++], destinationOffset, 0xAC00);
			++destinationOffset;
		}
		for (uint16_t tileColumn = 0; tileColumn < MapViewportFullTileColumns; ++tileColumn)
		{
			EGA_Copy_Tile_Full_Width_To_Destination(CombatMap[cacheOffset++], destinationOffset, 0xAC00);
			destinationOffset += MapTileByteWidth;
		}
		if (!oddX)
		{
			MapCopyLeftHalf = 1;
			Graphic_Memory_To_Destination(CombatMap[cacheOffset], destinationOffset, 0xAC00);
		}
	}
}


/* Original NEAR wrappers1AA8/1ACE/1AF4 preserve parent BX/DI. */
void Graphic_Memory_To_Destination(uint8_t tileId,uint16_t destination,uint16_t segment)
{
    EGA_Copy_A400_Memory_Word_Or_Byte_To_Destination((uint16_t)(tileId<<8),destination,segment);
}
void Graphic_Memory_Lower_Half_To_Destination(uint8_t tileId,uint16_t destination,uint16_t segment)
{
    EGA_MemoryCopy_Byte((uint16_t)(tileId<<8),destination,segment);
}
void EGA_Copy_Tile_Full_Width_To_Destination(uint8_t tileId,uint16_t destination,uint16_t segment)
{
    EGA_Copy_Tile_Two_Columns_To_Destination((uint16_t)(tileId<<8),destination,segment);
}
/* Original1B71: two columns for16 or8 rows; parent establishes CLD. */
void EGA_Copy_Tile_Two_Columns_To_Destination(uint16_t graphicsOffset,uint16_t destination,uint16_t segment)
{
    uint16_t source=graphicsOffset>>3;
    uint16_t rows=MapCopyHalfHeight?MapTileHalfHeight:MapTileHeight;
    do {
        SDLBackend_CopyEgaLatchByte(MapTilesetSegment,source++,segment,destination++);
        SDLBackend_CopyEgaLatchByte(MapTilesetSegment,source++,segment,destination++);
        destination+=(EgaFramebufferRowBytes-MapTileByteWidth);
    } while (--rows!=0);
}
/* Original1BDF: lower8 rows, full width. */
void EGA_MemoryCopy_Byte(uint16_t graphicsOffset,uint16_t destination,uint16_t segment)
{
    uint16_t source=(uint16_t)((graphicsOffset>>3)+MapTileHalfHeight*MapTileByteWidth);
    for (uint16_t row=0;row<MapTileHalfHeight;++row) {
        SDLBackend_CopyEgaLatchByte(MapTilesetSegment,source++,segment,destination++);
        SDLBackend_CopyEgaLatchByte(MapTilesetSegment,source++,segment,destination++);
        destination+=(EgaFramebufferRowBytes-MapTileByteWidth);
    }
}
/* Original1C83: one side column; independent lower-half/height flags. */
void EGA_Copy_A400_Memory_Word_Or_Byte_To_Destination(uint16_t graphicsOffset,uint16_t destination,uint16_t segment)
{
    uint16_t source=graphicsOffset>>3;
    if (MapCopyBottomHalf) source+=MapTileHalfHeight*MapTileByteWidth;
    if (!MapCopyLeftHalf) ++source;
    uint16_t rows=MapCopyHalfHeight?MapTileHalfHeight:MapTileHeight;
    do {
        SDLBackend_CopyEgaLatchByte(MapTilesetSegment,source,segment,destination);
        source+=MapTileByteWidth;
        destination+=EgaFramebufferRowBytes;
    } while (--rows!=0);
}
