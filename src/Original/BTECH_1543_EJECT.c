#include "game.h"

/* Original1543:0A35..0C71, complete raw ASM checked. Retire combatant,
 * preserve name initial and eject friendly occupants. Exact signed target
 * comparison does NOT mask the high bit. Occupant coordinates use normalized
 * Mech record ID, equal to combatant ID for the friendly ejection case. */
void Combat_Mech_Eject(uint16_t mechCombatantId)
{
    CombatantActive[mechCombatantId]=FALSE; CombatantCasualtyFlags[mechCombatantId]=TRUE;
    for(uint16_t id=0;id<AllCombatantCount;++id)
        for(uint16_t slot=0;slot<CombatWeaponTargetSlots;++slot) {
            uint8_t *target=&CombatWeaponTarget[id*CombatWeaponTargetSlots+slot];
            if((uint16_t)(int16_t)(int8_t)*target==mechCombatantId) *target=UINT8_MAX;
        }
    uint16_t mechRecordId=mechCombatantId;
    if((int16_t)mechRecordId>=Enemy_All_CombatantId_Range_First) mechRecordId-=Enemy_Infantry_Record_First;
    Mech *mech=&Mechs[mechRecordId];
    DestroyedMechNameInitial[mechRecordId]=mech->name[0]; mech->name[0]=MECH_Destroyed;
    if(ArenaRentalMechMode) {
        if(mechRecordId==Enemy_Mech_Record_First) {
            Mechs[Enemy_Mech_Record_First+1].name[0]=MECH_Destroyed;
            CombatantActive[ArenaRentalPathTargetId]=FALSE;
        } else if(mechRecordId==Enemy_Mech_Record_First+1)
            Starport_MapPatch_SaveApply_Or_Restore(FALSE); /*Native applies, not restores*/
    }
    CombatNotificationLatch=FALSE; ShowArmShotOffAnimation=FALSE;
    Menu_Memory_Variables(4); Set_Text_Colour_Bright_Green();
    if(!(ArenaRentalMechMode && mechCombatantId==ArenaRentalPathTargetId) && CombatMessageVerbosity!=CombatMessage_None)
        Combat_CombatMessageVerbosityFilter((uint8_t *)"\rMech is destroyed!");
    if((int16_t)mechRecordId<Friendly_Infantry_Combatant_Range_First) {
        Display_Text_From_Memory((uint8_t *)"\rMen eject!");
        uint16_t pilotId=mech->pilotId;
        if(pilotId!=MECH_NoPilot) {
            Characters[pilotId].mechAssignment=Character_OnFoot;
            uint16_t pilotCombatant=(uint16_t)(pilotId+Friendly_Infantry_Combatant_Range_First);
            CombatantActive[pilotCombatant]=TRUE;
            CombatantPackedX[pilotCombatant]=CombatantPackedX[mechRecordId];
            CombatantPackedY[pilotCombatant]=CombatantPackedY[mechRecordId];
        }
        uint16_t riderId=mech->riderId;
        if(riderId!=MECH_NoRider) {
            Characters[riderId].mechAssignment=Character_OnFoot;
            uint16_t riderCombatant=(uint16_t)(riderId+Friendly_Infantry_Combatant_Range_First);
            CombatantActive[riderCombatant]=TRUE;
            uint16_t riderX=(uint16_t)(CombatantPackedX[mechRecordId]+1);
            if(riderX&PackedPositionLocalCarryBit) riderX=(uint16_t)(riderX+PackedPositionLocalCarryBit);
            CombatantPackedX[riderCombatant]=riderX; CombatantPackedY[riderCombatant]=CombatantPackedY[mechRecordId];
        }
        if((int16_t)CombatSpeedSetting<CombatSpeedKeyWaitSetting) GameSpeed_RateControl();
    }
    TextColour=EGA_BrightWhite; /*Seats remain stored, no actor-coordinate clear*/
}
