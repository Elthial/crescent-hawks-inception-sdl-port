# Sol: 1CD3 position staging and ending assets

Sol: Current checkpoint (2026-09-17): the later1CD3 consistency and1F3D reviews
resolve the palette FAR helper/caller contract. Its old follow-up below is
historical; ending gameplay still needs live comparison.

Evidence: local original-version `BTECH_1CD3.asm`, 1513–174D. Raw BLD
actions are one higher than the C switch cases. This is an annotation and
transcription audit, not a runnable C implementation or gameplay validation.

## Raw2A: eight NPCs at the party position

151A uses SI=slot*26 and DI=slot*2. Each of eight slots copies the party
world WORD X/Y (246C:A44B/A44D) into both 3092:4024/4056+DI and
3092:D390/D392+SI. The former are indices16–23 in the full WORD combatant
position tables at4004/4036, not Jason or byte-valued coordinate arrays.
The existing `CombatantPackedX/Y` names are reused; these stored values are
world-position WORDs, not new file compression.

D399+SI receives the slot ordinal as a movement-delay BYTE. D398+SI receives
77h, start/destination waypoint7/7 according to the already-reviewed roaming
NPC consumer at0800:24C2. Destination WORDs D394/D396 are not written here.
After the loop, INC WORD A44D advances the party world Y, including native
FFFF->0000 wrap. It does not spread the eight NPCs over adjacent positions.

The corrected C uses the existing ten-byte `RoamingMapNpcRecordView` with a
26-byte stride; no claims about the remaining sixteen bytes were added.
Broken byte-array expressions, double WORD strides and the active/visibility
interpretation of `Infantry_OnMap` were removed from these branches only.

## Raw2B: NPC0 starts at waypoint7, travels toward waypoint0

157E copies WORD4572/45A4 to combatant16 and NPC0 current X/Y. These are
index7 of the existing WORD waypoint/interactable arrays4564/4596. It writes
D399=FF (movement delay255), D398=70 (start7/destination0), then copies
waypoint0 X/Y into D394/D396. It does not move Jason or set fog visibility;
NPC slots1–7 and the party position remain untouched. Story-specific purpose
is not inferred from coordinates alone.

## Raw2D: full-screen ending followed by tile restoration

15DD–174D establishes this order:

1. Original adapter0-only conversion table2FE8:01F0 and display-mode change
   (omitted from the maintained EGA-only body).
2. Clear graphics compatibility WORD3EDB:4FBC, select disk2, load external
   ENDMECH.CMP into3092:4614, decode into246C:244B.
3. Adapter2 only: stage packed image into EGA off-screen segmentA800.
4. Apply ending palette FAR3058:0000 (EGA); historical adapter3 uses3058:0010.
   Draw at0,0 with width40 EGA byte-columns and height200 pixel rows.
5. Set compatibility WORD4FBC=1; original adapter0 conversion2FE8:01B0.
   Load external STARLEAG.ICN compressed into246C:244B, decode into3092:4614.
   This reverses the ending-image buffer direction and restores reusable tiles;
   it is not a second full-screen image draw or a newly added tile upload.
6. Retrace/input bridge, ASCII keyboard call, black screen; historical adapter0
   mode restoration. Apply default palette FAR2FE8:0000 (adapter3:0010),
   redraw health/C-Bills sidebar and top sidebar.

No external asset was copied into the repository. Removed CGA/Tandy bodies
remain removed. New `seg3058` palette views preserve native addresses using
the requested named segment-field style; like the other scratchpad views,
they are not a sequential compiled memory layout.

### Explicit remaining pointer-contract issue

Follow-up: resolved by the [final consistency audit](BTECH_1CD3_FINAL_CONSISTENCY.md).
The following paragraph records the state at this block's original review.

`Set_Palette_registers` currently accepts an unsigned-byte offset and contains
the invalid Reko dereference `*(Offset+i)`. It cannot represent the native
FAR pointer. The ending calls now retain the correct named palette source
instead of silently treating3058:0000 and2FE8:0000 as the same zero offset.
A Sol TODO queues an ASM-backed helper and all-caller FAR-contract review.
This block does not pretend that the broken helper is fixed or compilable.
Likewise the reusable buffer fields retain inherited misleading “Ptr” names:
ASM passes their addresses as data buffers, not freshly dereferenced pointers.

## Checks and next boundary

`Verify-PositionStagingTranscriptions.ps1` models native little-endian stores,
seven boundary values per axis, subgroup WORD indexing, untouched destinations,
delay/waypoint bytes, party Y wrap and NPC0-only overwrite. Ending source-order
checks guard the annotated workflow, not actual image rendering or input timing.

Passed: 1,385 staging/ending assertions, 131,590 name/equipment regression
assertions, and `git diff --check`.

Next: raw2F signed training-flag adjustment and final1CD3 consistency/helper
review, beginning with the palette FAR contract and text/input wrappers.
