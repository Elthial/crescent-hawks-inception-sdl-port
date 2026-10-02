# Sol: systematic BTECH_1543 C-to-ASM comparison

Checked2026-09-17 against baseline `4481743`: all five retained methods.
Five mismatched. This checking pass changes C comments only; corrections await
the fixing pass. No new original-game bug is asserted.

## Evidence and boundaries

Private `BTech-Reko-expanded/BTECH.reko/BTECH_1543.asm` SHA256:
`E5AA51DAFF619E5CE9CAA60204FCA002C5B7F9B91EBB7DD3341941382E5E49AE`.
Corresponding `.dis` SHA256:
`328665E8AF4CE723CEA18C1446486EF4E85D6A7ACC59A2D55A9AA414681B07C9`.

Read the whole segment and the numeric routine's complete return in `.dis`.
Read native3EDB literal bytes and selector words5544..5576. Compared arguments,
returns, meaningful locals, accesses, branches, loops, calls and side effects.
Compiler stack probes/frame/saved-register machinery is abstracted; shared
callees retain their existing contracts and are not certified by this audit.
FAR views must retain their selector and16-bit offset arithmetic. Host struct
layout, promoted arithmetic and managed bounds are not native memory semantics.
Unsigned long is the native32-bit DX:AX result, not a64-bit host C long.

## Whole-method ledger

| Method / range | Result | Local workflow checked |
| --- | --- | --- |
| Combat_Weapon_UI_0004 /0004–07CA | Mismatch | Action reset; mech critical collection; shutdown/sensors; menu geometry, colours, ammo and targets; unavailable/override prompts; infantry branch; selection loop and panel cleanup |
| Combat_Select_Weapon_Target_UI /07CB–0A34 | Mismatch | Persistent enemy cursor, inactive skipping, menu/panel setup, identity/weapon display, range, target/cancel storage and cursor wrap |
| Combat_Mech_Eject_0A35 /0A35–0C71 | Mismatch | Active/wreck state;24-by12 exact target cleanup; record translation/name initial; arena companion/patch; flag clears; message suppression; pilot/rider placement; delay and colour restore |
| Starport_MapPatch_SaveApply_Or_Restore_0C72 /0C72–0CDD | Mismatch | Both38-byte save/nonzero-override and restore loops, selector/address and return |
| Prompt_For_Unsigned_Decimal_0CDE /0CDE–0EE5 | Mismatch | Buffer/cursor/colour setup, row clearing, digit/Backspace/Escape/Enter handling, decimal multiply/add/subtract and full DX:AX return |

## Common pointer discrepancy

Native DS is the3EDB text/table segment. Selector loads bind game state to3092
and map state to246C; locally assembled pointers also explicitly carry3092.
Current explicit near `Mech *`, raw byte/target/text pointers and
`StarportMapRegion` cannot retain those segments. Examples:3092:C724 mech,
3092:3800 targets,3092:0012 scratch text and246C:1C1D map patch. This is not
just a host struct-spacing issue. Model proper FAR/native-address views during
fixing, or explicitly replace them with port memory-object bindings.
The local weapon-list strlen passes FAR SS:BP-20 natively; a near stack pointer
converted using DS is not that address. Literal pointers require the existing
platform text binding, not an assumption that every global shares a segment.

## Weapon panel0004

Native scans raw mech offsets33..55, collects up to twelve components whose
masked IDs are10..20 and includes destroyed components. Current component
constants agree here: these are one-based component IDs, not weapon-table
indices. Displayed weapon index is masked component minus one. Ammo is read
unsigned from27+slot, including slots10/11 that alias movement fields. Native
strlen still lacks a guaranteed terminator when all twelve list entries fill;
these are existing BUG-012 behaviors, not permission to silently repair them.

Exact heat30 sets bit80 on all twelve targets; sensor1 warns and continues,
sensor2 returns directly without panel restoration. Menu target validation uses
the masked ID; drawing a surviving planned target clears its high bit. Destroyed
weapons clear their target. Numeric ammo is zero-extended and left-padded to
three columns. Both unavailable messages, their redraw tail and override logic
agree; the old target is cleared before the picker, including when Cancel follows.
The final4/3 panel restoration and repeated bright-white stores agree.

The weapon-header literal29BE is `Weapon`,09,0B,`Ammo Target  Range`;
current C replaces the column controls with spaces.29F9 starts06/0F before
`Done`; C omits those colour bytes. Remaining panel literals match the native
bytes, including the different sensor messages and CR-separated popup text.

Native ID gates0025/0038 and choice gate04B8 use signed WORD branches; C uses
unsigned variables. A menu resultFFFF is less than the count natively but not
in C; this is an input-contract discrepancy, not an observed menu result.
Infantry Weapon is CBW/byte-IMUL signed in the05FD–076A branch, unlike current unsigned
lookup and range/picker arguments. Native064C compares the signed target with
FFFF (confirmed `.dis`), not00FF despite the ASM printer's `0FFh`. Current
ExistingTarget=-1 already agrees. That infantry branch deliberately does not
mask target bit80 or revalidate active state. Preserve this until callers and
runtime reachability justify a separate behavior change.

