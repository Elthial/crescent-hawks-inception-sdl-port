#include "game.h"
#include "backend.h"
#include <assert.h>
#include <stdio.h>
static uint8_t blank[32000];
static const uint16_t *keys;
static unsigned keyIndex,keyCount,drains;
static int mutate;
uint16_t Keyboard_Get_ASCII_Hex_Input(void)
{
    assert(keyIndex<keyCount);
    if (mutate && keyIndex==0) {
        MenuControls[4].highlightWidth=3; MenuControls[4].highlightColour=6;
        EquipmentDistributionMenuOptionCount=2;
    }
    return keys[keyIndex++];
}
void Drain_Pending_Keyboard_Input(void)
{
    assert(SDLBackend_EgaWriteMode==2 && FramebufferBoxWidth==5);
    ++drains;
}
static void setup(const uint16_t *input,unsigned count)
{
    keys=input; keyIndex=0; keyCount=count; mutate=0;
    SDLBackend_EgaRasterOperation=EgaRaster_Replace; SDLBackend_EgaMapMask=15;
    SDLBackend_TransferPackedImage(blank,EgaScreenSegment);
    TextPanelLeft=2; TextPanelTop=3; DisableInput=0; AttractModeRecordingActive=0;
    MenuControls[4].baseRow=1; MenuControls[4].highlightWidth=5;
    MenuControls[4].optionCount=3; MenuControls[4].selection=1;
    MenuControls[4].highlightColour=9; MenuRendererFlag=7;
}
static void strip(uint16_t row,uint16_t column,uint8_t colour)
{
    for (uint16_t y=0;y<8;++y)
        for (uint8_t plane=0;plane<4;++plane)
            assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,
                (uint16_t)(row*320+y*40+column),plane)==((colour&(1<<plane))?255:0));
}
int main(void)
{
    /* Actual converter: letters/keypad navigate; positive00B0 is ignored.
     * Three south commands wrap0, then north wraps to the last option. */
    const uint16_t input[]={'w',0x00B0,27,'x','2',MenuNextCommand,'W','\r'};
    setup(input,8);
    assert(Display_Menu_Choices_And_Check(4)==2 && MenuControls[4].selection==2);
    assert(keyIndex==8 && drains==1 && MenuRendererFlag==0);
    for (uint16_t row=4;row<=6;++row)
        for (uint16_t column=0;column<40;++column)
            strip(row,column,(uint8_t)(row==6 && column>=2 && column<7?9:0));
    assert(SDLBackend_EgaRasterOperation==EgaRaster_Replace && SDLBackend_EgaRotateCount==0);
    const uint16_t confirm[]={' '};
    setup(confirm,1); DisableInput=1;
    assert(Display_Menu_Choices_And_Check(4)==0);
    setup(confirm,1); AttractModeRecordingActive=1;
    assert(Display_Menu_Choices_And_Check(4)==0);
    setup(confirm,1); MenuControls[4].selection=9;
    assert(Display_Menu_Choices_And_Check(4)==0);
    const uint16_t changing[]={MenuNextCommand,' '};
    setup(changing,2); mutate=1;
    assert(Display_Menu_Choices_And_Check(4)==0 && EquipmentDistributionMenuOptionCount==2);
    for (uint16_t column=2;column<7;++column) {
        strip(5,column,(uint8_t)(column<5?15:9));
        strip(4,column,(uint8_t)(column<5?6:0));
    }
    /* Zero width's eight complete-aperture XOR loops cancel their pixels. */
    setup(confirm,1); SDLBackend_EgaMapMask=13; SDLBackend_EgaRotateCount=7;
    assert(DrawCall_Combat_Menu(2,4,0,0xAA09)==3);
    for (uint32_t address=0;address<65536;++address)
        for (uint8_t plane=0;plane<4;++plane)
            assert(SDLBackend_ReadEgaPlaneByte(EgaScreenSegment,(uint16_t)address,plane)==0);
    assert(SDLBackend_EgaMapMask==13 && SDLBackend_EgaRotateCount==0 &&
        SDLBackend_EgaRasterOperation==0 && SDLBackend_EgaBitMask==255);
    puts("Original menu selection, sign-extended navigation, live fields and XOR highlight passed");
    return 0;
}
