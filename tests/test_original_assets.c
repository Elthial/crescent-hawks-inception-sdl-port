/* Sol: Opt-in LOCAL integration test. Requires the owner's original assets,
 * never copies them. Real native loaders/decoders and SDL-backed file I/O.
 * Unconverted UI/drive selection are test adapters, not game implementations. */
#include "game.h"
#include "dos.h"
#include "backend.h"
#include "music.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks;
static uint8_t raw[65536], loaded[65536], graphics[PackedGraphicsOutputBytes+2];
static uint8_t capturedTiles[66*BorderTileBytes+2];
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Local asset check %u failed\n",checks); exit(1); }
}
void DOS_Select_Default_Drive(uint16_t drive) { check(drive <= DOSDrive_B); }
void Menu_Memory_Variables(uint16_t layout) { (void)layout; }
void Draw_Top_Graphic_Sidebar(void)
{
    fprintf(stderr,"Original asset open failed; run in the original game directory\n"); exit(1);
}
void Display_Text_From_Memory(uint8_t *text) { (void)text; }
void Drain_Pending_Keyboard_Input(void) { }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { return 0; }
/* Only pending05BC allocation boundary: actual capture and draw methods run.
 * Tile requests256/2112 and sprite292; no production allocator substitute. */
uint8_t *Allocate_Far_Buffer(uint32_t bytes)
{
    check(bytes==8*BorderTileBytes || bytes==66*BorderTileBytes || bytes==292);
    memset(capturedTiles,0xCC,sizeof capturedTiles);
    return capturedTiles+1;
}
static uint16_t readRaw(const uint8_t *filename)
{
    int16_t handle = Get_FileHandle(filename,UINT16_C(0x8000));
    check(handle != -1);
    int32_t size = DOS_set_file_position((uint16_t)handle,0,0,2);
    check(size >= 3 && size <= UINT16_MAX);
    check(DOS_set_file_position((uint16_t)handle,0,0,0) == 0);
    check((uint16_t)DOS_read_file_handler((uint16_t)handle,raw,(uint16_t)size) == size);
    check(DOS_close_file((uint16_t)handle) == 0);
    uint16_t payload = (uint16_t)(raw[0] | ((uint16_t)raw[1] << 8));
    check(payload == size-2); return payload;
}
int main(void)
{
    HasHardDisk = TRUE;
    for (uint16_t id = 0; id < BldFileCount; ++id) {
        uint16_t size = readRaw(BldFileNameById[id]); check(size <= BldFixedDecodeSpan);
        memset(BTStatsOrBldMemory,0xCC,sizeof BTStatsOrBldMemory); BTStatsAssetLoaded = TRUE;
        Load_And_Decode_Indexed_BLD(id);
        check(BldFileIndex == id && BTStatsAssetLoaded == FALSE);
        for (unsigned i = 0; i < BldFixedDecodeSpan; ++i) {
            unsigned encoded = i < size ? raw[i+2] : 0xCC;
            check(BTStatsOrBldMemory[i] == (((encoded+41)&255)^233));
        }
    }
    const char *images[] = {"ANIMATE.ICN","BTBORDER.CMP","BTSTATS.CMP","BTTITLE.CMP",
        "BTTLTECH.ICN","DESTRUCT.ICN","ENDMECH.CMP","INFOCOM.CMP","MAP.ICN",
        "MECHSHAP.CMP","STARLEAG.ICN","TINYLAND.CMP"};
    /* Independently computed using the existing InceptionTools decoder on
     * this local installation. Checksums only; no source asset is embedded. */
    const uint32_t expectedHashes[] = {UINT32_C(0xE5423B21),UINT32_C(0x06EFBE26),
        UINT32_C(0x61C885EC),UINT32_C(0x5E04241E),UINT32_C(0xC3537AD1),UINT32_C(0xC862B48B),
        UINT32_C(0xD3958EA3),UINT32_C(0xC19E71D3),UINT32_C(0x8C561185),UINT32_C(0xACFBE845),
        UINT32_C(0xB83E5CDC),UINT32_C(0x71BA2B76)};
    for (unsigned image = 0; image < sizeof images / sizeof images[0]; ++image) {
        uint16_t size = readRaw((const uint8_t *)images[image]);
        if (!strcmp(images[image],"BTSTATS.CMP"))
            check(size <= sizeof BTStatsOrBldMemory);
        memset(loaded,0xCC,sizeof loaded);
        check(Load_File_To_Memory((const uint8_t *)images[image],loaded) == TRUE);
        check(!memcmp(loaded,raw+2,size) && loaded[size] == 0xCC);
        memset(graphics,0xCC,sizeof graphics);
        Decompress_File_Into_Memory(loaded,graphics+1);
        check(graphics[0] == 0xCC && graphics[PackedGraphicsOutputBytes+1] == 0xCC);
        uint32_t hash = UINT32_C(2166136261);
        for (unsigned i = 1; i <= PackedGraphicsOutputBytes; ++i)
            hash = (hash ^ graphics[i]) * UINT32_C(16777619);
        check(hash == expectedHashes[image]);
        if (image==0) {
            Load_And_Draw_ANIMATE_ICN();
            check(TilesetId==Tileset_BattleTech && GraphicsCompatibilityFlag==TRUE);
            for (unsigned group=0;group<AnimatedMapTileBytes/4;++group)
                for (unsigned plane=0;plane<4;++plane) {
                    uint8_t expected=0;
                    for (unsigned pixel=0;pixel<8;++pixel) {
                        uint8_t packed=graphics[1+AnimatedMapTileHeaderBytes+group*4+pixel/2];
                        uint8_t colour=(uint8_t)((pixel&1)?packed&15:packed>>4);
                        if (colour&(1<<plane)) expected|=(uint8_t)(0x80>>pixel);
                    }
                    check(AnimatedMapTileFrames[group*4+plane]==expected);
                }
        }
        DrawCall_Image_To_VGA_Memory(graphics+1,EgaTilesetBufferSegment);
        for (uint16_t column=0;column<EgaScreenPlaneBytes;++column) {
            for (unsigned pixel=0;pixel<EgaPixelsPerByte;++pixel) {
                uint8_t colour=0;
                for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
                    if (SDLBackend_ReadEgaPlaneByte(EgaTilesetBufferSegment,column,plane)&(0x80>>pixel))
                        colour|=(uint8_t)(1<<plane);
                uint8_t expected=graphics[1+column*4+pixel/2];
                expected=(uint8_t)((pixel&1)?expected&15:expected>>4);
                check(colour==expected);
            }
        }
        if (!strcmp(images[image],"BTBORDER.CMP") || !strcmp(images[image],"TINYLAND.CMP")) {
            uint16_t count=(uint16_t)(!strcmp(images[image],"BTBORDER.CMP")?8:66);
            /* Native startup uploads each decoded asset atA800 before capture. */
            DrawCall_Image_To_VGA_Memory(graphics+1,EgaSceneStagingSegment);
            uint8_t *tiles=Create_TileSet_Array(graphics+1,0,0,count);
            check(tiles==capturedTiles+1 && capturedTiles[0]==0xCC && tiles[count*32]==0xCC);
            for (uint16_t tile=0;tile<count;++tile) {
                uint16_t cellColumn=(uint16_t)(tile%40),cellRow=(uint16_t)(tile/40);
                DrawCall_SingleTile(tiles+tile*32,cellColumn,cellRow);
                for (uint16_t y=0;y<8;++y)
                    for (uint8_t plane=0;plane<4;++plane) {
                        uint8_t expected=0;
                        for (unsigned bit=0;bit<8;++bit) {
                            unsigned pixel=(cellRow*8+y)*320+cellColumn*8+bit;
                            uint8_t packedByte=graphics[1+pixel/2];
                            uint8_t colour=(uint8_t)((pixel&1)?packedByte&15:packedByte>>4);
                            if (colour&(1<<plane)) expected|=(uint8_t)(0x80>>bit);
                        }
                        check(tiles[tile*32+y*4+plane]==expected);
                        check(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,
                            (uint16_t)(cellRow*320+y*40+cellColumn),plane)==expected);
                    }
            }
            printf("%s native capture/redraw: %u tiles confirmed\n",images[image],count);
        }
        /* Actual startup0572 conversion in-place, independently checked against
         * original packed pixels for every local decoded artwork file. */
        memcpy(loaded,graphics+1,PackedGraphicsOutputBytes);
        VGA_Inline_ASM_Loop(loaded,loaded,PackedGraphicsOutputBytes/2);
        for (unsigned group=0;group<PackedGraphicsOutputBytes/4;++group)
            for (unsigned plane=0;plane<4;++plane) {
                uint8_t expected=0;
                for (unsigned pixel=0;pixel<8;++pixel) {
                    uint8_t pair=graphics[1+group*4+pixel/2];
                    uint8_t colour=(uint8_t)((pixel&1)?pair&15:pair>>4);
                    if (colour&(1<<plane)) expected|=(uint8_t)(0x80>>pixel);
                }
                check(loaded[group*4+plane]==expected);
            }
        if (!strcmp(images[image],"MECHSHAP.CMP")) {
            /* Actual startup source buffer and first24x24 Locust snapshot. */
            memcpy(GraphicsFileWorkspace,loaded,PackedGraphicsOutputBytes);
            Capture_Combat_Sprite(0,0,0,3,24);
            uint8_t *sprite=CombatSpritePointers[0];
            check(sprite==capturedTiles+1 && sprite[0]==0xCC && sprite[1]==23 &&
                sprite[2]==3 && sprite[3]==0xCC);
            for (unsigned y=0;y<24;++y)
                for (unsigned column=0;column<3;++column)
                    for (unsigned plane=0;plane<4;++plane) {
                        uint8_t expected=0;
                        for (unsigned bit=0;bit<8;++bit) {
                            unsigned pixel=y*320+column*8+bit;
                            uint8_t pair=graphics[1+pixel/2];
                            uint8_t colour=(uint8_t)((pixel&1)?pair&15:pair>>4);
                            if (colour&(1<<plane)) expected|=(uint8_t)(0x80>>bit);
                        }
                        check(sprite[4+y*12+column*4+plane]==expected);
                    }
            check(sprite[292]==0xCC);
            memset(loaded,0,PackedGraphicsOutputBytes);
            DrawCall_Image_To_VGA_Memory(loaded,MapViewportSegment);
            EgaMemoryAddress spriteDestination={0,MapViewportSegment};
            DrawCall_EGA_CharacterPos(spriteDestination,sprite,104,80);
            for (unsigned y=0;y<24;++y)
                for (unsigned x=0;x<24;++x) {
                    uint8_t colour=0;
                    for (uint8_t plane=0;plane<4;++plane)
                        if (SDLBackend_ReadEgaPlaneByte(MapViewportSegment,
                            (uint16_t)((80+y)*40+(104+x)/8),plane)&(0x80>>((104+x)%8)))
                            colour|=(uint8_t)(1<<plane);
                    uint8_t packed=graphics[1+y*PackedGraphicsRowBytes+x/2];
                    check(colour==((x&1)?packed&15:packed>>4));
                }
        }
        printf("%s packed FNV1a32=%08lX\n",images[image],(unsigned long)hash);
    }
    /* Actual game loader through SDL file/decode/EGA transfer, not an adapter. */
    TilesetId=Tileset_Destruct;
    Load_And_Draw_BTTLTECH_ICN();
    check(TilesetId==Tileset_BattleTech && GraphicsCompatibilityFlag==TRUE);
    uint32_t tilesetHash=UINT32_C(2166136261);
    for (unsigned i=0;i<PackedGraphicsOutputBytes;++i)
        tilesetHash=(tilesetHash^GraphicsSceneWorkspace[i])*UINT32_C(16777619);
    check(tilesetHash==UINT32_C(0xC3537AD1));
    for (uint16_t column=0;column<EgaScreenPlaneBytes;++column)
        for (unsigned pixel=0;pixel<EgaPixelsPerByte;++pixel) {
            uint8_t colour=0;
            for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
                if (SDLBackend_ReadEgaPlaneByte(EgaTilesetBufferSegment,column,plane)&(0x80>>pixel))
                    colour|=(uint8_t)(1<<plane);
            uint8_t expected=GraphicsSceneWorkspace[column*4+pixel/2];
            check(colour==((pixel&1)?expected&15:expected>>4));
        }
    /* Native map-file -> adjacency -> expanded cache -> viewport workflow.
     * Overhead loading avoids changing live NPC state in this asset test. */
    OverheadMapActive=TRUE;
    for (uint16_t map=1;map<=14;++map) {
        CrescentHawkMapPositionX=0x0555; CrescentHawkMapPositionY=0x5055;
        Map_Construct_Nine_Regions(0x55);
        DOS_Load_Map_Files(4,map);
        Map_NineGrid_Parent();
        PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
        Copy_Data_To_GraphicsMemory();
        EgaMemoryAddress viewport={0,MapViewportSegment},screen={0,EgaApertureSegment};
        EGA_DrawBox_Operation(viewport,screen,MapViewportLeftByte,0,MapViewportByteWidth,EgaScreenHeight);
        unsigned originRow=(((uint8_t)CrescentHawkMapPositionY>>1)&7)+2;
        unsigned originColumn=(((uint8_t)CrescentHawkMapPositionX>>1)&7)+2;
        for (uint16_t row=0;row<EgaScreenHeight;++row)
            for (uint16_t column=0;column<MapViewportByteWidth;++column)
                for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
                    unsigned tileY=row+MapTileHalfHeight;
                    unsigned tileX=column+1;
                    uint8_t tile=CombatMap[(originRow+tileY/MapTileHeight)*MapCacheWidth+
                        originColumn+tileX/MapTileByteWidth];
                    uint16_t source=(uint16_t)(tile*32+(tileY%MapTileHeight)*2+tileX%2);
                    check(SDLBackend_ReadEgaPlaneByte(MapViewportSegment,
                        (uint16_t)(row*EgaFramebufferRowBytes+MapViewportLeftByte+column),plane)==
                        SDLBackend_ReadEgaPlaneByte(MapTilesetSegment,source,plane));
                    check(SDLBackend_ReadEgaPlaneByte(EgaApertureSegment,
                        (uint16_t)(row*EgaFramebufferRowBytes+MapViewportLeftByte+column),plane)==
                        SDLBackend_ReadEgaPlaneByte(MapViewportSegment,
                        (uint16_t)(row*EgaFramebufferRowBytes+MapViewportLeftByte+column),plane));
                }
    }
    Load_And_Draw_BTTITLE_CMP();
    check(TilesetId==Tileset_None && GraphicsCompatibilityFlag==FALSE);
    uint32_t titleHash=UINT32_C(2166136261);
    for (unsigned i=0;i<PackedGraphicsOutputBytes;++i)
        titleHash=(titleHash^GraphicsFileWorkspace[i])*UINT32_C(16777619);
    check(titleHash==UINT32_C(0x5E04241E));
    for (uint16_t column=0;column<EgaScreenPlaneBytes;++column)
        for (unsigned pixel=0;pixel<EgaPixelsPerByte;++pixel) {
            uint8_t colour=0;
            for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
                if (SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,column,plane)&(0x80>>pixel))
                    colour|=(uint8_t)(1<<plane);
            uint8_t expected=GraphicsFileWorkspace[column*4+pixel/2];
            check(colour==((pixel&1)?expected&15:expected>>4));
        }
    for (uint8_t index=0;index<EgaPaletteRegisterCount;++index) {
        int16_t colour=(int8_t)DefaultEgaPalette[index];
        if (colour>7) colour+=8;
        check(SDLBackend_GetEgaPaletteRegister(index)==((uint16_t)colour&0x3F));
    }
    /* Headerless original SIF, actual native dispatcher/ticks. No timer/speed
     * wait or generated sound file: this checks stream traversal, not audibility. */
    int16_t musicHandle=Get_FileHandle((const uint8_t *)"WWOODBT.SIF",DOSFileMode_ReadBinary);
    check(musicHandle!=-1);
    int32_t musicBytes=DOS_set_file_position((uint16_t)musicHandle,0,0,DOSSeek_End);
    check(musicBytes>0 && musicBytes<GraphicsWorkspaceBytes-MusicTerminatorBytes);
    check(DOS_set_file_position((uint16_t)musicHandle,0,0,DOSSeek_Start)==0);
    check(DOS_read_file_handler((uint16_t)musicHandle,GraphicsFileWorkspace,(uint16_t)musicBytes)==musicBytes);
    check(DOS_close_file((uint16_t)musicHandle)==0);
    memset(GraphicsFileWorkspace+musicBytes,0,MusicTerminatorBytes);
    PC_Speaker_Music_Control(MusicControl_Start,GraphicsFileWorkspace,MusicPcCadenceTicks);
    unsigned cursor=0,notes=0;
    while (!Music_Playback_Finished()) {
        unsigned next=cursor;
        uint8_t note=GraphicsFileWorkspace[next++];
        if (!note) note=GraphicsFileWorkspace[next++];
        unsigned ticks=notes==0?1:MusicPcCadenceTicks;
        for (unsigned tick=0;tick<ticks;++tick) Music_Pc_Stream_Tick();
        if (!note) {
            check(Music_Playback_Finished() && MusicStreamOffset==cursor);
        } else {
            cursor=next; ++notes;
            check(!Music_Playback_Finished() && MusicStreamOffset==cursor);
        }
        check(cursor<=(unsigned)musicBytes+1);
    }
    check(notes>0);
    PC_Speaker_Music_Control(MusicControl_Stop);
    puts("Original assets: BLD/images/maps/title/animated tiles and real SIF stream traversal passed.");
    puts("Image checksums match InceptionTools; SDL rendering/emulator parity remain unverified.");
    return 0;
}
