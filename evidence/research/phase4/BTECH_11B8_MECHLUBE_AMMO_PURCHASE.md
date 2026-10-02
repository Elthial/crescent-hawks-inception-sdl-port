# `BTECH_11B8` Mech-Lube ammunition purchase

## Review boundary

This block covers `Mechlube_Buy_Ammo`, `11B8:1762` through the final return at
the end of segment `11B8`. It is reached from Citadel building action `0x2B`.
The clean ASM listing stops after the saved-register pops; the `.dis`
intermediate preserves the ordinary `SP/BP` restoration and return.

## Weapon and ammo-slot association

After the player chooses one of the four party mech records, the routine scans
that record's raw critical bytes `+0x33..+0x55` in increasing address order. It
copies at most ten bytes whose masked component IDs are `0x10..0x20` into a
local ten-byte list. The destroyed bit `0x80` remains attached.

The ordinal in this collected list is then used directly to index both
`CurrentAmmo[10]` at `+0x27` and `MaxAmmo[10]` at `+0x6B`. This is strong local
evidence that those arrays parallel the first ten weapon-valued critical bytes
in scan order. It does not by itself settle whether repeated critical bytes are
independent weapons or multi-slot occupancy; that broader question remains
Astra candidate A-004.

## Eligible weapons and damage handling

A functional weapon is offered ammunition only when its destroyed bit is clear
and its component is either Machine Gun (`0x18`) or a missile launcher
(`0x1A..0x20`). The original implements the destroyed exclusion through signed
byte comparisons. Autocannons `0x14..0x17` are not accepted by this path.
Whether that is an unreachable loadout limitation or player-visible omission
is not established, so it is not recorded as a confirmed game bug.

Any collected component carrying bit `0x80` sets a separate damaged-weapon
flag. Destroyed weapons receive no ammunition offer, and the routine ends with
the warning that buying ammunition for damaged weaponry would be wasteful.

## Prices and requested quantity

Machine-gun rounds cost 2 C-Bills each. Missile prices come from the seven-byte
table at `3EDB:2060`:

| Component | Weapon | C-Bills per round |
|---:|---|---:|
| `0x1A` | LRM-5 | 10 |
| `0x1B` | LRM-10 | 15 |
| `0x1C` | LRM-15 | 25 |
| `0x1D` | LRM-20 | 30 |
| `0x1E` | SRM-2 | 30 |
| `0x1F` | SRM-4 | 60 |
| `0x20` | SRM-6 | 80 |

The helper at `1543:0CDE` is a general decimal-number prompt, not a C-Bill
balance function. It returns the requested unsigned quantity in `DX:AX`, so it
is documented as a 32-bit return. Zero skips that weapon's transaction. A value
greater than the missing capacity is capped to the missing number of rounds
after displaying the capacity warning.

## Purchase loop

The executable purchases one round per iteration:

1. compare the round price with the full 32-bit C-Bill balance;
2. stop if the balance cannot cover one more round;
3. increment the selected current-ammo byte;
4. subtract the price with the native 32-bit balance arithmetic;
5. redraw the balance and decrement the remaining requested quantity.

If money runs out first, the routine displays its partial-fill warning. It also
redraws the balance once after every nonzero purchase attempt, even when the
per-round loop already redrew it.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:1762` to segment end;
- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.dis`, including the recovered
  standard epilogue;
- price bytes `3EDB:2060..2066` = `0A 0F 19 1E 1E 3C 50`;
- weapon records beginning at `3EDB:2ED8`, with component ID minus one used as
  the record index;
- exact byte accesses to current ammo at record `+0x27+slot` and maximum ammo
  at record `+0x6B+slot`;
- exact `DX:AX` quantity comparisons and 32-bit C-Bill subtraction.

The scan bounds, list ordering, eligible components, damage exclusion, prices,
quantity cap, purchase loop and message paths are directly verified. The
semantic relationship between repeated critical bytes and distinct weapon
instances remains open under A-004.
