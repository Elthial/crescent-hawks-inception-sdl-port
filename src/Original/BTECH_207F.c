/* Sol: original hardware methods; MS-DOS/port access replaced, not gameplay. */
#include "dos.h"
#include "game.h"
#include "backend.h"
/* Sol: Original207F:1FBE..200D retained EGA hardware fill. */
void Graphics_Set_Screen_To_Black(void) { SDLBackend_ClearEgaScreen(); }
/* Sol: Original207F:0377..0571 hardware renderer. All nine native callers
 * load DX=AC00 immediately before entry, matching destination segment. */
void DrawCall_EGA_CharacterPos(struct EgaMemoryAddress destination,uint8_t *sprite,int16_t x,int16_t y)
{
    SDLBackend_DrawEgaSprite(destination.segment,destination.offset,sprite,x,y);
}

/* Sol: Original207F:0260..0312 is entirely EGA controller/aperture IO.
 * Preserve its entry and redirect the hardware body to the separate backend. */
void DrawCall_Image_To_VGA_Memory(uint8_t *image,uint16_t destinationSegment)
{
    SDLBackend_TransferPackedImage(image,destinationSegment);
}
/* Sol: Original207F:0313..0376, raw ASM checked. Native destination FAR
 * offset/segment are represented by their resolved host RAM pointer. */
void DrawCall_Read_EGAMemory(uint8_t *destination,uint16_t byteColumn,uint16_t tileRow)
{
    SDLBackend_CaptureEgaTile(destination,byteColumn,tileRow);
}
/* Sol: Original207F:022A..025F, attribute-controller port IO only.
 * BL/CL narrowing is done at the original hardware boundary. */
void EGA_Set_Palette_Registers_in_out(uint16_t paletteRegister,uint16_t colour)
{
    SDLBackend_SetEgaPaletteRegister((uint8_t)paletteRegister,(uint8_t)colour);
}
/* 207F:3BDC DOS/runtime availability replaced by SDL's non-consuming queue. */
uint16_t Check_Input_For_Character(void) { return SDLBackend_InputPending(); }
/* 207F:001C */
void Timer_8253_5(void) { SDLBackend_ConfigureSpeaker(); }
/* 207F:0030: argument is PIT divisor, NOT frequency in Hz. */
void PC_Speaker_ON_ptr_freq(uint16_t divisor) { SDLBackend_SpeakerOn(divisor); }
/* 207F:0051 */
void PC_Speaker_OFF(void) { SDLBackend_SpeakerOff(); }
/* 207F:0067 */
void PC_speaker_OFF_2(void) { SDLBackend_SpeakerOff(); }
/*207F:007D returns native DX;00A9 accepts it explicitly without mutation. */
uint16_t PC_Speaker_XOR_ptr_freq(uint16_t seed) { return SDLBackend_ToggleSpeaker(seed); }
void Sound_Freq_Countdown_Loop(uint16_t delayState,uint16_t mask,uint16_t minimumBits)
{
    SDLBackend_SpeakerCountdown(delayState,mask,minimumBits);
}
/* 207F:0B40: original rising/falling-phase argument retained. */
void Wait_For_Retrace(uint8_t phase) { SDLBackend_WaitRetrace(phase); }
/* 207F:0B8A: blocking keyboard result, signed BYTE extended to WORD. */
int16_t Keyboard_GetKey(void) { return SDLBackend_ReadKeyboard(); }
