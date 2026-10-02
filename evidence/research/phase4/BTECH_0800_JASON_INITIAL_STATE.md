# `BTECH_0800` Jason and initial economy state

## Reviewed block

- Address range: `0800:4E79-4F0E`.
- Parent routine: currently `Load_Game_Map_Data`.
- Combatant sprite-family setup begins at `0800:4F0F`.

This section establishes Jason as the sole initial party member, initializes
cash and stock balances, reveals the starting Citadel map area, and fills the
non-skill fields of Jason's 17-byte character record.

## Party and economy

Jason's `NameId` is set to zero and his `MechAssignment` to eight, meaning on
foot. The party count at `3092:D456` becomes one.

The 32-bit C-Bill balance at `D370:D372` is initialized to hexadecimal
`00000014`, which is **20 C-Bills**. The previous annotation confused this
with the separate 16-bit allowance/total-wealth limit at `D33F:D340`, which a
later section initializes to hexadecimal `0032`—50 decimal.

All three 32-bit stock balances are cleared. The executable performs two
16-bit zero stores per entry at `D374/D376`, `D378/D37A`, and `D37C/D37E`,
confirming the four-byte stride and low-word/high-word layout.

## Starting fog reveal

The former `tD138[Counter]` expression lost a stride. `D138` lies inside the
saved fog bitmap:

```text
D138 - CB0C = 0x062C
```

The loop shifts its index left four bits, so it touches fog indexes `062C`,
`063C`, `064C`, `065C`, `066C`, and `067C`: the same byte-column in six
consecutive 16-byte rows. Each byte is ORed with `1F`, revealing five adjacent
horizontal cells. The result is a 5×6 initially visible rectangle near the
Citadel; existing high bits in those bytes are preserved.

This also resolves the old `D178` alias: `D178` is fog index `066C`, the fifth
row touched here, not an independent “inside Star League” state field.

## Jason's initial record

The preceding reset already cleared all seven skills. This block sets:

| Field | Value | Meaning |
| --- | ---: | --- |
| `NameId` | `00` | Jason |
| `Body` | `08` | Body 8 |
| `Dexterity` | `09` | Dexterity 9 |
| `Charisma` | `07` | Charisma 7 |
| `WeaponTableIndex` | `00` | Cludgel/club entry |
| `MechAssignment` | `08` | On foot |
| `ArmourType` | `00` | Initial type value |
| `ArmourValue` | `00` | No current armour points |
| `Health` | `50` | 80 decimal, exactly `Body × 10` |
| `TrainingFlags` | `00` | No training flags set |

The current `BTECH.h` field name `Riding` at character offset `+10` is a known
legacy alias; this write clears the already-verified `TrainingFlags` byte.

## Confidence and porting notes

Confidence is high for every write, width, stride, and value. The 5×6 fog
rectangle follows directly from the verified 16-byte row stride and MSB-first
bitmap renderer. Its general location is the starting Citadel region; exact
player-facing tile labels within that rectangle are not assigned here. No
Astra review is requested.

A C# new-game constructor should distinguish the 20-C-Bill starting cash from
the later 50-unit allowance limit, initialize the three stocks as 32-bit
values, and reveal the exact fog rectangle without treating `D138` or `D178`
as independent variables.
