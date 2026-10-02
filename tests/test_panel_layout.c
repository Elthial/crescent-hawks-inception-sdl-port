#include "game.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
    assert(MenuPanelContextInitialized==0);
    assert(MenuPanelLayouts[4].left==1 && MenuPanelLayouts[4].width==11);
    /* First activation must not save arbitrary outgoing scratch into layout0. */
    CurrentMenuLayoutIndex=0; TextColour=99; TextBackgroundColour=88;
    TextColumn=77; TextRow=66;
    Menu_Memory_Variables(4);
    assert(MenuPanelContextInitialized==1 && CurrentMenuLayoutIndex==4);
    assert(MenuPanelLayouts[0].foreground==15 && MenuPanelLayouts[0].column==0);
    assert(TextPanelLeft==1 && TextPanelTop==1 && TextPanelWidth==11 && TextPanelHeight==11);
    assert(TextColour==15 && TextBackgroundColour==0 && TextColumn==0 && TextRow==0);
    TextColour=12; TextBackgroundColour=3; TextColumn=7; TextRow=8;
    TextPanelLeft=90; TextPanelTop=91; TextPanelWidth=92; TextPanelHeight=93;
    Menu_Memory_Variables(3);
    assert(MenuPanelLayouts[4].foreground==12 && MenuPanelLayouts[4].background==3 &&
        MenuPanelLayouts[4].column==7 && MenuPanelLayouts[4].row==8);
    assert(MenuPanelLayouts[4].left==1 && MenuPanelLayouts[4].width==11);
    assert(TextPanelTop==13 && TextColour==15 && TextColumn==0);
    Menu_Memory_Variables(4);
    assert(TextPanelLeft==1 && TextPanelTop==1 && TextPanelWidth==11 && TextPanelHeight==11);
    assert(TextColour==12 && TextBackgroundColour==3 && TextColumn==7 && TextRow==8);
    TextColumn=9; TextRow=10; Menu_Memory_Variables(4);
    assert(TextColumn==9 && TextRow==10 && MenuPanelLayouts[4].column==9);
    for (uint16_t index=0;index<MenuPanelLayoutCount;++index) {
        MenuPanelLayout expected=MenuPanelLayouts[index];
        Menu_Memory_Variables(index);
        assert(CurrentMenuLayoutIndex==index);
        assert(TextPanelLeft==expected.left && TextPanelTop==expected.top &&
            TextPanelWidth==expected.width && TextPanelHeight==expected.height);
        assert(TextColour==expected.foreground && TextBackgroundColour==expected.background &&
            TextColumn==expected.column && TextRow==expected.row);
    }
    puts("Original panel context first activation, switching and same-ID restoration passed");
    return 0;
}
