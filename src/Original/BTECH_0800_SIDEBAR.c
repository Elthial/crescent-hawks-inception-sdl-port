#include "game.h"
enum { AttributeBarScale=12, AttributeBarBottom=13,AttributeBarFrameBottom=14,
    SidebarBodyColumn=9,SidebarDexterityColumn=10,SidebarCharismaColumn=11,
    SidebarHeadingRow=13,SidebarFirstCharacterRow=14,SidebarCharacterRowStride=2,
    SidebarMaximumCharacterRows=4,SidebarPartyPanel=3,SidebarLowerPanel=4,
    SidebarFrameColour=14,SidebarDamageColour=4,SidebarBackgroundColour=0 };

/* Sol: Native0800:4BC1..4CAB retained EGA. A twelve-unit display scale, not
 * attribute clamping. Literal frame offsets describe the original7x15 pixels.
 * WORD wrapping occurs before primitives interpret their signed endpoints. */
void Draw_BDC_Attribute_Bar(int16_t value,uint16_t column,uint16_t pixelY)
{
    uint16_t x=(uint16_t)(column*8);
    uint16_t empty=(uint16_t)(AttributeBarScale-value);
    Draw_Clipped_Axis_Aligned_EGA_Line(x+1,pixelY,x+5,pixelY,SidebarFrameColour);
    Draw_Clipped_Axis_Aligned_EGA_Line(x,pixelY+1,x,pixelY+AttributeBarBottom,SidebarFrameColour);
    Draw_Clipped_Axis_Aligned_EGA_Line(x+6,pixelY+1,x+6,pixelY+AttributeBarBottom,SidebarFrameColour);
    Draw_Clipped_Axis_Aligned_EGA_Line(x+1,pixelY+AttributeBarFrameBottom,x+5,pixelY+AttributeBarFrameBottom,SidebarFrameColour);
    Draw_Horizontal_EGA_Line(x+2,(uint16_t)(pixelY+empty+2),x+4,pixelY+AttributeBarBottom,EGA_BrightGreen);
}
/* Sol: Native0800:4AA6..4BC0; signed BYTE names/attributes/health, native
 * IDIV10 truncates toward zero. Zero health units become1 before damage count.
 * Damage is an inclusive red rectangle, retaining original off-by-one effects. */
void Draw_Character_BDC_Sidebar_Row(uint16_t id,uint16_t row)
{
    Character *character=&Characters[id];
    Draw_EGA_Text_To_Screen(CharacterNames[(int8_t)character->name],1,row,EGA_BrightWhite,SidebarBackgroundColour);
    uint16_t pixelY=(uint16_t)(row*8);
    int16_t body=(int8_t)character->body;
    Draw_BDC_Attribute_Bar(body,SidebarBodyColumn,pixelY);
    int16_t healthUnits=(int8_t)character->health/CharacterHealthPerBodyPoint;
    if (healthUnits==0) healthUnits=1;
    int16_t damage=(int16_t)(body-healthUnits);
    if (damage!=0) {
        uint16_t top=(uint16_t)(pixelY-body+AttributeBarFrameBottom);
        Draw_Horizontal_EGA_Line(SidebarBodyColumn*8+2,top,SidebarBodyColumn*8+4,
            (uint16_t)(top+damage),SidebarDamageColour);
    }
    Draw_BDC_Attribute_Bar((int8_t)character->dexterity,SidebarDexterityColumn,pixelY);
    Draw_BDC_Attribute_Bar((int8_t)character->charisma,SidebarCharismaColumn,pixelY);
}
/* Sol: Native0800:4CAC..4D56: scan eight records, show at most four. Argument
 * gates only top-panel clear; menu/border3 and final menu/border4 are unconditional. */
void Draw_Health_and_C_Bills_Sidebar(uint16_t redrawTopGraphic)
{
    Menu_Memory_Variables(SidebarPartyPanel); Draw_Menu_Border(SidebarPartyPanel);
    if (redrawTopGraphic!=FALSE) Draw_Top_Graphic_Sidebar();
    Draw_EGA_Text_To_Screen((uint8_t *)"BDC",SidebarBodyColumn,SidebarHeadingRow,EGA_BrightWhite,SidebarBackgroundColour);
    uint16_t displayed=0;
    for (uint16_t id=0;id<PartySize;++id)
        if (displayed<SidebarMaximumCharacterRows && Characters[id].name!=Character_Dead) {
            Draw_Character_BDC_Sidebar_Row(id,SidebarFirstCharacterRow+displayed*SidebarCharacterRowStride);
            ++displayed;
        }
    Display_Text_CBill_Balance();
    Menu_Memory_Variables(SidebarLowerPanel); Draw_Menu_Border(SidebarLowerPanel);
}
