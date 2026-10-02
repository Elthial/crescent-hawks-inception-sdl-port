#include "game.h"
#include "backend.h"
#include <assert.h>
#include <stdio.h>
static uint8_t tiles[8*BorderTileBytes];
static uint8_t blankImage[32000];

static void clear(void)
{
    SDLBackend_EgaMapMask=15;
    SDLBackend_EgaRasterOperation=EgaRaster_Replace;
    SDLBackend_EgaRotateCount=0;
    SDLBackend_TransferPackedImage(blankImage,EgaScreenSegment);
}
int main(void)
{
    for (unsigned i=0;i<sizeof tiles;++i) tiles[i]=(uint8_t)(i*37+11);
    BorderTileset=tiles;
    TextPanelLeft=2; TextPanelTop=3; TextPanelWidth=5; TextPanelHeight=2;
    for (uint16_t style=0;style<BorderStyleCount;++style) {
        clear();
        SDLBackend_EgaMapMask=0; SDLBackend_EgaEnableSetReset=15;
        Draw_Menu_Border(style);
        for (uint16_t y=0;y<EgaScreenHeight;++y)
            for (uint16_t column=0;column<EgaFramebufferRowBytes;++column) {
                uint16_t cellRow=(uint16_t)(y/8);
                int tile=-1;
                if (cellRow==2) {
                    if (column==1) tile=style==3?2:0;
                    else if (column==7) tile=style==3?3:1;
                    else if (column>=2 && column<=6) tile=4;
                } else if (cellRow==5) {
                    if (column==1) tile=style==4?2:6;
                    else if (column==7) tile=style==4?3:7;
                    else if (column>=2 && column<=6) tile=4;
                } else if ((cellRow==3 || cellRow==4) && (column==1 || column==7)) tile=5;
                for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
                    uint8_t expected=tile<0?0:tiles[tile*BorderTileBytes+(y%8)*4+plane];
                    assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,
                        (uint16_t)(y*EgaFramebufferRowBytes+column),plane)==expected);
                }
            }
        assert(SDLBackend_EgaWriteMode==0 && SDLBackend_EgaBitMask==255 &&
            SDLBackend_EgaMapMask==15 && SDLBackend_EgaEnableSetReset==0);
    }
    clear(); SDLBackend_EgaRotateCount=3;
    DrawCall_SingleTile(tiles,39,24);
    for (uint16_t y=0;y<8;++y)
        for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
            uint8_t value=tiles[y*4+plane];
            uint8_t expected=(uint8_t)((value>>3)|(value<<5));
            assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,
                (uint16_t)(24*320+y*40+39),plane)==expected);
        }
    SDLBackend_EgaRasterOperation=EgaRaster_Xor;
    DrawCall_SingleTile(tiles,39,24);
    for (uint16_t y=0;y<8;++y)
        for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
            assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,
                (uint16_t)(24*320+y*40+39),plane)==0);
    assert(SDLBackend_EgaRotateCount==3 && SDLBackend_EgaRasterOperation==EgaRaster_Xor);
    /* Actual animated-tile upload inherits rotation/ROP but disables set/reset,
     * selects each plane, and wraps the destination address WORD. */
    uint8_t source[AnimatedMapFrameBytes],initial[AnimatedMapFrameBytes];
    for(unsigned i=0;i<AnimatedMapFrameBytes;++i) {
        source[i]=(uint8_t)(i*29+3); initial[i]=(uint8_t)(i*17+5);
    }
    static const uint16_t offsets[]={87*AnimatedMapTilePlaneBytes,0xFFFE,0xF000};
    for(unsigned fixture=0;fixture<3;++fixture)
        for(uint8_t operation=0;operation<4;++operation)
            for(uint8_t rotate=0;rotate<8;++rotate) {
                SDLBackend_EgaRasterOperation=EgaRaster_Replace; SDLBackend_EgaRotateCount=0;
                EGA_Upload_Animated_Tile(initial,offsets[fixture]);
                SDLBackend_EgaRasterOperation=operation; SDLBackend_EgaRotateCount=rotate;
                SDLBackend_EgaMapMask=0; SDLBackend_EgaEnableSetReset=15;
                SDLBackend_EgaBitMask=0; SDLBackend_EgaWriteMode=2;
                EGA_Upload_Animated_Tile(source,offsets[fixture]);
                for(unsigned column=0;column<AnimatedMapTilePlaneBytes;++column)
                    for(uint8_t plane=0;plane<EgaPlaneCount;++plane) {
                        unsigned i=column*EgaPlaneCount+plane;
                        uint8_t expected=source[i];
                        if(rotate) expected=(uint8_t)((expected>>rotate)|(expected<<(8-rotate)));
                        if(operation==EgaRaster_And) expected&=initial[i];
                        else if(operation==EgaRaster_Or) expected|=initial[i];
                        else if(operation==EgaRaster_Xor) expected^=initial[i];
                        assert(SDLBackend_ReadEgaPlaneByte(MapTilesetSegment,
                            (uint16_t)(offsets[fixture]+column),plane)==expected);
                    }
                assert(SDLBackend_EgaMapMask==15 && SDLBackend_EgaWriteMode==0 &&
                    SDLBackend_EgaEnableSetReset==0 && SDLBackend_EgaBitMask==255);
                assert(SDLBackend_EgaRotateCount==rotate && SDLBackend_EgaRasterOperation==operation);
            }
    for(unsigned i=0;i<AnimatedMapTileBytes;++i) AnimatedMapTileFrames[i]=(uint8_t)(i*31+(i>>7));
    SDLBackend_EgaRasterOperation=EgaRaster_Replace; SDLBackend_EgaRotateCount=0;
    TilesetId=Tileset_BattleTech; AnimatedMapTileFrame=0;
    for(unsigned call=0;call<3;++call) {
        Update_Animated_Map_Tiles();
        for(unsigned tile=0;tile<AnimatedMapTileCount;++tile)
            for(unsigned column=0;column<AnimatedMapTilePlaneBytes;++column)
                for(uint8_t plane=0;plane<EgaPlaneCount;++plane) {
                    unsigned i=tile*AnimatedMapFrameCount*AnimatedMapFrameBytes
                        +AnimatedMapTileFrame*AnimatedMapFrameBytes+column*4+plane;
                    assert(SDLBackend_ReadEgaPlaneByte(MapTilesetSegment,
                        (uint16_t)(AnimatedMapTileId[tile]*AnimatedMapTilePlaneBytes+column),plane)==AnimatedMapTileFrames[i]);
                }
    }
    puts("Original borders, tile transfers and three-frame animated uploads passed");
    return 0;
}
