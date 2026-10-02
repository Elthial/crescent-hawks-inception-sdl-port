#include "game.h"
#include <stdio.h>
#include <stdlib.h>

/* Sol: original1F3D:05BC..063A. Original callers pass a zero high WORD
 * (sprite capture) or CWD sign extension (tileset capture). Allocation is
 * always the LOW WORD, including negative high WORD inputs.
 * Positive high WORDs expose an unassigned native FAR stack return. Until
 * represented, fail explicitly AFTER its original message/input, rather
 * than return an invented NULL or use an uninitialized host pointer.
 * This unsupported branch is NOT a preservation certificate. */
uint8_t *Allocate_Far_Buffer(uint32_t requestedBytes)
{
    uint16_t byteCount=(uint16_t)requestedBytes;
    if ((int16_t)(requestedBytes >> 16)>0) {
        Draw_EGA_Text_To_Screen((uint8_t *)"Alloc too big!",0,10,EGA_BrightWhite,EGA_Black);
        Keyboard_Get_ASCII_Hex_Input();
        fputs("Unresolved original1F3D:05BC stack return on oversized allocation\n",stderr);
        abort();
    }
    uint8_t *allocation=Runtime_Allocate_Word_Buffer(byteCount);
    if (allocation==NULL) {
        Draw_EGA_Text_To_Screen((uint8_t *)"Alloc: Null pointer return!",0,10,EGA_BrightWhite,EGA_Black);
        Keyboard_Get_ASCII_Hex_Input();
    }
    return allocation; /* Sol: original null result is returned; no retry/abort. */
}
