# Sol: 1CD3 character-name dialogue and laser equipment grant

Scope: cases23–28 (raw actions24–29), shared text/crew handoff variables and
labels. Evidence: clean expanded-EXE ASM139A–1514 and original3EDB prompt/name
tables. Local JAIL/WEAPON2 script inspection confirms the action uses. No
external asset fixture copied. Research pseudo-C, not a compiled program.

## Name display versus crew slot

Raw24 uses signed traitor slotD331*17 to read its character Name BYTE, then
CBW-extends the Name ID for the eleven-entry FAR name table at3EDB:01CA.
Raw26 similarly displays signed recruit Name IDD456. It does not recruit
anyone or interpret D456 as a party slot. Name IDs and character slots must
remain distinct despite shared scalar storage in the pseudo-C.

`DisplayCharacterName_13CE` and `DisplaySelectedDialogueText_01B7` replace
address-only labels, keeping the old names in Sol comments. The shared FAR
pointer is now `DialogueText_3405`, preserving the original3405 suffix.
Fixed-dialogue pointer TextToDisplayPtr also uses FAR-byte-pointer type;
the previous Reko Eq_21/WORD declarations were not correct text contracts.
Duplicate byte/WORD CharacterNameId declarations are consolidated into the
signed WORD produced by CBW. This does not invent bounds checks for bad IDs.

Raw27 loads WORD1 and calls1467:0002 via the shared crew path. This is
character slot1(Rex), not name-table entry1. The label is now
`AssignSelectedCharacterCrew_0E92`, with its original name recorded; the
roster rebuild still calls that same path for slot0(Jason).

## Death and jailbreak handoffs

Raw25 clears alive WORD014A and writes NameFF atC614. It neither zeroes
Jason's Health nor immediately writes D334. Raw28 calls the already-reviewed
11B8:152F jailbreak/Stinger routine, then sets D334 only if014A is zero.
An existing death latch is not cleared when the mission returns alive.
These instruction-level distinctions are now explicit; no new death policy.

## Raw29 laser grant and health cap

WEAPON2.BLD05D7 invokes raw29. Nonzero companion countD31A selects the
distribution path.1423 reloads its signed BYTE value and loops inclusively
0..count: normally companion count+1 grants. First grant is Laser Rifle13;
later grants subtract one random bit, yielding Laser Rifle13 or Laser Gun12.
Each goes through0FDC:15E6's recipient/exchange UI. The loop ordinal does
not itself guarantee a particular character receives that weapon.
Renamed grant locals retain2902/2910 suffixes.

Zero companions instead prompts whether Jason should replace his current
weapon with the laser rifle. Native weapon text starts3EDB:2ED8+signed ID*17;
the corrected typed lookup selects WeaponStats[id].Name, not a whole record.
Prompt strings now use named `seg3EDB` inline-text views at4D8A and4DA4,
preserving the original words without hard-coded FAR address expressions.

14AB then processes each living slot0..7. Independently choose6 or7, multiply
signed Body by that factor, and compare signed current Health. Only Health
**above** the result is replaced by its low BYTE. This is an upper health cap,
not healing: positive ordinary Body states end at no more than60%/70% of full
Body*10, while existing lower health remains unchanged. The former
RandomHealingAmount/AmountHealed/address-of names falsely suggested healing.
No recovery timer reset or random roll for dead slots is present.

No normalization of high-bit counts, new recipient selection, healing award,
alive/death reset, cap sanitization or bounds guard was invented. Signed BYTE
and low-BYTE-store semantics are retained for unusual inputs too.

## Checks / next boundary

`Verify-NameAndEquipmentDialogTranscriptions.ps1` covers all encoded Name/count
BYTEs, inclusive grant cardinality/weapon IDs, all encoded Body/Health BYTE
pairs and both cap factors, plus the death latch. These are synthetic models,
not actual dialogue or gameplay traces. Existing arena/roster/healing regressions
are rerun. Ordinary gameplay interpretation of the scenario cap is documented
without prematurely calling the deliberate-looking reduction an original bug.

Verification passed: 131,590 name/equipment checks, 18,826 arena-return checks,
18,692 friendly-roster checks and 8,568 healing checks. `git diff --check` also
passed. These counts measure synthetic assertions, not native gameplay coverage.

Next: raw2A/2B position-table staging, raw2D ending asset workflow, raw2F flag
adjustment and final1CD3 consistency/helper declaration review.
