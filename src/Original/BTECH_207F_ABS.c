#include "game.h"
/* Sol: original207F:3C6C..3C80 WORD NEG, not host int abs.
 * Minimum signed WORD8000 stays negative, preserving original overflow. */
int16_t Word_Absolute(int16_t value)
{
    return value<0?(int16_t)(uint16_t)-(uint16_t)value:value;
}
