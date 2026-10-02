#include "backend.h"

/* Hardware replacement for the EGA memory aperture. Four physical planes,
 * not independent staging images: A400/A800 are offsets into A000 RAM.
 * This checkpoint implements full packed-image mode2 transfer, not all EGA
 * register operations. Subsequent primitives must share this same backing. */
static uint8_t planes[EgaPlaneCount][EgaPlaneBytes];
static uint8_t egaLatches[EgaPlaneCount];
static void loadLatches(uint16_t address)
{
    for (uint8_t plane=0;plane<EgaPlaneCount;++plane) egaLatches[plane]=planes[plane][address];
}
uint8_t SDLBackend_EgaWriteMode,SDLBackend_EgaBitMask;
uint8_t SDLBackend_EgaMapMask=EgaAllPlanesMask,SDLBackend_EgaRasterOperation;
uint8_t SDLBackend_EgaReadMapSelect;
uint8_t SDLBackend_EgaRotateCount,SDLBackend_EgaEnableSetReset;
uint8_t SDLBackend_EgaSetReset;
uint16_t SDLBackend_LastSpriteCleanupPort;
static uint8_t paletteRegisters[16];

/* Host scanout only: physical screen starts at A000:0000. Reading the
 * backing planes directly must not prime EGA latches or change port state.
 * This game's200-line mode uses RGBI monitor output: bit4 supplies shared
 * intensity; bits3/5 do not supply independent blue/red intensities. RGBA32 is
 * a byte layout, so write bytes rather than assume the host's endian order. */
void SDLBackend_DecodeEgaScreen(uint32_t *rgba)
{
    uint8_t colours[16][4];
    uint8_t *destination=(uint8_t *)rgba;
    for(uint8_t index=0;index<16;++index) {
        uint8_t colour=paletteRegisters[index];
        colours[index][0]=(uint8_t)(((colour&4)?170:0)+((colour&16)?85:0));
        colours[index][1]=(uint8_t)(((colour&2)?170:0)+((colour&16)?85:0));
        colours[index][2]=(uint8_t)(((colour&1)?170:0)+((colour&16)?85:0));
        if ((colour&0x17)==6) colours[index][1]=85; /* RGBI dark yellow is brown. */
        colours[index][3]=255;
    }
    for(uint16_t address=0;address<EgaScreenPlaneBytes;++address) {
        for(uint8_t bit=128;bit!=0;bit>>=1) {
            uint8_t index=0;
            for(uint8_t plane=0;plane<EgaPlaneCount;++plane)
                if(planes[plane][address]&bit) index|=(uint8_t)(1<<plane);
            for(uint8_t channel=0;channel<4;++channel)
                *destination++=colours[index][channel];
        }
    }
}

/*207F:1E7E..1ECD. The88x88 scene occupies screen(8,8),11 byte-columns
 * per row at DI0141. Plane-interleaved source, inherited rotate/logical op. */
void SDLBackend_DrawAnmFrame(const uint8_t *source)
{
    enum { SceneRows=88,SceneColumns=11,SceneFirstOffset=0x0141 };
    SDLBackend_EgaWriteMode=0; SDLBackend_EgaBitMask=255;
    SDLBackend_EgaEnableSetReset=0;
    uint16_t address=SceneFirstOffset;
    uint8_t rotate=(uint8_t)(SDLBackend_EgaRotateCount&7);
    for (uint16_t row=0;row<SceneRows;++row) {
        for (uint16_t column=0;column<SceneColumns;++column) {
            for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
                SDLBackend_EgaMapMask=(uint8_t)(1<<plane);
                loadLatches(address);
                uint8_t value=*source++,old=planes[plane][address];
                if (rotate) value=(uint8_t)((value>>rotate)|(value<<(8-rotate)));
                switch (SDLBackend_EgaRasterOperation) {
                    case EgaRaster_And: value&=old; break;
                    case EgaRaster_Or: value|=old; break;
                    case EgaRaster_Xor: value^=old; break;
                    default: break;
                }
                planes[plane][address]=value;
            }
            ++address;
        }
        address+=(ScreenWidth/EgaPixelsPerByte)-SceneColumns;
    }
    SDLBackend_EgaMapMask=EgaAllPlanesMask;
}

