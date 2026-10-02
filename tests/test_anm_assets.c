#include "game.h"
#include "backend.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Independent asset oracle; original methods perform all production decoding.
 * These boundary callbacks suppress interactive menus and real-time waits. */
static uint16_t lastDelay;
static unsigned frames;
static uint8_t file[AnmFileMaximumBytes],reference[AnmPackedFrameBytes];
static uint8_t background[PackedGraphicsOutputBytes];
static void require(int condition) { if(!condition) { fputs("Local ANM verification failed\n",stderr); exit(1); } }
void Display_Text_From_Memory(uint8_t *text) { (void)text; require(0); }
void Select_Game_Disk_And_Drive(uint16_t disk) { require(disk==1 || disk==2); }
uint16_t Request_Game_Disk(uint16_t disk) { (void)disk; require(0); return 0; }
void Menu_Memory_Variables(uint16_t layout) { (void)layout; require(0); }
void Draw_Top_Graphic_Sidebar(void) { require(0); }
void Play_Sound_If_Enabled(uint16_t sound) { (void)sound; require(0); }
void Wait_For_N_Vertical_Retraces(uint16_t count) { lastDelay=count; }

static size_t referenceFrame(size_t cursor,size_t length)
{
    unsigned output=0;
    while(output<sizeof reference) {
        require(cursor<length);
        int command=(int8_t)file[cursor++];
        unsigned count;
        int literal=command>0;
        if(command==0) {
            require(cursor+2<=length);
            count=(unsigned)file[cursor]*256+file[cursor+1]; cursor+=2;
            if(!count) count=65536;
        } else count=(unsigned)(literal?command:-command);
        require(cursor<length);
        uint8_t repeated=file[cursor];
        if(!literal) ++cursor;
        while(count-- && output<sizeof reference) {
            require(!literal || cursor<length);
            reference[output++]^=literal?file[cursor++]:repeated;
        }
    }
    return cursor;
}

static uint8_t screenPixel(unsigned x,unsigned y)
{
    uint8_t colour=0;
    for(uint8_t plane=0;plane<EgaPlaneCount;++plane)
        if(SDLBackend_ReadEgaPlaneByte(EgaApertureSegment,(uint16_t)(y*40+x/8),plane)&(0x80>>(x%8)))
            colour|=(uint8_t)(1<<plane);
    return colour;
}

int main(void)
{
    static const unsigned counts[]={18,14,29,12,13,8,31,6,3,12,5,6,11,3,9,6,7,1,1,1,1,1};
    for(unsigned scene=0;scene<sizeof counts/sizeof counts[0];++scene) {
        char filename[16];
        (void)snprintf(filename,sizeof filename,"O%u.ANM",scene);
        FILE *input=fopen(filename,"rb"); require(input!=NULL);
        size_t length=fread(file,1,sizeof file,input); require(!ferror(input));
        require(fgetc(input)==EOF); require(fclose(input)==0);
        DOS_Load_File_to_memory((const uint8_t *)filename,AnimationFileData,AnmFileMaximumBytes);
        require(memcmp(file,AnimationFileData,length)==0);
        memset(reference,0,sizeof reference);
        memset(AnimationFrameWorkspace,0,AnmFrameWorkspaceBytes);
        memset(background,0x99,sizeof background);
        SDLBackend_EgaMapMask=EgaAllPlanesMask;
        SDLBackend_EgaRasterOperation=EgaRaster_Replace;
        SDLBackend_EgaRotateCount=0;
        SDLBackend_TransferPackedImage(background,EgaApertureSegment);
        SDLBackend_EgaReadMapSelect=2;
        AnimationFrameNumber=0; AnimationStreamOffset=AnmStreamNativeOffset;
        size_t cursor=AnmStreamNativeOffset-AnmFileNativeOffset;
        for(unsigned frame=0;frame<counts[scene];++frame) {
            require(file[frame]!=0);
            cursor=referenceFrame(cursor,length);
            Decode_Draw_Next_ANM_Frame();
            require(AnimationFrameNumber==frame+1);
            require(AnimationStreamOffset==AnmFileNativeOffset+cursor);
            require(memcmp(reference,AnimationFrameWorkspace,sizeof reference)==0);
            int timingIndex=(int8_t)file[frame]-AnmFirstTimingToken;
            require(timingIndex>=0 && timingIndex<18);
            int16_t product=(int16_t)(uint16_t)((int8_t)file[AnmDelayTableOffset+timingIndex]*(int8_t)file[AnmTimingScaleOffset]*3);
            int delay=product>=0?product/4:-((-(int)product+3)/4);
            require(lastDelay==(uint16_t)delay);
            for(unsigned y=0;y<AnmFrameHeight;++y)
                for(unsigned x=0;x<AnmFrameWidth;++x) {
                    uint8_t packed=reference[(y*AnmFrameWidth+x)/2];
                    uint8_t expected=(uint8_t)((x&1)?packed&15:packed>>4);
                    require(screenPixel(x+8,y+8)==expected);
                }
            require(screenPixel(7,8)==9 && screenPixel(96,8)==9 && screenPixel(8,96)==9);
            require(SDLBackend_EgaMapMask==15 && SDLBackend_EgaReadMapSelect==2);
            ++frames;
        }
        require(file[counts[scene]]==0);
    }
    printf("Verified 22 original ANM files, %u decoded and rendered frames; no asset exports.\n",frames);
    return 0;
}
