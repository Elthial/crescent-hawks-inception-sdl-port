#include "game.h"
#ifdef CHI_SDL_PLATFORM
#include "backend.h"
#endif

/* Sol: Original1F3D:06C3/06D1..06F5, EGA path checked against raw ASM.
 * Present the 216x200 working viewport, not a border-drawing operation. */
void EGA_DrawBox_Wrapper(void)
{
    EgaMemoryAddress source={0,MapViewportSegment},screen={0,EgaScreenSegment};
    EGA_DrawBox_Operation(source,screen,MapViewportLeftByte,0,
        MapViewportByteWidth,EgaScreenHeight);
#ifdef CHI_SDL_PLATFORM
    /* Sol: hardware replacement only. The original transfer was immediately
     * observable; a buffered SDL window needs an explicit scanout boundary. */
    SDLBackend_CommitEgaViewportTransfer();
#endif
}

/* Sol: Original1F3D:0086..00D4 retained EGA branch00B3. Original file
 * argument is deliberately unused: the caller already uploaded the image
 * toA800. Sizes retain byte-column/pixel-row units, not opposite corners. */
void Draw_GraphicsFile_In_Memory(uint8_t *fileBuffer,uint16_t column,uint16_t row,
    uint16_t width,uint16_t height)
{
    EgaMemoryAddress staging={0,EgaSceneStagingSegment},screen={0,EgaScreenSegment};
    (void)fileBuffer;
    EGA_DrawBox_Operation(staging,screen,column,row,width,height);
}
