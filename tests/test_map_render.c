#include "game.h"
#include "backend.h"
#include <assert.h>
#include <stdio.h>
static uint8_t pixels[PackedGraphicsOutputBytes];
int main(void)
{
    for (unsigned i=0;i<sizeof(pixels);++i) pixels[i]=(uint8_t)(i*17+(i>>5)*23);
    DrawCall_Image_To_VGA_Memory(pixels,MapTilesetSegment);
    for (unsigned i=0;i<MapCacheTileCount;++i) CombatMap[i]=(uint8_t)((i*37+i/24)%240);
    for (uint16_t localY=0;localY<16;++localY)
        for (uint16_t localX=0;localX<16;++localX) {
            CrescentHawkMapPositionX=(uint16_t)(0x0500+localX);
            CrescentHawkMapPositionY=(uint16_t)(0x5000+localY);
            Copy_Data_To_GraphicsMemory();
            unsigned originRow=(localY>>1)+2,originColumn=(localX>>1)+2;
            assert(CachedMapOriginIndex==originRow*MapCacheWidth+originColumn);
            assert(MapTileDestinationWidth==2 && MapFullBandGap==613 && MapHalfBandGap==293);
            assert(MapFullBandsRemaining==0 && MapCopyHalfHeight==!(localY&1));
            assert(MapCopyLeftHalf==!(localX&1) && MapCopyBottomHalf==0);
            assert(SDLBackend_EgaWriteMode==2 && SDLBackend_EgaBitMask==0);
            /* Independent viewport geometry, not the parent's band/helper schedule. */
            for (uint16_t row=0;row<EgaScreenHeight;++row)
                for (uint16_t column=0;column<EgaFramebufferRowBytes;++column)
                    for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
                        uint8_t expected=0;
                        if (column>=MapViewportLeftByte) {
                            unsigned tileY=row+(localY&1)*MapTileHalfHeight;
                            unsigned tileX=column-MapViewportLeftByte+(localX&1);
                            uint8_t tile=CombatMap[(originRow+tileY/MapTileHeight)*MapCacheWidth+
                                originColumn+tileX/MapTileByteWidth];
                            uint16_t source=(uint16_t)(tile*32+(tileY%MapTileHeight)*2+tileX%2);
                            expected=SDLBackend_ReadEgaPlaneByte(MapTilesetSegment,source,plane);
                        }
                        assert(SDLBackend_ReadEgaPlaneByte(MapViewportSegment,
                            (uint16_t)(row*EgaFramebufferRowBytes+column),plane)==expected);
                    }
        }
    for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
        assert(SDLBackend_ReadEgaPlaneByte(MapViewportSegment,EgaScreenPlaneBytes,plane)==0);
    puts("Original map viewport:256 local offsets, all plane bytes, edge halves and guards passed.");
    return 0;
}
