#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned returning,stage,boxes,keys,scripts,sidebars;
static char text[256];
static uint8_t originalTiles[MapFileMaximumTileBytes];
static void verify(int condition,unsigned line) { if(!condition) { fprintf(stderr,"Cache maproom mismatch line%u\n",line); exit(1); } }
#define check(condition) verify(!!(condition),__LINE__)
/* Actual two original room methods; file/decode/drawing boundaries inspect
 * buffer reuse and call ordering. No copyrighted fixture or production stub. */
void Draw_Message_Box(void) { ++boxes; }
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *message) {
    size_t used=strlen(text),length=strlen((char *)message); check(used+length<sizeof text); memcpy(text+used,message,length+1);
}
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keys; return 0; }
void Draw_Top_Graphic_Sidebar(void) { ++sidebars; }
void Interact_with_BLD(uint16_t id) { check(id==Bld_FindIt); ++scripts; }
void Select_Game_Disk_And_Drive(uint16_t disk) { check(stage++==0 && disk==GameDisk_Second); }
uint16_t Load_File_To_Memory(const uint8_t *filename,uint8_t *memory) {
    check(stage++==1 && memory==GraphicsFileWorkspace && GraphicsCompatibilityFlag==TRUE);
    check(!strcmp((const char *)filename,returning?"STARLEAG.ICN":"MAP.ICN"));
    for(unsigned i=0;i<MapDescriptorCacheBytes;++i) check(MapDescriptorCache[i]==(returning?CacheEmptyDescriptor:CacheMapRoomEmptyDescriptor));
    if(returning) check(!memcmp(MapFileTiles,originalTiles,sizeof originalTiles));
    return 100;
}
void Decompress_File_Into_Memory(uint8_t *source,uint8_t *destination) {
    check(stage++==2 && source==GraphicsFileWorkspace && destination==GraphicsSceneWorkspace);
    if(returning) check(!memcmp(MapFileTiles,originalTiles,sizeof originalTiles));
    memset(destination,0xAA,GraphicsWorkspaceBytes);
}
void DrawCall_Image_To_VGA_Memory(uint8_t *image,uint16_t segment) {
    check(stage++==3 && image==GraphicsSceneWorkspace && segment==EgaTilesetBufferSegment);
    for(unsigned i=0;i<CacheMapRoomTileBytes;++i) check(image[i]==0xAA);
}
void DOS_Load_File_to_memory(const uint8_t *filename,uint8_t *destination,uint16_t count) {
    check(!returning && stage++==4 && destination==MapFileTiles && count==768 && TilesetId==Tileset_MapRoom);
    check(!strcmp((const char *)filename,"MAP15.MTP"));
    check(!memcmp(GraphicsSceneWorkspace,originalTiles,768)); memset(destination,0x97,count);
}
void Map_NineGrid_Parent(void) {
    check(stage++==(returning?4u:5u) && CacheMapRoomLoaded==TRUE);
    unsigned center=MapCacheSlot_Centre*64;
    for(unsigned i=0;i<576;++i) {
        unsigned expected=returning?0xD0:0xD1;
        if(returning && i>=center && i<center+64) expected=0x90+i-center;
        if(!returning && i>=center && i<center+24 && (i-center)%8<4) expected=0x90+(i-center)/8*4+(i-center)%8;
        check(MapDescriptorCache[i]==expected);
    }
}
void PosXY_OffsetGrid(uint16_t x,uint16_t y) { check(stage++==(returning?5u:6u) && x==(returning?CacheLadderX:CacheMapLadderX) && y==(returning?CacheLadderY:CacheMapLadderY)); }
void Copy_Data_To_GraphicsMemory(void) { check(stage++==(returning?6u:7u)); }
void EGA_DrawBox_Wrapper(void) { check(stage++==(returning?7u:8u) && BlockingTileCodeThreshold==(returning?CacheMapRoomBlockingThreshold:MapCacheBlockingThreshold)); }
static void prepare(void) { stage=boxes=keys=scripts=sidebars=0; text[0]=0; }
int main(void) {
    for(unsigned i=0;i<sizeof originalTiles;++i) originalTiles[i]=(uint8_t)(i*13+7);
    memcpy(MapFileTiles,originalTiles,sizeof originalTiles);
    returning=0; prepare(); WhiteCacheCodeCorrect=0; CacheMapRoomLoaded=0;
    BlockingTileCodeThreshold=MapCacheBlockingThreshold; ExplorationStepsPerInput=4;
    StarLeague_Map_Room(0x0C47,0xC038);
    check(stage==9 && boxes==1 && keys==2 && sidebars==1 && ExplorationStepsPerInput==1 && CacheMapRoomLoaded==1);
    check(BlockingTileCodeThreshold==CacheMapRoomBlockingThreshold && strstr(text,"map painted on the floor."));
    check(!memcmp(GraphicsSceneWorkspace,originalTiles,768));
    check(!memcmp(MapFileTiles+768,originalTiles+768,sizeof originalTiles-768));
    returning=1; prepare(); Draw_STARLEAG_ICN_Scene();
    check(stage==8 && boxes==1 && keys==1 && CacheMapRoomLoaded==0 && BlockingTileCodeThreshold==MapCacheBlockingThreshold);
    check(ExplorationStepsPerInput==1 && !memcmp(MapFileTiles,originalTiles,sizeof originalTiles));
    for(unsigned white=2;white<256;++white) {
        returning=0; prepare(); WhiteCacheCodeCorrect=(uint8_t)white;
        StarLeague_Map_Room(71,56); check(stage==9 && WhiteCacheCodeCorrect==white);
        returning=1; prepare(); Draw_STARLEAG_ICN_Scene(); check(stage==8 && WhiteCacheCodeCorrect==white);
    }
    returning=0;
    for(unsigned flag=0;flag<256;++flag) {
        prepare(); MechPartsCacheFound=(uint8_t)flag; StarLeague_Map_Room(123,4);
        check(scripts==1 && MechPartsCacheFound==1 && MessageBoxOpen==1 && !stage && !boxes);
    }
    prepare(); WhiteCacheCodeCorrect=1; StarLeague_Map_Room(71,56);
    check(!stage && boxes==1 && keys==1 && !strcmp(text,"You already have the WHITE code."));
    prepare(); StarLeague_Map_Room(0,0); check(!stage && !boxes && !scripts && !keys);
    puts("Original cache maproom entry/backup/restore ordering passed"); return 0;
}
