#include "game.h"

/* Sol: Original 1E56:0D1D. Convert ASCII movement keys and native aliases,
 * leaving every other full WORD unchanged. Correct the annotated switch's
 * stale mappings using table 0E35..0E6F, including its last EXE-owned WORD. */
uint16_t Keyboard_Convert_To_MoveCommands(uint16_t key)
{
    switch (key) {
    case 'A': case 'a': case '4': case '|': return COMMAND_MOVE_West;
    case 'C': case 'c': case '3': return COMMAND_MOVE_SouthEast;
    case 'D': case 'd': case '6': case 0x000C: case 0xFF0C: return COMMAND_MOVE_East;
    case 'E': case 'e': case '9': return COMMAND_MOVE_NorthEast;
    case 'Q': case 'q': case '7': case '\\': return COMMAND_MOVE_NorthWest;
    case 'W': case 'w': case '8': case '~': return COMMAND_MOVE_North;
    case 'X': case 'x': case '2': case '`': return COMMAND_MOVE_South;
    case 'Z': case 'z': case '1': return COMMAND_MOVE_SouthWest;
    default: return key;
    }
}
