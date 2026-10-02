#include "game.h"
#include "dos.h"

/* Original DS3092:32C6/40B4: flat native rows, not host pointers.
 * Keep the next-row sentinel probe inside the full native allocation. */
uint16_t MovementPreviewEndpointColumn,MovementPreviewEndpointRow; /*3778/377A*/
static const uint8_t movementPreviewGlyphs[12]={8,1,2,0,7,0,3,0,6,5,4,0}; /*3EDB:3B06*/

/* Original183B:1774..193A, complete ASM checked. Mark selected actor,
 * optionally recenter/rebuild, and draw FRIENDLY movement orders only.
 * Reused BP locals and SS:glyph become ordinary C locals. No unit moves here. */
void Combat_Render_Movement_Preview(uint16_t combatantId,uint16_t showPlanningPreview)
{
    uint16_t packedX=CombatantPackedX[combatantId],packedY=CombatantPackedY[combatantId];
    if(showPlanningPreview) {
        Move_Map_View_To_Packed_Position(packedX,packedY);
        PosXY_OffsetGrid(CrescentHawkMapPositionX,CrescentHawkMapPositionY);
        Copy_Data_To_GraphicsMemory();
        Draw_Menu_MultiSelect();
    }
    int16_t markerSize=1,markerOffsetX=0,markerOffsetY=0;
    if((int16_t)combatantId<Friendly_Infantry_Combatant_Range_First ||
       ((int16_t)combatantId>=Enemy_All_CombatantId_Range_First &&
        (int16_t)combatantId<Enemy_Infantry_CombatantId_Range_First)) {
        markerSize=3; markerOffsetX=-1; markerOffsetY=-2; /*Mech footprint in cells*/
    }
    EGA_DrawBox_Wrapper();
    /* Native B782=1 only changes discarded adapter3 highlight colours;
     * the retained EGA branch does not read it. */
    for(int16_t row=0;row<markerSize;++row)
        (void)DrawCall_Combat_Menu((uint16_t)(CombatPreviewOriginColumn+markerOffsetX),
            (uint16_t)(CombatPreviewOriginRow+markerOffsetY+row),(uint16_t)markerSize,EGA_BrightWhite);
    if((int16_t)combatantId<Enemy_All_CombatantId_Range_First) {
        if((int16_t)combatantId<Friendly_Infantry_Combatant_Range_First)
            Combat_Mech_Movement(combatantId,(uint16_t)(int16_t)(int8_t)
                CombatMovementOrders[combatantId*CombatMovementOrderBytes]);
        else Combat_Infantry_Movement(combatantId);
        Combat_Calculate_Movement(combatantId);
        uint8_t glyph[2]={0,0};
        uint16_t column=CombatPreviewOriginColumn,row=CombatPreviewOriginRow;
        uint16_t stepByte=0,planStart=(uint16_t)(combatantId*CombatMovementPlanBytesPerUnit);
        /* Native1903 probes sentinel BEFORE the189A byte-count guard. */
        while(CombatMovementPlanBytes[planStart+stepByte]!=CombatMovementPlanEnd) {
            if(stepByte>=CombatMovementPlanBytesPerUnit) break;
            int16_t deltaX=(int8_t)CombatMovementPlanBytes[planStart+stepByte++];
            int16_t deltaY=(int8_t)CombatMovementPlanBytes[planStart+stepByte++];
            column=(uint16_t)(column+deltaX); row=(uint16_t)(row+deltaY);
            glyph[0]=movementPreviewGlyphs[(deltaX+1)+(deltaY+1)*CombatPreviewGlyphRowStride];
            Draw_EGA_Text_To_Screen(glyph,column,row,EGA_BrightWhite,CombatPreviewBackgroundBlack);
        }
        MovementPreviewEndpointColumn=column; MovementPreviewEndpointRow=row;
    }
}
