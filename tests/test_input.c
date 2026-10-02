/* Sol: Original availability/input/demo/drain/pause/text methods, hardware
 * and drawing adapters only. Generated replay bytes, not original DEMOFILE. */
#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, reads, probes, waits, keyCount;
static uint16_t keys[8], waitCounts[8];
static char text[64];
static void check(int condition)
{
    ++checks; if (!condition) { fprintf(stderr,"Input check %u failed\n",checks); exit(1); }
}
uint16_t Check_Input_For_Character(void) { ++probes; return reads < keyCount ? 255 : 0; }
int16_t Keyboard_GetKey(void) { check(reads < keyCount); return (int16_t)keys[reads++]; }
void Wait_For_N_Vertical_Retraces(uint16_t count) { check(waits < 8); waitCounts[waits++] = count; }
void Display_Text_From_Memory(uint8_t *value)
{
    check(strlen(text)+strlen((char *)value)<sizeof text);
    memcpy(text+strlen(text),value,strlen((char *)value)+1);
}
static void reset(void)
{
    DisableInput = AttractModeRecordingActive = AttractModeReplayIndex = ExitMainLoop = 0;
    reads = probes = waits = keyCount = 0; text[0] = 0;
    memset(BTStatsOrBldMemory,0xCC,sizeof BTStatsOrBldMemory);
}
int main(void)
{
    reset(); check(Pending_Input() == FALSE && probes == 1 && reads == 0);
    DisableInput = TRUE; check(Pending_Input() == TRUE && probes == 2 && reads == 0);
    reset(); keyCount = 1; keys[0] = 'X'; check(Pending_Input() == TRUE && reads == 0);
    for (uint32_t key = 0; key <= UINT16_MAX; ++key) {
        reset(); keyCount = 1; keys[0] = (uint16_t)key;
        check(Keyboard_Get_ASCII_Hex_Input() == key);
        check(reads == 1 && probes == 0 && waits == 0 && AttractModeReplayIndex == 0);
    }
    reset(); AttractModeRecordingActive = TRUE; keyCount = 3;
    keys[0] = 'h'; keys[1] = 'H'; keys[2] = UINT16_C(0xFFB8);
    check(Keyboard_Get_ASCII_Hex_Input() == UINT16_C(0xFFB8));
    check(reads == 3 && waits == 0 && AttractModeReplayIndex == 3);
    check(BTStatsOrBldMemory[0] == 'H' && BTStatsOrBldMemory[1] == 'H' && BTStatsOrBldMemory[2] == 0xB8);
    DisableInput = TRUE; AttractModeReplayIndex = 0;
    check(Keyboard_Get_ASCII_Hex_Input() == UINT16_C(0xFFB8));
    check(waits == 3 && waitCounts[0] == 30 && waitCounts[1] == 30 && waitCounts[2] == 1);
    check(AttractModeReplayIndex == 3 && ExitMainLoop == FALSE);
    reset(); DisableInput = TRUE; BTStatsOrBldMemory[0] = 'P';
    check(Keyboard_Get_ASCII_Hex_Input() == 'P' && ExitMainLoop && waits == 1 && waitCounts[0] == 1);
    reset(); DisableInput = TRUE; BTStatsOrBldMemory[0] = 'h'; keyCount = 1; keys[0] = 'Q';
    check(Keyboard_Get_ASCII_Hex_Input() == 'h');
    check(ExitMainLoop && reads == 1 && AttractModeReplayIndex == 1 && waits == 1);
    for (unsigned byte = 0; byte < 256; ++byte) {
        if (byte == 'H') continue;
        reset(); DisableInput = TRUE; BTStatsOrBldMemory[0] = (uint8_t)byte;
        check(Keyboard_Get_ASCII_Hex_Input() == (uint16_t)(int16_t)(int8_t)byte);
        check(waits == 1 && reads == 0 && AttractModeReplayIndex == 1);
    }
    reset(); keyCount = 3; keys[0] = 'A'; keys[1] = 'B'; keys[2] = 'C';
    Drain_Pending_Keyboard_Input(); check(reads == 3 && probes == 4);
    reset(); DisableInput = TRUE; keyCount = 1; keys[0] = 'A';
    Drain_Pending_Keyboard_Input(); check(reads == 0 && probes == 0);
    Wait_For_50Hz_Then_Check_Input(); check(waits == 1 && waitCounts[0] == 50 && reads == 0);
    reset(); keyCount = 1; keys[0] = 'A'; Wait_For_50Hz_Then_Check_Input();
    check(waits == 1 && waitCounts[0] == 50 && reads == 1 && probes == 2);
    reset(); keyCount = 1; keys[0] = 'X'; Prompt_And_Wait_For_Key();
    check(!strcmp(text,"\rPress a key.") && reads == 1);
    Display_Plural_Suffix(); Display_Sentence_Period();
    check(!strcmp(text,"\rPress a key.s."));
    printf("Original input bridge: %u checks passed\n",checks); return 0;
}
