# Sol: Mechlub ammunition — in-place preservation cleanup

Baseline `bb3b860`. Clean the existing `Mechlube_Buy_Ammo` at11B8:1762 through
its1A4E epilogue/complete FAR return in DIS. No duplicated game method or UI
rewrite; repair, upgrade and salvage bodies remain unchanged. Original address
and public method name remain; add its explicit void prototype.

## Workflow and BattleTech terminology

Choose the Mech, collect up to ten weapon-valued critical bytes in physical
33h–55h record order, then offer ammunition for each supported functional weapon.
Keep destroyed flags in the collected list. Laser/damaged/non-ammunition entries
still occupy weapon ordinals: do not compress the list to ammunition weapons,
or CurrentAmmo/MaxAmmo would refer to the wrong weapon.

The shop supports functional Machine Guns and the component1Ah–20h missile
range only. It excludes autocannons14h–17h despite their ammunition requirement;
do not add support during preservation cleanup. This is shop-specific behaviour,
not a claim that BattleTech autocannons are energy weapons.

`Mech_WeaponOrdinal_Count` replaces ten-as-a-literal/legacy slot naming;
`MechComponentToWeaponRecordBias` explains component ID minus1. Machine-gun
ammunition costs `MechAmmoMachineGunRoundCostCBills` (2) per round. Missile
prices come from the existing component-indexed seven-BYTE EXE table. This
routine has no Tech skill roll and no tabletop repair/check mechanic to invent.

Local names now describe the selected Mech, critical components, weapon slots,
missing rounds, requested rounds and remaining reload quantity, without suffixes.
Indexed weapon names remain indexed text; all ten fixed prompts/fragments are
verified EXE literals with address comments, including original carriage returns,
double spaces, punctuation and06/0F white-text control bytes. The quantity question
at1F19 is restored rather than replaced with just a price suffix.

## Incorporated ASM corrections

- Selected-Mech and raw critical pointers retain FAR3092.
- Missing rounds is signed WORD MaxAmmo minus CurrentAmmo. Display passes its
  low WORD; native requested-quantity capping sign-extends it to signed DWORD.
- The decimal parser returns an unsigned DWORD bit pattern, but native1906
  interprets that pattern as signed for the cap. Represent that signed value
  explicitly without implementation-defined unsigned32-to-signed32 conversion:
  nonnegative values cast directly; high-bit values use -1 minus their complement.
  INT32_MAX/UINT32_MAX explain the bit boundary without another magic hex value.
- Requested zero skips the purchase/final balance refresh. Otherwise compare the
  signed request to signed missing rounds, warn/cap if greater, or retain the low
  WORD request. Remaining quantity is uint16_t so native decrement/wrap is defined.
- Missile price uses signed BYTE load/CBW. Affordability and debit use the price
  sign-extended to DWORD and then interpreted unsigned, matching native CWD and
  JA/SUB/SBB. Do not replace corrupt negative prices with zero or reject them.
- Buy one round at a time: increment the ammo BYTE, debit unsigned DWORD balance,
  decrement remaining WORD, then display balance. Preserve the final extra balance
  display after a nonzero requested quantity and all warning/key ordering.

Re-read the entire clean ASM routine and DIS return. Existing valid collected-byte
eligibility predicates are equivalent to native signed raw-byte comparisons;
non-collected raw components never enter that predicate. No new original-game bug
fix, clamp, price validation or guard is introduced. Above-max ammunition can warn
and produce a wrapped large remaining quantity; high-bit decimal requests can avoid
the ordinary positive cap. Preserve those effects rather than impose sane inputs.

## Contracts and verification

Equivalence assumes valid/stable Mech selection across UI calls, native Mech/table
views and existing helper contracts. Native reloads Mech_Selected, while the current
method caches its selected record. The shared unsigned-long balance/parser interface
is native/Windows32-bit; test adapter uses uint32_t. Full header portability, shared
callee corrections, malformed pointer-index wrap and gameplay comparison remain
separate work. The local match is not whole-program certification.

`scripts/Verify-PreservationAmmo.ps1` compiles the ACTUAL method with MSVC C17
`/W4 /WX`, constants and Mech layout generated from BTECH.h, and the reused narrow
repair UI/state adapter. 789,469 checks pass, including instrumentation rather than
that many gameplay scenarios. Tests cover all256 raw critical values and signed
price BYTEs; all65,536 MaxAmmo/CurrentAmmo BYTE pairs; critical-order ammo ordinals;
ten-entry limit/last critical byte; damaged/full/no-functional weapons; zero, capped,
high-bit and low-WORD-wrapped requests; partial affordability and repeated balance
updates. `Verify-RestoredExeText.ps1` now verifies39 restored text locations, including
these ten prompts. Existing repair and static11B8 checks remain required.

Next reviewable block: salvage armour/internal structure and salvageable Mechs.
