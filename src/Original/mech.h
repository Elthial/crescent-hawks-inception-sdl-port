#ifndef CHI_MECH_H
#define CHI_MECH_H
#include <stddef.h>
#include <stdint.h>
/* Sol: Actual125-BYTE record. Group anatomy remains uncertain; criticalSlots
 * preserves the confirmed contiguous35-BYTE representation. */
enum { MechNameLength=16, MechArmourLocationCount=11,
       MechStructureLocationCount=8, MechActuatorSideCount=2,
       MechWeaponOrdinalCount=10, MechCriticalSlotCount=35, MechRecordSize=125 };
/* Sol: front armour and structure share this stored location order. Names
 * follow the EXE hit-location text table, not the disputed critical groups.
 * The last three indices apply only to the armour arrays. */
enum {
    MechLocation_LeftArm, MechLocation_LeftTorso, MechLocation_LeftLeg,
    MechLocation_Head, MechLocation_CenterTorso, MechLocation_RightArm,
    MechLocation_RightTorso, MechLocation_RightLeg,
    MechLocation_RearLeftTorso, MechLocation_RearCenterTorso,
    MechLocation_RearRightTorso
};
typedef struct Mech {
    uint8_t name[MechNameLength], tonnage;
    uint8_t currentArmour[MechArmourLocationCount], currentStructure[MechStructureLocationCount];
    uint8_t currentActuators[MechActuatorSideCount]; /* low nibble leg, high nibble arm; left/right */
    uint8_t engineHeatSinks, currentAmmo[MechWeaponOrdinalCount], walkMove, jumpMove;
    uint8_t criticalSlots[MechCriticalSlotCount];
    uint8_t maxArmour[MechArmourLocationCount], maxStructure[MechStructureLocationCount];
    uint8_t maxActuators[MechActuatorSideCount], maxAmmo[MechWeaponOrdinalCount];
    uint8_t engineHits, gyroHits, sensorHits, lifeSupportState;
    uint8_t pilotId, riderId, upgradePackageBase, upgradeLevelFlags;
} Mech;
/* Native routines pass record BYTE offsets, not array indices. These names
 * keep the two address domains distinct while deriving them from the record. */
enum {
    MechOffset_Armour_LeftTorso=offsetof(Mech,currentArmour)+MechLocation_LeftTorso,
    MechOffset_Armour_CenterTorso=offsetof(Mech,currentArmour)+MechLocation_CenterTorso,
    MechOffset_Armour_RightTorso=offsetof(Mech,currentArmour)+MechLocation_RightTorso,
    MechOffset_Armour_RearLeftTorso=offsetof(Mech,currentArmour)+MechLocation_RearLeftTorso,
    MechOffset_Armour_RearCenterTorso=offsetof(Mech,currentArmour)+MechLocation_RearCenterTorso,
    MechOffset_Armour_RearRightTorso=offsetof(Mech,currentArmour)+MechLocation_RearRightTorso,
    MechOffset_Structure_LeftArm=offsetof(Mech,currentStructure)+MechLocation_LeftArm,
    MechOffset_Structure_LeftTorso=offsetof(Mech,currentStructure)+MechLocation_LeftTorso,
    MechOffset_Structure_LeftLeg=offsetof(Mech,currentStructure)+MechLocation_LeftLeg,
    MechOffset_Structure_Head=offsetof(Mech,currentStructure)+MechLocation_Head,
    MechOffset_Structure_CenterTorso=offsetof(Mech,currentStructure)+MechLocation_CenterTorso,
    MechOffset_Structure_RightArm=offsetof(Mech,currentStructure)+MechLocation_RightArm,
    MechOffset_Structure_RightTorso=offsetof(Mech,currentStructure)+MechLocation_RightTorso,
    MechOffset_Structure_RightLeg=offsetof(Mech,currentStructure)+MechLocation_RightLeg
};
_Static_assert(sizeof(Mech)==MechRecordSize,"Native Mech record size");
_Static_assert(offsetof(Mech,criticalSlots)==0x33,"Native critical block");
_Static_assert(offsetof(Mech,maxArmour)==0x56,"Native maximum armour");
_Static_assert(offsetof(Mech,engineHits)==0x75,"Native engine hits");
#endif
