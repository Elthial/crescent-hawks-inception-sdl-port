#include "game.h"

/* Original 3EDB:03EE/03FE: text plus the renderer's colour/cursor controls.
 * Numeric escapes are controls, not replacement labels for the visible text. */
static uint8_t yesSelectedText[] = "\r\x06\x00\x02\x0E" "Yes" "\x06\x0F\x02\x00" " No";
static uint8_t noSelectedText[] = "\r\x02\x00" "Yes " "\x06\x00\x02\x0E" "No" "\x06\x0F\x02\x00";

/* Sol: Original 0800:1A13. Highlight the default; Y/N confirm directly,
 * east/west change selection, Enter/Space confirm. Redraw after every key,
 * including confirmation, then restore the caller's colour. */
uint16_t Prompt_Yes_No(uint16_t defaultYes)
{
    uint16_t savedTextColour = TextColour;
    uint16_t selectedYes = (defaultYes != FALSE);
    uint16_t selectionConfirmed = FALSE;

    Display_Text_From_Memory(selectedYes ? yesSelectedText : noSelectedText);
    Drain_Pending_Keyboard_Input();
    while (selectionConfirmed == FALSE) {
        uint16_t key = Keyboard_Convert_To_MoveCommands(Keyboard_Get_ASCII_Hex_Input());
        if (key == 'Y' || key == 'y') {
            selectedYes = TRUE;
            selectionConfirmed = TRUE;
        } else if (key == 'N' || key == 'n') {
            selectedYes = FALSE;
            selectionConfirmed = TRUE;
        } else if (key == COMMAND_MOVE_West) {
            selectedYes = TRUE;
        } else if (key == COMMAND_MOVE_East) {
            selectedYes = FALSE;
        } else if (key == '\r' || key == ' ') {
            selectionConfirmed = TRUE;
        }
        TextRow = (uint16_t)(TextRow - 1);
        Display_Text_From_Memory(selectedYes ? yesSelectedText : noSelectedText);
    }
    TextColour = savedTextColour;
    return selectedYes;
}
