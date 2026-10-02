#include "game.h"
#include "dos.h"

/* Original0800:1AFD..1C11, retained EGA pipeline. Persistent packed bytes
 * are XOR-updated, then converted into the adjoining planar frame buffer. */
void Decode_Draw_Next_ANM_Frame(void)
{
    uint16_t relativeStream=(uint16_t)(AnimationStreamOffset-AnmFileNativeOffset);
    uint16_t consumed=Animation_Decode(AnimationFileData+relativeStream,AnimationFrameWorkspace);
    VGA_Inline_ASM_Loop(AnimationFrameWorkspace,AnimationFrameWorkspace+AnmPackedFrameBytes,AnmPackedFrameBytes/2);
    DrawCall_EGA_Animations();
    AnimationStreamOffset+=consumed;
    int16_t timingIndex=(int16_t)((int8_t)AnimationFileData[AnimationFrameNumber]-AnmFirstTimingToken);
    int16_t multiplier=(int8_t)SceneAnimation.bytes[AnmFrameWorkspaceBytes+AnmDelayTableOffset+timingIndex];
    int16_t scale=(int8_t)AnimationFileData[AnmTimingScaleOffset];
    int16_t delayProduct=(int16_t)(uint16_t)(multiplier*scale*3);
    /* Native low-WORD IMUL result followed by two SARs, not promoted /4. */
    int16_t retraces=(int16_t)(delayProduct>=0?delayProduct/4:-((-(int32_t)delayProduct+3)/4));
    Wait_For_N_Vertical_Retraces((uint16_t)retraces);
    ++AnimationFrameNumber;
}

/* Original0800:48B7..4AA5. Filename/random call occur even for skipped scenes.
 * Native signed scene comparisons, close-before-failure-test and seven siren
 * passes are retained. No playback cancellation/decoder sanitization is added. */
void Display_Animation_Scene(uint16_t sceneId,uint16_t playbackMode)
{
    DynamicString[0]='O';
    ASM_Text_Formatting(sceneId,DynamicString+1,10);
    Append_Text_To_Memory(DynamicString,(uint8_t *)".ANM");
    uint16_t shouldPlay=!(Rand_0x00_to_0xFF()&OuttakeFrequencyRandomMask[(int8_t)OuttakeFrequency]);
    if (sceneId!=AnimationO16_WaspLostArm
        && (sceneId==AnimationO00_MechStartUp || (int16_t)sceneId>AnimationO07_WaspFiring
            || sceneId==AnimationO06_CrescentHawkCard || sceneId==AnimationO02_NeuroHelmet))
        shouldPlay=TRUE;
    if (playbackMode==AnimationPlayback_Force_CallerRedraw) shouldPlay=TRUE;
    if (!shouldPlay) return;
    Select_Game_Disk_And_Drive(GameDisk_First);
    if ((int16_t)sceneId>=AnimationO10_HPGTransmission && (int16_t)sceneId<AnimationO16_WaspLostArm)
        Select_Game_Disk_And_Drive(GameDisk_Second);
    if ((int16_t)sceneId>AnimationO16_WaspLostArm) {
        int16_t handle=Get_FileHandle(DynamicString,DOSFileMode_ReadBinary);
        (void)DOS_close_file((uint16_t)handle);
        if (handle==-1) { /* .dis49A9:FFFF, not annotated00FF. */
            Menu_Memory_Variables(4);
            Draw_Top_Graphic_Sidebar();
            return;
        }
    }
    DOS_Load_File_to_memory(DynamicString,AnimationFileData,AnmFileMaximumBytes);
    /* Original00D1 only rebuilds CGA tables; omitted in retained EGA pipeline. */
    uint16_t passes=sceneId==AnimationO08_SirenAlarm?AnmSirenRepeatCount:1;
    for (uint16_t pass=0;pass<passes;++pass) {
        for (uint16_t byte=0;byte<AnmFrameWorkspaceBytes;++byte) AnimationFrameWorkspace[byte]=0;
        AnimationFrameNumber=0; AnimationStreamOffset=AnmStreamNativeOffset;
        Decode_Draw_Next_ANM_Frame();
        if (sceneId!=AnimationO08_SirenAlarm) Wait_For_N_Vertical_Retraces(AnmFirstFrameHoldRetraces);
        while (AnimationFileData[AnimationFrameNumber]) Decode_Draw_Next_ANM_Frame();
    }
    if (playbackMode==AnimationPlayback_RestoreGameView) {
        if (sceneId==AnimationO00_MechStartUp) Play_Sound_If_Enabled(Sound_MechStartUp);
        Wait_For_N_Vertical_Retraces(AnmRestoreHoldRetraces);
        Menu_Memory_Variables(4);
        Draw_Top_Graphic_Sidebar();
    }
}
