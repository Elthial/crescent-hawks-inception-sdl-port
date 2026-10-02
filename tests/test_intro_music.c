#include "game.h"
#include "dos.h"
#include "music.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
static unsigned cancel, opens, seeks, reads, closes, installs, restores, controls, polls, finishes;
void Select_Game_Disk_And_Drive(uint16_t disk) { assert(disk==GameDisk_Second); }
int16_t Get_FileHandle(const uint8_t *name,uint16_t mode,...)
{
    assert(!strcmp((const char *)name,"WWOODBT.SIF") && mode==DOSFileMode_ReadBinary);
    return ++opens==1?-1:7;
}
uint16_t Request_Game_Disk(uint16_t disk) { assert(disk==RequestedGameDiskNumber && opens==1); return 0; }
int32_t DOS_set_file_position(uint16_t handle,uint16_t low,uint16_t high,uint16_t origin)
{
    assert(handle==7 && low==0 && high==0);
    assert(origin==(seeks++==0?DOSSeek_End:DOSSeek_Start));
    return origin==DOSSeek_End?0x10005:0; /* native AX-length truncation */
}
int16_t DOS_read_file_handler(uint16_t handle,void *buffer,uint16_t count)
{
    assert(handle==7 && buffer==GraphicsFileWorkspace && count==5 && seeks==2);
    memcpy(buffer,"abc",3); ++reads; return 3; /* preserve unchecked short read */
}
int16_t DOS_close_file(uint16_t handle) { assert(handle==7 && reads==1); ++closes; return -1; }
void Install_Music_Timer(void)
{
    assert(closes==1 && !memcmp(GraphicsFileWorkspace,"abc",3));
    assert(GraphicsFileWorkspace[3]==0xCC && GraphicsFileWorkspace[4]==0xCC);
    for (unsigned i=5;i<9;++i) assert(GraphicsFileWorkspace[i]==0);
    assert(GraphicsFileWorkspace[9]==0xCC); ++installs;
}
void PC_Speaker_Music_Control(uint16_t selector,...)
{
    assert(installs==1 && controls<3);
    assert(selector==(controls==1?MusicControl_Start:MusicControl_Stop));
    if (selector==MusicControl_Start) {
        va_list args; va_start(args,selector);
        assert(va_arg(args,uint8_t *)==GraphicsFileWorkspace);
        assert(va_arg(args,int)==MusicPcCadenceTicks);
        va_end(args);
    }
    ++controls;
}
uint16_t Pending_Input(void)
{
    assert(DisableInput==FALSE && controls==2); ++polls;
    return (uint16_t)(cancel && polls==3);
}
uint16_t Music_Playback_Finished(void) { ++finishes; return (uint16_t)(!cancel && finishes==3); }
void Restore_System_Timer(void) { assert(controls==3); ++restores; }
int main(void)
{
    for (cancel=0;cancel<2;++cancel) {
        opens=seeks=reads=closes=installs=restores=controls=polls=finishes=0;
        memset(GraphicsFileWorkspace,0xCC,sizeof GraphicsFileWorkspace);
        DisableInput=TRUE; RequestedGameDiskNumber=GameDisk_Second;
        Load_And_Play_Intro_Music();
        assert(opens==2 && restores==1 && polls==3 && finishes==(cancel?2u:3u));
    }
    puts("Original SIF loading retries, AX length, unchecked I/O, four terminators and completion/input cleanup passed");
    return 0;
}
