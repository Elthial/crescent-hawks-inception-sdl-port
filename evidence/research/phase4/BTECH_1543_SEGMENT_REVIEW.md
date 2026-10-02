# `BTECH_1543` — weapon planning, target selection, destruction and numeric input

## Scope and evidence

All five routines are reviewed against the `reko-expanded-2020` ASM and `.dis`.
Previously reviewed Starport map-patching behavior is retained, not redesigned.
This remains annotated research pseudo-C; gameplay execution is not yet tested
through a compiled replacement.

| Address | Routine | Purpose |
|---|---|---|
| `0004..07CA` | `Combat_Weapon_UI_0004` | Build/edit planned weapon targets |
| `07CB..0A34` | `Combat_Select_Weapon_Target_UI` | Cycle enemies and select/cancel a target |
| `0A35..0C71` | `Combat_Mech_Eject_0A35` | Retire a destroyed mech and eject friendly crew |
| `0C72..0CDD` | `Starport_MapPatch_SaveApply_Or_Restore_0C72` | Save/apply or restore 38 map bytes |
| `0CDE..0EE5` | `Prompt_For_Unsigned_Decimal_0CDE` | Edit up to seven decimal digits; return DX:AX |

The expanded executable is also checked directly. With this Reko load base,
file offset is `MZ-header-bytes + (segment-0800h)*16 + local-offset`. The
prologue at `1543:0004` matches before using that mapping. Raw bytes confirm
the infantry sentinel comparison and the final epilogues at `0C61` and `0ED0`.
This is not a mapping into the packed installation executable.

## Indexed storage and identifiers

| View | Width/extent | Interpretation |
|---|---|---|
| `3092:3800..391F` | 24 rows × 12 bytes | Planned target per combatant and weapon/action slot |
| `3092:3994..399F` | 12 bytes | Friendly combat action-state view |
| `3092:4004` / `4036` | 24 WORDs each | Packed X/Y position per combatant |
| `3092:406A..4099` | 24 WORDs | Active/on-map state per combatant |
| Mech `+27h..30h` | 10 bytes | Current ammo |
| Mech `+33h..55h` | 35 bytes | Raw critical components |

The header retains split position/active aliases used by older code, but this
segment uses explicit full-table views. They are documentation conveniences,
not a claim that the scratch-pad struct has a compilable packed DOS layout.

Friendly mechs are combatants `0..3`, friendly infantry `4..11`, enemy mechs
`12..15`, and enemy infantry `16..23`. Thus:

- friendly infantry weapon address `C5DB + combatId*11h` aliases
  `Infantry[combatId-4].Weapon`;
- enemy infantry weapon address `C597 + combatId*11h` aliases
  `Infantry[combatId-8].Weapon`;
- enemy mech name address `C33C + combatId*7Dh` aliases
  `Mechs[combatId-8].Name`, not a separately indexed wreck record.

Planned target `FF` means none. The high bit is used to disable an existing
planned action; its low seven bits retain the combatant ID.

## Weapon-planning menu (`0004`)

The argument is a stack WORD combatant ID, not a pointer. Friendly action state
is cleared before either infantry or mech handling.

### Infantry

Infantry uses its single weapon-table index directly and planned-target slot
zero. If it already has a target, the panel displays the weapon, target type/name
and range and asks whether to override. A no answer preserves its target. A yes
answer, or no existing target, opens the target picker. Cancel explicitly clears
the planned target; it is not a rollback to the old selection.

An apparent sentinel bug in the ASM text is **not a game defect**. The byte
target is sign-extended by `CBW`; instruction bytes `83 7E FA FF` at `064C`
compare it against sign-extended `FFFF`, not `00FF`. `.dis` agrees. Raw `FF`
correctly takes the no-existing-target branch. This illustrates why pretty-
printed immediate values cannot outweigh opcode semantics.

Unlike the mech panel, the existing-infantry-target branch neither masks the
high bit nor tests whether that target is still active. The caller's invariant
for flagged/stale infantry targets remains to be checked in the combat pass.

### Mechs

The collector scans critical offsets `33h..55h` and appends every byte whose
masked component ID is `10h..20h`, retaining the destroyed bit. **This collector
accepts twelve entries**, unlike the ten-entry ammunition-shop collector.
Later menu geometry and the Done index use the collected byte list as a
NUL-terminated string. Its boundary defects are recorded as BUG-012.

Heat exactly equal to 30 displays shutdown and ORs `80h` into all twelve target
slots. Sensor state is mech `+77h` (`C79B` for record zero): state one warns of
impaired accuracy; state two warns and returns directly through the epilogue,
without the normal combat-panel restoration.

Each populated weapon row shows its name, ammo, target and range. Weapon-table
index is masked component ID minus one. Assigned rows are yellow, destroyed
rows dark grey; the original adapter-0 alternatives are blue and green.

