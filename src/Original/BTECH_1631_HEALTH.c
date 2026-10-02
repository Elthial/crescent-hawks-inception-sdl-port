#include "game.h"

/* Original DS2E0C FAR table, EXE-owned strings including leading spaces. */
uint8_t *HealthDescriptions[HealthDescriptionCount]={
    (uint8_t *)" looks like Death himself.",
    (uint8_t *)" amazingly, is still standing.",
    (uint8_t *)" can't take another hit.",
    (uint8_t *)" is bleeding pretty hard.",
    (uint8_t *)" has definitely been injured.",
    (uint8_t *)" is grimacing in pain.",
    (uint8_t *)" is showing some wear and tear.",
    (uint8_t *)" could use some bandages.",
    (uint8_t *)" is starting to get mad.",
    (uint8_t *)" is just getting warmed up.",
    (uint8_t *)" is the picture of health."
};

/* Original1631:02E4..032E: CBW both BYTEs, CWD/IDIV, index FAR table.
 * This prints a health DESCRIPTION, not a bar or percentage. No zero-Body
 * guard, quotient clamp or replacement text exists in the original.
 * Nonzero body and a quotient naming an actual table entry are required. */
void Display_Text_Human_Health(uint16_t characterId)
{
    int16_t healthBand=(int16_t)((int8_t)Characters[characterId].health/
        (int16_t)(int8_t)Characters[characterId].body);
    Display_Text_From_Memory(HealthDescriptions[healthBand]);
}
