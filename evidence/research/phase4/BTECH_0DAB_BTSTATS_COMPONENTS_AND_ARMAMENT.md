# `BTECH_0DAB` BTSTATS components and armament

## Scope

This note covers `0DAB:1D8B-1F85` inside
`Examine_Screen_BTSTATS_CMP`: component-status pips, operational heat-sink
counting, and the armament/location list. The actuator section beginning at
`0DAB:1F86` remains the next review block.

## Engine, gyro, and sensor pips

The status screen displays:

| Component | Total pips | Green pips |
|---|---:|---:|
| Engine | 3 | `3 - EngineHits` |
| Gyro | 2 | `2 - GyroHits` |
| Sensors | 2 | `2 - SensorHits` |

`Draw_Component_Status_Pips_1858` changes from green to red at the supplied
healthy count, so accumulated hits appear as red pips at the end of each row.
All arithmetic and arguments are native 16-bit WORDs.

## Operational heat sinks

The green heat-sink count begins with the mech record's byte at `+0x26`,
`EngineHeatsinks`. The routine scans all 35 critical bytes from `+0x33` through
`+0x55` and increments the count only for a byte exactly equal to `0x22`.

Consequently:

- intact critical-slot heat sinks (`0x22`) count as operational;
- destroyed heat sinks (`0xA2`) do not count; and
- the display always has ten pips, changing the remainder to red when fewer
  than ten heat sinks operate.

The reference records support this model. A Locust stores six engine heat
sinks and four intact `0x22` critical bytes, producing ten green pips.

## Armament list

The same inclusive `+0x33..+0x55` range is scanned again. Each byte is masked
with `0x7F`; component IDs `0x10..0x20` are displayed, covering Small Laser
through SR Missile 6. Kick (`0x21`), heat sinks (`0x22`), zeroes, and other
components are omitted.

The destroyed high bit does not remove a weapon from the list. Intact weapons
are bright yellow and destroyed weapons are dark grey in EGA mode.

Component IDs and weapon-table indexes differ by one:

```text
weaponTableIndex = componentId - 1
```

Thus component `0x10` addresses table index `0x0F`, whose record begins at
`DS:2FD7` and is Small Laser. The former pseudo-C multiplied an already typed
array index by the record size and did not account for this bias.

## Location labels

The absolute critical-byte offset selects the label printed at column 11:

| Record offsets | Label |
|---|---|
| `33..39` | `LA` |
| `3A..40` | `LT` |
| `41..47` | `RA` |
| `48..4E` | `RT` |
| `4F..50` | `LL` |
| `51..52` | `RL` |
| `53..54` | `CT` |
| `55` | `H` |

Weapon rows begin at text row 6 and advance only when a weapon component is
found. In this simplified game record, each qualifying critical byte is one
weapon entry; repeated IDs represent multiple weapons rather than a multi-slot
tabletop BattleTech construction rule.

## Confidence

High. Loop bounds, exact/masked component comparisons, colour selection, biased
weapon-table address, and every location threshold agree directly with the
clean expanded assembly. The reference-mech bytes provide an independent
cross-check. No Astra review is required for this block.
