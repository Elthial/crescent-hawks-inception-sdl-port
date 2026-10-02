/* Sol: original OS/library import names retained. DOS bodies are replaced
 * by redirects only; file interpretation remains in the original callers. */
#include "dos.h"
#include "backend.h"
int16_t Get_FileHandle(const uint8_t *filename,uint16_t mode,...)
{
    return SDLBackend_OpenFile(filename,mode);
}
int16_t DOS_read_file_handler(uint16_t handle,void *buffer,uint16_t count)
{
    return SDLBackend_ReadFile(handle,buffer,count);
}
int16_t DOS_write_memory_to_save_file(uint16_t handle,const void *buffer,uint16_t count)
{
    return SDLBackend_WriteFile(handle,buffer,count);
}
int16_t DOS_close_file(uint16_t handle)
{
    return SDLBackend_CloseFile(handle);
}
int32_t DOS_set_file_position(uint16_t handle,uint16_t low,uint16_t high,uint16_t origin)
{
    return SDLBackend_SeekFile(handle,low,high,origin);
}
