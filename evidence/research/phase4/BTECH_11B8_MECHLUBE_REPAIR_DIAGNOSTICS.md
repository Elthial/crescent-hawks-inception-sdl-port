# `BTECH_11B8` Mech-Lube repair diagnostics

## Review boundary

This block covers `Mechlube_Repair_Mech` from `11B8:0002` through `01E1`. It
selects a party mech, inventories every damage category the shop can repair,
and handles the no-repairable-damage path. The paid repair workflow begins at
`11B8:01E2`; its first stage is documented in
[Mech-Lube armour repair](BTECH_11B8_MECHLUBE_ARMOUR_REPAIR.md).

## Selected mech and exact prompt

The function draws the top sidebar, prints the executable's stored prompt, and
calls the shared party-mech selector:

```text
Fix which 'Mech?\r
```

The selected index is the WORD at `305B:0068`. All subsequent fields come from
the corresponding `0x7D`-byte live party-mech record.

## Armour and structure totals

Two WORD accumulators calculate:

```text
missing armour   = sum(MaxArmour[i]   - CurrentArmour[i]),   i = 0..10
missing structure= sum(MaxStructure[i]- CurrentStructure[i]), i = 0..7
```

The old pseudo-C overwrote each total on every iteration and read current
structure while calculating armour. The assembly instead accumulates all
eleven armour differences and all eight structure differences independently.
The same native accumulators later decrease as individual points are repaired.

## Destroyed critical components

The function scans every byte from mech offsets `0x33` through `0x55`
inclusive—the complete contiguous critical-slot block. Only bytes with high bit
`0x80` set are damaged candidates. After clearing that flag:

- component `0x22` increments the destroyed-heat-sink WORD;
- components `0x10..0x20` increment one of seventeen WORD weapon counters;
- finding any destroyed weapon sets a separate WORD control flag;
- component `0x21` (`Kick`) is outside the weapon-repair list.

The stack frame reserves and clears two arrays of twenty WORDs. One holds the
destroyed-weapon counts; the other is reused later to map visible weapon menu
rows back to component indexes. They are not byte arrays or pointer expressions.

## Packed actuator comparison

The two current actuator bytes at offsets `+0x24/+0x25` are compared directly
with the two maximum bytes at `+0x69/+0x6A`:

| Repair flag | Comparison |
|---:|---|
| `0x01` | current left packed byte differs from maximum left |
| `0x02` | current right packed byte differs from maximum right |

The flags indicate damaged sides. Each compared byte still contains a leg in
its low nibble and an arm in its high nibble.

## Unrepairable internal damage

Only when every repairable inventory value is zero does the function inspect
mech offsets `+0x75..+0x77`: `EngineHits`, `GyroHits`, then `SensorHits`. The
first nonzero byte produces:

```text
Our facility doesn't have the equipment to fix your damaged [engine|gyro|sensors].
```

The three suffixes come from a native far-pointer table at `3EDB:1D12`. The
former `t1B3E[InternalComponent]` expression was a pre-biased decompiler view,
not a table physically beginning at `1B3E`.

If none of those three counters is nonzero, the routine selects the stored
“shiny new” response. Offset `+0x78`, currently identified only probably as
life-support state, is not inspected by this shop diagnostic.

Because the internal-damage scan is skipped whenever any repairable damage
exists and is not repeated later, mixed repairable/unrepairable damage can hide
the warning. This is recorded as original-game `BUG-007`.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:0002-01E1`;
- exact text and the three far pointers at `3EDB:17CE-17E3` and `1D12-1D1D`;
- the verified `0x7D` mech layout and packed actuator mapping;
- the later weapon menu's reads of the two twenty-WORD stack arrays.

The loop bounds, field offsets, accumulator widths, component ranges, actuator
flags, and internal-damage branch are directly verified. The `+0x78`
life-support label remains probable, but it does not affect the reconstructed
control flow. No Astra review is required for this block.
