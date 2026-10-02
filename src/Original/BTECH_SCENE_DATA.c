#include "game.h"
/* Original246C:244B..42C2 cleared frame workspace followed immediately by
 * loaded header/stream42C3 onward. Signed timing indices can address the
 * neighbouring frame bytes; do not turn this into separate unrelated arrays. */
uint16_t AnimationStreamOffset,AnimationFrameNumber;
uint16_t OuttakeFrequencyRandomMask[OuttakeFrequencySettingCount]={3,15,63};
