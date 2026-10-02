#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { NativeStatsTableBytes=126 };
static uint8_t tableBytes[NativeStatsTableBytes];
static unsigned byteCount;
static void check(int condition)
{
    if(!condition) { fputs("Native Mech-statistics table mismatch\n",stderr); exit(1); }
}
static void byte(uint8_t value)
{
    check(byteCount<NativeStatsTableBytes);
    tableBytes[byteCount++]=value;
}
static void word(uint16_t value)
{
    byte((uint8_t)value); byte((uint8_t)(value>>8));
}
static uint16_t littleWord(const uint8_t *data)
{
    return (uint16_t)((uint16_t)data[0]|((uint16_t)data[1]<<8));
}
int main(int argc,char **argv)
{
    uint32_t fingerprint=UINT32_C(2166136261);
    unsigned i;
    for(i=0;i<MechArmourLocationCount;++i) {
        uint16_t offset=MechStructureOffsetByArmourLocation[i];
        check(offset==(i<MechStructureLocationCount?
            (uint16_t)(offsetof(Mech,currentStructure)+i):0));
        check(MechStatusGaugeX[i]<320 && MechStatusGaugeX[i]%8==0);
        check(MechStatusGaugeBottomY[i]<200);
        word(offset);
    }
    for(i=0;i<MechArmourLocationCount;++i) word(MechStatusGaugeX[i]);
    for(i=0;i<MechArmourLocationCount;++i) word(MechStatusGaugeBottomY[i]);
    for(i=0;i<EgaPaletteRegisterCount;++i) byte(BTStatsEgaPalette[i]);
    for(i=0;i<EgaPaletteRegisterCount;++i) word(BTStatsMcgaPalette[i]);
    for(i=0;i<BTStatsPaletteCycleCount;++i) byte(BTStatsEgaRedCycle[i]);
    for(i=0;i<BTStatsPaletteCycleCount;++i) word(BTStatsMcgaRedCycle[i]);
    check(byteCount==NativeStatsTableBytes);
    for(i=0;i<byteCount;++i) fingerprint=(fingerprint^tableBytes[i])*UINT32_C(16777619);
    /* Fingerprint of original EXE3EDB:1306..1383, serialized little-endian.
     * This proves table transcription, NOT execution of the stats parent. */
    check(fingerprint==UINT32_C(0x0A95DD72));
    if(argc==2) {
        uint8_t header[16],nativeBytes[NativeStatsTableBytes];
        FILE *file=fopen(argv[1],"rb");
        long fileOffset;
        check(file!=NULL);
        check(fread(header,1,sizeof header,file)==sizeof header);
        check(header[0]=='M' && header[1]=='Z');
        fileOffset=(long)littleWord(header+8)*16L+(0x3EDB-0x0800)*16L+0x1306;
        check(fseek(file,fileOffset,SEEK_SET)==0);
        check(fread(nativeBytes,1,sizeof nativeBytes,file)==sizeof nativeBytes);
        check(fclose(file)==0);
        check(memcmp(tableBytes,nativeBytes,sizeof nativeBytes)==0);
        puts("Mech-statistics tables match all 126 bytes in the local unpacked EXE.");
    } else {
        check(argc==1);
        puts("Native Mech-statistics table fingerprint, record offsets and geometry passed.");
    }
    return 0;
}
