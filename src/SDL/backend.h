#ifndef CHI_SDL_BACKEND_H
#define CHI_SDL_BACKEND_H
#include <stddef.h>
#include <stdint.h>
uint8_t *SDLBackend_AllocateWordBuffer(uint16_t byteCount);
/* Hardware boundary only. No SDL types in game code. */
enum { ScreenWidth=320, ScreenHeight=200, AudioSampleRate=48000,
    AudioQueueCapacity=AudioSampleRate*8 };
int SDLBackend_Open(void);
void SDLBackend_Close(void);
enum { SDLBackend_ApplicationReturned=0, SDLBackend_ApplicationClosed=1 };
/* Main-thread, non-reentrant platform lifetime boundary. OS window close is
 * not a gameplay ESC key; original blocking DOS menus need not be rewritten. */
int SDLBackend_RunApplication(void (*entry)(void));
int SDLBackend_PollQuit(void);
int SDLBackend_Present(const uint32_t *rgba);
void SDLBackend_DecodeEgaScreen(uint32_t *rgba);
int SDLBackend_PresentEgaScreen(void);
/* A native A800->A000 viewport transfer became visible as the EGA scanned it.
 * Buffered hosts must expose and pace that completed transfer explicitly. */
void SDLBackend_CommitEgaViewportTransfer(void);
uint64_t SDLBackend_Milliseconds(void);
void SDLBackend_Delay(uint32_t milliseconds);
/* Main-thread producer; retained PCM plays before live speaker state. Atomic
 * enqueue: returns false if full, never truncates or overwrites pending audio. */
int SDLBackend_QueueAudio(const int16_t *samples,size_t count);
/* Capture current PIT/gate state over an explicit host duration. Fractional
 * samples carry between spans; overflow rejects the entire span unchanged. */
int SDLBackend_QueueSpeakerSpan(uint64_t nanoseconds);
void SDLBackend_BeginSoundEffect(void);
void SDLBackend_EndSoundEffect(void);
int SDLBackend_SoundEffectActive(void); /* Main-thread platform status, not game state. */
void SDLBackend_ToneDelayIteration(int sweep);
/* Original effects are CPU busy loops. DOSBox-X cycles/ms profiles provide
 * the preservation clock; 240 is the IBM-PC-compatible default. */
enum { SDLBackend_SoundCpuIbmPc=240,SDLBackend_SoundCpu286=750,
    SDLBackend_SoundCpu1510=1510,SDLBackend_SoundCpuDosBoxNostalgia=3000 };
extern uint32_t SDLBackend_SoundCpuCyclesPerMillisecond;
void SDLBackend_SetSoundCpuCyclesPerMillisecond(uint32_t cyclesPerMillisecond);
extern uint32_t SDLBackend_FixedToneIterationNs,SDLBackend_SweepIterationNs,SDLBackend_NoiseIterationNs;
extern uint64_t SDLBackend_TimedEffectSamples,SDLBackend_TimedEffectNonzeroSamples;
int16_t SDLBackend_OpenFile(const uint8_t *path,uint16_t mode);
void SDLBackend_SelectDefaultDrive(uint8_t drive);
extern uint8_t SDLBackend_DefaultDrive;
void SDLBackend_DirectConsoleIO(uint8_t character);
int16_t SDLBackend_ReadFile(uint16_t handle,void *bytes,uint16_t count);
int16_t SDLBackend_WriteFile(uint16_t handle,const void *bytes,uint16_t count);
int16_t SDLBackend_CloseFile(uint16_t handle);
int32_t SDLBackend_SeekFile(uint16_t handle,uint16_t low,uint16_t high,uint16_t origin);
int16_t SDLBackend_ReadKeyboard(void);
uint16_t SDLBackend_InputPending(void);
void SDLBackend_WaitRetrace(uint8_t phase);
void SDLBackend_SetVideoMode(uint16_t mode);
void SDLBackend_InitializeGraphicsRuntime(void);
void SDLBackend_ConfigureSpeaker(void);
void SDLBackend_SpeakerOn(uint16_t divisor);
void SDLBackend_SpeakerOff(void);
void SDLBackend_SetSpeakerDivisor(uint16_t divisor);
void SDLBackend_SetSpeakerGate(int enabled);
extern uint8_t SDLBackend_SpeakerControl;
extern uint16_t SDLBackend_SpeakerDelayState,SDLBackend_SpeakerDelayMask,SDLBackend_SpeakerDelayMinimum;
uint16_t SDLBackend_ToggleSpeaker(uint16_t seed);
void SDLBackend_SpeakerCountdown(uint16_t delayState,uint16_t mask,uint16_t minimumBits);
void SDLBackend_InstallMusicTimer(void (*callback)(void),uint16_t divisor);
void SDLBackend_RestoreSystemTimer(uint16_t divisor);
void SDLBackend_ChainSystemTimer(void);
void SDLBackend_ServiceMusicTimer(void);
extern uint64_t SDLBackend_SystemTimerTicks;
void SDLBackend_RenderSpeaker(int16_t *samples,size_t count);
extern uint32_t SDLBackend_RetracesPerSecond;
/* EGA aperture backing belongs to the hardware replacement, not game state.
 * Native A000/A400/A800 segment views share the same four64KiB planes. */
