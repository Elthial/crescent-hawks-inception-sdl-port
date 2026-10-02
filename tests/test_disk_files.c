/* Sol: Real original disk/prompt/raw-loader bodies; UI/OS adapters only. */
#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, drives, draws, menus, drains, keys, opens, reads, closes, failedOpens;
static uint16_t selectedDrive, readCount;
static char prompt[128];
static uint8_t buffer[16];
static int16_t openHandle, readResult;
static int lengthPrefixed;
static int16_t prefixReadResult = 2;
static void check(int condition)
{
    ++checks; if (!condition) { fprintf(stderr,"Disk/file check %u failed\n",checks); exit(1); }
}
void DOS_Select_Default_Drive(uint16_t drive) { ++drives; selectedDrive = drive; }
void Menu_Memory_Variables(uint16_t layout)
{
    check(layout == (menus % 2 == 0 ? DiskPromptMenuLayout : 9));
    ++menus; CurrentMenuLayoutIndex = layout;
}
void Draw_Top_Graphic_Sidebar(void) { ++draws; }
void Display_Text_From_Memory(uint8_t *text)
{
    check(TextColour == EGA_BrightRed && strlen(prompt) + strlen((char *)text) < sizeof prompt);
    memcpy(prompt+strlen(prompt),text,strlen((char *)text)+1);
}
void Drain_Pending_Keyboard_Input(void) { ++drains; }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { check(drains == keys+1); ++keys; return UINT16_C(0xFFB8); }
int16_t Get_FileHandle(const uint8_t *filename,uint16_t mode,...)
{
    check(!strcmp((const char *)filename,"synthetic.ANM") && mode == UINT16_C(0x8000));
    ++opens;
    if (opens <= failedOpens) return -1;
    return openHandle;
}
int16_t DOS_read_file_handler(uint16_t handle,void *destination,uint16_t count)
{
    check(handle == (uint16_t)openHandle && closes == 0);
    if (lengthPrefixed && reads == 0) {
        check(count == 2 && destination != buffer);
        if (prefixReadResult > 0) ((uint8_t *)destination)[0] = 12;
        if (prefixReadResult > 1) ((uint8_t *)destination)[1] = 0;
        ++reads; return prefixReadResult;
    }
    check(destination == buffer);
    ++reads; readCount = count;
    if (readResult > 0) memset(destination,0xA5,(size_t)readResult);
    return readResult;
}
int16_t DOS_close_file(uint16_t handle) { check(handle == (uint16_t)openHandle && reads == (lengthPrefixed ? 2u : 1u)); ++closes; return -1; }
static void reset(void)
{
    drives = draws = menus = drains = keys = opens = reads = closes = failedOpens = 0;
    HasHardDisk = 0; SecondFloppyDriveAvailable = 0; CurrentMenuLayoutIndex = 9;
    RequestedGameDiskNumber = GameDisk_Second; TextColour = 7;
    prompt[0] = 0; memset(buffer,0xCC,sizeof buffer); openHandle = 3; readResult = 4;
    lengthPrefixed = 0;
}
int main(int argc,char **argv)
{
    if (argc == 2) {
#ifdef _MSC_VER
        /* Sol: test subprocess only: suppress CRT crash-report dialogs so
         * the harness observes termination rather than an interactive wait. */
        _set_abort_behavior(0,_WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif
        reset(); lengthPrefixed = 1;
        prefixReadResult = (int16_t)atoi(argv[1]);
        Load_File_To_Memory((uint8_t *)"synthetic.ANM",buffer);
        return 0; /* Harness rejects this: an unresolved prefix must stop. */
    }
    for (unsigned hard = 0; hard < 2; ++hard)
        for (unsigned second = 0; second < 2; ++second)
            for (uint32_t disk = 0; disk <= UINT16_MAX; ++disk) {
                reset(); HasHardDisk = (uint16_t)hard; SecondFloppyDriveAvailable = (uint16_t)second;
                Select_Game_Disk_And_Drive((uint16_t)disk);
                unsigned usesB = !hard && second && disk >= 2 && disk <= 32767;
                check(RequestedGameDiskNumber == disk && drives == (hard ? 0 : 1 + usesB));
                if (!hard) check(selectedDrive == (usesB ? DOSDrive_B : DOSDrive_A));
            }
    for (unsigned second = 0; second < 2; ++second) {
        for (uint16_t disk = 1; disk <= 2; ++disk) {
            reset(); SecondFloppyDriveAvailable = (uint16_t)second;
            check(Request_Game_Disk(disk) == UINT16_C(0xFFB8));
            const char *expected = second ?
                disk == 1 ? "\rPut the BattleTech Game Disk in drive A: and press a key.\x06\x0F" :
                    "\rPut the BattleTech Disk 2 in drive B: and press a key.\x06\x0F" :
                disk == 1 ? "\rPut the BattleTech Game Disk in the drive and press a key.\x06\x0F" :
                    "\rPut the BattleTech Disk 2 in the drive and press a key.\x06\x0F";
            check(!strcmp(prompt,expected));
            check(draws == 2 && menus == 2 && keys == 1 && CurrentMenuLayoutIndex == 9);
        }
    }
    reset(); DOS_Load_File_to_memory((uint8_t *)"synthetic.ANM",buffer,12);
    check(opens == 1 && reads == 1 && closes == 1 && readCount == 12 && keys == 0);
    check(buffer[0] == 0xA5 && buffer[3] == 0xA5 && buffer[4] == 0xCC);
    reset(); failedOpens = 1; DOS_Load_File_to_memory((uint8_t *)"synthetic.ANM",buffer,12);
    check(opens == 2 && keys == 1 && reads == 1 && closes == 1);
    reset(); readResult = -1; DOS_Load_File_to_memory((uint8_t *)"synthetic.ANM",buffer,0);
    check(reads == 1 && readCount == 0 && opens == 1 && closes == 1 && buffer[0] == 0xCC);
    reset(); openHandle = -2; readResult = 0; DOS_Load_File_to_memory((uint8_t *)"synthetic.ANM",buffer,12);
    check(opens == 1 && keys == 0 && reads == 1 && closes == 1); /* ExactlyFFFF fails, not every negative WORD. */
    reset(); lengthPrefixed = 1;
    check(Load_File_To_Memory((uint8_t *)"synthetic.ANM",buffer) == TRUE);
    check(opens == 1 && reads == 2 && closes == 1 && readCount == 12 && buffer[4] == 0xCC);
    reset(); lengthPrefixed = 1; failedOpens = 1;
    check(Load_File_To_Memory((uint8_t *)"synthetic.ANM",buffer) == TRUE);
    check(opens == 2 && reads == 2 && closes == 1 && keys == 1);
    reset(); lengthPrefixed = 1; readResult = -1;
    check(Load_File_To_Memory((uint8_t *)"synthetic.ANM",buffer) == TRUE);
    check(reads == 2 && closes == 1 && buffer[0] == 0xCC);
    printf("Original disk/file methods: %u checks passed\n",checks); return 0;
}
