#include "game.h"
#include "dos.h"
#include <stdio.h>
#include <stdlib.h>

/* 1F3D:063B..06C2. Sol: Original length-prefixed loader. Native SS:BP-2
 * becomes a local two-BYTE prefix, explicitly recombined little-endian.
 * Read/close statuses are ignored. A truncated prefix leaves native stack
 * bytes indeterminate; that malformed case has no portable host contract. */
uint16_t Load_File_To_Memory(const uint8_t *filename, uint8_t *memory)
{
    uint16_t retryOpen;
    do
    {
        retryOpen = FALSE;
        int16_t fileHandle = Get_FileHandle(filename,UINT16_C(0x8000));
        if (fileHandle != -1)
        {
            uint8_t payloadLengthBytes[2];
            int16_t prefixBytesRead = DOS_read_file_handler((uint16_t)fileHandle,payloadLengthBytes,2);
            /* Sol: unsupported-path guard, not an original error check. At
             * 0665 the native read targets uninitialised SS:BP-2; a failed or
             * partial read leaves residual stack bytes which we cannot guess.
             * Stop before host C reads those indeterminate bytes. Do not retry,
             * zero-fill, or claim that native payload/close statuses matter. */
            if (prefixBytesRead != 2)
            {
                fputs("Unsupported native asset length-prefix stack state (1F3D:0665 BP-2)\n",stderr);
                abort();
            }
            uint16_t payloadByteCount = (uint16_t)(payloadLengthBytes[0] |
                ((uint16_t)payloadLengthBytes[1] << NativeByteBits));
            DOS_read_file_handler((uint16_t)fileHandle,memory,payloadByteCount);
            DOS_close_file((uint16_t)fileHandle);
        }
        else
            retryOpen = TRUE;
        if (retryOpen != FALSE)
            Request_Game_Disk(RequestedGameDiskNumber);
    } while (retryOpen != FALSE);
    return TRUE; /* Original AX1 means open path completed, not verified payload integrity. */
}

/* 1F3D:0814..0869. Sol: Raw fixed-count asset loader (no length prefix).
 * Only FFFF open failure prompts/retries. Read/close status is ignored.
 * Buffer capacity is the original caller's responsibility. */
void DOS_Load_File_to_memory(const uint8_t *filename, uint8_t *buffer, uint16_t bytesToRead)
{
    int16_t fileHandle = Get_FileHandle(filename,UINT16_C(0x8000)); /* runtime binary/read-only flag */
    while (fileHandle == -1)
    {
        Request_Game_Disk(RequestedGameDiskNumber);
        fileHandle = Get_FileHandle(filename,UINT16_C(0x8000));
    }
    DOS_read_file_handler((uint16_t)fileHandle,buffer,bytesToRead);
    DOS_close_file((uint16_t)fileHandle);
}
