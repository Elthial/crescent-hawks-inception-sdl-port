#include "game.h"
#include "../SDL/backend.h"

/* Original207F:0A9F..0B25:128 interleaved bytes ->32 consecutive A400
 * byte columns. Hardware upload, not an invented graphics/gameplay helper. */
void EGA_Upload_Animated_Tile(uint8_t *source,uint16_t destinationOffset)
{
    SDLBackend_UploadAnimatedEgaTile(source,destinationOffset);
}

/* Original207F:1E37 retained1E7E..1ECD EGA/shared1E78 exit.
 * Native packed244B buffer is followed by converted planes336B. */
void DrawCall_EGA_Animations(void)
{
    SDLBackend_DrawAnmFrame(AnimationFrameWorkspace+AnmPackedFrameBytes);
}

/* Sol: Original207F:275C retained284D..28A7 EGA body/shared2819 exit,
 * checked against complete raw ASM. Source is32 RAM bytes, four plane bytes
 * per eight-pixel row. Native B78C source-segment staging is DOS address
 * bookkeeping, replaced by the flat source parameter, not a guessed segment
 * value. No deleted adapter branch or annotation FAR helper is included.
 * Native EGA branch inherits DF-clear; valid non-wrapping source tiles are
 * the host contract. Hardware/register effects remain in the SDL folder. */
void DrawCall_SingleTile(uint8_t *tile,uint16_t column,uint16_t row)
{
    SDLBackend_DrawEgaTile(tile,column,row);
}
