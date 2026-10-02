#include "game.h"

/* 0800:28CC..2912. Sol: Store the requested logical game disk and select
 * floppy A/B when not installed on hard disk. Native JLE compares SIGNED
 * WORDs; IDs8000..FFFF therefore leave drive A selected. */
void Select_Game_Disk_And_Drive(uint16_t requestedDiskNumber)
{
    RequestedGameDiskNumber = requestedDiskNumber;
    if (HasHardDisk == FALSE)
    {
        DOS_Select_Default_Drive(DOSDrive_A);
        if (SecondFloppyDriveAvailable != FALSE && (int16_t)requestedDiskNumber > GameDisk_First)
            DOS_Select_Default_Drive(DOSDrive_B);
    }
}

/* 0800:2913..29F4 maintained EGA path. Sol: Show the original disk/drive
 * prompt, drain/read a key, redraw and restore the prior menu layout.
 * Its colour/control effects are intentionally not separately restored. */
uint16_t Request_Game_Disk(uint16_t requestedDiskNumber)
{
    Select_Game_Disk_And_Drive(requestedDiskNumber);
    uint16_t previousMenuLayout = CurrentMenuLayoutIndex;
    Menu_Memory_Variables(DiskPromptMenuLayout);
    Draw_Top_Graphic_Sidebar();
    TextColour = EGA_BrightRed;
    Display_Text_From_Memory((uint8_t *)"\rPut the BattleTech "); /* 3EDB:04BA */
    if (RequestedGameDiskNumber == GameDisk_First)
        Display_Text_From_Memory((uint8_t *)"Game Disk"); /* 3EDB:04CF */
    else
        Display_Text_From_Memory((uint8_t *)"Disk 2"); /* 3EDB:04D9 */
    Display_Text_From_Memory((uint8_t *)" in "); /* 3EDB:04E0 */
    if (SecondFloppyDriveAvailable == FALSE)
        Display_Text_From_Memory((uint8_t *)"the drive"); /* 3EDB:04E5 */
    else
    {
        DiskDriveNameText[DiskDriveLetterPosition] =
            (uint8_t)('A' + (requestedDiskNumber != GameDisk_First));
        Display_Text_From_Memory(DiskDriveNameText);
    }
    Display_Text_From_Memory((uint8_t *)" and press a key.\x06\x0F"); /* 3EDB:04EF */
    Drain_Pending_Keyboard_Input();
    uint16_t keyPress = Keyboard_Get_ASCII_Hex_Input();
    Draw_Top_Graphic_Sidebar();
    Menu_Memory_Variables(previousMenuLayout);
    return keyPress;
}
