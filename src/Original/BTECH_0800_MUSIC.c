#include "game.h"
#include "dos.h"
#include "music.h"

/* Sol: Original0800:476D..48B6 retained EGA/PC-speaker path. Complete ASM
 * checked: seek length narrows AX, read/close results are not checked, four
 * terminator bytes follow the headerless external SIF stream. Valid original
 * stream length plus terminator must fit the shared graphics workspace.
 * DOS IRQ scheduling belongs behind the music timer's SDL boundary. */
void Load_And_Play_Intro_Music(void)
{
    Select_Game_Disk_And_Drive(GameDisk_Second);
    int16_t handle;
    do {
        handle=Get_FileHandle((const uint8_t *)"WWOODBT.SIF",DOSFileMode_ReadBinary);
        if (handle==-1) Request_Game_Disk(RequestedGameDiskNumber);
    } while (handle==-1);
    uint16_t length=(uint16_t)DOS_set_file_position((uint16_t)handle,0,0,DOSSeek_End);
    DOS_set_file_position((uint16_t)handle,0,0,DOSSeek_Start);
    DOS_read_file_handler((uint16_t)handle,GraphicsFileWorkspace,length);
    DOS_close_file((uint16_t)handle);
    for (uint16_t index=0;index<MusicTerminatorBytes;++index)
        GraphicsFileWorkspace[(uint16_t)(length+index)]=0;
    Install_Music_Timer();
    PC_Speaker_Music_Control(MusicControl_Stop);
    PC_Speaker_Music_Control(MusicControl_Start,GraphicsFileWorkspace,MusicPcCadenceTicks);
    DisableInput=FALSE;
    while (Pending_Input()==FALSE && Music_Playback_Finished()==FALSE)
        ;
    PC_Speaker_Music_Control(MusicControl_Stop);
    Restore_System_Timer();
}
