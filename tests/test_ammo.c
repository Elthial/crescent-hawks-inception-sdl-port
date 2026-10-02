/* Sol: Execute the original ammo method with test-only unconverted UI calls.
 * Prices, ordinal order and arithmetic quirks derive from ASM, not gameplay. */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks, quantityPrompts, balanceUpdates, keyReads;
static uint32_t requestedRounds;
static int capped, outOfCash, noFunctionalWeapon, damagedWeapon, fullAmmo;
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Ammo check %u failed\n",checks); exit(1); }
}
void Draw_Top_Graphic_Sidebar(void) { }
uint16_t Display_Text_Mech_Names(void) { return SelectedMechId; }
void Display_Text_From_Memory(uint8_t *text) { (void)text; }
void Display_Text_From_Memory_ScreenRetrace_KeyboardInput(uint8_t *text)
{
    if (strstr((char *)text,"more than you can carry")) capped = 1;
    if (strstr((char *)text,"ran out of money")) outOfCash = 1;
    if (strstr((char *)text,"don't have any functional")) noFunctionalWeapon = 1;
    if (strstr((char *)text,"weaponry is damaged")) damagedWeapon = 1;
    if (strstr((char *)text,"maximum ammunition")) fullAmmo = 1;
}
uint16_t Keyboard_Get_ASCII_Hex_Input(void) { ++keyReads; return '\r'; }
void Set_Text_Colour_Bright_Green(void) { }
void Display_Text_Dynamic_Value(uint16_t value) { (void)value; }
void Display_Text_CBill_Balance(void) { ++balanceUpdates; }
uint32_t Prompt_For_Unsigned_Decimal(void) { ++quantityPrompts; return requestedRounds; }
static Mech *reset(uint32_t funds, uint32_t quantity)
{
    static const uint8_t prices[7] = {10,15,25,30,30,60,80};
    memset(Mechs,0,sizeof Mechs);
    memcpy(MissileAmmoPriceByComponent,prices,sizeof prices);
    SelectedMechId = 1;
    CBills = funds;
    requestedRounds = quantity;
    quantityPrompts = balanceUpdates = keyReads = 0;
    capped = outOfCash = noFunctionalWeapon = damagedWeapon = fullAmmo = 0;
    return &Mechs[1];
}
int main(void)
{
    Mech *mech;
    mech = reset(100,3); mech->criticalSlots[0] = Mech_MachineGun; mech->maxAmmo[0] = 10;
    Mechlube_Buy_Ammo();
    check(CBills == 94 && mech->currentAmmo[0] == 3 && balanceUpdates == 4);
    check(quantityPrompts == 1 && Mechs[0].currentAmmo[0] == 0);

    mech = reset(5,10); mech->criticalSlots[0] = Mech_MachineGun; mech->maxAmmo[0] = 10;
    Mechlube_Buy_Ammo(); check(CBills == 1 && mech->currentAmmo[0] == 2 && outOfCash);
    mech = reset(100,99); mech->criticalSlots[0] = Mech_MachineGun; mech->maxAmmo[0] = 4;
    Mechlube_Buy_Ammo(); check(CBills == 92 && mech->currentAmmo[0] == 4 && capped);
    mech = reset(0,1); mech->criticalSlots[0] = Mech_MachineGun; mech->maxAmmo[0] = 4;
    Mechlube_Buy_Ammo(); check(CBills == 0 && mech->currentAmmo[0] == 0 && outOfCash);
    mech = reset(100,0); mech->criticalSlots[0] = Mech_MachineGun; mech->maxAmmo[0] = 4;
    Mechlube_Buy_Ammo(); check(CBills == 100 && mech->currentAmmo[0] == 0 && balanceUpdates == 0);

    for (uint16_t missile = 0; missile < 7; ++missile) {
        mech = reset(1000,2); mech->criticalSlots[0] = (uint8_t)(Mech_LRMissile5 + missile);
        mech->maxAmmo[0] = 4;
        Mechlube_Buy_Ammo();
        check(CBills == 1000U - 2U * MissileAmmoPriceByComponent[missile] && mech->currentAmmo[0] == 2);
    }

    mech = reset(100,2);
    mech->criticalSlots[0] = Mech_Small_Laser;
    mech->criticalSlots[1] = Component_Destroyed | Mech_MachineGun;
    mech->criticalSlots[2] = Mech_MachineGun;
    mech->maxAmmo[2] = 4;
    Mechlube_Buy_Ammo();
    check(CBills == 96 && mech->currentAmmo[2] == 2 && mech->currentAmmo[0] == 0 && mech->currentAmmo[1] == 0);
    check(damagedWeapon && quantityPrompts == 1);

    mech = reset(100,2); mech->criticalSlots[0] = 0x14; /* Autocannon2: unsupported by original shop. */
    mech->maxAmmo[0] = 4;
    Mechlube_Buy_Ammo(); check(noFunctionalWeapon && !damagedWeapon && quantityPrompts == 0 && keyReads == 1);
    mech = reset(100,2); mech->criticalSlots[0] = Component_Destroyed | Mech_MachineGun;
    Mechlube_Buy_Ammo(); check(noFunctionalWeapon && damagedWeapon && quantityPrompts == 0 && keyReads == 1);
    mech = reset(100,2); mech->criticalSlots[0] = Mech_MachineGun;
    mech->maxAmmo[0] = mech->currentAmmo[0] = 4;
    Mechlube_Buy_Ammo(); check(fullAmmo && quantityPrompts == 0 && CBills == 100);

    mech = reset(100,0x80000002U); mech->criticalSlots[0] = Mech_MachineGun; mech->maxAmmo[0] = 1;
    Mechlube_Buy_Ammo(); check(!capped && mech->currentAmmo[0] == 2 && CBills == 96); /* Signed request avoids cap. */
    mech = reset(2,1); mech->criticalSlots[0] = Mech_MachineGun; mech->currentAmmo[0] = 1;
    Mechlube_Buy_Ammo(); check(capped && outOfCash && mech->currentAmmo[0] == 2 && CBills == 0); /* Negative deficit wraps. */
    mech = reset(UINT32_MAX,1); mech->criticalSlots[0] = Mech_LRMissile5; mech->maxAmmo[0] = 1;
    MissileAmmoPriceByComponent[0] = 255; /* Original signed BYTE/CWD price behaviour. */
    Mechlube_Buy_Ammo(); check(mech->currentAmmo[0] == 1 && CBills == 0);

    mech = reset(100,1);
    for (unsigned i = 0; i < MechWeaponOrdinalCount; ++i) mech->criticalSlots[i] = Mech_Small_Laser;
    mech->criticalSlots[MechWeaponOrdinalCount] = Mech_MachineGun;
    Mechlube_Buy_Ammo(); check(noFunctionalWeapon && quantityPrompts == 0); /* Collection stops at ten. */
    printf("%u ammunition assertions passed\n",checks);
    return 0;
}
