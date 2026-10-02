#include "game.h"
#include "backend.h"

/* Sol: complete hardware redirects207F:014C/0213. Native INT21h services
 * consume DL, not the full argument WORD. Their register results are unused
 * by these original void callers. Interpretation stays in the SDL layer. */
void DOS_Select_Default_Drive(uint16_t drive)
{
    SDLBackend_SelectDefaultDrive((uint8_t)drive);
}
void Platform_Write_Startup_Character(int16_t character)
{
    SDLBackend_DirectConsoleIO((uint8_t)character);
}
