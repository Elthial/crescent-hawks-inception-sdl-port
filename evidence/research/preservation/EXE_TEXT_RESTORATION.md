# Sol: restoring readable EXE text

Baseline `c40cc89`. The ASM annotation pass unnecessarily replaced known text
with native memory expressions in several places. Restore exact EXE-owned
literals and retain the original addresses as comments. This is allowed source
data from the executable; no external art, levels, sounds or private EXE are
added to version control.

## Traced edits and restored locations

Reviewed the repository-wide Git history for removed literal text calls and text
assignments, then searched current text consumers for address-only replacements.
Many historical removals were ordinary rewrites that still retain readable text
today (salvage, combat menus, shop prompts, security codes). Do not revert those
or reintroduce inaccurate earlier spelling/spacing. Restore from original bytes.

| Current file / routine | Locations | Finding |
| --- | --- | --- |
| BTECH_1431.c / Heal_Characters | 9: 252D,2538,2543,2553,256F,2587,2591,25A4,25CF | Lost in4f51106; exact fragments restored, not guessed prose;256F renamed HealingAssistWoundedText |
| BTECH_1CD3.c / Citadel_Building_Dialogs | 11 repair prose locations:4C1C,4C60,4C75,4C79,4C81,4CAE,4CD3,4CEE,4D14,4D4F,4D67 | Lost in78c3134; restore exact armor spelling, double spaces, punctuation and carriage returns |
| BTECH_1CD3.c / Citadel_Building_Dialogs | 2 Laser Rifle fragments:4D8A,4DA4 | Literal readability replaced with named memory fields in684ddb5; restore actual sentences |
| BTECH_1CD3.c / Citadel_Building_Dialogs | 4C18 | Additional opaque pointer; restore color-control bytes06,0F and period, not repair prose |
| BTECH_1CD3.c / Display_Text_4FA0_Value | 4FA0 | Restore literal CR with address; helper naming deferred to its own cleanup |
| BTECH_0D27.c / Setup_Game | 5:0CD6,0D92,0DD0,0E2C,0E78 | Also make newly reconstructed startup/drive strings readable; no need to chase raw addresses |
| BTECH_11B8.c / Mechlube_Buy_Ammo | 10:1E99,1EA8,1EAE,1ED1,1EFF,1F19,1F49,1F94,1FC9,2008 | Subsequent ammo cleanup restores exact visible strings and missing CR/color/quantity-question bytes |
| BTECH_0DAB.c / Salvage_Mechs_Dialog | 0FFF | Whole-Mech cleanup restores inspection ellipsis and carriage return |

All addresses above are in native segment3EDB: 40 locations total after salvage
cleanup (39 after ammunition; 29 at the initial text-restoration checkpoint). The startup
additions and control/line-break literals are readability improvements too, not
claims that each was previously a user-written sentence.

## Deliberate remaining memory consumers

Keep computed/runtime text in memory: stock-dialogue offset selection, indexed
character/weapon names, mutable save-game filename, dynamic number buffers and
other tables. Their content depends on state or an index; do not freeze them to
one string. DiskDriveNameText remains a named memory view with known `drive A:`
content; it was a reconstructed drive binding rather than deleted inline prose.

0800 yes/no selected-line offsets03EE/03FE have embedded renderer control bytes,
including zero-valued color operands. A NUL scanner cannot safely extract their
complete text. Existing `Yes  No` explanation and outstanding FAR-pointer TODO
remain; do not silently treat a color operand as a terminator. Reconstruct the
complete control stream when that prompt receives its bounded cleanup.

## Verification and audit boundaries

`scripts/Verify-RestoredExeText.ps1` checks all40 literal locations byte-for-byte,
including the terminating NUL, against private expanded EXE SHA256
F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE.
It reads local private bytes and writes nothing. C escape sequences preserve
CR/LF and control bytes; no external asset extraction or upload.

Original text-call order, branches, counters and side effects are unchanged.
Literal host pointers replace immutable native string pointers under the existing
preservation text-consumer abstraction; exact pointer identity/segment values are
not claimed. Do not use this transformation for text that is mutated or inspected
by address. Fresh hash updates for non-healing methods retain their old status and
record this text-only scope; in particular the dispatcher is still mismatched.
The healing nibble transformation is rechecked against native0282–02A7 for all
BYTE values and passes actual-source C17 tests. Original bugs stay preserved.

Future rule: retain readable EXE strings with address provenance. When a native
control string cannot yet be represented, keep its readable explanation and
explicit uncertainty instead of replacing it with an unexplained address.
