#include "game.h"
#include "dos.h"

/* EXE-owned contiguous3EDB:0A38..0A99. The13-WORD read-span table ends
 * immediately before glyph coordinates: index13 reads those next two BYTEs. */
uint8_t OverviewMapMetadata[OverviewMetadataBytes]={
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    8,8,4,4,4,4,4,4,4,4,8,1,1,0,
    8,8,4,4,4,4,4,4,4,4,8,1,1,0,
    0,16,0,16,0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,16,0,2,0,2,
    20,23,23,23,20,16,16,16,8,8,12,15,15,15,12,8};

/* Original0800:3FAE..45C1. Sol: Build15 reduced region blocks, show visible
 * tiles and objective glyph, blink fixed party marker, return input. Normal
 * map IDs1..14, valid original metadata and scratch extents are required.
 * Original file reads ignore lengths, and demo timeout is601 redraws. */
uint16_t Overhead_Map_Draw(uint16_t partyX,uint16_t partyY)
{
    Graphics_Set_Screen_To_Black();
    uint16_t nextDynamic=OverviewDynamicFirstTile,scratch=OverviewMtpScratchOffset;
    uint16_t viewportX=CrescentHawkMapPositionX,viewportY=CrescentHawkMapPositionY;
    int16_t firstColumn=(int16_t)(((viewportX&0x0F00)>>8)-2);
    int16_t firstRow=(int16_t)(((viewportY&0xF000)>>8)-MapRegionRowStride);
    if(firstColumn<0) firstColumn=0;
    if(firstColumn>OverviewLastFirstColumn) firstColumn=OverviewLastFirstColumn;
    if(firstRow<0) firstRow=0;
    if(firstRow>OverviewLastFirstRow) firstRow=OverviewLastFirstRow;
    uint16_t fog=(uint16_t)(firstColumn+firstRow*MapBlockWidth);
    if(InsideStarLeagueCache!=FALSE) {
        for(uint16_t byte=0;byte<OverviewCacheCopyBytes;++byte) GraphicsFileWorkspace[scratch+byte]=MapFileTiles[byte];
        for(uint16_t cell=0;cell<OverviewBufferCells;++cell) GraphicsFileWorkspace[cell]=(uint8_t)(CacheMapRoomLoaded-OverviewCacheBackgroundAdjustment);
        Overhead_Map_Build_Centre_Block(OverviewCacheBlockDestination);
    } else {
        for(uint16_t rowOffset=0;rowOffset<OverviewRegionRows*MapRegionRowStride;rowOffset+=MapRegionRowStride) {
            uint16_t regionRow=(uint16_t)(firstRow+rowOffset);
            CrescentHawkMapPositionX=(uint16_t)(firstColumn*256); CrescentHawkMapPositionY=(uint16_t)(regionRow*256);
            Map_Construct_Nine_Regions((uint16_t)(firstColumn|regionRow));
            for(uint16_t column=0;column<OverviewRegionColumns;++column) {
                uint16_t anyMap=FALSE;
                int16_t neighbour=(int16_t)(((firstColumn+column)|regionRow)-MapRegionPreviousRowAndColumn);
                for(uint16_t y=0;y<MapNeighbourhoodWidth;++y) {
                    for(uint16_t x=0;x<MapNeighbourhoodWidth;++x) {
                        int16_t region=(int16_t)(uint16_t)(neighbour+x);
                        if(region>=0 && region<WorldRegionCount && MapFileByWorldRegion[region]!=0) {
                            anyMap=TRUE;
                            int16_t index=(int16_t)((int8_t)MapFileByWorldRegion[region]-1);
                            int16_t offset=(int16_t)((int8_t)OverviewMapMetadata[OverviewMetadataY+index]*MapBlockWidth+(int8_t)OverviewMapMetadata[index]);
                            int16_t rows=(int8_t)OverviewMapMetadata[OverviewMetadataRows+index],columns=(int8_t)OverviewMapMetadata[OverviewMetadataColumns+index];
                            uint16_t base=(uint16_t)((y*MapNeighbourhoodWidth+x)*MapBlockTileCount+offset);
                            for(int16_t blockY=0;blockY<rows;++blockY) for(int16_t blockX=0;blockX<columns;++blockX) {
                                uint16_t cell=(uint16_t)(base+blockY*MapBlockWidth+blockX);
                                if(y==1 && x==1) MapDescriptorCache[cell]=(uint8_t)nextDynamic++;
                                else if(MapDescriptorCache[cell]<OverviewDynamicFirstTile) MapDescriptorCache[cell]=(uint8_t)nextDynamic;
                            }
                            if(y==1 && x==1) {
                                uint8_t prefix[]="MAP",extension[]=".MTP";
                                Append_Large_Text_To_Memory(DynamicString,prefix);
                                ASM_Text_Formatting((uint16_t)(index+1),DynamicString+3,NumericRadix_Decimal);
                                Append_Text_To_Memory(DynamicString,extension);
                                Select_Game_Disk_And_Drive(GameDisk_First);
                                if(index==0 || index==10 || index>=13) Select_Game_Disk_And_Drive(GameDisk_Second);
                                int16_t handle;
                                do { handle=Get_FileHandle(DynamicString,DOSFileMode_ReadBinary); if(handle==-1) Request_Game_Disk(RequestedGameDiskNumber); } while(handle==-1);
                                uint16_t span=(uint16_t)(OverviewMapMetadata[OverviewMetadataSpans+index*2]|((uint16_t)OverviewMapMetadata[OverviewMetadataSpans+index*2+1]<<8));
                                DOS_read_file_handler((uint16_t)handle,GraphicsFileWorkspace+scratch,OverviewMtpHeaderBytes);
                                DOS_read_file_handler((uint16_t)handle,GraphicsFileWorkspace+scratch,span);
                                DOS_close_file((uint16_t)handle); scratch=(uint16_t)(scratch+span);
                            }
                        }
                    }
                    neighbour=(int16_t)(uint16_t)(neighbour+MapRegionRowStride);
                }
                if(anyMap!=FALSE) Map_NineGrid_Parent();
                Overhead_Map_Build_Centre_Block((uint16_t)(rowOffset*20+column*MapBlockWidth));
                CrescentHawkMapPositionX|=PackedPositionLocalMask; Map_Move_East();
            }
        }
    }
    for(uint16_t row=0;row<OverviewBufferRows;++row) {
        uint16_t bit=0x80;
        for(uint16_t column=0;column<OverviewBufferColumns;++column) {
            uint8_t tile=GraphicsFileWorkspace[row*OverviewBufferColumns+column];
            if((bit&(uint16_t)(int16_t)(int8_t)MapFogOfWar[fog])!=0) {
                uint8_t *pixels;
                if(tile<OverviewDynamicFirstTile) pixels=TinylandTileset+tile*OverviewPackedTileBytes;
                else { Build_Dynamic_Overhead_Tile(tile); pixels=GraphicsFileWorkspace+OverviewPackedTileOffset; }
                DrawCall_SingleTile(pixels,column,row);
            }
            bit>>=1; if(bit==0) { bit=0x80; ++fog; }
        }
        fog=(uint16_t)(fog+MapFogOfWarRowBytes-OverviewRegionColumns);
    }
    if(InsideStarLeagueCache==FALSE && ShowOverheadObjectiveDirection!=FALSE && ((viewportX|viewportY)&0xFF00)!=OverviewObjectiveRegion) {
        int16_t direction=Get_Target_Compass_Direction(viewportX,viewportY,OverviewObjectiveX,OverviewObjectiveY);
        DynamicString[0]=(uint8_t)(direction+1); DynamicString[1]=0;
        Draw_EGA_Text_To_Screen(DynamicString,(uint16_t)(int16_t)(int8_t)OverviewMapMetadata[OverviewMetadataGlyphX+direction],
            (uint16_t)(int16_t)(int8_t)OverviewMapMetadata[OverviewMetadataGlyphY+direction],EGA_BrightWhite,0);
    }
    Draw_Horizontal_EGA_Line(0,OverviewTerrainPixelsHigh,EgaScreenWidth-1,EgaScreenHeight-1,0);
    Draw_EGA_Text_To_Screen((uint8_t *)(InsideStarLeagueCache==FALSE && KuritaDestroyedCitadel!=FALSE?
        "Arrows to move, space to exit.":"Press any key to return to game."),0,OverviewBufferRows,EGA_BrightWhite,0);
    CrescentHawkMapPositionX=partyX; CrescentHawkMapPositionY=partyY;
    uint16_t ticks=OverviewDemoTimeout,ready=FALSE;
    do {
        int16_t markerX=(int16_t)(uint16_t)(((CrescentHawkMapPositionX&0x7F)>>1)+((((CrescentHawkMapPositionX&0xF00)>>8)-firstColumn)*64));
        int16_t markerY=(int16_t)(uint16_t)(((CrescentHawkMapPositionY&0x7F)>>1)+((((CrescentHawkMapPositionY&0xF000)>>8)-firstRow)*4));
        if(markerX>=0 && markerX<EgaScreenWidth && markerY>=0 && markerY<OverviewTerrainPixelsHigh) {
            if(markerX>EgaScreenWidth-2) markerX=EgaScreenWidth-2;
            if(markerY>OverviewTerrainPixelsHigh-1) markerY=OverviewTerrainPixelsHigh-1;
            Draw_Horizontal_EGA_Line((uint16_t)markerX,(uint16_t)markerY,(uint16_t)(markerX+1),(uint16_t)(markerY+1),Rand_0x00_to_0xFF()&15);
        }
        if(DisableInput==FALSE) ready=Pending_Input();
        else { uint16_t old=ticks--; if(old==0) ready=TRUE; Wait_For_N_Vertical_Retraces(1); }
    } while(ready==FALSE);
    uint16_t key=Keyboard_Get_ASCII_Hex_Input(); Drain_Pending_Keyboard_Input(); return key;
}
