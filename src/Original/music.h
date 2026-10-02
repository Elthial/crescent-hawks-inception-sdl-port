#ifndef CHI_MUSIC_H
#define CHI_MUSIC_H
#include <stdint.h>
enum { MusicControl_Stop=0, MusicControl_Start=1,
    MusicPcCadenceTicks=4, MusicTerminatorBytes=4 };
/* Native204B methods. Selector1 has stream/cadence extra arguments, selector0
 * has none. Converted dispatcher must bind them, unlike damaged annotations. */
void PC_Speaker_Music_Control(uint16_t selector,...);
uint16_t Music_Playback_Finished(void);
void Install_Music_Timer(void);
void Restore_System_Timer(void);
uint16_t Music_Note_To_Pit_Divisor(uint8_t note);
void Music_Pc_Stream_Tick(void);
void Music_Clock_Tick(void);
void PC_Speaker_Set_Value(void);
extern uint16_t MusicSystemTickDivider, MusicSystemTickDividerReload;
extern uint16_t MusicStreamOffset;
extern uint8_t MusicTicksUntilNextNote, MusicNoteTickReload;
#endif
