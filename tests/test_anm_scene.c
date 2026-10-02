#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks,draws,waitCount,loads,disks,menus,sidebars,sounds,closes;
static uint16_t waits[64],selectedDisks[4],closedHandle;
static uint8_t roll,fileFixture[128];
static int16_t probeHandle;
static void checkAtLine(int condition,unsigned line) {
    ++checks;
    if(!condition) { fprintf(stderr,"ANM check%u failed line%u\n",checks,line); exit(1); }
}
#define check(condition) checkAtLine((condition),__LINE__)
uint8_t Rand_0x00_to_0xFF(void) { return roll; }
void Display_Text_From_Memory(uint8_t *text) { (void)text; check(0); }
void Select_Game_Disk_And_Drive(uint16_t disk) { check(disks<4); selectedDisks[disks++]=disk; }
int16_t Get_FileHandle(const uint8_t *filename,uint16_t mode,...) {
    check(mode==DOSFileMode_ReadBinary && filename==DynamicString); return probeHandle;
}
int16_t DOS_close_file(uint16_t handle) { ++closes; closedHandle=handle; return 0; }
void DOS_Load_File_to_memory(const uint8_t *filename,uint8_t *buffer,uint16_t count) {
    check(filename==DynamicString && buffer==AnimationFileData && count==AnmFileMaximumBytes);
    ++loads; memcpy(buffer,fileFixture,sizeof fileFixture);
}
void Wait_For_N_Vertical_Retraces(uint16_t count) {
    check(waitCount<64); waits[waitCount++]=count;
}
void Menu_Memory_Variables(uint16_t layout) { check(layout==4); ++menus; }
void Draw_Top_Graphic_Sidebar(void) { ++sidebars; }
void Play_Sound_If_Enabled(uint16_t id) { check(id==Sound_MechStartUp); ++sounds; }
void DrawCall_EGA_Animations(void) {
    static const uint8_t expected[]={0x12,0x26,0x70};
    uint8_t value=expected[draws%3];
    check(AnimationFrameWorkspace[0]==value && AnimationFrameWorkspace[AnmPackedFrameBytes-1]==value);
    for(unsigned plane=0;plane<4;++plane) {
        uint8_t bits=(uint8_t)((((value>>4)&(1<<plane))?0xAA:0)|((value&(1<<plane))?0x55:0));
        check(AnimationFrameWorkspace[AnmPackedFrameBytes+plane]==bits);
        check(AnimationFrameWorkspace[2*AnmPackedFrameBytes-4+plane]==bits);
    }
    ++draws;
}
static void prepare(void) {
    memset(fileFixture,0,sizeof fileFixture); memset(SceneAnimation.bytes,0x55,sizeof SceneAnimation.bytes);
    fileFixture[0]=fileFixture[1]=fileFixture[2]='A';
    fileFixture[AnmDelayTableOffset]=4; fileFixture[AnmTimingScaleOffset]=4;
    for(unsigned frame=0;frame<3;++frame) {
        unsigned offset=0x33+frame*4;
        fileFixture[offset]=0; fileFixture[offset+1]=0x0F; fileFixture[offset+2]=0x20;
        fileFixture[offset+3]=(uint8_t)(0x12+frame*0x22);
    }
    OuttakeFrequency=0; roll=0; probeHandle=255;
    draws=waitCount=loads=disks=menus=sidebars=sounds=closes=0;
}
int main(void) {
    uint8_t frame[AnmPackedFrameBytes];
    uint8_t mixed[]={3,0x10,0x20,0x30,0xFE,0x44,0x80,0x55,0,0x0E,0x9B,0x66};
    memset(frame,0xA0,sizeof frame);
    check(Animation_Decode(mixed,frame)==sizeof mixed);
    for(unsigned i=0;i<sizeof frame;++i) {
        uint8_t delta=i<3?mixed[i+1]:i<5?0x44:i<133?0x55:0x66;
        check(frame[i]==(uint8_t)(0xA0^delta));
    }
    check(Animation_Decode(mixed,frame)==sizeof mixed);
    for(unsigned i=0;i<sizeof frame;++i) check(frame[i]==0xA0);
    uint8_t zeroRun[]={0,0,0,0x77},longRun[]={0,0xFF,0xFF,0x77};
    memset(frame,0,sizeof frame); check(Animation_Decode(zeroRun,frame)==4);
    for(unsigned i=0;i<sizeof frame;++i) check(frame[i]==0x77);
    check(Animation_Decode(longRun,frame)==4);
    for(unsigned i=0;i<sizeof frame;++i) check(frame[i]==0);

    prepare(); roll=3; Display_Animation_Scene(1,0);
    check(draws==0 && loads==0 && disks==0 && !strcmp((char *)DynamicString,"O1.ANM"));
    prepare(); roll=3; Display_Animation_Scene(16,0); check(loads==0);
    prepare(); roll=3; Display_Animation_Scene(1,AnimationPlayback_Force_CallerRedraw);
    check(draws==3 && loads==1 && waitCount==4 && waits[0]==12 && waits[1]==50 && waits[2]==12 && waits[3]==12);
    check(menus==0 && sidebars==0);
    prepare(); roll=3; Display_Animation_Scene(2,0);
    check(draws==3 && waits[4]==60 && menus==1 && sidebars==1);
    prepare(); Display_Animation_Scene(0,0); check(sounds==1);
    prepare(); Display_Animation_Scene(15,1);
    check(disks==2 && selectedDisks[0]==1 && selectedDisks[1]==2 && menus==0 && draws==3);
    prepare(); Display_Animation_Scene(8,0);
    check(draws==21 && loads==1 && waitCount==22 && waits[21]==60);
    check(AnimationFrameNumber==3 && AnimationStreamOffset==AnmStreamNativeOffset+12);
    for(unsigned i=2*AnmPackedFrameBytes;i<AnmFrameWorkspaceBytes;++i) check(AnimationFrameWorkspace[i]==0);
    prepare(); probeHandle=-1; Display_Animation_Scene(17,0);
    check(closes==1 && closedHandle==0xFFFF && loads==0 && draws==0 && menus==1 && sidebars==1);
    prepare(); probeHandle=255; Display_Animation_Scene(17,1);
    check(closes==1 && closedHandle==255 && loads==1 && draws==3); /*255 is not failure*/

    prepare(); memcpy(AnimationFileData,fileFixture,sizeof fileFixture);
    memset(AnimationFrameWorkspace,0,sizeof SceneAnimation.fields.frame);
    AnimationFrameNumber=0; AnimationStreamOffset=AnmStreamNativeOffset;
    AnimationFileData[AnmDelayTableOffset]=127; AnimationFileData[AnmTimingScaleOffset]=127;
    Decode_Draw_Next_ANM_Frame(); check(waits[0]==61248 && AnimationFrameNumber==1);
    prepare(); memcpy(AnimationFileData,fileFixture,sizeof fileFixture);
    memset(AnimationFrameWorkspace,0,sizeof SceneAnimation.fields.frame);
    AnimationFrameNumber=0; AnimationStreamOffset=AnmStreamNativeOffset;
    AnimationFileData[AnmDelayTableOffset]=1; AnimationFileData[AnmTimingScaleOffset]=255;
    Decode_Draw_Next_ANM_Frame(); check(waits[0]==0xFFFF);
    prepare(); memcpy(AnimationFileData,fileFixture,sizeof fileFixture);
    memset(AnimationFrameWorkspace,0,sizeof SceneAnimation.fields.frame);
    AnimationFrameNumber=0; AnimationStreamOffset=AnmStreamNativeOffset;
    AnimationFileData[0]=0; AnimationFrameWorkspace[AnmFrameWorkspaceBytes-33]=7;
    Decode_Draw_Next_ANM_Frame(); check(waits[0]==21); /*Original neighbouring timing address*/
    printf("Original ANM decoding and scene workflow: %u checks\n",checks);
    return 0;
}
