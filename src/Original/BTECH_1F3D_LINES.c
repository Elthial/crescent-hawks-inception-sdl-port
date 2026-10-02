#include "game.h"

/* Sol: Original1F3D:01FB..0258 retained EGA path. Historical name is a
 * rectangle fill: inclusive pixel endpoints, signed row test, no clipping. */
void Draw_Horizontal_EGA_Line(uint16_t left,uint16_t top,uint16_t right,uint16_t bottom,uint16_t colour)
{
    while ((int16_t)bottom >= (int16_t)top) {
        uint16_t row=top++;
        Draw_EGA_Horizontal_Span(row,left,right,colour);
    }
}

/* Sol: Original1F3D:03EB..049C. Leading edge pixels, full byte groups,
 * inclusive tail. End alignment happens even when the group count is zero.
 * The actual EXE EGA end mask is01F8, not an invented unrestrictedFFF8. */
void Draw_EGA_Horizontal_Span(uint16_t row,uint16_t left,uint16_t right,uint16_t colour)
{
    uint16_t alignmentMask=EgaSpanAlignmentMask;
    while ((left & alignmentMask) != 0) {
        if ((int16_t)left > (int16_t)right) break;
        uint16_t column=left++;
        EGA_Draw_Vertical_Pixel_Run(column,row,row,colour);
    }
    if ((int16_t)left < (int16_t)right) {
        uint16_t groups=(uint16_t)(right-left);
        /* Native SAR: explicitly extend the sign rather than host signed >>. */
        uint8_t shift=(uint8_t)EgaSpanGroupShift;
        while (shift--) groups=(uint16_t)((groups>>1)|(groups&0x8000));
        if (groups != 0) EGA_Draw_Aligned_Span(left,row,groups,colour);
        left=right & EgaSpanEndAlignmentMask;
    }
    while ((int16_t)left <= (int16_t)right) {
        uint16_t column=left++;
        EGA_Draw_Vertical_Pixel_Run(column,row,row,colour);
    }
}

/* Sol: Original1F3D:031C..03EA. Sort/clamp EACH endpoint, then draw only
 * horizontal/vertical lines. Offscreen lines can collapse to a border point;
 * a single point takes the vertical branch. No general diagonal renderer. */
void Draw_Clipped_Axis_Aligned_EGA_Line(uint16_t left,uint16_t top,uint16_t right,uint16_t bottom,uint16_t colour)
{
    uint16_t swap;
    if ((int16_t)right < (int16_t)left) { swap=left; left=right; right=swap; }
    if ((int16_t)bottom < (int16_t)top) { swap=top; top=bottom; bottom=swap; }
    if ((int16_t)left < 0) left=0;
    if ((int16_t)right < 0) right=0;
    if ((int16_t)top < 0) top=0;
    if ((int16_t)bottom < 0) bottom=0;
    if (left > EgaScreenWidth-1) left=EgaScreenWidth-1;
    if (right > EgaScreenWidth-1) right=EgaScreenWidth-1;
    if (top > EgaScreenHeight-1) top=EgaScreenHeight-1;
    if (bottom > EgaScreenHeight-1) bottom=EgaScreenHeight-1;
    if (left == right) EGA_Draw_Vertical_Pixel_Run(left,top,bottom,colour);
    else if (top == bottom) Draw_EGA_Horizontal_Span(top,left,right,colour);
}
