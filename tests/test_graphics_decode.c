/* Sol: Real native decoders; generated streams only, no copyrighted art. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks;
static uint8_t source[65536], sourceBackup[65536], output[PackedGraphicsOutputBytes+2];
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Graphics decode check %u failed\n",checks); exit(1); }
}
static void decode(uint8_t marker)
{
    source[0] = marker; memcpy(sourceBackup,source,sizeof source);
    memset(output,0xCC,sizeof output);
    Decompress_File_Into_Memory(source,output+1);
    check(GraphicsTransferSource == source+1 && GraphicsTransferDestination == output+1);
    check(output[0] == 0xCC && output[PackedGraphicsOutputBytes+1] == 0xCC);
    check(!memcmp(source,sourceBackup,sizeof source));
}
int main(void)
{
    for (unsigned marker = 0; marker < 256; ++marker) {
        memset(source,0,sizeof source);
        source[1] = 0; source[2] = 0; source[3] = 125; source[4] = 0xA5;
        decode((uint8_t)marker);
        for (unsigned i = 0; i < PackedGraphicsOutputBytes; ++i) check(output[i+1] == 0xA5);
    }
    for (unsigned format = 1; format <= 2; ++format) {
        /* Extended zero count must produce output, not skip this run. */
        memset(source,0,sizeof source); source[4] = 0x5A;
        decode((uint8_t)format);
        for (unsigned i = 0; i < PackedGraphicsOutputBytes; ++i) check(output[i+1] == 0x5A);
        /* Extended countFFFF is clipped by the fixed output size. */
        source[2] = source[3] = 255; source[4] = 0x39;
        decode((uint8_t)format);
        check(output[1] == 0x39 && output[PackedGraphicsOutputBytes] == 0x39);

        /* Literal127, repeat128, repeat1, then extended repetition.
         * Distinct commands cross column boundaries in format2. */
        unsigned at = 1; source[at++] = 127;
        for (unsigned i = 0; i < 127; ++i) source[at++] = (uint8_t)i;
        source[at++] = 128; source[at++] = 0xEA;
        source[at++] = 255; source[at++] = 0x77;
        source[at++] = 0; source[at++] = 0; source[at++] = 125; source[at++] = 0x44;
        decode((uint8_t)format);
        for (unsigned i = 0; i < PackedGraphicsOutputBytes; ++i) {
            unsigned destination = format == 1 ? i : (i % 200) * 160 + i / 200;
            uint8_t expected = i < 127 ? (uint8_t)i : i < 255 ? 0xEA : i == 255 ? 0x77 : 0x44;
            check(output[destination+1] == expected);
        }
        /* Entirely literal output, spanning all160 columns/200 rows. */
        at = 1; unsigned produced = 0;
        while (produced < PackedGraphicsOutputBytes) {
            unsigned count = PackedGraphicsOutputBytes - produced;
            if (count > 127) count = 127;
            source[at++] = (uint8_t)count;
            for (unsigned i = 0; i < count; ++i) source[at++] = (uint8_t)produced++;
        }
        decode((uint8_t)format);
        for (unsigned i = 0; i < PackedGraphicsOutputBytes; ++i) {
            unsigned destination = format == 1 ? i : (i % 200) * 160 + i / 200;
            check(output[destination+1] == (uint8_t)i);
        }
    }
    printf("Original graphics decoders: %u checks passed\n",checks);
    return 0;
}
