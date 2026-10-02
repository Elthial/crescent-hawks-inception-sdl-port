#include "game.h"
#include <stdio.h>
#include <stdlib.h>

/* Instruction-shaped WORD oracle: unsigned shifts plus explicit sign bit,
 * decrement/subtract wrapping and native flag/zero-test order. */
static uint16_t sar(uint16_t value) { return (uint16_t)((value>>1)|(value&0x8000)); }
static void check(int condition) { if(!condition) { fputs("Mech movement mismatch\n",stderr); exit(1); } }
static void run(uint16_t id,uint8_t walk,uint8_t jump,uint8_t left,uint8_t right,int8_t heat,uint16_t mode) {
    uint16_t expected=walk,damage=0,penalty=0;
    if(mode==2) expected=jump;
    else {
        if(!((left|right)&8)) { expected=1; damage=1; }
        else {
            if(!(left&8) || !(right&8)) { expected=(uint16_t)(sar(expected)+(walk&1)); damage=1; }
            for(uint16_t mask=4;mask;mask=sar(mask)) {
                if(!(left&mask)) { --expected; damage=1; }
                if(!(right&mask)) { --expected; damage=1; }
                if(damage && expected==0) expected=1;
            }
        }
        penalty=(uint16_t)(int16_t)(heat/5);
        expected=(uint16_t)(expected-penalty);
        if(mode==1) expected=(uint16_t)(expected+sar(expected)+(walk&1));
    }
    if(heat==30) expected=0;
    if(expected&0x8000) expected=0;
    Mechs[id].walkMove=walk; Mechs[id].jumpMove=jump;
    Mechs[id].currentActuators[0]=left; Mechs[id].currentActuators[1]=right;
    MechHeatLevel[id]=heat;
    CurrentMechHeatMovementPenalty=0x1234; CurrentMechLegDamage=0x5678;
    Combat_Mech_Movement(id,mode);
    check(CharacterMovementPointsRemaining==expected && CurrentMechLegDamage==damage && CurrentMechHeatMovementPenalty==penalty);
}
int main(void) {
    static const uint8_t walking[]={0,1,2,3,5,8,255};
    static const uint16_t modes[]={0,1,2,0xFFFF};
    for(unsigned w=0;w<sizeof walking;++w)
        for(unsigned left=0;left<16;++left) for(unsigned right=0;right<16;++right)
            for(int heat=-128;heat<=127;++heat) for(unsigned m=0;m<4;++m)
                run((uint16_t)((left+right)%MechRecordCount),walking[w],7,
                    (uint8_t)(left|0xA0),(uint8_t)(right|0x50),(int8_t)heat,modes[m]);
    /* Tabletop-facing readable scenarios, including original oddities. */
    run(0,5,3,15,15,0,MovementMode_Run); check(CharacterMovementPointsRemaining==8);
    run(0,5,3,15,15,9,MovementMode_Walk); check(CharacterMovementPointsRemaining==4);
    run(0,5,3,7,15,0,MovementMode_Walk); check(CharacterMovementPointsRemaining==3);
    run(0,5,3,7,7,0,MovementMode_Run); check(CharacterMovementPointsRemaining==2);
    run(0,5,3,0,0,29,MovementMode_Jump); check(CharacterMovementPointsRemaining==3 && !CurrentMechLegDamage);
    run(0,5,3,0,0,30,MovementMode_Jump); check(CharacterMovementPointsRemaining==0);
    run(0,5,3,0,0,31,MovementMode_Jump); check(CharacterMovementPointsRemaining==3);
    puts("Original mech movement budgets passed"); return 0;
}
