#include "game.h"

/* Sol: Original1543:0C72..0CDD, complete ASM checked. FAR246C:1C1D
 * becomes the corresponding view inside the shared original tile payload.
 * Repeated apply saves the already patched bytes: preserve that native behavior. */
void Starport_MapPatch_SaveApply_Or_Restore(uint16_t restore)
{
    uint8_t *region=MapFileTiles+StarportPatchTileOffset;
    if (restore!=FALSE) {
        for (uint16_t index=0;index<StarportPatchBytes;++index)
            region[index]=StarportSavedMapBytes[index];
    } else {
        for (uint16_t index=0;index<StarportPatchBytes;++index) {
            StarportSavedMapBytes[index]=region[index];
            if (StarportMapPatchOverrides[index]!=0)
                region[index]=StarportMapPatchOverrides[index];
        }
    }
}