/*207F:0A9F..0B25. Two columns per16-row tile, contiguous EGA storage.
 * Native read primes all latches before each plane's mode0 MOVSB write.
 * DF-clear source is the retained caller contract; WORD destination wraps. */
void SDLBackend_UploadAnimatedEgaTile(const uint8_t *source,uint16_t destinationOffset)
{
    SDLBackend_EgaWriteMode=0; SDLBackend_EgaBitMask=255;
    SDLBackend_EgaEnableSetReset=0;
    uint16_t address=(uint16_t)(((EgaTilesetSegment-EgaApertureSegment)<<4)+destinationOffset);
    uint8_t rotate=(uint8_t)(SDLBackend_EgaRotateCount&7);
    for (uint16_t column=0;column<32;++column) {
        for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
            SDLBackend_EgaMapMask=(uint8_t)(1<<plane);
            loadLatches(address);
            uint8_t value=*source++,old=planes[plane][address];
            if (rotate) value=(uint8_t)((value>>rotate)|(value<<(8-rotate)));
            switch (SDLBackend_EgaRasterOperation) {
                case EgaRaster_And: value&=old; break;
                case EgaRaster_Or: value|=old; break;
                case EgaRaster_Xor: value^=old; break;
                default: break;
            }
            planes[plane][address]=value;
        }
        ++address;
    }
    SDLBackend_EgaMapMask=EgaAllPlanesMask;
}

/*207F:284D..28A7 mode0 tile transfer. Explicit plane selection overrides
 * incoming map mask; each source byte gets inherited rotate/logical operation
 * against that destination's primed latch. Enable-set/reset is disabled, mask
 * isFF, and all planes are enabled on exit. No coordinate clipping is added. */
void SDLBackend_DrawEgaTile(const uint8_t *tile,uint16_t column,uint16_t row)
{
    SDLBackend_EgaWriteMode=0;
    SDLBackend_EgaBitMask=255;
    SDLBackend_EgaEnableSetReset=0;
    uint16_t address=(uint16_t)(column+row*(ScreenWidth/EgaPixelsPerByte)*8);
    uint8_t rotate=(uint8_t)(SDLBackend_EgaRotateCount&7);
    for (uint8_t scanline=0;scanline<8;++scanline) {
        for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
            SDLBackend_EgaMapMask=(uint8_t)(1<<plane);
            loadLatches(address); /* native read before mode0 write */
            uint8_t old=planes[plane][address];
            uint8_t value=*tile++;
            if (rotate) value=(uint8_t)((value>>rotate)|(value<<(8-rotate)));
            switch (SDLBackend_EgaRasterOperation) {
                case EgaRaster_And: value&=old; break;
                case EgaRaster_Or: value|=old; break;
                case EgaRaster_Xor: value^=old; break;
                default: break;
            }
            planes[plane][address]=value;
        }
        address+=(ScreenWidth/EgaPixelsPerByte);
    }
    SDLBackend_EgaMapMask=EgaAllPlanesMask;
}

/* Latch-primed mode2 write at one physical byte address. Original primitives
 * read that same location before writing CPU colour, preserving enabled planes
 * and raster operation. This is hardware state, not a gameplay helper. */
static void writeLatchedColour(uint16_t address,uint8_t colour)
{
    loadLatches(address);
    uint8_t mask=SDLBackend_EgaBitMask;
    for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
        if (!(SDLBackend_EgaMapMask&(1<<plane))) continue;
        uint8_t old=planes[plane][address];
        uint8_t value=(colour&(1<<plane))?255:0;
        switch (SDLBackend_EgaRasterOperation) {
            case EgaRaster_And: value&=old; break;
            case EgaRaster_Or: value|=old; break;
            case EgaRaster_Xor: value^=old; break;
            default: break;
        }
        planes[plane][address]=(uint8_t)((old&(uint8_t)~mask)|(value&mask));
    }
}

