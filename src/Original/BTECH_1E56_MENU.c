#include "game.h"

/* Sol: Original1E56:0B5E..0D1C, full raw ASM checked. Draw initial XOR
 * marker before draining input, erase/redraw for recognized movement, retain
 * marker and save selection on ENTER/SPACE. Native ESC tail is unreachable
 * through normal confirmation; no modern Escape-to-cancel is invented. */
uint16_t Display_Menu_Choices_And_Check(uint16_t index)
{
    MenuControl *menu=&MenuControls[index];
    uint16_t baseRow=(uint16_t)(menu->baseRow+TextPanelTop);
    if ((int16_t)menu->selection >= (int16_t)menu->optionCount) menu->selection=0;
    if (DisableInput != 0 || AttractModeRecordingActive != 0) menu->selection=0;
    uint16_t initialSelection=menu->selection;
    uint16_t selection=initialSelection,key=0,awaitingConfirmation=1;
    MenuRendererFlag=0;
    DrawCall_Combat_Menu(TextPanelLeft,(uint16_t)(baseRow+selection),
        menu->highlightWidth,menu->highlightColour);
    Drain_Pending_Keyboard_Input();
    while (awaitingConfirmation != 0) {
        key=Keyboard_Convert_To_MoveCommands(Keyboard_Get_ASCII_Hex_Input());
        if (key=='\r' || key==' ') { awaitingConfirmation=0; continue; }
        /* Sol: EXE83/7 sign-extends immediate B8/B0 to FFB8/FFB0.
         * Generated ASM text lost that extension; .dis retains it. */
        if (key != MenuPreviousCommand && key != MenuNextCommand) continue;
        DrawCall_Combat_Menu(TextPanelLeft,(uint16_t)(baseRow+selection),
            menu->highlightWidth,menu->highlightColour);
        if (key==MenuPreviousCommand) --selection;
        if (key==MenuNextCommand) ++selection;
        if ((int16_t)selection < 0) selection=(uint16_t)(menu->optionCount-1);
        if ((int16_t)menu->optionCount <= (int16_t)selection) selection=0;
        DrawCall_Combat_Menu(TextPanelLeft,(uint16_t)(baseRow+selection),
            menu->highlightWidth,menu->highlightColour);
    }
    if (key != 27) menu->selection=selection;
    else selection=initialSelection;
    return selection;
}
