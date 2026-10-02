#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned stage,passage,replays;
static Mech saved[LanceSize];
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"Cache entry mismatch line%u stage%u\n",line,stage); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
/* Actual original bodies; platform, map assembly and script boundaries
 * inspect state and call order without pretending to execute gameplay. */
void Select_Game_Disk_And_Drive(uint16_t disk) { check(stage++==0 && disk==GameDisk_Second); }
void DOS_Load_Map_Files(uint16_t slot,uint16_t map) {
    check(stage++==1 && slot==MapCacheSlot_Centre && map==Map_StarLeagueCache);
    for(unsigned i=0;i<MapDescriptorCacheBytes;++i) check(MapDescriptorCache[i]==CacheEmptyDescriptor);
    for(unsigned i=0;i<PartySize;++i) check(Characters[i].mechAssignment==0);
}
uint16_t Load_File_To_Memory(const uint8_t *name,uint8_t *memory) {
    check(stage++==2 && !strcmp((const char *)name,"STARLEAG.ICN") && memory==GraphicsFileWorkspace && GraphicsCompatibilityFlag==TRUE);
    for(unsigned i=0;i<3;++i) check(PendingMapGridSlot[i]==255 && PendingMapRegionIndex[i]==i+7);
    return 1;
}
void Decompress_File_Into_Memory(uint8_t *source,uint8_t *destination) { check(stage++==3 && source==GraphicsFileWorkspace && destination==GraphicsSceneWorkspace); }
void DrawCall_Image_To_VGA_Memory(uint8_t *image,uint16_t segment) { check(stage++==4 && image==GraphicsSceneWorkspace && segment==EgaTilesetBufferSegment); }
void Map_NineGrid_Parent(void) { check(stage++==5 && TilesetId==Tileset_StarLeagueCache); }
void PosXY_OffsetGrid(uint16_t x,uint16_t y) { check(stage++==(passage?0u:6u) && x==CrescentHawkMapPositionX && y==CrescentHawkMapPositionY); }
void Copy_Data_To_GraphicsMemory(void) { check(stage++==(passage?1u:7u)); }
void Draw_Infantry_And_Mechs(void) { check(passage && stage++==2); }
void EGA_DrawBox_Wrapper(void) { check(stage++==(passage?3u:8u)); }
void Wait_For_N_Vertical_Retraces(uint16_t count) { check(stage++==9 && count==60); }
void StarLeague_Key_Codes(uint16_t x,uint16_t y,int16_t id) {
    if(id<0) {
        check(stage++==10 && x==0x0C03 && y==0x097D && id==-1);
        for(unsigned i=0;i<PartySize;++i) check(Characters[i].mechAssignment==0);
    } else { check(stage==12 && x==0 && y==0 && id==(int16_t)replays && id<11); ++replays; }
}
void Interact_with_BLD(uint16_t id) {
    check(stage++==11 && id==23);
    for(unsigned i=0;i<PartySize;++i) check(Characters[i].mechAssignment==Character_OnFoot);
}
void Draw_Message_Box(void) { check(passage && stage++==4); }
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text) {
    check(passage && stage++==5 && !strcmp((char *)text,"Part of the hallway here crumbles away, revealing a secret passageway!"));
}
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { check(passage && stage++==6); return 0; }
int main(void) {
    for(unsigned i=0;i<LanceSize;++i) { memset(&Mechs[i],(int)(i+31),sizeof(Mech)); saved[i]=Mechs[i]; }
    for(unsigned i=0;i<3;++i) PendingMapRegionIndex[i]=(uint8_t)(i+7);
    memset(CacheDoorOpened,255,CacheDoorCount); PersistentState.fields.insideStarLeagueCache=77;
    Draw_STARLEAG_ICN_AND_Game_Logic();
    check(stage==12 && replays==11 && CrescentHawkMapPositionX==0x0C06 && CrescentHawkMapPositionY==0xC07E);
    check(PersistentState.fields.insideStarLeagueCache==77);
    for(unsigned i=0;i<LanceSize;++i) {
        check(CacheEntryMechNameInitial[i]==saved[i].name[0]); saved[i].name[0]=255;
        check(!memcmp(&Mechs[i],&saved[i],sizeof(Mech)));
    }
    static const uint8_t expected[18]={187,28,0,188,0,1,189,0,28,190,0,1,191,28,0,192,1,0};
    for(unsigned repeat=0;repeat<2;++repeat) {
        passage=1; stage=0; MessageBoxOpen=0; memset(MapFileTiles,0xA5,MapFileMaximumTileBytes);
        StarLeague_Secret_Passageway_Discovered(); check(stage==7 && MessageBoxOpen==TRUE);
        for(unsigned i=0;i<MapFileMaximumTileBytes;++i) {
            unsigned relative=i-0x0A10;
            uint8_t value=(i>=0x0A10 && relative/8<6 && relative%8<3)?expected[relative/8*3+relative%8]:0xA5;
            check(MapFileTiles[i]==value);
        }
    }
    return 0;
}