/*207F:0637..067C retained EGA body. MUL DL truncates the initial Y only;
 * signed endpoint testing still uses the full WORD and occurs after writing. */
void SDLBackend_DrawEgaVerticalRun(uint16_t column,uint16_t top,uint16_t bottom,uint8_t colour)
{
    SDLBackend_EgaWriteMode=2;
    SDLBackend_EgaBitMask=(uint8_t)(0x80>>(column&(EgaPixelsPerByte-1)));
    uint16_t address=(uint16_t)((uint8_t)top*(ScreenWidth/EgaPixelsPerByte)+(column>>3));
    do {
        writeLatchedColour(address,colour);
        address+=(ScreenWidth/EgaPixelsPerByte);
        ++top;
    } while ((int16_t)top <= (int16_t)bottom);
}

/*207F:07D8..080B. Native LOOP underflows an initial zero count, writing
 * all65536 byte addresses once. No invented screen clipping is performed. */
void SDLBackend_DrawEgaAlignedSpan(uint16_t column,uint16_t row,uint16_t groupCount,uint8_t colour)
{
    SDLBackend_EgaWriteMode=2;
    SDLBackend_EgaBitMask=255;
    uint16_t address=(uint16_t)((uint8_t)row*(ScreenWidth/EgaPixelsPerByte)+(column>>3));
    do { writeLatchedColour(address++,colour); } while (--groupCount != 0);
}
void SDLBackend_SetEgaPaletteRegister(uint8_t index,uint8_t colour)
{
    /* Native calls use indices0..15; hardware palette data is six bits.
     * Preserve actual register values, not guessed RGB monitor conversion. */
    paletteRegisters[index]=(uint8_t)(colour&0x3F);
}
/*207F:2CB0..2CDE. Eight scanlines, unguarded width LOOP and inherited
 * enabled planes. XOR register write also zeroes rotate count; exit resets
 * logical operation to replace. Width0 visits all64KiB eight times. */
void SDLBackend_ToggleEgaHighlight(uint16_t column,uint16_t row,uint16_t width,uint8_t colour)
{
    SDLBackend_EgaWriteMode=2;
    SDLBackend_EgaBitMask=255;
    SDLBackend_EgaRotateCount=0;
    SDLBackend_EgaRasterOperation=EgaRaster_Xor;
    uint16_t address=(uint16_t)(column+row*8*(ScreenWidth/EgaPixelsPerByte));
    for (uint8_t scanline=0;scanline<8;++scanline) {
        uint16_t remaining=width;
        do { writeLatchedColour(address++,colour); } while (--remaining != 0);
        address=(uint16_t)(address+ScreenWidth/EgaPixelsPerByte-width);
    }
    SDLBackend_EgaRasterOperation=EgaRaster_Replace;
}
uint8_t SDLBackend_GetEgaPaletteRegister(uint8_t index)
{
    return paletteRegisters[index];
}

/*2251 hardware replacement: two latch-primed mode2 writes per bitmap row.
 * Foreground mask then its complement; inherited enabled planes/raster state.
 * No clipping or mask restoration added. Legal native glyph locations required. */
