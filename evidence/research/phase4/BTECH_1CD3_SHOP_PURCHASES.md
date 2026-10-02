# Sol: 1CD3 armour/weapon purchase and cash dialogue

Scope: C cases08,09,0A (raw actions09,0A,0B). Evidence: clean expanded-EXE
ASM `096F–0AE9`, initialized `3EDB` data dump, and local read-only BLD
inspection. No external assets copied into source, documentation or tests.

## Armour purchase: 09C7

The WORD menu selection at `305B:0178` supplies armour type unless D316 is
nonzero, in which case `305B:0168` supplies it. These are selected-row fields
in menu-control records0E and0D. Named header overlays preserve their offsets.
Native price lookup is DWORD `3EDB:4F26 + type*4`. Maintained C erroneously
multiplied the index of a presumed typed array by four again.

Only armour types1–5 are purchasable: the BLD's row0 leaves the purchase path.
The five real DWORD prices start at4F2A; the bytes at4F26 are the tail FAR
pointer of an earlier text table, **not a price for None**. The corrected
named table therefore begins at4F2A and uses `type-1`.
Initialized prices for types1–5 are 50,150,200,10000,1000 C-Bills.

D317 is cleared before affordability testing. Unsigned cash>=price includes
equality. Success debits DWORD cash with SUB/SBB, sets D317=1, refreshes its
display, and calls0FDC:13DE with WORD type and CBW-extended full durability
BYTE from4DDB+type. The purchase is already committed before distribution;
this parent does not refund it if the player later drops the item.
Native01BC only cleans four argument bytes and exits; C now expresses that
return directly rather than jumping to a distant stack-cleanup label.

ARMOR.BLD decoded0961 invokes raw09;0964 tests state0B=D317. Success goes
to0C5A; failure stays in script-controlled affordability dialogue.

## Weapon purchase: 0A4B

D318 is a weapon-category offset, not a Mech-Lube billing flag. WEAPON.BLD
0275 sets it to0 (melee/bows),0A44 to6 (firearms),0F2D to9 (heavy weapons),
using state index0C. Named constants describe these offsets rather than
pretending the comparison is a weapon ID or a record stride.

The corresponding selected-row WORDs are0198,01A8,01B8 (menus10,11,12).
Rows1–6 in the first menu produce weapon IDs1–6; rows1–3 in the firearm menu
produce7–9; the shipped heavy menu has one purchasable row, producing ID10.
The category offset is added and DEC converts the one-based ID to price index.
DWORD prices are at `4F44 + index*4`, ten entries for IDs1–10. The old
`ArmourTypePurchaseCost[6]` BYTE declaration was the wrong table and width.

Payment follows the same unsigned DWORD comparison/debit/D317 protocol as
armour.0FDC:15E6 receives index+1, the original weapon ID.0181 only discards
that WORD argument and exits. WEAPON.BLD13AA invokes raw0B and13AD tests
state0B to choose success or failure dialogue; the parent does not invent
an affordability message or item recipient.

## Shared cash dialogue: 096F

Raw action0A formats cash, sets green text, displays it, then emits FAR
3EDB:4C18. That string is bytes06,0F,'.',NUL: a positioning control followed
by a dot. The brand-new-armour repair message begins separately at4C1C.
The maintained literal had merged those distinct strings. This pass restores
the explicit original pointer, preserving the text control bytes.

## Verification and boundaries

`Verify-ShopPurchaseTranscriptions.ps1` checks the biased armour price address,
all shipped weapon selection/category mappings, unsigned DWORD affordability
boundaries including exact balances, borrow debit, result flag and distribution
gating. Prices in arithmetic tests are synthetic, not asset fixtures. Existing
training, lifecycle, stock, personnel and menu checks are rerun. None executes
the annotated C or proves gameplay equivalence.

Results:113 shop assertions,65887 lifecycle,5373 training,2816 personnel,
2058 stock and3175 menu assertions pass, along with whitespace validation.

No bounds guard, refund or invalid-category policy was added. Invalid scripted
indices still require an independent policy/evidence review before a C# port.
Errors fixed here are transcription errors, not original-game bug claims.
Next bounded chunk: Tech/Medical training purchases/results, then armour repair
and medical services. Their existing expressions remain unchanged in this pass.
