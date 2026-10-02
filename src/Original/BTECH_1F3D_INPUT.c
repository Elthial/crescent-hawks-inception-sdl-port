#include "game.h"
#include "dos.h"

/* 1F3D:002F. Sol: Probe live input FIRST, then report replay input available.
 * DisableInput3938 is the legacy name for attract-mode replay activation. */
uint16_t Pending_Input(void)
{
    if (Check_Input_For_Character() != 0 || DisableInput != 0) return TRUE;
    return FALSE;
}

/* 1F3D:0259..031B. Sol: Read live input, optionally record it, or consume
 * signed bytes from the shared asset/replay buffer. Delay tokens wait/continue;
 * live interruption and replay P set ExitMainLoop. Valid replay indexes within
 * the declared shared buffer are required; no new bounded demo parser. */
uint16_t Keyboard_Get_ASCII_Hex_Input(void)
{
    int16_t inputWord;
    if (DisableInput == FALSE)
    {
        do
        {
            inputWord = Keyboard_GetKey();
            if (AttractModeRecordingActive == FALSE) return (uint16_t)inputWord;
            if (inputWord == 'h') inputWord = AttractModeDelayToken;
            uint16_t replayWriteIndex = AttractModeReplayIndex++;
            BTStatsOrBldMemory[replayWriteIndex] = (uint8_t)inputWord;
        } while (inputWord == AttractModeDelayToken);
    }
    else
    {
        do
        {
            uint16_t replayReadIndex = AttractModeReplayIndex++;
            inputWord = (int8_t)BTStatsOrBldMemory[replayReadIndex];
            if (inputWord == AttractModeDelayToken)
                Wait_For_N_Vertical_Retraces(AttractModeDelayRetraces);
        } while (inputWord == AttractModeDelayToken);
        if (Check_Input_For_Character() != 0)
        {
            ExitMainLoop = TRUE;
            Keyboard_GetKey();
        }
        Wait_For_N_Vertical_Retraces(1);
        if (inputWord == AttractModeExitToken) ExitMainLoop = TRUE;
    }
    return (uint16_t)inputWord;
}

/* 1F3D:086A. Sol: Original fifty-retrace pause, then discard pending keys.
 * This method does not set a50Hz clock. */
void Wait_For_50Hz_Then_Check_Input(void)
{
    Wait_For_N_Vertical_Retraces(InputPauseRetraces);
    Drain_Pending_Keyboard_Input();
}
