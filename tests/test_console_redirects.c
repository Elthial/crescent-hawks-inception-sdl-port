#include "game.h"
#include "backend.h"
#include <stdio.h>
#include <stdlib.h>
static uint8_t driveSeen,characterSeen;
static unsigned drives,characters;
void SDLBackend_SelectDefaultDrive(uint8_t drive){driveSeen=drive;++drives;}
void SDLBackend_DirectConsoleIO(uint8_t character){characterSeen=character;++characters;}
int main(void){
    for(unsigned value=0;value<65536;++value){
        DOS_Select_Default_Drive((uint16_t)value);
        Platform_Write_Startup_Character((int16_t)value);
        if(driveSeen!=(uint8_t)value || characterSeen!=(uint8_t)value || drives!=value+1 || characters!=value+1){
            fprintf(stderr,"DOS DL redirect mismatch%u\n",value);return EXIT_FAILURE;
        }
    }
    puts("Original DOS redirects preserve every argument's low byte.");return EXIT_SUCCESS;
}
