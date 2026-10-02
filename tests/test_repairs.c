/* Sol: Run the real repair and Yes/No bodies. Presentation/input contracts
 * are isolated here. Expected costs/quirks derive from the original ASM audit;
 * these are not gameplay captures. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, prompts, menus, balanceUpdates;
static unsigned quoteCount;
static uint16_t quotes[32];
static uint16_t answer, choices[4];
static int cannotRepair, allWeaponsFixed;
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Repair check %u failed\n",checks); exit(1); }
}
void Draw_Top_Graphic_Sidebar(void) { TextRow = TextColumn = 0; }
uint16_t Display_Text_Mech_Names(void) { return SelectedMechId; }
void Display_Text_From_Memory(uint8_t *text)
{
    if (text[0] == '\r' && text[1] == 6) { ++prompts; ++TextRow; return; }
    if (text[0] == '\r' && text[1] == 2) { ++TextRow; return; }
    if (strstr((char *)text,"doesn't have the equipment")) cannotRepair = 1;
    if (!strcmp((char *)text,"All your weapons are fixed.")) allWeaponsFixed = 1;
    /* The estimate heading wraps once in the original narrow text panel. */
    if (!strcmp((char *)text,"Here's your weapons estimate:")) ++TextRow;
    for (uint8_t *p = text; *p; ++p) if (*p == '\r') ++TextRow;
}
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { return answer; }
void Drain_Pending_Keyboard_Input(void) { }
void Display_Text_Shop_Cannot_Afford_Text(void) { }
void Display_Text_Dynamic_Value(uint16_t value) { check(quoteCount<32); quotes[quoteCount++]=value; }
void Display_Text_CBill_Balance(void) { ++balanceUpdates; }
void Display_Text_4FA0_Value(void) { ++TextRow; }
void Citadel_Building_Dialogs(uint16_t action)
{
    check(action == CitadelDialog_DisplayCBillBalance);
}
uint16_t Display_Menu_Choices_And_Check(uint16_t bottomRow)
{
    check(bottomRow == MechRepairWeaponMenuBottomRow && menus < 4);
    return choices[menus++];
}
static Mech *reset(uint32_t funds)
{
    memset(Mechs,0,sizeof Mechs);
    SelectedMechId = 1;
    CBills = funds;
    answer = 'Y';
    prompts = menus = balanceUpdates = 0;
    quoteCount=0;
    cannotRepair = allWeaponsFixed = 0;
    memset(choices,0,sizeof choices);
    return &Mechs[1];
}
int main(void)
{
    Mech *mech;
    check(sizeof(Weapon) == 17 && sizeof WeaponStats == 561);
    check(!strcmp((char *)WeaponStats[15].name,"SmallLaser"));
    check(WeaponStats[31].damage == 2 && WeaponStats[31].attackCountOrClusterColumn == 5);

    mech = reset(100); mech->maxArmour[0] = 3;
    Mechlube_Repair_Mech();
    check(CBills == 88 && mech->currentArmour[0] == 3 && balanceUpdates == 3);
    check(Mechs[0].currentArmour[0] == 0 && prompts == 2);
    check(quoteCount==2 && quotes[0]==3 && quotes[1]==12); /* Missing points, then4 C-bills each. */

    mech = reset(100); mech->maxArmour[0] = 3; mech->maxStructure[0] = 2;
    Mechlube_Repair_Mech();
    check(CBills==70 && mech->currentArmour[0]==3 && mech->currentStructure[0]==2);
    check(quoteCount==3 && quotes[0]==3 && quotes[1]==12 && quotes[2]==18); /* Structure9 per point. */

    mech = reset(11); mech->maxArmour[0] = 3; mech->maxArmour[1] = 2;
    Mechlube_Repair_Mech();
    check(CBills == 3 && mech->currentArmour[0] == 2 && mech->currentArmour[1] == 0);

    mech = reset(100); mech->maxArmour[0] = 3; answer = 'n';
    Mechlube_Repair_Mech(); check(CBills == 100 && mech->currentArmour[0] == 0);
    mech = reset(0); mech->maxArmour[0] = 1;
    Mechlube_Repair_Mech(); check(CBills == 0 && prompts == 0 && mech->currentArmour[0] == 0);

    mech = reset(17); mech->maxStructure[0] = 2;
    Mechlube_Repair_Mech(); check(CBills == 8 && mech->currentStructure[0] == 1);
    mech = reset(65540); mech->maxArmour[0] = 1;
    Mechlube_Repair_Mech(); check(CBills == 65536 && mech->currentArmour[0] == 1);

    mech = reset(800); mech->maxStructure[0] = 1;
    mech->criticalSlots[0] = Destroyed_Heat_Sink;
    Mechlube_Repair_Mech();
    check(CBills == 791 && mech->criticalSlots[0] == Destroyed_Heat_Sink);
    mech = reset(800); mech->criticalSlots[0] = Destroyed_Heat_Sink;
    Mechlube_Repair_Mech(); check(CBills == 0 && mech->criticalSlots[0] == Heat_Sink);

    mech = reset(600);
    mech->criticalSlots[0] = mech->criticalSlots[1] = Component_Destroyed | Mech_Small_Laser;
    Mechlube_Repair_Mech();
    check(CBills == 0 && menus == 2); /* BUG-008: redundant second purchase. */
    check(mech->criticalSlots[0] == Mech_Small_Laser && mech->criticalSlots[1] == Mech_Small_Laser);

    mech = reset(600); mech->criticalSlots[0] = Component_Destroyed | Mech_Small_Laser;
    mech->criticalSlots[1] = Component_Destroyed | Mech_SRMissile6;
    Mechlube_Repair_Mech();
    check(CBills == 300 && menus == 1 && allWeaponsFixed);
    check(mech->criticalSlots[1] == (Component_Destroyed | Mech_SRMissile6)); /* BUG-009. */

    mech = reset(300); mech->criticalSlots[0] = Component_Destroyed | Mech_Small_Laser;
    choices[0] = 1; /* One visible weapon row, followed by Nothing. */
    Mechlube_Repair_Mech();
    check(CBills == 300 && menus == 1 && mech->criticalSlots[0] == (Component_Destroyed | Mech_Small_Laser));
    mech = reset(299); mech->criticalSlots[0] = Component_Destroyed | Mech_Small_Laser;
    Mechlube_Repair_Mech();
    check(CBills == 299 && mech->criticalSlots[0] == (Component_Destroyed | Mech_Small_Laser));

    mech = reset(200); mech->maxActuators[0] = 0xAF; mech->maxActuators[1] = 0x5F;
    Mechlube_Repair_Mech();
    check(CBills == 0 && mech->currentActuators[0] == 0xAF && mech->currentActuators[1] == 0x5F);
    mech = reset(199); mech->maxActuators[0] = 0xAF;
    Mechlube_Repair_Mech(); check(CBills == 199 && mech->currentActuators[0] == 0);

    mech = reset(100); mech->engineHits = 1;
    Mechlube_Repair_Mech(); check(cannotRepair && CBills == 100 && mech->engineHits == 1);
    mech = reset(4); mech->currentArmour[0] = 1; /* Native wrapped deficit, no clamp. */
    Mechlube_Repair_Mech(); check(CBills == 0 && mech->currentArmour[0] == 2);
    printf("%u repair assertions passed\n",checks);
    return 0;
}
