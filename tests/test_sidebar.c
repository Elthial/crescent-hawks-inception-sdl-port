#include "game.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct Rectangle { uint16_t left,top,right,bottom,colour; } Rectangle;
static Rectangle fills[32];
static unsigned fillCount,lineCount,textCount,topClears,menus,borders,money;
void Draw_Horizontal_EGA_Line(uint16_t x,uint16_t y,uint16_t right,uint16_t bottom,uint16_t colour)
{
    assert(fillCount<32); fills[fillCount++]=(Rectangle){x,y,right,bottom,colour};
}
void Draw_Clipped_Axis_Aligned_EGA_Line(uint16_t x,uint16_t y,uint16_t right,uint16_t bottom,uint16_t colour)
{
    (void)x; (void)y; (void)right; (void)bottom; assert(colour==14); ++lineCount;
}
void Draw_EGA_Text_To_Screen(uint8_t *text,uint16_t column,uint16_t row,uint16_t foreground,uint16_t background)
{
    assert(foreground==15 && background==0);
    if (textCount==0) assert(!strcmp((const char *)text,"BDC") && column==9 && row==13);
    else {
        static const unsigned ids[]={1,3,4,5};
        assert(textCount<=4 && text==CharacterNames[ids[textCount-1]] && column==1 && row==14+2*(textCount-1));
    }
    ++textCount;
}
void Menu_Memory_Variables(uint16_t panel) { assert(panel==(menus++==0?3:4)); }
void Draw_Menu_Border(uint16_t style) { assert(style==(borders++==0?3:4)); }
void Draw_Top_Graphic_Sidebar(void) { ++topClears; }
void Display_Text_CBill_Balance(void) { assert(textCount==5 && menus==1 && borders==1); ++money; }
int main(void)
{
    for (unsigned redraw=0;redraw<2;++redraw) {
        fillCount=lineCount=textCount=topClears=menus=borders=money=0;
        for (unsigned i=0;i<PartySize;++i) Characters[i]=(Character){0};
        for (unsigned i=0;i<PartySize;++i) {
            Characters[i].name=(uint8_t)i; Characters[i].body=8;
            Characters[i].dexterity=9; Characters[i].charisma=7; Characters[i].health=80;
        }
        Characters[0].name=Characters[2].name=Character_Dead;
        Characters[1].health=0;
        Draw_Health_and_C_Bills_Sidebar((uint16_t)redraw);
        assert(textCount==5 && lineCount==48 && fillCount==13 && topClears==redraw && money==1 && menus==2 && borders==2);
        assert(fills[0].left==74 && fills[0].right==76 && fills[0].top==118 && fills[0].bottom==125 && fills[0].colour==10);
        assert(fills[1].left==74 && fills[1].right==76 && fills[1].top==118 && fills[1].bottom==125 && fills[1].colour==4);
    }
    fillCount=lineCount=0;
    Draw_BDC_Attribute_Bar(-1,9,112); /* native signed attribute, not clamped */
    assert(fills[0].top==127 && fills[0].bottom==125);
    fillCount=lineCount=0;
    Draw_BDC_Attribute_Bar(12,9,112); assert(fills[0].top==114);
    puts("Original sidebar party cap/skips, panel order, signed bar values and inclusive zero-health overlay passed");
    return 0;
}
