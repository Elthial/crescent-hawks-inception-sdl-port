#include "game.h"

int16_t MechHeatGaugeFlashCountdown=MechHeatGaugeFlashBaseDelay; /*3EDB:1218, EXE000A*/
int16_t MechHeatGaugeFlashResetDelay=MechHeatGaugeFlashBaseDelay; /*3EDB:121A, EXE000A*/
uint16_t MechHeatGaugeFlashColour; /*3EDB:121C, EXE0000*/

/* Sol: original EXE-owned tables, 3EDB:1306..1383. These are not
 * BTSTATS.CMP pixels: the external silhouette asset remains required.
 * 0DAB:21AC..2253 loads all three gauge tables as WORDs. A zero structure
 * offset suppresses the internal-structure gauge for rear torso armour. */
uint16_t MechStructureOffsetByArmourLocation[MechArmourLocationCount]={
    28,29,30,31,32,33,34,35,0,0,0
}; /*1306*/
uint16_t MechStatusGaugeX[MechArmourLocationCount]={
    248,208,224,192,192,136,176,160,272,288,304
}; /*131C: pixels, not text columns*/
uint16_t MechStatusGaugeBottomY[MechArmourLocationCount]={
    87,55,151,31,71,87,55,151,135,143,135
}; /*1332: inclusive bottom scanlines*/
uint8_t BTStatsEgaPalette[EgaPaletteRegisterCount]={
    0,0,2,3,4,9,6,7,8,1,10,11,12,13,14,15
}; /*1348: mutable; original updates entry4 AFTER applying the palette*/
uint16_t BTStatsMcgaPalette[EgaPaletteRegisterCount]={
    0x0000,0x0000,0x0460,0x0365,0x0620,0x0357,0x0630,0x0555,
    0x0444,0x0236,0x0573,0x0477,0x0732,0x0747,0x0772,0x0777
}; /*1358: WORD entries even with the retained EGA renderer*/
uint8_t BTStatsEgaRedCycle[BTStatsPaletteCycleCount]={6,4,12,4}; /*1378*/
uint16_t BTStatsMcgaRedCycle[BTStatsPaletteCycleCount]={
    0x0630,0x0620,0x0732,0x0620
}; /*137C: 0DAB:22B1 scales the masked phase by TWO before loading a WORD*/
