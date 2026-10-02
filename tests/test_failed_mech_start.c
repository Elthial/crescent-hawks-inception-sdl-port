#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned stage,frames,expectedFrames;
static uint8_t control;
static void verify(int ok,unsigned line) {
    if (!ok) { fprintf(stderr,"Failed Mech startup mismatch line%u stage%u frames%u\n",line,stage,frames); exit(1); }
}
#define check(x) verify(!!(x),__LINE__)
void Select_Game_Disk_And_Drive(uint16_t disk) { check(stage++==0 && disk==1); }
void DOS_Load_File_to_memory(const uint8_t *name,uint8_t *buffer,uint16_t count) {
    check(stage++==1 && !strcmp((const char *)name,"O0.ANM"));
    check(buffer==AnimationFileData && count==0x3E80);
    memset(buffer,0x55,AnmFileMaximumBytes); buffer[1]=control; buffer[2]='I';
}
void Decode_Draw_Next_ANM_Frame(void) {
    check(stage==2 && AnimationFrameNumber==frames && AnimationStreamOffset==AnmStreamNativeOffset+frames);
    if (!frames) for (unsigned byte=0;byte<AnmFrameWorkspaceBytes;++byte) check(AnimationFrameWorkspace[byte]==0);
    ++frames; ++AnimationFrameNumber; ++AnimationStreamOffset;
    check(frames<=expectedFrames);
}
void Play_Sound_If_Enabled(uint16_t sound) { check(stage++==2 && sound==14 && frames==expectedFrames); }
void Wait_For_N_Vertical_Retraces(uint16_t count) { check(stage++==3 && count==60); }
void Menu_Memory_Variables(uint16_t panel) { check(stage++==4 && panel==4); }
void Draw_Top_Graphic_Sidebar(void) { check(stage++==5); }
int main(void) {
    for (unsigned value=0;value<256;++value) {
        stage=frames=0; control=(uint8_t)value;
        expectedFrames=(int8_t)control<'I'?2:1;
        memset(AnimationFrameWorkspace,0xA5,AnmFrameWorkspaceBytes);
        AnimationFrameNumber=0x8000; AnimationStreamOffset=0xFFFF;
        Play_Failed_Mech_Startup_Scene();
        check(stage==6 && frames==expectedFrames && AnimationFileData[1]==control);
        check(AnimationFileData[AnmFileMaximumBytes-1]==0x55);
    }
    puts("Complete original failed-start scene: all256 signed control bytes, clear range and call order passed.");
    return 0;
}