Destroyed weapons have their targets cleared and display `Destroyed`. Ammo
`FF` displays `Full`, zero displays `Out`, and other values are left-padded to
three decimal columns. Displaying an active planned target clears its high bit;
inactive targets become `FF`. Target names and range lookups use the unflagged
combatant ID.

Destroyed and empty-ammo selections have distinct popup text and share a redraw
tail. The former malformed goto made the destroyed message appear overwritten;
the executable does not do that. An available assigned weapon asks to override.
Accepting clears its old target **before** opening the picker. The picker
receives the weapon-table index and selected weapon slot, not a text address.

`Done` is menu index equal to the string length. The original WORD geometry is
`0056 = length+2`, `0052 = 22-length`, and `00E6 = length+1`; these are not stack
pointers despite older C names. Normal completion restores panels four and
three, retaining the explicit repeated redraw/colour assignments.

## Target picker (`07CB`)

All three parameters are stack WORDs: attacking combatant, weapon-table index
and planned-action slot. `EnemyTargetId` at `3EDB:2B20` is a persistent cursor.
Inactive entries are skipped; cycling wraps from 23 to 12.

The panel displays the target and range; enemy infantry also shows its weapon.
The attacker world position is restored before range calculation. The string
at `3EDB:2B6A` is just **two spaces**, not the nearby destruction message at
`2B6D` incorrectly shown by the prior C.

Menu choices are `0 = Target here`, `1 = Next enemy`, `2 = Cancel`. Zero stores
the target byte, two stores `FF`, and one continues cycling. Out-of-range is
displayed but does not prevent selection here. The kick routine reuses the
picker with weapon-table index `20h` and action slot 11.

No all-inactive escape or cursor validation exists in this routine. Its callers
must establish an active enemy and valid cursor; audit those preconditions
before turning this into a safe C# service. This is not yet a confirmed ordinary-
play hang.

## Destruction and ejection (`0A35`)

The original combatant ID is retained separately from the translated mech record
ID. The routine clears that combatant's active WORD, marks its wreck WORD, and
scans all 288 planned target bytes. Matching targets become `FF`. The comparison
sign-extends each byte **without clearing its high bit**; flagged target bytes
therefore do not match positive combatant IDs in this pass.

Enemy combatants `12..15` translate to mech records `4..7` by subtracting eight.
The first name byte is saved at `3092:323E + recordId` for salvage and replaced
with `FF`; the remainder of the mech record and both crew IDs are retained.

In Arena rental mode, destroying record four also hides record five at `C995`
and clears combatant 13's active WORD at `4084`. Destroying record five applies
the existing Starport map patch. The generic destruction message is suppressed
for rental combatant 13 using the original ID, not translated record ID five.

Friendly crew is changed to on foot and activated at combatant `partySlot+4`.
The pilot receives the mech's position. The rider receives X+1 and the same Y;
if low-byte bit `80h` is set after incrementing X, `0080h` is added to the full
WORD. This is packed-coordinate boundary arithmetic, not a Boolean flag toggle.
No search for a safe adjacent tile exists in this local routine.

The WORDs at `4586` and `3986` are cleared. Pacing is invoked only for friendly
ejection when the configured speed is below five, then text colour returns to
bright white.

## Decimal-number input (`0CDE`)

The prompt starts at `"0"`, saves its text row/column and redraws at that origin
on every key. The remaining eight-pixel-high row is cleared using the cursor
**after** text display. Coordinate sums are parenthesized before shifting by
three; the original rectangle call has all five arguments.

- Digits append only while the stored string length is below seven.
- Leading zeroes are removed before appending an accepted digit.
- Backspace removes one character and may leave the field empty.
- Escape resets the field to `"0"` but does not cancel or exit.
- Enter accepts; other keys are ignored.

Leading-zero removal uses the original forward copy from text+1 onto text. This
specific overlap direction is safe; do not infer a general memmove guarantee
from the legacy helper name.

Acceptance accumulates `value = value*10 + digit` using original 32-bit multiply
and carry arithmetic. At most seven digits yields `0..9,999,999`, so overflow
does not arise under this input limit. Empty text returns zero. The result is
returned in `DX:AX`; there is no money-state mutation. Text colour is reset to
bright white, not restored to its prior arbitrary value.

## Remaining boundaries and verification

A-004 now records the twelve-versus-ten collector mismatch and potential
repeated-critical occupancy. The target picker's no-active-enemy precondition,
flagged target treatment and remaining legacy aliases belong to the next combat
review. Shared drawing/range helper signatures still contain Reko artifacts;
they are not wholesale repaired in this pass.

Verification comprises the direct executable-byte checks, the `.dis` sentinel
cross-check, eight passing numeric-editor transcription sanity cases,
`git diff --check`, and the existing InceptionTools regression suite (182
assertions passed). These do not substitute for a DOS
runtime trace or execution tests of a future C# implementation. No copyrighted
external assets are added and no expensive model run is started.
