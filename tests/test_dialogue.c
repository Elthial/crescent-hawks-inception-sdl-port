#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test adapter for still-unconverted07CB only. Original03F5 and the native
 * strcat/strcpy entries execute unchanged; capture consumption/cursor effects. */
typedef struct Flush { uint8_t text[100]; uint16_t advance,column,foreground,background; } Flush;
static Flush flushes[1024];
static unsigned flushCount,checks;
static void check(int condition)
{
    ++checks;
    if (!condition) { fprintf(stderr,"Dialogue check %u failed\n",checks); exit(1); }
}
void Display_Text_In_TextBox(uint8_t *text,uint16_t advanceLine)
{
    check(flushCount < sizeof flushes/sizeof flushes[0]);
    Flush *flush = &flushes[flushCount++];
    size_t length = strlen((char *)text);
    check(length < sizeof flush->text);
    memcpy(flush->text,text,length+1);
    flush->advance = advanceLine;
    flush->column = TextColumn;
    flush->foreground = TextColour;
    flush->background = TextBackgroundColour;
    if (!advanceLine) TextColumn = (uint16_t)(TextColumn+length);
    if (advanceLine || (int16_t)TextColumn >= (int16_t)TextPanelWidth) {
        TextColumn = 0;
        ++TextRow;
    }
    text[0] = 0;
}
static void reset(uint16_t width,uint16_t column)
{
    flushCount = 0; TextPanelWidth = width; TextColumn = column;
    TextRow = 0; TextColour = 15; TextBackgroundColour = 0;
}
static void expect(unsigned index,const char *text,uint16_t advance)
{
    check(index < flushCount);
    check(!strcmp((char *)flushes[index].text,text));
    check(flushes[index].advance == advance);
}
int main(void)
{
    reset(5,0); Display_Text_From_Memory((uint8_t *)""); check(flushCount == 0);
    reset(5,0); Display_Text_From_Memory((uint8_t *)"aa bb cc");
    check(flushCount == 2); expect(0,"aa bb",1); expect(1,"cc",0);
    check(TextColumn == 2 && TextRow == 1);
    reset(5,0); Display_Text_From_Memory((uint8_t *)"abcdefghi");
    check(flushCount == 2); expect(0,"abcde",1); expect(1,"fghi",0);
    reset(5,0); Display_Text_From_Memory((uint8_t *)"abc def   ghi");
    check(flushCount == 3); expect(0,"abc ",1); expect(1,"def ",1); expect(2,"ghi",0);
    reset(5,0); Display_Text_From_Memory((uint8_t *)"a\rb\r");
    check(flushCount == 2); expect(0,"a",1); expect(1,"b",1);
    reset(5,0); Display_Text_From_Memory((uint8_t *)"a\xC2ignored");
    check(flushCount == 1); expect(0,"aB",0);
    uint8_t colours[] = {'A',2,254,'B',6,128,'C',0};
    reset(8,0); Display_Text_From_Memory(colours);
    check(flushCount == 3); expect(0,"A",0); expect(1,"B",0); expect(2,"C",0);
    check(flushes[0].background == 0 && flushes[1].background == 65534);
    check(flushes[1].foreground == 15 && flushes[2].foreground == 65408);
    check(TextColumn == 3 && TextBackgroundColour == 65534 && TextColour == 65408);
    uint8_t tab[] = {'A',9,3,'B',0};
    reset(5,0); Display_Text_From_Memory(tab);
    check(flushCount == 2); expect(0,"A",0); expect(1,"B",0);
    check(flushes[1].column == 3 && TextColumn == 4);
    tab[2] = 5;
    reset(5,0); Display_Text_From_Memory(tab);
    check(flushCount == 2 && flushes[1].column == 0);
    uint8_t padding[] = {'A',19,3,'Z',0};
    reset(5,0); Display_Text_From_Memory(padding);
    check(flushCount == 1); expect(0,"A   Z",0);
    /* Native padding uses the drawn cursor, not pending word/line width. */
    padding[2] = 5;
    reset(5,2); Display_Text_From_Memory(padding);
    check(flushCount == 2); expect(0,"A   ",1); expect(1,"Z",0);
    for (uint16_t width=1;width<=39;++width) {
        uint8_t text[79]; memset(text,'X',78); text[78]=0;
        reset(width,0); Display_Text_From_Memory(text);
        unsigned consumed=0;
        for (unsigned i=0;i<flushCount;++i) {
            size_t length=strlen((char *)flushes[i].text);
            check(length <= width);
            for (size_t j=0;j<length;++j) check(flushes[i].text[j] == 'X');
            consumed += (unsigned)length;
        }
        check(consumed == 78);
    }
    printf("Original dialogue: %u checks passed\n",checks);
    return 0;
}
