# `BTECH_11B8` Mech-Lube internal-structure repair

## Review boundary

This block covers `Mechlube_Repair_Mech` from `11B8:032F` through `0458`. It
quotes and repairs the eight internal-structure locations. The destroyed heat
sink stage begins at `11B8:0459` and is the next review block.

## Quote and prompt

When the diagnostic WORD reports missing structure, the shop explains that
weapons and heat sinks need structure on which to mount and quotes:

```text
repair cost = total missing structure points * 9 C-bills
```

The displayed balance is the current 32-bit balance, which may already have
been reduced by armour work earlier in the same visit. There is no separate
zero-balance exit at this stage. Declining changes nothing and advances to the
heat-sink stage.

## Correct record fields

The native loop counter runs from raw mech offsets `+0x1C` through `+0x23`.
These are the eight `CurrentStructure` bytes, not armour indexes. The paired
maximum values are at `+0x61` through `+0x68`, exactly `0x45` bytes later.
Typed source can therefore express the calculation without biased addresses:

```text
missing at location = MaxStructure[index] - CurrentStructure[index]
index = 0..7
```

The locations are visited in stored order: right arm, right leg, right torso,
head, centre torso, left arm, left leg, and left torso.

## Per-point transaction

Each successful point is processed independently:

1. compare the complete 32-bit balance with nine;
2. subtract nine using the native low-WORD `SUB` and high-WORD `SBB` pair;
3. decrement the per-location and overall missing-structure WORDs;
4. increment the location's current-structure byte;
5. redraw the displayed C-bill balance.

If fewer than nine C-bills remain, the routine clears its temporary count for
that location. It still visits later locations, which fail the same balance
test without modifying the mech.

## Partial repairs

Completed points are not rolled back. If the overall missing-structure WORD
remains nonzero after the eight locations, the shop displays its insufficient-
funds message and consumes one keyboard input before continuing. The executable
uses “We fixed”, not the annotated source's former “We've fixed”.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:032F-0458`;
- exact strings at `3EDB:1981`, `19BC`, `1A19`, `1A4F`, `1A66`, and `1A77`;
- verified structure fields at mech offsets `+0x1C/+0x61`;
- native raw loop bounds `0x1C..0x23` and paired `SUB`/`SBB` charge.

The control flow, field pairing, location order, nine-C-bill price, integer
widths, and partial-repair behavior are directly verified. No Astra review is
required for this block. Review continues with
[Mech-Lube heat-sink repair](BTECH_11B8_MECHLUBE_HEAT_SINK_REPAIR.md).
