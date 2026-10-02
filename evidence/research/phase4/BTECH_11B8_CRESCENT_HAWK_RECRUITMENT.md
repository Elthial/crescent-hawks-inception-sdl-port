# BTECH_11B8 scripted Crescent Hawk recruitment

## Scope and name

This review covers `11B8:0D58-104D`, formerly annotated
`Teammate_Mech_Assign` and now named `Recruit_Crescent_Hawk_Agent`.

The name is directly supported by all three shipped BLD call sites. Each story
branch has just announced discovery of a Crescent Hawk agent before executing
opcode `E9`:

| Script/offset | Operand | Established specialty |
|---|---:|---|
| `HOSPITAL.BLD:09BC` | `06` | Medical |
| `JAIL.BLD:0DD5` | `04` | Piloting |
| `REPAIR.BLD:123A` | `05` | Tech |

The caller reads one byte, sign-extends it to a native WORD argument, and calls
`11B8:0D58`. The callee uses that WORD as an index into the seven contiguous
skill bytes at character offsets `+04..+0A`.

## Party slot and identity

The routine scans party slots `0..7` in order and operates on the first record
whose Name ID is `0xFF`. If all eight are occupied, it creates no agent and
proceeds directly to the map-position restoration epilogue.

Byte `3092:D456` is not simply the current party count. It is the next generated
Name ID:

1. New-game setup initializes it to `1`, Rex's ID.
2. Rex's scripted join assigns the current ID and increments the byte.
3. This routine assigns the current ID to the new agent and increments it.
4. If the incremented value reaches `0x0A`, it is replaced with `0x02`.

The cleaned field name is `NextRecruitNameId_D456`. The wrap permits IDs `2..9`
to be reused after enough deaths and replacements; it does not create ID 10.

## Deterministic generation

Before assigning the record, the routine calculates:

```text
signed32 randomState = signExtend(NameId) * 0x0187
```

The low and high WORDs replace `3EDB:4FC0` and `3EDB:4FC2`. Native `CBW`, signed
`IMUL`, and `CWD` establish the widths; this is not a Reko 32-bit-pointer
artifact.

The new 17-byte character record is then initialized as follows:

- Name ID is the pre-increment `D456` value;
- weapon, armour type/value, and training flags are zero;
- Body, Dexterity, and Charisma each use `Rand_Dice_Two_D6`;
- Health is `Body × 10`;
- sprite-family byte for friendly infantry combatant `4 + partySlot` is zero;
- `MechAssignment` begins at `8` (on foot);
- all seven skills receive independent `(RandByte & 1)` values;
- Piloting is then forced to Unskilled (`0`);
- the operand-selected skill is assigned Good (`3`).

For specialty index four (Piloting), the routine additionally subtracts
`Body × 2` from Health, so the pilot recruit begins at `Body × 8`. This is an
explicit special case, not arithmetic damage inferred from the decompiler.

The executable performs no bounds check on the skill operand. The three shipped
scripts use only valid indexes `4..6`; tooling should preserve the byte while a
portable runtime may validate custom scripts explicitly.

## Optional mech passenger assignment

The four party mech records are scanned in order. The first mech whose
`Name[0] != 0xFF` and whose `RiderId == 0xFF` receives the new party-slot index
as its Rider ID. The recruit's `MechAssignment` receives that mech index.

The routine never changes `PilotId`. Its exact display text confirms the role:

```text
His name is <recruit> and he will be riding as a passenger in <pilot>'s 'Mech.
```

If no rider slot is available, the suffix is `on foot.` instead. The pilot name
is obtained by following the assigned mech's `PilotId` to that party record and
then indexing the character-name far-pointer table.

## Input and traitor selection

After redrawing the health/C-bill sidebar, interactive play waits for pending
input. Every idle poll advances the PRNG once. Recorded-input mode skips that
polling loop. The function then drains pending input and waits for/consumes one
keyboard value.

If `Traitor_EventOccurred` is still zero, one subsequent random bit determines
whether this recruit becomes the future traitor. On a nonzero bit it writes:

| Address | Value |
|---:|---|
| `D331` | recruited party-slot index |
| `D332` | `1` (traitor event assigned) |
| `D333` | `1` (traitor still in party) |
| `D330` | `0x1F` (battle probability state) |

The random outcome can therefore depend on how long the player leaves the
recruitment text open, because the wait loop advances the same PRNG. `D331` is
a party slot, not the recruit's Name ID.

## Exit position

Whether recruitment succeeds or the party is full, the epilogue reads the
WORD building ID at `3092:4584` and restores `246C:A44B/A44D` from the paired
WORD return-position tables at `3092:39B4/39D4`.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `0D58-104D`;
- opcode caller at `BTECH_0FDC.asm:04CC-04DF`;
- decoded shipped call sites in `HOSPITAL.BLD`, `JAIL.BLD`, and `REPAIR.BLD`;
- exact strings at `3EDB:1E2F`, `1E3C`, `1E4D`, `1E56`, and `1E70`;
- verified character stride `0x11`, mech stride `0x7D`, and pilot/rider fields.

The routine purpose, operand, widths, slot selection, record initialization,
passenger assignment, traitor state, input dependency, and exit restoration are
verified. No Astra review is required for this block.