void SDLBackend_DrawEgaGlyph(const uint8_t *bitmap,uint16_t rowByteOffset,uint16_t column,uint8_t foreground,uint8_t background)
{
    uint16_t address=(uint16_t)(rowByteOffset+column);
    SDLBackend_EgaWriteMode=2;
    for (uint16_t row=0;row<8;++row) {
        for (uint8_t pass=0;pass<2;++pass) {
            uint8_t mask=pass?(uint8_t)~bitmap[row]:bitmap[row];
            uint8_t colour=pass?background:foreground;
            SDLBackend_EgaBitMask=mask;
            loadLatches(address);
            for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
                if (!(SDLBackend_EgaMapMask&(1<<plane))) continue;
                uint8_t old=planes[plane][address];
                uint8_t value=(colour&(1<<plane))?255:0;
                switch (SDLBackend_EgaRasterOperation) {
                    case EgaRaster_And: value&=old; break;
                    case EgaRaster_Or: value|=old; break;
                    case EgaRaster_Xor: value^=old; break;
                    default: break;
                }
                planes[plane][address]=(uint8_t)((old&(uint8_t)~mask)|(value&mask));
            }
        }
        address+=ScreenWidth/EgaPixelsPerByte;
    }
}

uint8_t SDLBackend_ReadEgaPlaneByte(uint16_t segment,uint16_t offset,uint8_t plane)
{
    uint16_t address=(uint16_t)(((uint32_t)(uint16_t)(segment-EgaApertureSegment)<<4)+offset);
    return planes[plane][address];
}

/* Hardware mode1 or mode2/mask0 MOVSB latch transfer used by original copies.
 * All plane latches load before any destination write; enabled planes only.
 * Callers configure one of those latch-copy modes; no state reset is implicit. */
void SDLBackend_CopyEgaLatchByte(uint16_t sourceSegment,uint16_t sourceOffset,uint16_t destinationSegment,uint16_t destinationOffset)
{
    loadLatches((uint16_t)(((uint32_t)(uint16_t)(sourceSegment-EgaApertureSegment)<<4)+sourceOffset));
    uint16_t address=(uint16_t)(((uint32_t)(uint16_t)(destinationSegment-EgaApertureSegment)<<4)+destinationOffset);
    for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
        if (SDLBackend_EgaMapMask&(1<<plane)) planes[plane][address]=egaLatches[plane];
}

/* Original207F:0260 writes eight packed pixel nibbles into each byte-column
 * using write-mode2 and masks80,40,...,01. All eight bits are replaced, so
 * the result is equivalent to constructing the four plane bytes directly.
 * 8000 byte-columns consume32000 packed BYTEs; no palette remapping occurs.
 * Final hardware write sets bitmask zero, notFF. */
void SDLBackend_TransferPackedImage(const uint8_t *image,uint16_t destinationSegment)
{
    uint16_t source=0;
    uint16_t address=(uint16_t)((uint32_t)(uint16_t)(destinationSegment-EgaApertureSegment)<<4);
    SDLBackend_EgaWriteMode=2;
    for (uint16_t column=0;column<EgaScreenPlaneBytes;++column) {
        uint8_t value[EgaPlaneCount]={0,0,0,0};
        for (uint8_t pair=0;pair<EgaPixelsPerByte/PackedPixelsPerByte;++pair) {
            uint8_t packed=image[source++];
            uint8_t highMask=(uint8_t)(0x80>>(pair*2));
            uint8_t lowMask=(uint8_t)(highMask>>1);
            for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
                if ((packed>>4)&(1<<plane)) value[plane]|=highMask;
                if (packed&(1<<plane)) value[plane]|=lowMask;
            }
        }
        /* The native method changes neither sequencer map-mask nor raster
         * operation. Preserve those hardware states instead of assuming reset. */
        for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
            uint8_t old=planes[plane][address];
            if (!(SDLBackend_EgaMapMask&(1<<plane))) { egaLatches[plane]=old; continue; }
            switch (SDLBackend_EgaRasterOperation) {
                case EgaRaster_And: value[plane]&=old; break;
                case EgaRaster_Or: value[plane]|=old; break;
                case EgaRaster_Xor: value[plane]^=old; break;
                default: break;
            }
            planes[plane][address]=value[plane];
            /*0260's final aperture read precedes its bit01 write. The lower
             * bit is still incoming RAM, other seven bits are the new image. */
            egaLatches[plane]=(uint8_t)((value[plane]&0xFE)|(old&1));
        }
        ++address;
    }
    SDLBackend_EgaBitMask=0;
}

