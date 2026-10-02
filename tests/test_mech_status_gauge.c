#include "game.h"
#include <stdio.h>
#include <stdlib.h>

struct Rectangle { uint16_t x0,y0,x1,y1,colour; };
static struct Rectangle rectangles[2];
static unsigned calls,scenarios;
static void check(int condition)
{
    if(!condition) { fputs("Native Mech status gauge mismatch\n",stderr); exit(1); }
}
void Draw_Horizontal_EGA_Line(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t colour)
{
    struct Rectangle rectangle={x0,y0,x1,y1,colour};
    check(calls<2); rectangles[calls++]=rectangle;
}
static int signedWord(unsigned value)
{
    return value>=32768?(int)value-65536:(int)value;
}
static void expect(unsigned index,uint16_t x,uint16_t top,uint16_t bottom,uint16_t colour)
{
    check(index<calls);
    check(rectangles[index].x0==x && rectangles[index].x1==(uint16_t)(x+5));
    check(rectangles[index].y0==top && rectangles[index].y1==bottom);
    check(rectangles[index].colour==colour);
}
static void run(uint16_t x,uint16_t bottom,uint16_t green,uint16_t red,
    uint16_t initialCountdown,uint16_t initialColour)
{
    uint16_t top=bottom,expectedColour=initialColour;
    int expectedCountdown=signedWord(initialCountdown),expectedDelay=10;
    unsigned expectedCalls=0;
    MechHeatGaugeFlashCountdown=(int16_t)initialCountdown;
    MechHeatGaugeFlashResetDelay=10;
    MechHeatGaugeFlashColour=initialColour;
    calls=0;
    Draw_Vertical_Mech_Status_Gauge(x,bottom,green,red);
    if(red!=0) {
        uint16_t drawnColour=4;
        if(signedWord(red)>1) top=(uint16_t)((unsigned)bottom-red+1);
        if(bottom==183) {
            expectedDelay=10-signedWord(red)/6;
            if(expectedColour==0) expectedColour=4;
            expectedCountdown=signedWord(((unsigned)initialCountdown+65535)&65535);
            if(expectedCountdown<0) {
                expectedCountdown=expectedDelay;
                expectedColour=(uint16_t)(expectedColour^10);
            }
            drawnColour=expectedColour;
        }
        expect(expectedCalls++,x,top,bottom,drawnColour);
        bottom=(uint16_t)(top-1);
    }
    if(green!=0) {
        if(signedWord(green)>1) top=(uint16_t)((unsigned)bottom-green+1);
        expect(expectedCalls++,x,top,bottom,2);
    }
    check(calls==expectedCalls);
    check(MechHeatGaugeFlashCountdown==expectedCountdown);
    check(MechHeatGaugeFlashResetDelay==expectedDelay);
    check(MechHeatGaugeFlashColour==expectedColour);
    ++scenarios;
}
int main(void)
{
    unsigned word,green,red;
    check(MechHeatGaugeFlashCountdown==10 && MechHeatGaugeFlashResetDelay==10);
    check(MechHeatGaugeFlashColour==0);
    for(green=0;green<=35;++green) for(red=0;red<=35;++red) {
        run(248,87,(uint16_t)green,(uint16_t)red,10,0);
        run(256,183,(uint16_t)green,(uint16_t)red,0,0);
    }
    /* All native WORD classes: signed JLE1, signed heat division, DEC/JNS,
     * high-byte-preserving colour toggle and wrapped coordinates. */
    for(word=0;word<=65535;++word) {
        run(65534,87,1,(uint16_t)word,10,0);
        run(248,87,(uint16_t)word,7,10,0);
        run(256,183,0,(uint16_t)word,0,0xA504);
        run(256,183,0,30,(uint16_t)word,0xA504);
    }
    run(248,87,1,7,10,0);
    check(calls==2 && rectangles[1].y0==81 && rectangles[1].y1==80); /*BUG-006*/
    /* Real helper call sequence retains global flashing state between frames. */
    MechHeatGaugeFlashCountdown=10; MechHeatGaugeFlashColour=0;
    for(word=0;word<11;++word) { calls=0; Draw_Vertical_Mech_Status_Gauge(256,183,0,30); }
    check(MechHeatGaugeFlashCountdown==5 && MechHeatGaugeFlashColour==14);
    calls=0; Draw_Vertical_Mech_Status_Gauge(256,183,0,30);
    check(MechHeatGaugeFlashCountdown==4 && MechHeatGaugeFlashColour==14);
    printf("Native status gauge: %u scenarios, BUG-006 and shared flash sequence passed.\n",scenarios);
    return 0;
}
