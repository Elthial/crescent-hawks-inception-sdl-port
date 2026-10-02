#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t tiles[144*32],metadata[98];
static unsigned drawn,expectedDrawn,constructed,moved,assembled,opened,reads,closed;
static unsigned markers,polls,retraces,texts,drained,randomCalls,mapMode,failOpen;
static uint16_t expectedSpan;
static uint8_t visible[960];
static unsigned nextVisible;
static void verify(int ok,unsigned line) { if(!ok) { fprintf(stderr,"Overview renderer mismatch line%u\n",line); exit(1); } }
#define check(x) verify(!!(x),__LINE__)
uint8_t SDLBackend_EgaWriteMode;
void SDLBackend_CopyEgaLatchByte(uint16_t sourceSegment,uint16_t sourceOffset,uint16_t destinationSegment,uint16_t destinationOffset)
{ (void)sourceSegment; (void)sourceOffset; (void)destinationSegment; (void)destinationOffset; check(0); }
void Graphics_Set_Screen_To_Black(void) { }
void Map_Construct_Nine_Regions(uint16_t centre)
{
    check(centre==(uint16_t)((CrescentHawkMapPositionX|CrescentHawkMapPositionY)>>8));
    memset(MapDescriptorCache,1,MapDescriptorCacheBytes); memset(MapAdjacencyCache,0,MapDescriptorCacheBytes); ++constructed;
}
void Map_Move_East(void)
{
    check((CrescentHawkMapPositionX&127)==127); ++moved;
    CrescentHawkMapPositionX=(uint16_t)((CrescentHawkMapPositionX&0xFF00)+256);
}
void Map_NineGrid_Parent(void) { ++assembled; }
void Select_Game_Disk_And_Drive(uint16_t disk) { check(disk==1 || disk==2); RequestedGameDiskNumber=disk; }
uint16_t Request_Game_Disk(uint16_t disk) { check(disk==2); return 0; }
int16_t Get_FileHandle(const uint8_t *name,uint16_t mode,...)
{
    check(mode==DOSFileMode_ReadBinary && RequestedGameDiskNumber==2);
    check(strcmp((const char *)name,mapMode==14?"MAP14.MTP":"MAP1.MTP")==0);
    if(failOpen) { failOpen=0; return -1; } ++opened; return 5;
}
int16_t DOS_read_file_handler(uint16_t handle,void *destination,uint16_t count)
{
    check(handle==5 && count==(reads%2?expectedSpan:0x21D));
    check(destination==GraphicsFileWorkspace+0x4000+(reads/2)*expectedSpan);
    memset(destination,reads%2?0x30:0xEE,count); ++reads;
    return 0; /*Original caller ignores short/error lengths.*/
}
int16_t DOS_close_file(uint16_t handle) { check(handle==5); ++closed; return -1; }
void DrawCall_SingleTile(uint8_t *tile,uint16_t column,uint16_t row)
{
    check(column<40 && row<24);
    while(nextVisible<960 && !visible[nextVisible]) ++nextVisible;
    check(nextVisible==(unsigned)row*40+column); ++nextVisible;
    check(tile==tiles+32 || tile==GraphicsFileWorkspace+0x3FE0); ++drawn;
}
void Draw_Horizontal_EGA_Line(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t colour)
{
    if(y0==192) { check(x0==0 && x1==319 && y1==199 && colour==0); return; }
    check(x0<319 && y0<192 && x1==x0+1 && y1==y0+1 && colour==15); ++markers;
}
void Draw_EGA_Text_To_Screen(uint8_t *text,uint16_t x,uint16_t y,uint16_t foreground,uint16_t background)
{
    check(foreground==15 && background==0);
    if(texts++==0 && ShowOverheadObjectiveDirection && !InsideStarLeagueCache) {
        check(text[0]==4 && text[1]==0 && x==23 && y==15);
    } else {
        check(x==0 && y==24);
        check(strcmp((char *)text,(!InsideStarLeagueCache && KuritaDestroyedCitadel)?
            "Arrows to move, space to exit.":"Press any key to return to game.")==0);
    }
}
int16_t Get_Target_Compass_Direction(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1)
{ check(x0==0x335 && y0==0x3047 && x1==0xA38 && y1==0x8038); return 3; }
uint8_t Rand_0x00_to_0xFF(void) { ++randomCalls; return 255; }
uint16_t Pending_Input(void) { return ++polls==3; }
void Wait_For_N_Vertical_Retraces(uint16_t count) { check(count==1); ++retraces; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { check(drawn==expectedDrawn && !drained); return 0x1234; }
void Drain_Pending_Keyboard_Input(void) { ++drained; }
static void reset(void)
{
    memcpy(OverviewMapMetadata,metadata,sizeof metadata);
    memset(MapFileByWorldRegion,0,WorldRegionCount); memset(MapFogOfWar,255,MapFogOfWarBytes);
    memset(GraphicsFileWorkspace,0xA5,GraphicsWorkspaceBytes);
    CrescentHawkMapPositionX=0x335; CrescentHawkMapPositionY=0x3047;
    InsideStarLeagueCache=ShowOverheadObjectiveDirection=KuritaDestroyedCitadel=0; DisableInput=0;
    drawn=constructed=moved=assembled=opened=reads=closed=markers=polls=retraces=texts=drained=randomCalls=mapMode=failOpen=0;
    expectedSpan=64; expectedDrawn=960;
}
static void run(uint16_t x,uint16_t y)
{
    /*Independent byte-grid visibility oracle: each row occupies16 fog BYTEs,
     * each of the40 displayed columns consumes one MSB-first bit.*/
    int firstX=(CrescentHawkMapPositionX>>8 & 15)-2;
    int firstY=(CrescentHawkMapPositionY>>12)*16-16;
    if(firstX<0) firstX=0; if(firstX>11) firstX=11;
    if(firstY<0) firstY=0; if(firstY>208) firstY=208;
    unsigned start=(unsigned)(firstX+firstY*8); expectedDrawn=nextVisible=0;
    for(unsigned row=0;row<24;++row) for(unsigned column=0;column<40;++column) {
        visible[row*40+column]=(uint8_t)((MapFogOfWar[start+row*16+column/8]>>(7-column%8))&1);
        expectedDrawn+=visible[row*40+column];
    }
    check(Overhead_Map_Draw(x,y)==0x1234);
    check(CrescentHawkMapPositionX==x && CrescentHawkMapPositionY==y && drained==1);
    check(constructed==(InsideStarLeagueCache?0u:3u) && moved==(InsideStarLeagueCache?0u:15u));
    check(reads==opened*2 && closed==opened && randomCalls==markers);
    check(polls==(DisableInput?0u:3u) && retraces==(DisableInput?601u:0u));
}
int main(void)
{
    memcpy(metadata,OverviewMapMetadata,sizeof metadata); TinylandTileset=tiles;
    GraphicsAdapter=GraphicsAdapter_Ega;
    for(unsigned fogPattern=0;fogPattern<256;++fogPattern) {
        reset(); memset(MapFogOfWar,(int)fogPattern,MapFogOfWarBytes);
        unsigned bits=0; for(unsigned bit=0;bit<8;++bit) bits+=(fogPattern>>bit)&1;
        expectedDrawn=bits*120; run(0x335,0x3047); check(markers==3 && opened==0);
        for(unsigned cell=0;cell<960;++cell) check(GraphicsFileWorkspace[cell]==1);
    }
    for(unsigned region=0;region<256;++region) {
        reset(); CrescentHawkMapPositionX=(uint16_t)((region&15)*256+0x35);
        CrescentHawkMapPositionY=(uint16_t)((region&240)*256+0x47);
        for(unsigned byte=0;byte<MapFogOfWarBytes;++byte) MapFogOfWar[byte]=(uint8_t)(byte*73+(byte>>3));
        run(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
    }
    reset(); ShowOverheadObjectiveDirection=KuritaDestroyedCitadel=1; run(0x335,0x3047); check(texts==2);
    reset(); DisableInput=1; run(0x335,0x3047); check(markers==601);
    reset(); run(0xF00,0xF000); check(markers==0);
    reset(); InsideStarLeagueCache=1; CacheMapRoomLoaded=0;
    memset(MapDescriptorCache,1,MapDescriptorCacheBytes); memset(MapAdjacencyCache,0,MapDescriptorCacheBytes);
    for(unsigned byte=0;byte<0x1080;++byte) MapFileTiles[byte]=(uint8_t)byte;
    run(0x335,0x3047);
    check(memcmp(GraphicsFileWorkspace+0x4000,MapFileTiles,0x1080)==0);
    for(unsigned cell=0;cell<960;++cell) {
        unsigned relative=cell-0x150;
        check(GraphicsFileWorkspace[cell]==(cell>=0x150 && relative/40<8 && relative%40<8?1:0xD0));
    }
    reset(); mapMode=1; failOpen=1; memset(MapFileByWorldRegion,1,WorldRegionCount);
    OverviewMapMetadata[28]=OverviewMapMetadata[42]=1;
    OverviewMapMetadata[56]=64; OverviewMapMetadata[57]=0;
    run(0x335,0x3047); check(opened==15 && assembled==15);
    for(unsigned cell=0;cell<960;++cell) {
        unsigned block=cell/320*5+(cell%40)/8;
        check(GraphicsFileWorkspace[cell]==(cell/40%8==0 && cell%8==0?144+block:1));
    }
    reset(); mapMode=14; expectedSpan=0x1714; MapFileByWorldRegion[0x21]=14;
    run(0x335,0x3047); check(opened==1); /*Span13 aliases glyph-X BYTEs20,23.*/
    puts("Original overview renderer boundary scenarios passed"); return 0;
}
