#include "game.h"
#include <stdio.h>
#include <stdlib.h>

/* Sol: original1631:1122..11AA. Offsets are raw anatomy bytes in the
 *125-byte Mech record, not critical-slot indices. Native fatal returns
 * leave AX unspecified. The sole original caller ejects the Mech before
 * another damage iteration, which tests Name==FF before using this result.
 * Thus zero below is NOT a recovered native AX: it is unobservable under
 * that caller contract. Invalid offsets still require native stack residue. */
uint16_t Combat_StructureHit(uint16_t hitLocationOffset)
{
    enum {
        FirstPrimaryArmourOffset=offsetof(Mech,currentArmour),
        PrimaryArmourLocationCount=MechStructureLocationCount,
        ArmourToStructureOffset=offsetof(Mech,currentStructure)-offsetof(Mech,currentArmour)
    };
    /* Sol:1631:112C..1141 maps the first eight armour bytes to the eight
     * corresponding internal-structure bytes. The +11 is the storage distance
     * between arrays, not eleven damage points or a tabletop modifier.
     * The final three armour bytes use the rear-armour branches below. */
    _Static_assert(FirstPrimaryArmourOffset==0x11 && PrimaryArmourLocationCount==8 &&
        ArmourToStructureOffset==0x0B,"ASM primary armour/structure mapping");
    if(hitLocationOffset>=FirstPrimaryArmourOffset &&
        hitLocationOffset<FirstPrimaryArmourOffset+PrimaryArmourLocationCount)
        return (uint16_t)(hitLocationOffset+ArmourToStructureOffset);
    switch(hitLocationOffset) {
    case MechOffset_Armour_RearLeftTorso:return MechOffset_Structure_LeftTorso;
    case MechOffset_Armour_RearCenterTorso:return MechOffset_Structure_CenterTorso;
    case MechOffset_Armour_RearRightTorso:return MechOffset_Structure_RightTorso;
    /* Sol: preserve native inward transfer to FRONT armour, not directly to
     * torso structure. Do not substitute modern/tabletop damage semantics. */
    case MechOffset_Structure_LeftArm:case MechOffset_Structure_LeftLeg:
        return MechOffset_Armour_LeftTorso;
    case MechOffset_Structure_LeftTorso:case MechOffset_Structure_RightTorso:
        return MechOffset_Armour_CenterTorso;
    case MechOffset_Structure_Head:case MechOffset_Structure_CenterTorso:
        MechDestroyedFlag=TRUE;return 0;
    case MechOffset_Structure_RightArm:case MechOffset_Structure_RightLeg:
        return MechOffset_Armour_RightTorso;
    default:
        fputs("Unresolved original1631:1122 invalid-offset stack return\n",stderr);
        abort();
    }
}
