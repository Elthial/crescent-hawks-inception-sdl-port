#include "dos.h"
/* Sol: 1F3D:0006; original WORD loop retained, only underlying port polling
 * redirects to SDL. Zero performs no waits. No invented timing/game helper. */
uint16_t VideoStatusInactiveLevel; /* original3092:32AC WORD */
void Wait_For_N_Vertical_Retraces(uint16_t numberOfRetraces)
{
    while(numberOfRetraces--!=0)
        Wait_For_Retrace((uint8_t)VideoStatusInactiveLevel); /*0B40 consumes BL*/
}
