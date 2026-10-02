#include "music.h"
#include "../SDL/backend.h"
#include <assert.h>
#include <stdarg.h>

/* Original writable CS state, host runtime pointers instead of DOS addresses.
 * Not a saved-game/file record. Only retained PC/idle callbacks are required. */
uint16_t MusicSystemTickDivider,MusicSystemTickDividerReload;
uint16_t MusicStreamOffset;
uint8_t MusicTicksUntilNextNote,MusicNoteTickReload;
static uint8_t *musicStream;
static void (*musicInterruptCallback)(void);

/* Native0139 and inline00CF: original table, DWORD DIV and unmasked8086 CL
 * shift. High notes can wrap the BYTE shift count; do not use x86-64 masking. */
uint16_t Music_Note_To_Pit_Divisor(uint8_t note)
{
    static const uint16_t semitoneFrequency[12]={
        0x105A,0x1153,0x125B,0x1372,0x149A,0x15D4,
        0x1720,0x1880,0x19F5,0x1B80,0x1D23,0x1EDE};
    uint16_t divisor=(uint16_t)(UINT32_C(1193182)/semitoneFrequency[note%12]);
    uint8_t shifts=(uint8_t)(8-note/12);
    while (shifts--!=0) divisor=(uint16_t)(divisor<<1);
    return divisor;
}

/* Native0179/00C6: gate off and select idle; frequency is NOT reset. */
void PC_Speaker_Set_Value(void)
{
    musicInterruptCallback=NULL; /*native0186 RETF idle*/
    SDLBackend_SpeakerOff();
}

/* Native0298..02E3. A single zero skips to the next BYTE in the SAME tick;
 * two zeros stop without committing the cursor. High-bit notes write divisor
 *14, not silence. Cursor commits before pitch I/O. Valid non-straddling original
 * streams and DF-clear are required; corrupted stream bounds are not repaired. */
void Music_Pc_Stream_Tick(void)
{
    if (--MusicTicksUntilNextNote!=0) return;
    MusicTicksUntilNextNote=MusicNoteTickReload;
    uint16_t cursor=MusicStreamOffset;
    uint8_t note=musicStream[cursor++];
    if (note==0) {
        note=musicStream[cursor++];
        if (note==0) { PC_Speaker_Set_Value(); return; }
    }
    MusicStreamOffset=cursor;
    /* Native00B2 writes channel2 divisor only, preserving the gate. */
    SDLBackend_SetSpeakerDivisor((note&0x80)!=0?14:Music_Note_To_Pit_Divisor(note));
}

/* Native0306 dispatcher. Only selectors0/1 have executable control records
 * used by the game. Other signed values below13 jump into arbitrary native CS
 * data, with no portable C contract. Values>=13 return without hardware writes. */
void PC_Speaker_Music_Control(uint16_t selector,...)
{
    if ((int16_t)selector>=13) return;
    assert(selector==MusicControl_Stop || selector==MusicControl_Start);
    SDLBackend_SetSpeakerGate(1); /*native00BD: no divisor/phase reset*/
    if (selector==MusicControl_Stop) PC_Speaker_Set_Value();
    else {
        /* Inline near0233 consumes the dispatcher's actual variadic arguments.
         * Do not import the annotation-only explicit-parameter setup helper. */
        va_list args; va_start(args,selector);
        musicStream=va_arg(args,uint8_t *);
        MusicStreamOffset=0; /* host pointer absorbs original initial FAR offset */
        MusicNoteTickReload=(uint8_t)va_arg(args,int);
        MusicTicksUntilNextNote=1;
        va_end(args);
        musicInterruptCallback=Music_Pc_Stream_Tick;
    }
}

/* Native0020/003C: callback FIRST; first IRQ chains, thereafter every17
 * interrupts (old low divider zero, reload16). Only AL decrements. PIC/IRET and
 * BIOS vector chaining are replaced by the SDL timer boundary. */
void Music_Clock_Tick(void)
{
    if (musicInterruptCallback) musicInterruptCallback();
    if ((uint8_t)MusicSystemTickDivider==0) {
        MusicSystemTickDivider=MusicSystemTickDividerReload;
        SDLBackend_ChainSystemTimer();
    } else MusicSystemTickDivider=(uint16_t)((MusicSystemTickDivider&0xFF00)|
        (uint8_t)(MusicSystemTickDivider-1));
}
void Install_Music_Timer(void)
{
    MusicSystemTickDividerReload=16;
    MusicSystemTickDivider=0;
    musicInterruptCallback=NULL;
    SDLBackend_InstallMusicTimer(Music_Clock_Tick,0x0FFF);
}
void Restore_System_Timer(void)
{
    SDLBackend_RestoreSystemTimer(0xFFFF);
}
uint16_t Music_Playback_Finished(void)
{
    return (uint16_t)(musicInterruptCallback==NULL);
}
