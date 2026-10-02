#include "game.h"
#include "dos.h"

/* Sol: Original0800:2DA8..320A, verified against the complete raw ASM.
 * Retained EGA path:207F:00D1 is CGA-only and returns without writes for EGA.
 * File reads deliberately ignore returned lengths: EOF retains working tails.
 * Valid native files/slot indices are the contract, not a new safe parser. */
void DOS_Load_Map_Files(uint16_t mapGridSlot,uint16_t mapNumber)
{
    uint8_t reserved,blockOffsetX,blockOffsetY,blockColumns,blockRows;
    int16_t fileHandle;
    uint8_t prefix[]="MAP",extension[]=".MTP";
    BlockingTileCodeThreshold=MapOrdinaryBlockingThreshold;
    InsideStarLeagueCache=FALSE;
    if (mapNumber==Map_StarLeagueCache) {
        BlockingTileCodeThreshold=MapCacheBlockingThreshold;
        InsideStarLeagueCache=TRUE;
    }
    Append_Large_Text_To_Memory(DynamicString,prefix);
    ASM_Text_Formatting(mapNumber,DynamicString+3,NumericRadix_Decimal);
    Append_Text_To_Memory(DynamicString,extension);
    Select_Game_Disk_And_Drive(GameDisk_Second);
    if (mapNumber==Map_DestroyedCitadel && TilesetId!=Tileset_Destruct) {
        GraphicsCompatibilityFlag=TRUE;
        Load_File_To_Memory((const uint8_t *)"DESTRUCT.ICN",GraphicsFileWorkspace);
        Decompress_File_Into_Memory(GraphicsFileWorkspace,GraphicsSceneWorkspace);
        DrawCall_Image_To_VGA_Memory(GraphicsSceneWorkspace,EgaTilesetBufferSegment);
        TilesetId=Tileset_Destruct;
    }
    if (mapNumber!=Map_DestroyedCitadel && TilesetId!=Tileset_BattleTech)
        Load_And_Draw_BTTLTECH_ICN();
    Select_Game_Disk_And_Drive(GameDisk_First);
    if (mapNumber==Map_Citadel || mapNumber==Map_DestroyedCitadel ||
        (int16_t)mapNumber>=Map_StarLeagueCache)
        Select_Game_Disk_And_Drive(GameDisk_Second);
    do {
        fileHandle=Get_FileHandle(DynamicString,UINT16_C(0x8000));
        if (fileHandle==-1) Request_Game_Disk(RequestedGameDiskNumber);
    } while (fileHandle==-1);
    DOS_read_file_handler((uint16_t)fileHandle,&reserved,1);
    DOS_read_file_handler((uint16_t)fileHandle,&blockOffsetX,1);
    DOS_read_file_handler((uint16_t)fileHandle,&blockOffsetY,1);
    DOS_read_file_handler((uint16_t)fileHandle,&blockColumns,1);
    DOS_read_file_handler((uint16_t)fileHandle,&blockRows,1);
    DOS_read_file_handler((uint16_t)fileHandle,MapCharacterNames,sizeof(MapCharacterNames));
    DOS_read_file_handler((uint16_t)fileHandle,MapBuildingNames,sizeof(MapBuildingNames));
    DOS_read_file_handler((uint16_t)fileHandle,MapInteractablePositionX,sizeof(MapInteractablePositionX));
    DOS_read_file_handler((uint16_t)fileHandle,MapInteractablePositionY,sizeof(MapInteractablePositionY));
    DOS_read_file_handler((uint16_t)fileHandle,MapPartyPositionX,sizeof(MapPartyPositionX));
    DOS_read_file_handler((uint16_t)fileHandle,MapPartyPositionY,sizeof(MapPartyPositionY));
    DOS_read_file_handler((uint16_t)fileHandle,AlternativeBldByBuildingId,sizeof(AlternativeBldByBuildingId));
    DOS_read_file_handler((uint16_t)fileHandle,RoamingNpcWaypointLink,sizeof(RoamingNpcWaypointLink));
    blockColumns>>=3; blockRows>>=3; /* One descriptor per8x8-tile block. */
    DOS_read_file_handler((uint16_t)fileHandle,MapFileTiles,MapFileMaximumTileBytes);
    DOS_close_file((uint16_t)fileHandle);
    /* Native nine FAR pointers designate these contiguous64-byte grids. */
    uint8_t *grid=MapDescriptorCache+mapGridSlot*MapBlockTileCount+
        blockOffsetY*MapBlockWidth+blockOffsetX;
    uint8_t descriptor=MapFileFirstBlockDescriptor;
    for (uint8_t row=0;row<blockRows;++row)
        for (uint8_t column=0;column<blockColumns;++column)
            grid[row*MapBlockWidth+column]=descriptor++;
    if (mapNumber==Map_Starport && StarportCountdown!=0)
        Starport_MapPatch_SaveApply_Or_Restore(FALSE);
    if (OverheadMapActive!=FALSE) return;
    for (uint8_t slot=0;slot<MapCharacterCount;++slot) {
        RoamingMapNpc *npc=&RoamingMapNpcs[slot];
        uint16_t combatantId=(uint16_t)(Enemy_Infantry_CombatantId_Range_First+slot);
        npc->movementDelay=1;
        uint8_t start=Rand_0x00_to_0xFF()&(MapCharacterCount-1);
        uint8_t destination=RoamingNpcWaypointLink[start];
        npc->waypointPair=(uint8_t)((start<<4)|destination);
        npc->destinationX=MapInteractablePositionX[destination];
        npc->destinationY=MapInteractablePositionY[destination];
        npc->currentPositionX=npc->destinationX;
        CombatantPackedX[combatantId]=npc->currentPositionX;
        npc->currentPositionY=npc->destinationY;
        CombatantPackedY[combatantId]=npc->currentPositionY;
        CombatantAnimationDirection[combatantId]=AnimationDirection_Refresh;
        CombatantSpriteFrame[combatantId]=RoamingNpcInitialFrame;
        if ((int16_t)mapNumber>Map_LastVillage) {
            npc->movementDelay=0;
            CombatantPackedY[combatantId]=0;
            CombatantPackedX[combatantId]=0;
        }
    }
}
