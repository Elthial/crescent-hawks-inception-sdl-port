# BTECH_11B8 Mech-Lube Locust first-stage modification

## Scope

This note covers `Mechlube_Upgrade_Mech`, native addresses
`11B8:09D3-0A0F`. It is jump-table selector zero, named
`MECH_LOCUST_LEVEL1` in the annotated source.

## Selected-mech addressing

The routine reads the selected-mech index from `2FE8:0068`, multiplies it by
`0x7D`, and uses the resulting offset relative to the first party-mech record
at `3092:C724`. This independently confirms the 125-byte `Mech` record stride.
The cleaned C represents that segmented address calculation as `SelectedMech`.

## Mutations

The package performs seven byte writes in this order:

| Native address | Record field | Written value | Meaning |
|---:|---|---:|---|
| `C765 + index*7D` | `Critical_R_Arm[0]` (`+41`) | `11` | medium laser |
| `C757 + index*7D` | `Critical_L_Arm[0]` (`+33`) | `11` | medium laser |
| `C790 + index*7D` | `MaxAmmo[1]` (`+6C`) | `FF` | laser/infinite-ammo sentinel |
| `C78F + index*7D` | `MaxAmmo[0]` (`+6B`) | `FF` | laser/infinite-ammo sentinel |
| `C74C + index*7D` | `CurrentAmmo[1]` (`+28`) | `FF` | laser/infinite-ammo sentinel |
| `C74B + index*7D` | `CurrentAmmo[0]` (`+27`) | `FF` | laser/infinite-ammo sentinel |
| `C7A0 + index*7D` | `UpgradeLevelFlags` (`+7C`) | `01` | exact first-stage state |

The two arm writes therefore implement the advertised Locust modification:
the existing first arm weapons (the stock machine-gun positions) become two
medium lasers. Ammo-state slots zero and one are changed to `0xFF` because
energy weapons do not consume ammunition.

The final state write is an assignment rather than a bitwise OR. On completion
the case jumps directly to the common success message at `11B8:0D41`; it does
not fall through any other chassis package.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:09D3-0A0F`;
- selected-mech index at `2FE8:0068` and native signed multiply by `0x7D`;
- verified `Mech` offsets `+27`, `+28`, `+33`, `+41`, `+6B`, `+6C`, and `+7C`;
- established constants `Mech_Med_Laser == 0x11` and
  `Ammo_Laser_Infinite == 0xFF`.

The writes, widths, record offsets, selected-record calculation, and final
stage value are directly verified. No Astra review is required for this block.
