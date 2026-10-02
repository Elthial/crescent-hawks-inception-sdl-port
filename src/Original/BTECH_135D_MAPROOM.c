#include "game.h"

/* Original135D:055A..079B, maintained EGA path. MAP.ICN must be uploaded
 * BEFORE its decoded buffer is reused for the cache's first768 tile bytes.
 * This enters/stages the room; it does not own an interaction loop. */
void StarLeague_Map_Room(uint16_t worldX,uint16_t worldY)
{
    uint16_t localX=(uint16_t)(worldX+1)&CacheLocalEvenCoordinateMask;
    uint16_t localY=worldY&CacheLocalEvenCoordinateMask;
    if(localX==CachePartsX && localY==CachePartsY) {
        Interact_with_BLD(Bld_FindIt); MechPartsCacheFound=TRUE; MessageBoxOpen=TRUE;
    }
    if(localX!=CacheWhiteSecurityX || localY!=CacheWhiteSecurityY) return;
    Draw_Message_Box(); MessageBoxOpen=TRUE;
    if(WhiteCacheCodeCorrect==TRUE) {
        Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"You already have the WHITE code.");
        (void)Keyboard_Get_ASCII_Hex_Input();
    } else {
        Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"You peer down the hole and see a huge map painted on the floor.");
        (void)Keyboard_Get_ASCII_Hex_Input(); Draw_Top_Graphic_Sidebar();
        Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"You climb down the ladder to investigate it.");
        (void)Keyboard_Get_ASCII_Hex_Input();
        ExplorationStepsPerInput=1;
        CrescentHawkMapPositionX=CacheMapLadderX; CrescentHawkMapPositionY=CacheMapLadderY;
        /* Original207F:00D1 is CGA-only and returns unchanged for EGA. */
        Select_Game_Disk_And_Drive(GameDisk_Second);
        for(uint16_t index=0;index<MapDescriptorCacheBytes;++index) MapDescriptorCache[index]=CacheMapRoomEmptyDescriptor;
        GraphicsCompatibilityFlag=TRUE;
        Load_File_To_Memory((const uint8_t *)"MAP.ICN",GraphicsFileWorkspace);
        Decompress_File_Into_Memory(GraphicsFileWorkspace,GraphicsSceneWorkspace);
        DrawCall_Image_To_VGA_Memory(GraphicsSceneWorkspace,EgaTilesetBufferSegment);
        for(uint16_t tile=0;tile<CacheMapRoomTileBytes;++tile) GraphicsSceneWorkspace[tile]=MapFileTiles[tile];
        TilesetId=Tileset_MapRoom;
        DOS_Load_File_to_memory((const uint8_t *)"MAP15.MTP",MapFileTiles,CacheMapRoomTileBytes);
        CacheMapRoomLoaded=TRUE;
        uint16_t descriptor=MapFileFirstBlockDescriptor;
        for(uint16_t row=0;row<CacheMapRoomDescriptorRows;++row)
            for(uint16_t column=0;column<CacheMapRoomDescriptorColumns;++column)
                MapDescriptorCache[MapCacheSlot_Centre*MapBlockTileCount+row*MapBlockWidth+column]=(uint8_t)descriptor++;
        Map_NineGrid_Parent();
        PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
        Copy_Data_To_GraphicsMemory(); EGA_DrawBox_Wrapper();
        BlockingTileCodeThreshold=CacheMapRoomBlockingThreshold;
    }
}

/* Original135D:079C..0912, maintained EGA path. Restore original tiles
 * BEFORE decoding STARLEAG.ICN overwrites the backup buffer4614. Does NOT
 * restore the user's exploration steps/input or clear the WHITE result. */
void Draw_STARLEAG_ICN_Scene(void)
{
    Draw_Message_Box();
    Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"You easily scale the ladder back up to the Star League cache.");
    (void)Keyboard_Get_ASCII_Hex_Input();
    CrescentHawkMapPositionX=CacheLadderX; CrescentHawkMapPositionY=CacheLadderY;
    Select_Game_Disk_And_Drive(GameDisk_Second);
    for(uint16_t index=0;index<MapDescriptorCacheBytes;++index) MapDescriptorCache[index]=CacheEmptyDescriptor;
    for(uint16_t tile=0;tile<CacheMapRoomTileBytes;++tile) MapFileTiles[tile]=GraphicsSceneWorkspace[tile];
    GraphicsCompatibilityFlag=TRUE; MessageBoxOpen=TRUE;
    Load_File_To_Memory((const uint8_t *)"STARLEAG.ICN",GraphicsFileWorkspace);
    Decompress_File_Into_Memory(GraphicsFileWorkspace,GraphicsSceneWorkspace);
    DrawCall_Image_To_VGA_Memory(GraphicsSceneWorkspace,EgaTilesetBufferSegment);
    TilesetId=Tileset_StarLeagueCache;
    for(uint16_t tile=0;tile<MapBlockTileCount;++tile)
        MapDescriptorCache[MapCacheSlot_Centre*MapBlockTileCount+tile]=(uint8_t)(MapFileFirstBlockDescriptor+tile);
    Map_NineGrid_Parent();
    PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
    Copy_Data_To_GraphicsMemory(); EGA_DrawBox_Wrapper();
    CacheMapRoomLoaded=FALSE; BlockingTileCodeThreshold=MapCacheBlockingThreshold;
}
