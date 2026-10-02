#include "game.h"

/* Sol: original 0DAB:174C..1857, retained EGA path. Native signed JLE1
 * tests and IDIV6 are intentional even for WORDs with the high bit set.
 * BUG-006 is retained: after a red segment, a one-unit green segment reuses
 * the red top and passes reversed Y bounds to the rectangle primitive. */
void Draw_Vertical_Mech_Status_Gauge(uint16_t x,uint16_t bottomY,
    uint16_t greenHeight,uint16_t redHeight)
{
    uint16_t topY=bottomY;
    if(redHeight!=0) {
        uint16_t colour=EGA_Red;
        if((int16_t)redHeight>1) topY=(uint16_t)(bottomY-redHeight+1);
        if(bottomY==MechHeatGaugeBottomY) {
            MechHeatGaugeFlashResetDelay=(int16_t)(MechHeatGaugeFlashBaseDelay-
                (int16_t)redHeight/MechHeatGaugeHeatPerDelayStep);
            if(MechHeatGaugeFlashColour==EGA_Black) MechHeatGaugeFlashColour=EGA_Red;
            MechHeatGaugeFlashCountdown=(int16_t)(uint16_t)(
                (uint16_t)MechHeatGaugeFlashCountdown-1);
            if(MechHeatGaugeFlashCountdown<0) {
                MechHeatGaugeFlashCountdown=MechHeatGaugeFlashResetDelay;
                /* Native XOR BYTE121C: high byte remains unchanged. */
                MechHeatGaugeFlashColour^=MechHeatGaugeEgaColourToggle;
            }
            colour=MechHeatGaugeFlashColour;
        }
        Draw_Horizontal_EGA_Line(x,topY,(uint16_t)(x+MechStatusGaugeWidth-1),bottomY,colour);
        bottomY=(uint16_t)(topY-1);
    }
    if(greenHeight!=0) {
        if((int16_t)greenHeight>1) topY=(uint16_t)(bottomY-greenHeight+1);
        Draw_Horizontal_EGA_Line(x,topY,(uint16_t)(x+MechStatusGaugeWidth-1),bottomY,EGA_Green);
    }
}
