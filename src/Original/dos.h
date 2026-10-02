#ifndef CHI_DOS_H
#define CHI_DOS_H
#include <stdint.h>
enum { DOSFileMode_ReadBinary=0x8000, DOSSeek_Start=0, DOSSeek_End=2 };
enum { DOSFileMode_WriteCreateBinary=0x8101,DOSFileCreateOwnerReadWrite=0x0180 };
int16_t Get_FileHandle(const uint8_t *filename,uint16_t mode,...);
int16_t DOS_read_file_handler(uint16_t handle,void *buffer,uint16_t count);
int16_t DOS_write_memory_to_save_file(uint16_t handle,const void *buffer,uint16_t count);
int16_t DOS_close_file(uint16_t handle);
int32_t DOS_set_file_position(uint16_t handle,uint16_t low,uint16_t high,uint16_t origin);
void Timer_8253_5(void);
void PC_Speaker_ON_ptr_freq(uint16_t divisor);
void PC_Speaker_OFF(void);
void PC_speaker_OFF_2(void);
void Wait_For_Retrace(uint8_t phase);
void Wait_For_N_Vertical_Retraces(uint16_t count);
int16_t Keyboard_GetKey(void);
uint16_t Check_Input_For_Character(void);
void Set_BIOS_Video_Mode(uint16_t mode);
void Restore_BIOS_Text_Mode(void);
extern uint16_t VideoStatusInactiveLevel;
#endif
