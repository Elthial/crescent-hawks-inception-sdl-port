#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint16_t keys[8];
static unsigned keyCount, keyIndex, redraws, drains;
static uint8_t lastText[18];
static void check(int condition)
{
    if (!condition) { fputs("Original prompt witness failed\n", stderr); exit(1); }
}
/* Test-only input and renderer adapters; renderer consumes the embedded
 * zero-valued control parameters rather than treating them as terminators. */
uint16_t Keyboard_Get_ASCII_Hex_Input(void)
{
    check(keyIndex < keyCount);
    return keys[keyIndex++];
}
void Drain_Pending_Keyboard_Input(void) { ++drains; }
void Display_Text_From_Memory(uint8_t *text)
{
    size_t bytes = text[1] == 6 ? 16 : 18;
    memcpy(lastText, text, bytes);
    ++redraws;
    TextRow = (uint16_t)(TextRow + 1);
    TextColour = 14;
}
static void run(uint16_t defaultYes, uint16_t expected)
{
    keyIndex = redraws = drains = 0;
    TextRow = 10;
    TextColour = 12;
    check(Prompt_Yes_No(defaultYes) == expected);
    check(keyIndex == keyCount && redraws == keyCount + 1 && drains == 1);
    check(TextRow == 11 && TextColour == 12);
    check(lastText[1] == (expected ? 6 : 2));
}
int main(void)
{
    check(Keyboard_Convert_To_MoveCommands('{') == '{');
    check(Keyboard_Convert_To_MoveCommands('|') == COMMAND_MOVE_West);
    check(Keyboard_Convert_To_MoveCommands('~') == COMMAND_MOVE_North);
    check(Keyboard_Convert_To_MoveCommands(0xFF0C) == COMMAND_MOVE_East);
    check(Keyboard_Convert_To_MoveCommands(0xABCD) == 0xABCD);
    keyCount = 1; keys[0] = '\r'; run(2, TRUE);
    keys[0] = ' '; run(FALSE, FALSE);
    keys[0] = 'n'; run(TRUE, FALSE);
    keys[0] = 'Y'; run(FALSE, TRUE);
    keyCount = 3; keys[0] = '?'; keys[1] = 'd'; keys[2] = '\r'; run(TRUE, FALSE);
    keyCount = 2; keys[0] = '|'; keys[1] = ' '; run(FALSE, TRUE);
    puts("Original prompt and movement-key witnesses passed");
    return 0;
}
