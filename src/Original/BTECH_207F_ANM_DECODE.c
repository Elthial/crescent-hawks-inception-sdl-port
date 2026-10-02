#include "game.h"

/* Original207F:23EC..245B, all instructions checked. XOR delta,3872 bytes.
 * Zero command: BIG-endian WORD repeat count; negative BYTE: repeated value.
 * Literal/repeat zero counts run until output exhaustion (native LOOP).
 * Valid non-straddling source/destination RAM windows are the host contract. */
uint16_t Animation_Decode(uint8_t *encoded,uint8_t *frame)
{
    uint16_t source=0,destination=0,outputRemaining=AnmPackedFrameBytes;
    for (;;) {
        uint16_t count=encoded[source];
        uint16_t repeat=FALSE;
        if (!count) {
            ++source;
            count=(uint16_t)(((uint16_t)encoded[source]<<8)|encoded[(uint16_t)(source+1)]);
            ++source;
            repeat=TRUE;
        } else if (count&0x80) {
            count=(uint8_t)(0-count);
            repeat=TRUE;
        }

        if (repeat) {
            uint8_t value=encoded[++source];
            do {
                frame[destination++]^=value;
                if (!--outputRemaining) return (uint16_t)(source+1);
            } while (--count);
        } else {
            do {
                uint8_t value=encoded[++source];
                frame[destination++]^=value;
                if (!--outputRemaining) return (uint16_t)(source+1);
            } while (--count);
        }
        ++source;
    }
}
