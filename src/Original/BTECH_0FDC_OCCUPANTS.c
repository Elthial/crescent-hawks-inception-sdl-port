#include "game.h"

/* EXE-owned FAR string tables3EDB:4E32/4E5E resolved to host pointers.
 * Original map character/building names still come from external assets. */
uint8_t *NpcActivityReasonText[NpcActivityReasonCount]={
    (uint8_t *)"go on a training mission.  Hey, you see that fool?  He took a Locust out on mission two!",
    (uint8_t *)"hone some of my weapon skills in the combat class.",
    (uint8_t *)"buy me some of that Baker stock.  I'll make a fortune if they don't go broke.",
    (uint8_t *)"buy a new pistol.",(uint8_t *)"get me one of those flak vests.",
    (uint8_t *)"watch the men work, and see if I can't learn something.",
    (uint8_t *)"catch up on my sleep.",(uint8_t *)"boogie.",(uint8_t *)"",
    (uint8_t *)"buy a MedKit.",(uint8_t *)"watch some good 'Mech combat."
};
uint8_t *DestroyedCitadelNpcReplyText[DestroyedCitadelReplyCount]={
    (uint8_t *)"Go away, kid, you bother me.",
    (uint8_t *)"Wait your turn.  I'll be done soon.  Kids these days...",
    (uint8_t *)"Back offa me or I'll knock your block off.",
    (uint8_t *)"Excuse me, but I'm trying to do something here.  Please go away.",
    (uint8_t *)"I don't have time to chat about the weather.",
    (uint8_t *)"What's the big idea, bumpin' into me like that?",
    (uint8_t *)"Whatcha lookin' for, buried treasure?",
    (uint8_t *)"Know what you need?  A good, swift kick... hey!  Come back here!"
};

/* Original0FDC:17B9..19E0. Keep the native last-waiting-waypoint Rick
 * condition, including off-building waiting NPCs. No waiting NPC means
 * early return before that local can be read. */
void Talk_To_Building_Occupants(void)
{
    uint16_t building=0,waitingCount=0,lastWaitingBuilding=0;
    /* Initial value is unobservable: waitingCount>0 implies an assignment
     * in the scan; otherwise the routine returns before reading it. */
    uint8_t waiting[MapCharacterCount];
    while(building<MapCharacterCount && AlternativeBldByBuildingId[building]!=NpcActivityBuildingId) ++building;
    Draw_Top_Graphic_Sidebar();
    for(uint16_t npc=0;npc<MapCharacterCount;++npc) {
        waiting[npc]=FALSE;
        if(RoamingMapNpcs[npc].movementDelay!=0) {
            lastWaitingBuilding=(RoamingMapNpcs[npc].waypointPair>>NpcCurrentBuildingShift)&NpcBuildingRouteMask;
            if(lastWaitingBuilding==building) { waiting[npc]=TRUE; ++waitingCount; }
        }
    }
    if(waitingCount==0) {
        Display_Text_From_Memory((uint8_t *)"Nobody here seems interested in talking to you.");
        (void)Keyboard_Get_ASCII_Hex_Input(); return;
    }
    Display_Text_From_Memory((uint8_t *)"You look around and see:\r");
    for(uint16_t npc=0;npc<MapCharacterCount;++npc)
        if(waiting[npc]!=FALSE) { Display_Text_From_Memory(MapCharacterNames[npc]); Display_Text_4FA0_Value(); }
    Display_Text_From_Memory((uint8_t *)"Done");
    BuildingOccupantMenuOptionCount=waitingCount+1;
    uint16_t choice=Display_Menu_Choices_And_Check(BuildingOccupantMenuControl);
    if(choice==waitingCount) return;
    if(HoldRickAtlasForConversation!=FALSE && lastWaitingBuilding==NpcLoungeBuildingSlot && choice==0 && waiting[0]!=FALSE) {
        RickAtlasConversationTriggered=TRUE; RoamingMapNpcs[0].movementDelay=0; return;
    }
    RickAtlasConversationTriggered=FALSE; Draw_Top_Graphic_Sidebar();
    if(KuritaDestroyedCitadel!=FALSE) {
        Display_Text_From_Memory(DestroyedCitadelNpcReplyText[Rand_0x00_to_0xFF()&NpcBuildingRouteMask]);
        (void)Keyboard_Get_ASCII_Hex_Input(); return;
    }
    Display_Text_From_Memory((uint8_t *)"Hi, Jason, how are you?  I just stopped in here to ");
    Display_Text_From_Memory(NpcActivityReasonText[(int8_t)NpcActivityBuildingId]);
    Display_Text_From_Memory((uint8_t *)"  After I'm done here, I'm going over to the ");
    int16_t remainingChoice=(int16_t)choice;
    for(uint16_t npc=0;npc<MapCharacterCount;++npc)
        if(waiting[npc]!=FALSE && (remainingChoice=(int16_t)(remainingChoice-1))<0) {
            Display_Text_From_Memory(MapBuildingNames[RoamingMapNpcs[npc].waypointPair&NpcBuildingRouteMask]);
            Display_Text_From_Memory((uint8_t *)"."); (void)Keyboard_Get_ASCII_Hex_Input(); return;
        }
}
