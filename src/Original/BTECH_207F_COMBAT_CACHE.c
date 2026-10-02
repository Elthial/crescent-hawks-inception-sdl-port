#include "game.h"

/* Original207F:1ECE..1F03. REP MOVSW/CLD transfers 288 words of the
 * generated TILE cache07AD (not descriptors0564). Any nonzero restores.
 * Explicit per-word load-before-store preserves overlapping forward-copy
 * behaviour. No memmove, pointer address alignment assumption or DOS segment
 * emulation. Caller supplies the original valid 576-byte RAM extent. Host C
 * has no represented x86 direction flag; this routine always copies forward. */
void Combat_Copy_Map_Cache(uint8_t *externalBuffer,uint16_t restoreCache)
{
    uint8_t *source=restoreCache?externalBuffer:CombatMap;
    uint8_t *destination=restoreCache?CombatMap:externalBuffer;
    for(uint16_t word=0;word<MapCacheTileCount/sizeof(uint16_t);++word) {
        uint8_t low=source[word*2],high=source[word*2+1];
        destination[word*2]=low; destination[word*2+1]=high;
    }
}
