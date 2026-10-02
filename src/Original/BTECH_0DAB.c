#include "dos.h"
#include "backend.h"
/* Original0DAB:0C8F..0D11 is hardware/runtime setup, not gameplay. Sol:
 * DOS file-buffer registration, adapter selection/BIOS mode, spin calibration,
 * graphics-runtime configuration and interrupt24 installation are replaced by
 * the SDL boundary. Asset workspaces already have host storage. Do not measure
 * host instruction speed to manufacture original CPU calibration values. */
void Initialize_Graphics_Runtime(void)
{
    SDLBackend_InitializeGraphicsRuntime();
}
/* Sol: original0DAB:0D12 shutdown routine; BIOS operation is redirected. */
void Restore_BIOS_Text_Mode(void)
{
    Set_BIOS_Video_Mode(2);
}
/* Original BIOS service contract, not a new gameplay method. */
void Set_BIOS_Video_Mode(uint16_t mode)
{
    SDLBackend_SetVideoMode(mode);
}
