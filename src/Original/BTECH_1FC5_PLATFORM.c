#include "game.h"
#include "dos.h"
/* Sol: native1FC5:046E..047A and04E4..04F0 are redirects to the existing
 * original207F hardware entry points. Retain this original call chain. */
void A_Timer(void) { Timer_8253_5(); }
void A_PC_Speaker_OFF(void) { PC_Speaker_OFF(); }
