#include "game.h"
#include <stdio.h>
#include <stdlib.h>
static uint16_t expected[12],firstActor;
static unsigned calls,expectedCalls,mutate;
static void verify(int ok,unsigned line) {
    if (!ok) { fprintf(stderr,"Computer side dispatch mismatch line%u\n",line); exit(1); }
}
#define check(x) verify(!!(x),__LINE__)
/* Only the single-actor planning boundary is controlled here. The actual
 * complete side dispatcher runs; existing AI tests cover the real callee. */
void Combat_Computer_Control(uint16_t actor,uint16_t preview) {
    check(calls<expectedCalls && actor==expected[calls] && preview==FALSE);
    ++calls;
    if (mutate && actor==firstActor) {
        CombatantActive[firstActor+1]=0;
        CombatantActive[firstActor+2]=0x8000;
    }
}
int main(void) {
    for (unsigned side=0;side<2;++side) for (unsigned mask=0;mask<4096;++mask) {
        firstActor=(uint16_t)(side*12); calls=expectedCalls=mutate=0;
        for (unsigned actor=0;actor<24;++actor) CombatantActive[actor]=0xFFFF;
        for (unsigned slot=0;slot<12;++slot) {
            unsigned actor=firstActor+slot;
            CombatantActive[actor]=(mask&(1u<<slot))?(uint16_t)(slot&1?0x8000:0x0100):0;
            if (CombatantActive[actor]) expected[expectedCalls++]=(uint16_t)actor;
        }
        Combat_Plan_Computer_Side(firstActor);
        check(calls==expectedCalls);
        for (unsigned slot=0;slot<12;++slot)
            check(CombatantActive[(side?0:12)+slot]==0xFFFF);
    }
    for (unsigned side=0;side<2;++side) {
        firstActor=(uint16_t)(side*12); calls=0; expectedCalls=2; mutate=1;
        for (unsigned actor=0;actor<24;++actor) CombatantActive[actor]=0;
        CombatantActive[firstActor]=1; CombatantActive[firstActor+1]=1;
        expected[0]=firstActor; expected[1]=(uint16_t)(firstActor+2);
        Combat_Plan_Computer_Side(firstActor);
        check(calls==2 && CombatantActive[firstActor+1]==0 && CombatantActive[firstActor+2]==0x8000);
    }
    puts("Original side dispatcher: all8192 active masks, WORD flags and live mutations passed.");
    return 0;
}
