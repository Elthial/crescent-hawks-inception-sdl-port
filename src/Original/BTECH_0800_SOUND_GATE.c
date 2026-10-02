#include "game.h"

/* Sol: Complete original0800:19BF..19DC, ASM checked. Forward the original
 * WORD ID unchanged when the WORD sound setting is nonzero. No validation,
 * remapping or queue is introduced by this original gateway. */
void Play_Sound_If_Enabled(uint16_t soundId)
{
    if (SoundEffectsEnabled!=FALSE) Sound_Setup(soundId);
}