/* Original207F:0313..0376: A800 source, eight rows, four successive plane
 * bytes per row. Arguments are byte-column/tile-row, not pixel coordinates.
 * Original destination FAR pointer is supplied as its corresponding host RAM
 * pointer. Legal native rectangles and sufficient destination RAM are required. */
void SDLBackend_CaptureEgaTile(uint8_t *destination,uint16_t byteColumn,uint16_t tileRow)
{
    enum { TileHeight=8, EgaRowBytes=ScreenWidth/EgaPixelsPerByte,
        EgaStagingSegment=0xA800 };
    uint16_t source=(uint16_t)(tileRow*(TileHeight*EgaRowBytes)+byteColumn);
    SDLBackend_EgaWriteMode=2;
    SDLBackend_EgaBitMask=0;
    for (uint16_t row=0;row<TileHeight;++row) {
        for (uint8_t plane=0;plane<EgaPlaneCount;++plane) {
            SDLBackend_EgaReadMapSelect=plane;
            loadLatches((uint16_t)(0x8000+source));
            *destination++=SDLBackend_ReadEgaPlaneByte(EgaStagingSegment,source,plane);
        }
        source=(uint16_t)(source+EgaRowBytes);
    }
}

/*207F:1FBE..200D retained EGA.8000 STOSW writes16000 bytes, NOT8000.
 * No aperture read occurs: inherited latches stay frozen through the fill.
 * Inherited map-mask/ROP survive; CPU colour0 with FF bitmask means logical
 * OR/XOR writes those stale latches, AND/replace writes zero. DF-clear contract. */
void SDLBackend_ClearEgaScreen(void)
{
    SDLBackend_EgaWriteMode=2; SDLBackend_EgaBitMask=255;
    for (uint16_t address=0;address<2*EgaScreenPlaneBytes;++address)
        for (uint8_t plane=0;plane<EgaPlaneCount;++plane)
            if (SDLBackend_EgaMapMask&(1<<plane))
                planes[plane][address]=(SDLBackend_EgaRasterOperation==EgaRaster_Or ||
                    SDLBackend_EgaRasterOperation==EgaRaster_Xor)?egaLatches[plane]:0;
}

/* Native mode0 CPU read/modify/write. Every read loads ALL latches; writes
 * inherit rotate, set/reset enable/value and logical operation with maskFF. */
static void spriteReadModifyWrite(uint16_t address,uint8_t plane,uint8_t preserve,uint8_t bits)
{
    for (uint8_t operation=0;operation<2;++operation) {
        loadLatches(address);
        uint8_t value=operation?(uint8_t)(egaLatches[plane]|bits):(uint8_t)(egaLatches[plane]&preserve);
        uint8_t rotate=(uint8_t)(SDLBackend_EgaRotateCount&7);
        if (rotate) value=(uint8_t)((value>>rotate)|(value<<(8-rotate)));
        if (SDLBackend_EgaEnableSetReset&(1<<plane)) value=(SDLBackend_EgaSetReset&(1<<plane))?255:0;
        switch (SDLBackend_EgaRasterOperation) {
            case EgaRaster_And: value&=egaLatches[plane]; break;
            case EgaRaster_Or: value|=egaLatches[plane]; break;
            case EgaRaster_Xor: value^=egaLatches[plane]; break;
            default: break;
        }
        planes[plane][address]=value;
    }
}

/*207F:0377..0571 complete retained hardware body. Valid stable RAM sprites,
 * non-straddling header WORD, DF-clear and native caller DX=destination segment
 * are contracts. Header heightFF wraps to zero, do/LOOP underflows preserved.
 * Colour0 is transparent; no next-row spill from byte-column39. */
