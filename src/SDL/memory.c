#include "backend.h"
#include <SDL3/SDL.h>

uint8_t *SDLBackend_AllocateWordBuffer(uint16_t byteCount)
{
    /* Sol: original zero-byte heap requests still acquire an allocation.
     * Do not clear it: sprite-header padding and other scratch bytes are
     * not initialized by the original allocation routine. */
    return SDL_malloc(byteCount==0?1:(size_t)byteCount);
}
