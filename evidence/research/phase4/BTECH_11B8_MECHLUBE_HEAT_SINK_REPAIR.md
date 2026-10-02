# `BTECH_11B8` Mech-Lube heat-sink repair

## Review boundary

This block covers the destroyed-heat-sink stage of `Mechlube_Repair_Mech` from
`11B8:0459` through `0569`, including its branch through the shared dispatcher
at `0744`. Weapon repair begins at `11B8:056A` and is the next review block.

## Offer

The destroyed-heat-sink WORD was populated by the initial critical-slot scan.
When it is nonzero, the shop prints the count, pluralizes “heat sink” only when
the count exceeds one, advertises the unit price, displays the current balance,
and asks whether to repair some heat sinks:

```text
heat-sink repair price = 800 C-bills per destroyed critical slot
```

There is no computed total and no quantity selector. Declining leaves every
slot unchanged and advances through the shared repair-stage dispatcher.

## Critical-slot scan

On acceptance, a WORD offset walks the complete critical component block from
mech offset `+0x33` through `+0x55` inclusive. A slot is eligible only when its
raw byte equals `0xA2` exactly:

```text
0xA2 = HeatSink component 0x22 | Destroyed flag 0x80
```

This is not a generic test for any destroyed component, nor does it modify the
`EngineHeatSinks` field at mech offset `+0x26`.

## Per-slot transaction

For each eligible slot, the game compares the complete 32-bit balance with
`0x320` (800). When affordable it:

1. subtracts 800 with a low-WORD `SUB` and high-WORD `SBB`;
2. decrements the destroyed-heat-sink WORD;
3. replaces raw slot byte `0xA2` with intact heat-sink byte `0x22`;
4. redraws the displayed balance.

Slots are repaired in physical record order. If a slot is unaffordable, it is
left destroyed and scanning continues. Because the balance cannot increase in
this loop, all subsequent eligible slots will also remain destroyed.

## Completion and dispatch

If the count reaches zero, the routine advances silently. Otherwise it draws
the shop sidebar and displays the exact stored message that the job is not
complete and more cash is needed, then consumes one keyboard input. Both paths
reach the dispatcher at `11B8:0744`, which enters weapon repair when its flag is
set or proceeds toward actuator repair when it is not.

The C source's former `char JobRepairText[]` could not represent the assembly's
loaded string offset and was not valid as an unbounded local array. It is now a
near data-segment text pointer shared by the heat-sink and weapon result paths;
the native branches carry the 16-bit offset in `AX` to their common display
label.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:0459-0569` and
  dispatcher `0744-0755`;
- exact strings at `3EDB:1ACE`, `1ADA`, `1AE5`, `1AE7`, `1B44`, and `1B63`;
- verified component range at mech offsets `+0x33..+0x55`;
- native exact-byte comparison with `0xA2` and 32-bit charge of `0x320`.

The offer, pluralization, component identity, scan order, unit price, balance
width, mutation, partial-repair behavior, and dispatch are directly verified.
No Astra review is required for this block. Review continues with
[Mech-Lube weapon repair](BTECH_11B8_MECHLUBE_WEAPON_REPAIR.md).