void SDLBackend_DrawEgaSprite(uint16_t segment,uint16_t offset,const uint8_t *sprite,int16_t x,int16_t y)
{
    uint16_t cleanupPort=segment,source=4;
    uint16_t rows=(uint8_t)(sprite[1]+1),columns=sprite[2],stride=(uint16_t)(columns*4);
    /* Native SAR floors negative X, unlike C signed division. */
    int16_t column=(int16_t)(x>=0?x/8:-((-(int32_t)x+7)/8));
    if (y<0) {
        uint16_t clippedRows=(uint16_t)(-(int32_t)y);
        if (clippedRows>=rows) goto SpriteCleanup;
        uint32_t skip=(uint32_t)clippedRows*stride;
        cleanupPort=(uint16_t)(skip>>16); /* MUL overwrites incoming DX */
        source=(uint16_t)(source+(uint16_t)skip);
        uint16_t remaining=(uint16_t)(rows+y);
        if ((int16_t)remaining<0) goto SpriteCleanup;
        rows=remaining; y=0;
    }
    int16_t remainingRows=(int16_t)(uint16_t)(ScreenHeight-y);
    if (remainingRows<=0) goto SpriteCleanup;
    if ((uint16_t)remainingRows<rows) rows=(uint16_t)remainingRows;
    if (column<0) {
        columns=(uint16_t)(columns+column);
        uint16_t skip=(uint16_t)((uint16_t)(-(int32_t)column)*4);
        source=(uint16_t)(source+skip);
        if (skip>=stride) goto SpriteCleanup;
        column=0;
    }
    int16_t remainingColumns=(int16_t)(uint16_t)(ScreenWidth/EgaPixelsPerByte-column);
    if (remainingColumns<=0) goto SpriteCleanup;
    if ((uint16_t)remainingColumns<columns) columns=(uint16_t)remainingColumns;
    uint16_t destination=(uint16_t)(((uint32_t)(uint16_t)(segment-EgaApertureSegment)<<4)+
        offset+(uint8_t)y*(ScreenWidth/EgaPixelsPerByte)+(uint16_t)column);
    uint16_t rowGap=(uint16_t)(ScreenWidth/EgaPixelsPerByte-columns);
    uint8_t shift=(uint8_t)((uint16_t)x&7);
    cleanupPort=0x3CE;
    SDLBackend_EgaWriteMode=0; SDLBackend_EgaBitMask=255;
    do {
        uint16_t screenColumn=(uint16_t)column,remaining=columns;
        do {
            uint8_t occupied=(uint8_t)(sprite[source]|sprite[(uint16_t)(source+1)]|
                sprite[(uint16_t)(source+2)]|sprite[(uint16_t)(source+3)]);
            uint16_t preserve=(uint16_t)~((uint16_t)(occupied<<8)>>shift);
            for (uint8_t plane=0;plane<4;++plane) {
                SDLBackend_EgaReadMapSelect=plane; SDLBackend_EgaMapMask=(uint8_t)(1<<plane);
                cleanupPort=0x3C4;
                uint16_t bits=(uint16_t)((uint16_t)(sprite[source++]<<8)>>shift);
                spriteReadModifyWrite(destination,plane,(uint8_t)(preserve>>8),(uint8_t)(bits>>8));
                if (screenColumn<ScreenWidth/EgaPixelsPerByte-1)
                    spriteReadModifyWrite((uint16_t)(destination+1),plane,(uint8_t)preserve,(uint8_t)bits);
            }
            ++screenColumn; ++destination;
        } while (--remaining!=0);
        source=(uint16_t)(source+stride-columns*4);
        destination=(uint16_t)(destination+rowGap);
    } while (--rows!=0);
SpriteCleanup:
    SDLBackend_LastSpriteCleanupPort=cleanupPort;
    /* BUG-019: do NOT turn every early return into successful mask restoration.
     * Valid original early paths have AC00 or MUL-high<=3, not sequencer3C4. */
    if (cleanupPort==0x3C4) SDLBackend_EgaMapMask=EgaAllPlanesMask;
}
