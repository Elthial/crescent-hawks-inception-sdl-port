# `BTECH_11B8` Mech-Lube modification setup

## Review boundary

This block covers `Mechlube_Modify_Mech`, `11B8:080A-0924`. It selects a mech,
decides which modification package applies, and publishes the selector and
price for the purchase routine. `Mechlube_Upgrade_Mech` begins at `11B8:0925`
and is the next review block.

## Selection and fully-upgraded gate

The routine prints the exact `Modify which 'Mech?` prompt and invokes the
shared party-mech menu, which stores the chosen record index at `305B:0068`.
It then reads mech byte `+0x7C`, the verified `UpgradeLevelFlags` field.

Exact value `3` means both supported upgrade stages are installed. The routine
prints the “already pretty powerful” refusal, clears workflow byte
`3092:D31E`, and returns. The old C mistook this comparison for a Chameleon
chassis check; no chassis identity is read at this gate.

All other values set `D31E` and continue, including malformed flag values and
chassis/package bytes unsupported by the eight real packages. The routine
draws the sidebar and displays the current 32-bit C-bill balance at line
`0x16`, then resets the text cursor.

## Package mapping

Mech byte `+0x7B` supplies the base selector. Its broader meaning remains
probable because other code gives special meaning to value `0xC8`, but this
routine's local conversion is direct:

| Base | Level flags | Selected package | Stage |
|---:|---:|---:|---|
| `0` | not `1` | `0` | Locust first |
| `1` | not `1` | `1` | Wasp first |
| `2` | not `1` | `2` | Stinger first |
| `3` | not `1` | `3` | Commando first |
| `0..3` | exactly `1` | base + 4 | corresponding second |
| `>3` | any admitted value | `8` | unsupported sentinel |

A final `>7` clamp also maps to eight. The selected byte is stored at
`3092:D31D` for the purchase routine.

## Verified price table

The executable contains eight WORD prices at `3EDB:1D8C..1D9B`:

| Selector | Package | C-bills |
|---:|---|---:|
| `0` | Locust first | 11000 |
| `1` | Wasp first | 10400 |
| `2` | Stinger first | 12200 |
| `3` | Commando first | 13800 |
| `4` | Locust second | 15800 |
| `5` | Wasp second | 14000 |
| `6` | Stinger second | 13200 |
| `7` | Commando second | 17600 |

The selected price is stored as one signed WORD at `3092:0076`; the old
`unsigned long` declaration was a Reko-width error. Downstream code uses `CWD`
to sign-extend this WORD when comparing or subtracting it from the 32-bit
balance.

## Original unsupported-selector defect

The native routine also indexes the table when the selector is eight. That is
one WORD beyond the table, at `3EDB:1D9C`, where the following purchase prompt
begins with two carriage returns. Little-endian `0D 0D` becomes the accidental
price `0x0D0D`, or 3341 C-bills.

The cleaned C preserves the observed pseudo-price through a named constant
instead of reproducing undefined array access. The downstream no-op charge is
recorded as original-game `BUG-010` in
[`ORIGINAL_GAME_BUGS.md`](../investigations/ORIGINAL_GAME_BUGS.md).

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:080A-0924`;
- exact strings at `3EDB:1D1E`, `1D33`, and `1D82`;
- eight price WORDs and adjacent `0D 0D` bytes at `3EDB:1D8C-1D9D`;
- verified mech field offsets `+0x7B/+0x7C` and state bytes `D31D/D31E`;
- the purchase routine's selector bound and signed-WORD balance operation.

The gate, package conversion, table contents, WORD widths, state outputs, and
out-of-bounds sentinel behavior are directly verified. The broader dual use of
mech byte `+0x7B` remains the existing Astra candidate A-005; no new Astra
review item is required for this block. Review continues with
[Mech-Lube modification purchase](BTECH_11B8_MECHLUBE_MODIFICATION_PURCHASE.md).
