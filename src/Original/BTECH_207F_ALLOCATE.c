#include "game.h"
#include "backend.h"

/* Sol: original207F:3835 runtime heap entry. DOS arena lists, INT21 memory
 * allocation and original FAR pointer layout are platform internals, replaced
 * here. Caller byte count and allocation failure remain visible contracts. */
uint8_t *Runtime_Allocate_Word_Buffer(uint16_t byteCount)
{
    return SDLBackend_AllocateWordBuffer(byteCount);
}