enum { EgaPlaneCount=4, EgaPlaneBytes=65536, EgaScreenPlaneBytes=8000,
    EgaApertureSegment=0xA000,EgaTilesetSegment=0xA400, EgaPixelsPerByte=8, PackedPixelsPerByte=2,
    EgaAllPlanesMask=15, EgaRaster_Replace=0, EgaRaster_And=1,
    EgaRaster_Or=2, EgaRaster_Xor=3 };
void SDLBackend_TransferPackedImage(const uint8_t *image,uint16_t destinationSegment);
void SDLBackend_CopyEgaLatchByte(uint16_t sourceSegment,uint16_t sourceOffset,uint16_t destinationSegment,uint16_t destinationOffset);
void SDLBackend_CaptureEgaTile(uint8_t *destination,uint16_t byteColumn,uint16_t tileRow);
uint8_t SDLBackend_ReadEgaPlaneByte(uint16_t segment,uint16_t offset,uint8_t plane);
extern uint8_t SDLBackend_EgaWriteMode,SDLBackend_EgaBitMask;
extern uint8_t SDLBackend_EgaMapMask,SDLBackend_EgaRasterOperation;
extern uint8_t SDLBackend_EgaReadMapSelect;
void SDLBackend_SetEgaPaletteRegister(uint8_t index,uint8_t colour);
uint8_t SDLBackend_GetEgaPaletteRegister(uint8_t index);
void SDLBackend_DrawEgaGlyph(const uint8_t *bitmap,uint16_t rowByteOffset,uint16_t column,uint8_t foreground,uint8_t background);
void SDLBackend_DrawEgaVerticalRun(uint16_t column,uint16_t top,uint16_t bottom,uint8_t colour);
void SDLBackend_DrawEgaAlignedSpan(uint16_t column,uint16_t row,uint16_t groupCount,uint8_t colour);
void SDLBackend_DrawEgaTile(const uint8_t *tile,uint16_t column,uint16_t row);
void SDLBackend_UploadAnimatedEgaTile(const uint8_t *source,uint16_t destinationOffset);
void SDLBackend_DrawAnmFrame(const uint8_t *planes);
void SDLBackend_ClearEgaScreen(void);
void SDLBackend_DrawEgaSprite(uint16_t segment,uint16_t offset,const uint8_t *sprite,int16_t x,int16_t y);
extern uint16_t SDLBackend_LastSpriteCleanupPort;
extern uint8_t SDLBackend_EgaSetReset;
extern uint8_t SDLBackend_EgaRotateCount,SDLBackend_EgaEnableSetReset;
void SDLBackend_ToggleEgaHighlight(uint16_t column,uint16_t row,uint16_t width,uint8_t colour);
#endif
