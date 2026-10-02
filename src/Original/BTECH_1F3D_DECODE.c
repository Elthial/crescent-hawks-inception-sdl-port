#include "game.h"

/* 1F3D:049D maintained EGA path. Sol: Read ONE marker BYTE; select the
 * sequential decoder only for marker1, otherwise the column-wise decoder.
 * Adapter0 post-conversion was removed from the annotated EGA-only source. */
void Decompress_File_Into_Memory(uint8_t *compressed, uint8_t *graphics)
{
    uint8_t formatMarker = *compressed;
    uint8_t *encodedPayload = compressed + 1;
    if (formatMarker == GraphicsFormat_Sequential)
        Format01_Decode(encodedPayload,graphics);
    else
        Format02_Decode(encodedPayload,graphics);
}
