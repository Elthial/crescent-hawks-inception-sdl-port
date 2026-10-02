# Sol: 1CD3 skill/personnel CHECK ASM audit

Scope: the remaining three marked locations, skill transcript display (case
0x03) and living-party selection (case 0x0C). Evidence: local expanded-EXE
`Btech-Reko-expanded/BTECH.reko/BTECH_1CD3.asm`, not UnBattletech.

## Skill transcript

`023B–026B` iterates three skill BYTEs at `3092:C618–C61A`: Jason's
bows/blade, pistol and rifle fields. `IMUL BYTE` multiplies each signed skill
by 125; `ADD AX,4Bh` adds 75. The displayed score is therefore
`75 + 125 * skillLevel`, passed as a WORD value to the decimal formatter.
125 is a score multiplier here, despite also being the mech record stride.
The old C incorrectly turned the arithmetic into a skill-array address.
Character records are 17 bytes, and the header has named skill fields rather
than a real `Skill[]` array. The explicit DOS memory view preserves the
contiguous three-byte lookup without implying a new compiled structure.

## Personnel selection

`0B36` initializes eight BYTEs at `SS:BP-08..BP-01`. `0B74–0BBF` visits
character slots 0 through 7, skipping records with `Name == 0xFF`. Each
displayed name adds its actual character slot to this stack map (`0BAA`).
`0BC2–0BD9` converts the menu's row result through the map, then stores the
resulting character slot at `3092:D31A`. It does not store the row directly.
For example, living slots 0, 3 and 7 produce menu rows 0, 1 and 2, mapping
back to character slots 0, 3 and 7. The old `fp - 0x0A` expression was a
Reko stack reconstruction error, not a file pointer.

The preceding action (case 0x0D, called with raw action 0x0E) counts living
companions in slots 1–7 and writes that count to D31A. Selection sets menu
metadata to this signed BYTE count plus one, assuming Jason is alive, then
reuses D31A for the chosen slot. `SelectedPartyMemberSlot_D31A` is an
overlapping scratchpad name for this later meaning, not separate storage.
The original has no result bounds check here; none was invented in this pass.
The Jason-alive assumption needs a gameplay trace before classification as
an original-game fault.

## Verification and remaining scope

`scripts/Verify-PersonnelDialogTranscriptions.ps1` covers all 256 encoded
skill values and all 256 living/dead party masks, checking score arithmetic,
row-to-slot mapping, unused zero bytes and the Jason-alive metadata assumption.
These are synthetic instruction-model checks, not compiled-C or gameplay tests.

All remaining `CHECK ASM` markers in this file have now been checked and
resolved. This does **not** certify the entire segment: unmarked training
skill-pointer expressions in case 0x04 and the larger switch/goto workflow
still need separate bounded reviews. The corrected errors above are C
transcription faults, not evidence of original-game bugs. No assets changed.

Follow-up: the unmarked case-0x04 expressions are now reviewed in
[the training audit](BTECH_1CD3_TRAINING.md). The broader workflow remains pending.