## Target picker07CB

Native2B22 ends06/0F after the range label.2B54 is `Weapon:` followed by06/0F,
not a trailing space. Current C omits/substitutes these controls. Contrary to a
possible layout suspicion,2B36 really is `Target here Next enemy Cancel` with
spaces: it matches C. Human padding2B5E and two-space suffix2B6A also match.

Enemy kind and cursor overflow tests are signed WORD comparisons; C unsigned
tests differ for high-bit IDs. Enemy weapon-name lookup uses signed BYTE IMUL
in08BA–0916, not unsigned BYTE multiplication. Ordinary valid IDs/weapon bytes agree.
The biased mech-name and enemy-character aliases otherwise resolve correctly.
Both hide the appropriate weapon rows for mech versus personnel targets, move
the view to the shooter before computing range, and store only the target BYTE.
Target here is allowed even out of range. Cancel storesFF; Next cycles12..23,
skipping inactive slots; there is no all-inactive termination guard. Other menu
values exit without storing a target. The kick caller shares this picker with
weapon-table index20 and slot11, not a separate melee-distance filter.

## Ejection0A35

The24-by12 cleanup CBW-sign-extends each target and compares its full WORD with
the incoming ID; no bit80 mask is applied. Current comparison agrees for valid
IDs, unlike a masked-target cleanup elsewhere. Name initial is saved before
Name[0]=FF. Arena record4 additionally destroys record5/actor13; record5 invokes
the save/apply map patch, not restore. Rental message suppression tests original
actor13 and rereads the rental flag after the helper. Text2B6D/2B81 agrees.

Header Bool_ShowArmShotOffAnimation_3986 is already unsigned short: clearing it
matches the native WORD store at0B1A, including its second byte. Do not invent
a missing-byte-write discrepancy here.4586 is also correctly WORD-sized.

Record normalization and friendly-record<4 use native signed WORD tests, unlike
C unsigned tests; the speed<5 gate is likewise signed versus current unsigned
CombatSpeedSetting. Native pilot/rider source-position index is normalized record
BP-0A; C uses the incoming actor ID. They are equal for ordinary friendly0..3,
but differ outside that valid ejection contract. Capture/define invalid-input
policy rather than claiming enemy pilots are routinely misplaced.
Pilot/rider IDs are unsigned and FF suppresses each; native assignments set
Piloting=8, activate ID+4 and copy coordinates. Rider X increments as a WORD,
then adds0080 if low-byte bit80 is set. Seat bytes remain stored in the wreck.
Messages/delay may occur even if neither occupant exists; final white restore
does not clear additional state.

## Map patch0C72

Selector5572 contains246C. MapTile begins101D; payload offset0C00 therefore
addresses1C1D. Nonzero argument restores38 saved bytes from3EDB:5804. Zero
argument first saves each live byte and writes its3EDB:2B8E override only when
the override is nonzero. Zero overrides leave the original byte unchanged.
The loops match except for the explicit near map pointer discussed above;
no patch interpretation or change to38-byte extent is needed.

## Decimal prompt0CDE

The near scratch pointer is the outstanding discrepancy under normal generated
input. Other local operations agree under FAR3092:0012 and native-width bindings.
The buffer starts `0`; colour is1 for adapter0, otherwise2. Original text cursor
is restored before every display; row-clear coordinates use the updated cursor,
both sums shifted by3, right edge width3990 plus start minus one. Enter accepts
without editing; Escape resets to `0` but does not exit; Backspace can produce
empty text. Seven stored digits are checked before removing leading zeroes.
The overlapping forward string copy removes the first zero and its old NUL is
copied; subsequent digits remain NUL-terminated.

Parsing multiplies DX:AX by ten, adds CBW/CWD digit and subtracts30 through
ADD/ADC/SUB/SBB. Current unsigned expression agrees for the generated ASCII
digits0..9; empty input returns0, seven9s return9999999. Native signed length
gate and signed digit conversion differ only if an external actor corrupts the
buffer/length contract, not during this routine's ordinary input flow.
Final colour is15; `.dis` supplies saved-SI/frame restoration and FAR return
after the ASM listing's last MOV DX. No input-cancel sentinel is returned.

## Validation and follow-up

Verify-1543SystematicAudit.ps1 models signed gates, byte indexing, target clearing,
rider boundary arithmetic, map loops and decimal editing/parsing witnesses.
It does not run either C or native gameplay. Registry hashes cover final annotated
method bodies. No expensive Astra confirmation is necessary for these directly
observable pointer/text/test discrepancies. Correction approval and emulator
validation remain separate; the known full-list BUG-012 behavior is preserved.
