#include "game.h"

uint16_t CurrentMechHeatMovementPenalty,CurrentMechLegDamage; /*3092:4592/377C*/

/* Original183B:22BC..2473, complete ASM checked. Mech RECORD ID, not
 * combatant ID: enemy callers normalize12..15 to records4..7.
 * Walking base is unsigned BYTE; heat is signed BYTE. Native SAR rounds
 * negative values downward, unlike C division. No maximum-MP clamp exists. */
void Combat_Mech_Movement(uint16_t mechId,uint16_t selectedMovementMode)
{
    CurrentMechHeatMovementPenalty=0; CurrentMechLegDamage=0;
    int16_t points=Mechs[mechId].walkMove;
    if(selectedMovementMode==MovementMode_Jump) points=Mechs[mechId].jumpMove;
    else {
        uint8_t left=Mechs[mechId].currentActuators[MechActuatorSide_Left];
        uint8_t right=Mechs[mechId].currentActuators[MechActuatorSide_Right];
        if(!((left|right)&MechLegPrimaryActuatorMask)) {
            points=1; CurrentMechLegDamage=1;
        } else {
            if(!(left&MechLegPrimaryActuatorMask) || !(right&MechLegPrimaryActuatorMask)) {
                points=(int16_t)(points/2+(Mechs[mechId].walkMove&1));
                CurrentMechLegDamage=1;
            }
            for(uint16_t mask=MechLegSecondaryActuatorFirstMask;mask;mask/=2) {
                if(!(left&mask)) { --points; CurrentMechLegDamage=1; }
                if(!(right&mask)) { --points; CurrentMechLegDamage=1; }
                /* Native repair of EXACT zero after BOTH side decrements,
                 * not a generic minimum1 rule. Negative values survive. */
                if(CurrentMechLegDamage && points==0) points=1;
            }
        }
        int16_t heatPenalty=(int16_t)(MechHeatLevel[mechId]/MechHeatPerMovementPointLost);
        CurrentMechHeatMovementPenalty=(uint16_t)heatPenalty;
        points=(int16_t)(points-heatPenalty);
        if(selectedMovementMode==MovementMode_Run) {
            int16_t half=(int16_t)(points>=0?points/2:-((-points+1)/2));
            points=(int16_t)(points+half+(Mechs[mechId].walkMove&1));
        }
    }
    /* Original shutdown equality, NOT >=30. Jump bypasses actuator/heat
     * deductions but remains subject to this test. Preserve this oddity. */
    if(MechHeatLevel[mechId]==MechHeatShutdownLevel) points=0;
    CharacterMovementPointsRemaining=points<0?0:(uint16_t)points;
}
