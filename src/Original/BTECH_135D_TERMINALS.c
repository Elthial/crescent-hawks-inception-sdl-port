#include "game.h"

/* Original EXE-owned arrays, not external level data. Expanded EXE
 * F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE.
 * Native coordinate and colour loads use CBW; all stored values are positive.
 * The 34th separating byte in each native array is not a terminal entry. */
int8_t SecurityTerminalPosX[CacheSecurityCodeCount]={19,23,91,99,103,125,111,19,43,75,89,103,117,99,115,67,71,43,79,13,3,25,41,83,99,105,125,67,21,119,3,99,85};
int8_t SecurityTerminalPosY[CacheSecurityCodeCount]={115,115,123,123,123,117,113,93,97,87,93,93,91,83,75,75,75,3,75,79,67,59,61,57,49,49,31,41,41,3,49,27,27};
int8_t SecurityTerminalColour[CacheSecurityCodeCount]={0,0,1,2,2,2,1,0,1,2,2,1,0,1,0,2,0,2,1,0,2,2,1,1,0,2,1,0,0,0,1,2,1};
uint8_t *SecurityCodeColourText[CacheCodeColourCount]={(uint8_t *)"RED",(uint8_t *)"BLUE",(uint8_t *)"YELLOW"};

/* Original135D:0288..02A7. The ENDMECH script owns discovery writes. */
void StarLeague_Cache_PhoenixHawk(void)
{
    if(!PhoenixHawkFound) Interact_with_BLD(Bld_EndMech);
}

/* Original135D:02A8..02D1. Corrected template's missing second space. */
void Display_StarLeague_Cache_Dialog_Window(void)
{
    Draw_Message_Box();
    Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"These boxes are full of 'Mech gyros!  The cache must be around here!");
    (void)Keyboard_Get_ASCII_Hex_Input();
    MessageBoxOpen=TRUE;
}

/* Original135D:02D2..0326. WORD increment, discard page/parity; repeated
 * qualifying interactions still display the message with power already on. */
void StarLeague_HyperPulse_Power_Dialog_Window(uint16_t worldX,uint16_t worldY)
{
    uint16_t localX=(uint16_t)(worldX+1)&CacheLocalEvenCoordinateMask;
    uint16_t localY=worldY&CacheLocalEvenCoordinateMask;
    if(localX==CachePowerControlX && localY>=CachePowerControlFirstY && localY<=CachePowerControlLastY) {
        Draw_Message_Box();
        Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"You have turned on the power to the Hyperpulse Generator.");
        (void)Keyboard_Get_ASCII_Hex_Input();
        HPGTerminalPowerOn=TRUE; MessageBoxOpen=TRUE;
    }
}

/* Original135D:03AA..04AA. Scan ALL33 terminals (no first-match break).
 * Imprinting replaces the colour's selected code, without consuming it.
 * Used-code bytes are consumed by door handling, not by this method. */
void StarLeague_Security_Terminal(uint16_t worldX,uint16_t worldY)
{
    int16_t localX=(int16_t)((uint16_t)(worldX+1)&PackedPositionLocalMask);
    int16_t localY=(int16_t)(worldY&PackedPositionLocalMask);
    for(uint16_t code=0;code<CacheSecurityCodeCount;++code) {
        int16_t terminalX=SecurityTerminalPosX[code],terminalY=SecurityTerminalPosY[code];
        if(localY==terminalY && localX>=terminalX && localX<terminalX+CacheTerminalWidthCells) {
            Draw_Message_Box();
            if(!CacheSecurityCodeUsed[code]) {
                Display_Text_From_Memory((uint8_t *)"Security Terminal.\rDo you want to imprint ");
                Display_Text_From_Memory(SecurityCodeColourText[SecurityTerminalColour[code]]);
                Display_Text_From_Memory((uint8_t *)" code ");
                Display_Text_Dynamic_Value((uint16_t)(code+1));
                Display_Text_From_Memory((uint8_t *)" on your code key?\x06\x0F");
                if(Prompt_Yes_No(TRUE))
                    SelectedCacheCodeByColour[SecurityTerminalColour[code]]=(uint8_t)code; /* Native reload AFTER prompt. */
            } else {
                Display_Text_From_Memory_ScreenRetrace_KeyboardInput((uint8_t *)"You've already used this terminal's code.");
                (void)Keyboard_Get_ASCII_Hex_Input();
            }
            MessageBoxOpen=TRUE;
        }
    }
}

/* Original135D:04AB..0559. Prerequisite order is power!=0, WHITE==1,
 * parts!=0. WINSCENE script call precedes notification/story flag writes. */
void HPGTransmitter(uint16_t worldX,uint16_t worldY)
{
    uint16_t localX=(uint16_t)(worldX+1)&CacheLocalEvenCoordinateMask;
    uint16_t localY=worldY&CacheLocalEvenCoordinateMask;
    if((localX!=CacheTransmitterFirstX || localY!=CacheTransmitterFirstY) &&
        (localX!=CacheTransmitterSecondX || localY!=CacheTransmitterSecondY)) return;
    uint8_t *message;
    if(!HPGTerminalPowerOn) message=(uint8_t *)"It seems the power to these terminals is turned off.";
    else if(WhiteCacheCodeCorrect!=TRUE) message=(uint8_t *)"Incorrect WHITE code.";
    else if(MechPartsCacheFound) {
        Interact_with_BLD(Bld_WinScene); MessageBoxOpen=TRUE; TransmittedCacheFound=TRUE; return;
    } else message=(uint8_t *)"You want to radio Katrina, but you haven't found the cache.";
    Draw_Message_Box();
    Display_Text_From_Memory_ScreenRetrace_KeyboardInput(message);
    (void)Keyboard_Get_ASCII_Hex_Input(); MessageBoxOpen=TRUE;
}
