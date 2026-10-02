#include "game.h"

/* Original135D:0327..03A9. Called on overhead-view entry and exit.
 * Swap250 BYTEs, not the256-byte XLAT domain. Room flag is read anew
 * for every iteration, as in the native body. This does not render. */
void OverHead_Map_Function(void)
{
    for(uint16_t tile=0;tile<CacheOverviewColourSwapCount;++tile) {
        uint8_t colour=OverviewTileColours[tile];
        if(CacheMapRoomLoaded!=FALSE) {
            OverviewTileColours[tile]=MapRoomOverviewTileColours[tile];
            MapRoomOverviewTileColours[tile]=colour;
        } else {
            OverviewTileColours[tile]=CacheOverviewTileColours[tile];
            CacheOverviewTileColours[tile]=colour;
        }
    }
}
