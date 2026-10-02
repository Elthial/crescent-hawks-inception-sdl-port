#include "backend.h"
#include <SDL3/SDL.h>
#include <stdio.h>
uint8_t SDLBackend_DefaultDrive;
void SDLBackend_SelectDefaultDrive(uint8_t drive)
{
    /* Installed preservation build: both original floppy volumes live in
     * the selected original-asset working directory. Keep logical selection,
     * never change host drives or invent different asset contents. */
    SDLBackend_DefaultDrive=drive;
}
void SDLBackend_DirectConsoleIO(uint8_t character)
{
    /* INT21h/AH06 uses FF as nonblocking input, not a printable character.
     * Startup strings never use it, but retain that service distinction. */
    if(character==UINT8_MAX) {
        if(SDLBackend_InputPending()) (void)SDLBackend_ReadKeyboard();
    } else {
        /* Host console is a C stream; SDL3 does not wrap FILE pointers. */
        (void)fwrite(&character,1,1,stdout);
        (void)fflush(stdout);
    }
}
enum { FileHandleCapacity=64, BinaryReadMode=0x8000, BinaryWriteCreateMode=0x8101 };
static SDL_IOStream *files[FileHandleCapacity];
static SDL_IOStream *File(uint16_t handle)
{
    return handle<FileHandleCapacity ? files[handle] : NULL;
}
int16_t SDLBackend_OpenFile(const uint8_t *path,uint16_t mode)
{
    unsigned handle;
    SDL_IOStream *file;
    if(!path) return -1;
    for(handle=0;handle<FileHandleCapacity && files[handle];++handle) { }
    if(handle==FileHandleCapacity) return -1;
    if(mode==BinaryReadMode) file=SDL_IOFromFile((const char *)path,"rb");
    else if(mode==BinaryWriteCreateMode) {
        /* O_CREAT is NOT O_TRUNC: preserve an existing file's trailing bytes. */
        file=SDL_IOFromFile((const char *)path,"r+b");
        if(!file) file=SDL_IOFromFile((const char *)path,"w+b");
    } else return -1; /* Other native open modes need explicit implementation. */
    if(!file) return -1;
    files[handle]=file; return (int16_t)handle;
}
int16_t SDLBackend_ReadFile(uint16_t handle,void *bytes,uint16_t count)
{
    size_t result;
    SDL_IOStream *file=File(handle);
    if(!file || !bytes) return -1;
    result=SDL_ReadIO(file,bytes,count);
    if(SDL_GetIOStatus(file)==SDL_IO_STATUS_ERROR) return -1;
    return (int16_t)(uint16_t)result; /* native count/error WORD representation */
}
int16_t SDLBackend_WriteFile(uint16_t handle,const void *bytes,uint16_t count)
{
    size_t result;
    SDL_IOStream *file=File(handle);
    if(!file || !bytes) return -1;
    result=SDL_WriteIO(file,bytes,count);
    if(SDL_GetIOStatus(file)==SDL_IO_STATUS_ERROR) return -1;
    return (int16_t)(uint16_t)result;
}
int16_t SDLBackend_CloseFile(uint16_t handle)
{
    SDL_IOStream *file=File(handle);
    if(!file) return -1;
    files[handle]=NULL; return SDL_CloseIO(file) ? 0 : -1;
}
int32_t SDLBackend_SeekFile(uint16_t handle,uint16_t low,uint16_t high,uint16_t origin)
{
    uint32_t bits=(uint32_t)low|((uint32_t)high<<16);
    int64_t offset=bits<=INT32_MAX ? bits : (int64_t)bits-INT64_C(4294967296);
    Sint64 position;
    SDL_IOStream *file=File(handle);
    if(!file || origin>2) return -1;
    position=SDL_SeekIO(file,offset,origin==0 ? SDL_IO_SEEK_SET : origin==1 ? SDL_IO_SEEK_CUR : SDL_IO_SEEK_END);
    return position<0 || position>INT32_MAX ? -1 : (int32_t)position;
}
