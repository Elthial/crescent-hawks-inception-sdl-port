#include "game.h"
#include "dos.h"

/* Sol: Complete original11B8:16B2..1761, expanded ASM checked.
 * Specialized O0 prefix, not the general scene player: draw at least once
 * and inspect the NEXT signed control BYTE. Restore the signed JL49 test
 * from the audit; high-bit controls do not terminate this native loop.
 * Valid original ANM storage/decoder progress are caller/asset contracts. */
void Play_Failed_Mech_Startup_Scene(void)
{
    enum { FailedStartupFileBytes=0x3E80,StartupFailureControl='I',
        Sound_MechStartUpFailed=14,FailureHoldRetraces=60,
        PartyDescriptionPanel=4 };
    Select_Game_Disk_And_Drive(GameDisk_First);
    DOS_Load_File_to_memory((const uint8_t *)"O0.ANM",AnimationFileData,FailedStartupFileBytes);
    for (uint16_t byte=0;byte<AnmFrameWorkspaceBytes;++byte)
        AnimationFrameWorkspace[byte]=0;
    AnimationFrameNumber=0;
    AnimationStreamOffset=AnmStreamNativeOffset;
    do {
        Decode_Draw_Next_ANM_Frame();
    } while ((int8_t)AnimationFileData[AnimationFrameNumber]<StartupFailureControl);
    Play_Sound_If_Enabled(Sound_MechStartUpFailed);
    Wait_For_N_Vertical_Retraces(FailureHoldRetraces);
    Menu_Memory_Variables(PartyDescriptionPanel);
    Draw_Top_Graphic_Sidebar();
}
