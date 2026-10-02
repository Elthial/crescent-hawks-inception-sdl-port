#include "game.h"
#include "dos.h"

uint8_t CacheEntryMechNameInitial[LanceSize]; /* Original3092:3248. */
/* EXE-owned3EDB:20BC, not external level artwork. */
static const uint8_t SecretPassageTiles[CachePassageRows][CachePassageColumns]={
    {187,28,0},{188,0,1},{189,0,28},{190,0,1},{191,28,0},{192,1,0}
};

/* Original135D:0004..01E8. Preserve temporary assignment to mech0,
 * BYTE-only mech hiding, and instruction-script/door-replay ordering. */
void Draw_STARLEAG_ICN_AND_Game_Logic(void)
{
    for(uint16_t mech=0;mech<LanceSize;++mech) {
        CacheEntryMechNameInitial[mech]=Mechs[mech].name[0];
        Mechs[mech].name[0]=MECH_Destroyed;
    }
    for(uint16_t member=0;member<PartySize;++member) Characters[member].mechAssignment=0;
    CrescentHawkMapPositionX=CacheEntranceCameraX;
    CrescentHawkMapPositionY=CacheEntranceCameraY;
    /* Original207F:00D1 palette conversion is CGA-only, unchanged for EGA. */
    Select_Game_Disk_And_Drive(GameDisk_Second);
    for(uint16_t index=0;index<MapDescriptorCacheBytes;++index) MapDescriptorCache[index]=CacheEmptyDescriptor;
    DOS_Load_Map_Files(MapCacheSlot_Centre,Map_StarLeagueCache);
    for(uint16_t pending=0;pending<MapNeighbourhoodWidth;++pending) PendingMapGridSlot[pending]=UINT8_MAX;
    GraphicsCompatibilityFlag=TRUE;
    Load_File_To_Memory((const uint8_t *)"STARLEAG.ICN",GraphicsFileWorkspace);
    Decompress_File_Into_Memory(GraphicsFileWorkspace,GraphicsSceneWorkspace);
    DrawCall_Image_To_VGA_Memory(GraphicsSceneWorkspace,EgaTilesetBufferSegment);
    TilesetId=Tileset_StarLeagueCache;
    Map_NineGrid_Parent();
    PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
    Copy_Data_To_GraphicsMemory(); EGA_DrawBox_Wrapper();
    Wait_For_N_Vertical_Retraces(CacheEntranceRetraces);
    StarLeague_Key_Codes(CacheEntranceDoorX,CacheEntranceDoorY,CacheDoorLookupByPosition);
    for(uint16_t member=0;member<PartySize;++member) Characters[member].mechAssignment=Character_OnFoot;
    Interact_with_BLD(Bld_CacheInstructions);
    for(uint16_t door=0;door<CacheDoorEntrance;++door)
        if(CacheDoorOpened[door]!=0) StarLeague_Key_Codes(0,0,(int16_t)door);
}

/* Original135D:01E9..0287. Source rows have three bytes; destination
 * rows have eight. No once-only guard or persistent discovery write. */
void StarLeague_Secret_Passageway_Discovered(void)
{
    uint16_t rowOffset=CachePassageTileOffset;
    for(uint16_t row=0;row<CachePassageRows;++row) {
        for(uint16_t column=0;column<CachePassageColumns;++column)
            MapFileTiles[rowOffset+column]=SecretPassageTiles[row][column];
        rowOffset+=MapBlockWidth;
    }
    PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
    Copy_Data_To_GraphicsMemory(); Draw_Infantry_And_Mechs(); EGA_DrawBox_Wrapper();
    Draw_Message_Box();
    Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"Part of the hallway here crumbles away, revealing a secret passageway!");
    (void)Keyboard_Get_ASCII_Hex_Input(); MessageBoxOpen=TRUE;
}
