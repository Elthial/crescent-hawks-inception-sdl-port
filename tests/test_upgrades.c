/* Sol: Complete-record witnesses for original upgrade packages. Test-only UI
 * adapters; selection/purchase/Yes-No are the actual preservation methods. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, balanceUpdates, rejected;
static uint16_t answer;
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Upgrade check %u failed\n",checks); exit(1); }
}
void Draw_Top_Graphic_Sidebar(void) { TextRow = TextColumn = 0; }
uint16_t Display_Text_Mech_Names(void) { return SelectedMechId; }
void Display_Text_From_Memory(uint8_t *text) { (void)text; ++TextRow; }
void Display_Text_Dynamic_Value(uint16_t value) { (void)value; }
void Display_Text_CBill_Balance(void) { ++balanceUpdates; }
void Display_Text_4FA0_Value(void) { ++TextRow; }
void Display_Text_Shop_Cannot_Afford_Text(void) { ++rejected; }
void Drain_Pending_Keyboard_Input(void) { }
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { return answer; }
void Citadel_Building_Dialogs(uint16_t action) { check(action == CitadelDialog_DisplayCBillBalance); }
static Mech *reset(uint16_t package, uint32_t funds)
{
    memset(Mechs,0,sizeof Mechs);
    SelectedMechId = 1;
    Mechs[1] = MechRefs[package % MechUpgrade_ChassisCount];
    Mechs[1].upgradePackageBase = (uint8_t)(package % MechUpgrade_ChassisCount);
    Mechs[1].upgradeLevelFlags = package >= MechUpgrade_ChassisCount ? MechUpgrade_StageOneInstalled : 0;
    CBills = funds;
    answer = 'Y';
    balanceUpdates = rejected = 0;
    return &Mechs[1];
}
int main(void)
{
    static const uint16_t prices[8] = {11000,10400,12200,13800,15800,14000,13200,17600};
    Mech expected, before, *mech;
    check(sizeof MechRefs == 1000 && !strcmp((char *)MechRefs[0].name,"LOCUST         "));
    check(!memcmp(prices,MechUpgradeCostByPackage,sizeof prices));
    for (uint16_t package = 0; package < MechUpgrade_PackageCount; ++package) {
        mech = reset(package,50000);
        expected = *mech;
        switch (package) {
        case 0:
            expected.criticalSlots[14] = expected.criticalSlots[0] = Mech_Med_Laser;
            expected.currentAmmo[0] = expected.currentAmmo[1] = 255;
            expected.maxAmmo[0] = expected.maxAmmo[1] = 255;
            break;
        case 1:
            expected.criticalSlots[28] = Mech_Med_Laser;
            expected.currentAmmo[1] = expected.maxAmmo[1] = 255;
            memcpy(expected.currentArmour,MechRefs[0].currentArmour,11);
            memcpy(expected.maxArmour,MechRefs[0].maxArmour,11);
            break;
        case 2:
            expected.criticalSlots[16] = expected.criticalSlots[15] = Mech_Small_Laser;
            expected.criticalSlots[1] = expected.criticalSlots[0] = Mech_Small_Laser;
            memset(expected.currentAmmo,255,5); memset(expected.maxAmmo,255,5);
            break;
        case 3:
            expected.criticalSlots[23] = expected.criticalSlots[14] = Mech_Med_Laser;
            expected.currentAmmo[3] = expected.currentAmmo[2]; expected.maxAmmo[3] = expected.maxAmmo[2];
            expected.currentAmmo[1] = expected.currentAmmo[2] = 255;
            expected.maxAmmo[1] = expected.maxAmmo[2] = 255;
            memcpy(expected.currentArmour,CommandoStageOneArmour,11);
            memcpy(expected.maxArmour,CommandoStageOneArmour,11);
            break;
        case 4:
            expected.walkMove = 7; expected.engineHits = 0;
            expected.criticalSlots[22] = expected.criticalSlots[8] = Mech_Small_Laser;
            expected.currentAmmo[3] = expected.currentAmmo[4] = 255;
            expected.maxAmmo[3] = expected.maxAmmo[4] = 255;
            break;
        case 5:
            expected.jumpMove = 0;
            expected.criticalSlots[32] = expected.criticalSlots[0] = expected.criticalSlots[34] = Mech_Med_Laser;
            memset(expected.currentAmmo + 2,255,3); memset(expected.maxAmmo + 2,255,3);
            break;
        case 6:
            expected.jumpMove = 0;
            memcpy(expected.currentArmour,MechRefs[0].currentArmour,11);
            memcpy(expected.maxArmour,MechRefs[0].currentArmour,11);
            expected.criticalSlots[33] = expected.criticalSlots[32] = Mech_Med_Laser;
            expected.currentAmmo[5] = expected.currentAmmo[6] = 255;
            expected.maxAmmo[5] = expected.maxAmmo[6] = 255;
            break;
        case 7:
            memset(expected.criticalSlots + 28,Mech_Small_Laser,6);
            expected.criticalSlots[34] = Mech_Med_Laser;
            memset(expected.currentAmmo,255,10); memset(expected.maxAmmo,255,10);
            break;
        }
        expected.upgradeLevelFlags = package < 4 ? 1 : 3;
        Mechlube_Modify_Mech();
        check(MechModificationWorkflowEnabled && SelectedMechUpgradePackage == package && MechUpgradeCost == prices[package]);
        Mechlube_Upgrade_Mech();
        check(CBills == 50000U - prices[package] && balanceUpdates == 1 && rejected == 0);
        check(!memcmp(mech,&expected,sizeof expected));
        check(Mechs[0].tonnage == 0);
    }
    mech = reset(0,10999); before = *mech;
    Mechlube_Modify_Mech(); Mechlube_Upgrade_Mech();
    check(rejected == 1 && CBills == 10999 && !memcmp(mech,&before,sizeof before));
    mech = reset(0,50000); before = *mech; answer = 'N';
    Mechlube_Modify_Mech(); Mechlube_Upgrade_Mech();
    check(CBills == 50000 && balanceUpdates == 0 && !memcmp(mech,&before,sizeof before));
    mech = reset(0,50000); mech->upgradePackageBase = 200; before = *mech;
    Mechlube_Modify_Mech(); check(SelectedMechUpgradePackage == 8 && MechUpgradeCost == 3341);
    Mechlube_Upgrade_Mech();
    check(CBills == 46659 && !memcmp(mech,&before,sizeof before)); /* BUG-010. */
    mech = reset(0,50000); mech->upgradeLevelFlags = 3; SelectedMechUpgradePackage = 7; MechUpgradeCost = 123;
    Mechlube_Modify_Mech();
    check(!MechModificationWorkflowEnabled && SelectedMechUpgradePackage == 7 && MechUpgradeCost == 123);
    mech = reset(0,UINT32_MAX); before = *mech; SelectedMechUpgradePackage = 8; MechUpgradeCost = -1;
    Mechlube_Upgrade_Mech(); check(CBills == 0 && !memcmp(mech,&before,sizeof before));
    printf("%u upgrade assertions passed\n",checks);
    return 0;
}
