#define _CRT_SECURE_NO_WARNINGS /* Portable C17 stdio, test-only local reads. */
#include "game.h"
#include "dos.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Test-only OS/remaining UI adapters. Production loader and decoder are real. */
static uint8_t fixture[565]; /* 021D metadata bytes plus24 tile bytes */
static uint16_t offset,reads,opens,closes,prompts,draws;
static uint16_t disks[64],diskCount;
static uint16_t openedMapX[64],openedMapY[64];
static uint16_t interactionBlocked;
static int16_t attemptedX,attemptedY;
static uint8_t failFirst;
static char openedName[32];
static uint8_t destructFile[]={5,0,1,0,0,125,0x37}; /*32000 repeat pixels*/
static uint8_t *fileBytes;
static uint16_t fileLength;
static const char *assetDirectory;
static FILE *originalFile;
void Display_Text_From_Memory(uint8_t *text) { (void)text; assert(0); }
void Select_Game_Disk_And_Drive(uint16_t disk)
{ assert(diskCount<64); disks[diskCount++]=disk; RequestedGameDiskNumber=disk; }
uint16_t Map_Interactables_Building_Or_Items(int16_t deltaX,int16_t deltaY)
{ attemptedX=deltaX; attemptedY=deltaY; return interactionBlocked; }
uint16_t Request_Game_Disk(uint16_t disk)
{ assert(disk==RequestedGameDiskNumber); ++prompts; return TRUE; }
int16_t Get_FileHandle(const uint8_t *name,uint16_t mode,...)
{
    assert(mode==0x8000); ++opens;
    assert(opens<=64);
    openedMapX[opens-1]=CrescentHawkMapPositionX;
    openedMapY[opens-1]=CrescentHawkMapPositionY;
    if (assetDirectory) {
        char path[1024];
        snprintf(path,sizeof(path),"%s/%s",assetDirectory,(const char *)name);
        originalFile=fopen(path,"rb");
        assert(originalFile!=NULL); return 3;
    }
    if (failFirst) { failFirst=0; return -1; }
    snprintf(openedName,sizeof(openedName),"%s",(const char *)name);
    offset=0;
    if (strcmp(openedName,"DESTRUCT.ICN")==0 || strcmp(openedName,"BTTLTECH.ICN")==0) {
        fileBytes=destructFile; fileLength=sizeof(destructFile);
    } else { fileBytes=fixture; fileLength=sizeof(fixture); }
    return 3;
}
int16_t DOS_read_file_handler(uint16_t handle,void *buffer,uint16_t count)
{
    assert(handle==3); ++reads;
    if (assetDirectory) return (int16_t)fread(buffer,1,count,originalFile);
    uint16_t available=(uint16_t)(fileLength-offset);
    if (count>available) count=available;
    memcpy(buffer,fileBytes+offset,count); offset=(uint16_t)(offset+count);
    return (int16_t)count;
}
int16_t DOS_close_file(uint16_t handle) {
    assert(handle==3); ++closes;
    if (assetDirectory) return (int16_t)fclose(originalFile);
    return 0;
}
void DrawCall_Image_To_VGA_Memory(uint8_t *image,uint16_t segment)
{
    assert(image==GraphicsSceneWorkspace && segment==EgaTilesetBufferSegment);
    assert(GraphicsCompatibilityFlag==TRUE);
    if (!assetDirectory)
        for (size_t i=0;i<PackedGraphicsOutputBytes;++i) assert(image[i]==0x37);
    ++draws;
}
static void prepare(void)
{
    memset(fixture,0,sizeof(fixture));
    fixture[1]=2; fixture[2]=1; fixture[3]=16; fixture[4]=16;
    memset(fixture+5,'N',128); memset(fixture+133,'B',256);
    for (uint16_t i=0;i<MapBuildingCount;++i) {
        fixture[389+i*2]=(uint8_t)(40+i); fixture[421+i*2]=(uint8_t)(80+i);
        fixture[453+i*2]=(uint8_t)(120+i); fixture[485+i*2]=(uint8_t)(160+i);
        fixture[517+i]=(uint8_t)(i+1);
    }
    for (uint16_t i=0;i<MapCharacterCount;++i) fixture[533+i]=(uint8_t)((i+1)&7);
    memset(fixture+541,0x23,24);
    /* Short payload: original ignores EOF and retains the remaining tiles. */
    memset(MapFileTiles,0xA7,MapFileMaximumTileBytes);
    memset(MapDescriptorCache,0x44,MapDescriptorCacheBytes);
    memset(RoamingMapNpcs,0xCC,sizeof(RoamingMapNpcs));
    memset(CombatantPackedX,0x66,sizeof(CombatantPackedX));
    reads=opens=closes=prompts=draws=diskCount=0;
    TilesetId=Tileset_BattleTech; OverheadMapActive=FALSE; StarportCountdown=0;
    failFirst=0;
    interactionBlocked=FALSE;
}
int main(int argc,char **argv)
{
    prepare(); failFirst=1;
    PurchasedMedkit=1; PurchasedFieldSurgeryKit=2;
    StoredPartyMechNameInitial[0]='L'; NextRecruitNameId=3;
    DOS_Load_Map_Files(4,Map_Citadel);
    assert(strcmp(openedName,"MAP1.MTP")==0);
    assert(opens==2 && prompts==1 && reads==14 && closes==1);
    assert(diskCount==3 && disks[0]==2 && disks[1]==1 && disks[2]==2);
    assert(MapCharacterNames[0][0]=='N' && MapBuildingNames[15][15]=='B');
    assert(MapPartyPositionX[15]==135 && MapPartyPositionY[15]==175);
    assert(AlternativeBldByBuildingId[15]==16);
    assert(PurchasedMedkit==1 && PurchasedFieldSurgeryKit==2);
    assert(StoredPartyMechNameInitial[0]=='L' && NextRecruitNameId==3);
    for (uint16_t i=0;i<MapDescriptorCacheBytes;++i) {
        uint8_t expected=0x44;
        if (i==266) expected=0x90; if (i==267) expected=0x91;
        if (i==274) expected=0x92; if (i==275) expected=0x93;
        assert(MapDescriptorCache[i]==expected);
    }
    assert(MapFileTiles[0]==0x23 && MapFileTiles[23]==0x23);
    assert(MapFileTiles[24]==0xA7 && MapFileTiles[4095]==0xA7);
    for (uint16_t i=0;i<MapCharacterCount;++i) {
        RoamingMapNpc *npc=&RoamingMapNpcs[i];
        uint8_t destination=npc->waypointPair&15;
        assert(npc->movementDelay==1 && npc->destinationX==40+destination);
        assert(npc->currentPositionY==80+destination);
        assert(CombatantPackedX[16+i]==npc->destinationX);
        assert(CombatantAnimationDirection[16+i]==255 && CombatantSpriteFrame[16+i]==16);
        assert(npc->remainingNativeRecord[0]==(i==7?1:0xCC));
    }
    prepare(); OverheadMapActive=TRUE; StarportCountdown=1;
    uint8_t randomLow=RandomByteLow,randomMiddle=RandomByteMiddle,randomHigh=RandomByteHigh;
    DOS_Load_Map_Files(0,Map_Starport);
    assert(diskCount==2);
    for (uint16_t i=0;i<StarportPatchBytes;++i) {
        assert(StarportSavedMapBytes[i]==0xA7);
        assert(MapFileTiles[StarportPatchTileOffset+i]==
            (StarportMapPatchOverrides[i]!=0?StarportMapPatchOverrides[i]:0xA7));
    }
    Starport_MapPatch_SaveApply_Or_Restore(2);
    for (uint16_t i=0;i<StarportPatchBytes;++i)
        assert(MapFileTiles[StarportPatchTileOffset+i]==0xA7);
    Starport_MapPatch_SaveApply_Or_Restore(FALSE);
    Starport_MapPatch_SaveApply_Or_Restore(FALSE);
    Starport_MapPatch_SaveApply_Or_Restore(TRUE);
    for (uint16_t i=0;i<StarportPatchBytes;++i)
        assert(MapFileTiles[StarportPatchTileOffset+i]==
            (StarportMapPatchOverrides[i]!=0?StarportMapPatchOverrides[i]:0xA7));
    assert(RoamingMapNpcs[0].movementDelay==0xCC && CombatantPackedX[16]==0x6666);
    assert(RandomByteLow==randomLow && RandomByteMiddle==randomMiddle && RandomByteHigh==randomHigh);
    prepare(); DOS_Load_Map_Files(0,Map_StarLeagueCache);
    assert(InsideStarLeagueCache==TRUE && BlockingTileCodeThreshold==MapCacheBlockingThreshold);
    assert(RoamingMapNpcs[0].movementDelay==0 && CombatantPackedX[16]==0);
    prepare(); DOS_Load_Map_Files(0,0x8000);
    assert(strcmp(openedName,"MAP-32768.MTP")==0 && diskCount==2);
    assert(RoamingMapNpcs[0].movementDelay==1); /* signed JLE, not unsigned */
    prepare(); DOS_Load_Map_Files(0,Map_DestroyedCitadel);
    assert(draws==1 && closes==2 && TilesetId==Tileset_Destruct);
    prepare(); TilesetId=Tileset_Destruct;
    DOS_Load_Map_Files(0,3);
    assert(draws==1 && closes==2 && TilesetId==Tileset_BattleTech);
    assert(diskCount==3 && disks[0]==2 && disks[1]==2 && disks[2]==1);
    uint8_t originalMapFiles[WorldRegionCount];
    memcpy(originalMapFiles,MapFileByWorldRegion,sizeof(originalMapFiles));
    assert(MapFileByWorldRegion[0xCC]==1 && MapFileByWorldRegion[0x8A]==2);
    prepare(); OverheadMapActive=TRUE;
    memset(MapFileByWorldRegion,3,sizeof(originalMapFiles));
    memset(PendingMapGridSlot,PendingMapSlot_Unused,sizeof(PendingMapGridSlot));
    Map_Construct_Nine_Regions(0x55);
    CrescentHawkMapPositionX=0x057F; CrescentHawkMapPositionY=0x507F;
    Character_Movement_On_Map(Command_MoveSouthEast);
    assert(attemptedX==1 && attemptedY==1 && opens==6 && closes==6);
    assert(CrescentHawkMapPositionX==0x0600 && CrescentHawkMapPositionY==0x6000);
    for (uint16_t i=0;i<3;++i) {
        assert(openedMapX[i]==0x057F && openedMapY[i]==0x6000);
        assert(openedMapX[i+3]==0x0600 && openedMapY[i+3]==0x6000);
        assert(PendingMapGridSlot[i]==PendingMapSlot_Unused);
    }
    assert(CachedMapPositionX==0x0600 && CachedMapPositionY==0x6000);
    prepare(); OverheadMapActive=TRUE;
    memset(MapFileByWorldRegion,0,sizeof(originalMapFiles));
    Map_Construct_Nine_Regions(0x55);
    CrescentHawkMapPositionX=0x0500; CrescentHawkMapPositionY=0x5000;
    Character_Movement_On_Map(Command_MoveNorthWest);
    assert(opens==0 && CrescentHawkMapPositionX==0x047F && CrescentHawkMapPositionY==0x407F);
    for (uint16_t i=0;i<3;++i) assert(PendingMapGridSlot[i]==PendingMapSlot_Unused);
    prepare(); OverheadMapActive=TRUE;
    Map_Construct_Nine_Regions(0x55);
    CrescentHawkMapPositionX=0x0560; CrescentHawkMapPositionY=0x5060;
    memset(PendingMapGridSlot,PendingMapSlot_Unused,sizeof(PendingMapGridSlot));
    PendingMapGridSlot[0]=0; PendingMapRegionIndex[0]=10; MapFileByWorldRegion[10]=0x80;
    Character_Movement_On_Map(0);
    assert(strcmp(openedName,"MAP-128.MTP")==0 && opens==1);
    prepare(); interactionBlocked=TRUE;
    Map_Construct_Nine_Regions(0x55);
    CrescentHawkMapPositionX=0x0560; CrescentHawkMapPositionY=0x5060;
    PendingMapGridSlot[0]=2;
    static const uint16_t commands[]={Command_MoveNorth,Command_MoveNorthEast,
        Command_MoveEast,Command_MoveSouthEast,Command_MoveSouth,Command_MoveSouthWest,
        Command_MoveWest,Command_MoveNorthWest,0x00B8};
    static const int16_t expectedX[]={0,1,1,1,0,-1,-1,-1,0};
    static const int16_t expectedY[]={-1,-1,0,1,1,1,0,-1,0};
    for (size_t i=0;i<sizeof(commands)/sizeof(commands[0]);++i) {
        CachedMapPositionX=0; CachedMapPositionY=0;
        Character_Movement_On_Map(commands[i]);
        assert(attemptedX==expectedX[i] && attemptedY==expectedY[i]);
        assert(CrescentHawkMapPositionX==0x0560 && CrescentHawkMapPositionY==0x5060);
        assert(CachedMapPositionX==0x0560 && CachedMapPositionY==0x5060);
        assert(PendingMapGridSlot[0]==2 && opens==0);
    }
    memcpy(MapFileByWorldRegion,originalMapFiles,sizeof(originalMapFiles));
    if (argc==2) {
        assetDirectory=argv[1];
        /* MAP15 is not a headered MTP level (startsF6 F7 F7 F7 F7).
         * Do not feed that separately interpreted asset into this routine. */
        for (uint16_t map=1;map<=14;++map) {
            char path[1024]; uint8_t raw[5000];
            snprintf(path,sizeof(path),"%s/MAP%u.MTP",assetDirectory,(unsigned)map);
            FILE *input=fopen(path,"rb"); assert(input!=NULL);
            size_t length=fread(raw,1,sizeof(raw),input); fclose(input);
            assert(length>=541 && length<=541+MapFileMaximumTileBytes);
            prepare(); OverheadMapActive=TRUE;
            if (map==Map_DestroyedCitadel) TilesetId=Tileset_Destruct;
            DOS_Load_Map_Files(4,map);
            assert(memcmp(MapCharacterNames,raw+5,sizeof(MapCharacterNames))==0);
            assert(memcmp(MapBuildingNames,raw+133,sizeof(MapBuildingNames))==0);
            assert(memcmp(MapInteractablePositionX,raw+389,32)==0);
            assert(memcmp(MapInteractablePositionY,raw+421,32)==0);
            assert(memcmp(MapPartyPositionX,raw+453,32)==0);
            assert(memcmp(MapPartyPositionY,raw+485,32)==0);
            assert(memcmp(AlternativeBldByBuildingId,raw+517,16)==0);
            assert(memcmp(RoamingNpcWaypointLink,raw+533,8)==0);
            assert(memcmp(MapFileTiles,raw+541,length-541)==0);
            for (size_t i=length-541;i<MapFileMaximumTileBytes;++i) assert(MapFileTiles[i]==0xA7);
            uint8_t descriptor=MapFileFirstBlockDescriptor;
            for (uint16_t row=0;row<(raw[4]>>3);++row)
                for (uint16_t column=0;column<(raw[3]>>3);++column)
                    assert(MapDescriptorCache[4*MapBlockTileCount+(row+raw[2])*MapBlockWidth+column+raw[1]]==descriptor++);
        }
        puts("All14 headered local MTP levels match native loaded metadata, payload and descriptors.");
        prepare(); TilesetId=Tileset_Destruct;
        Load_And_Draw_BTTLTECH_ICN();
        uint32_t checksum=UINT32_C(2166136261);
        for (size_t i=0;i<PackedGraphicsOutputBytes;++i)
            checksum=(checksum^GraphicsSceneWorkspace[i])*UINT32_C(16777619);
        assert(checksum==UINT32_C(0xC3537AD1));
        assert(draws==1 && closes==1 && TilesetId==Tileset_BattleTech);
        puts("Original BTTLTECH loader decoded the local tileset to its verified checksum.");
    }
    puts("Original MTP loader: placement, EOF tails, NPCs, signed gates and tilesets passed.");
    return 0;
}
