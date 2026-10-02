#include "game.h"

/* 0800:2A2B. Sol: Do not drain real input during replay; otherwise consume
 * through the original input bridge until its availability probe is empty. */
void Drain_Pending_Keyboard_Input(void)
{
    if (DisableInput != FALSE) return;
    while (Pending_Input() != FALSE) Keyboard_Get_ASCII_Hex_Input();
}
/* 0800:2A4F. Sol: Exact continue prompt, then original blocking input. */
void Prompt_And_Wait_For_Key(void)
{
    Display_Text_From_Memory((uint8_t *)"\rPress a key."); /* 3EDB:050D */
    Keyboard_Get_ASCII_Hex_Input();
}
/* 0800:2A69/2A7E. Sol: Fixed suffix and punctuation, not new text helpers. */
void Display_Plural_Suffix(void) { Display_Text_From_Memory((uint8_t *)"s"); }
void Display_Sentence_Period(void) { Display_Text_From_Memory((uint8_t *)"."); }
