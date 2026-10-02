#include "game.h"
#include "dos.h"

/* Original1543:07CB..0A34, whole raw ASM checked. Persistent cursor skips
 * inactive enemies. There is NO all-inactive escape. Range is informational,
 * not a rejection gate. Unexpected menu choices leave the old target intact. */
void Combat_Select_Weapon_Target_UI(uint16_t attackerId,uint16_t weaponId,uint16_t weaponSlot)
{
    Set_Text_Colour_Bright_Green();
    Display_Text_From_Memory((uint8_t *)"\r\rTarget:\r\rRange:\006\017");
    uint16_t choice=CombatTargetChoice_Next;
    do {
        if(!CombatantActive[EnemyTargetId]) goto NextEnemy;
        Menu_Memory_Variables(3); Draw_Menu_Border(3); Draw_Top_Graphic_Sidebar();
        CombatMessageMenuOptionCount=3;
        Display_Text_From_Memory((uint8_t *)"Target here Next enemy Cancel");
        Combat_Render_Movement_Preview(EnemyTargetId,TRUE);
        Menu_Memory_Variables(4);
        if((int16_t)EnemyTargetId>=Enemy_Infantry_CombatantId_Range_First) {
            Set_Text_Colour_Bright_Green(); TextColumn=0; TextRow=8;
            Display_Text_From_Memory((uint8_t *)"Weapon:\006\017");
        } else Draw_Horizontal_EGA_Line(8,0x48,0x57,0x57,0);
        TextColumn=0; TextRow=5;
        if((int16_t)EnemyTargetId>=Enemy_Infantry_CombatantId_Range_First) {
            Display_Text_From_Memory((uint8_t *)"Human      ");
            Draw_Horizontal_EGA_Line(8,0x50,0x57,0x57,0);
            Draw_EGA_Text_To_Screen(WeaponStats[(int8_t)Characters[EnemyTargetId-Enemy_Infantry_Record_First].weapon].name,1,10,15,0);
        } else {
            Display_Text_From_Memory(Mechs[EnemyTargetId-Enemy_Infantry_Record_First].name);
            Draw_Horizontal_EGA_Line(8,0x48,0x57,0x4F,0);
        }
        TextColumn=0; TextRow=7;
        Move_Map_View_To_Packed_Position(CombatantPackedX[attackerId],CombatantPackedY[attackerId]);
        Display_Text_From_Memory(WeaponRangeText[Combat_Calculate_RangeBracket(EnemyTargetId,weaponId)]);
        Display_Text_From_Memory((uint8_t *)"  ");
        Menu_Memory_Variables(3); choice=Display_Menu_Choices_And_Check(3);
        if(choice==CombatTargetChoice_Next) goto NextEnemy;
        if(choice==CombatTargetChoice_Target)
            CombatWeaponTarget[attackerId*CombatWeaponTargetSlots+weaponSlot]=(uint8_t)EnemyTargetId;
        else if(choice==CombatTargetChoice_Cancel)
            CombatWeaponTarget[attackerId*CombatWeaponTargetSlots+weaponSlot]=UINT8_MAX;
        continue;
NextEnemy:
        ++EnemyTargetId;
        if((int16_t)EnemyTargetId>=AllCombatantCount) EnemyTargetId=Enemy_All_CombatantId_Range_First;
    } while(choice==CombatTargetChoice_Next);
}
