# `BTECH_11B8` Mech-Lube weapon repair

## Review boundary

This block covers the weapon menu and transaction loop in
`Mechlube_Repair_Mech`, `11B8:056A-0743`, plus its return through the shared
dispatcher at `0744`. Actuator repair begins at `11B8:074D` and is the next
review block.

## Damage-count and menu-index domains

The initial diagnostic scan creates seventeen WORD counts. Their indexes are
not weapon-table indexes or raw component IDs:

```text
repair type index = 0x00..0x10
component ID      = repair type index + 0x10
destroyed byte    = repair type index + 0x90
weapon-table index= repair type index + 0x0F
```

Thus repair index zero represents component `0x10` and the SmallLaser record
at `3EDB:2FD7`; index `0x10` represents component `0x20` and the SRMissile6
record at `3EDB:30E7`.

## Menu construction

Each pass clears a twenty-WORD row map, then checks all seventeen damage-count
entries. For every nonzero type, it displays the stored count and the name from
the corresponding `0x11`-byte weapon record. A compact row map records which
repair type belongs to each visible choice.

The routine saves the generated menu boundary in the generic words at
`305B:0202/0206/0208`, appends the stored “Nothing” option and the fixed
300-C-bill price prompt, displays the current balance, and invokes menu `0x17`.
The existing C field names at `305B:0206/0208` describe party-member uses and
are misleading in this generic menu context.

The assembly stores the menu helper's returned `AX` in a WORD stack local. The
helper's signature was still BYTE-sized at this bounded review. Sol: the later
1E56 audit and header now use unsigned-short return/argument widths; that
signature TODO is closed (2026-09-17), without changing purchase behaviour.

## Selection and charge

Choosing “Nothing” clears the workflow flag and proceeds to actuator repair.
For a weapon row, the game first requires 300 C-bills from the full 32-bit
balance. Insufficient funds end weapon repair and display the stored out-of-cash
message. An affordable selection performs these steps:

1. subtract 300 through the low-WORD `SUB` and high-WORD `SBB` pair;
2. redraw the C-bill balance;
3. map the visible choice back to its repair type;
4. decrement that type's destroyed-slot count by one;
5. scan all critical offsets `+0x33..+0x55` and clear bit `0x80` from matching
   destroyed component bytes.

If the remaining-count scan finds another type, the dispatcher redraws the
entire menu. Otherwise it displays “All your weapons are fixed.” and advances.

## Original bookkeeping defects

The transaction contains two independently verified faults:

- **BUG-008:** the critical-slot loop clears every byte matching the selected
  destroyed component, while its count is decremented only once. Multiple
  identical slots become intact after the first payment, but stale menu entries
  can demand additional payments. Exiting via “Nothing” retains all mutations.
- **BUG-009:** the menu builder includes indexes `0x00..0x10`, but the
  post-purchase remaining-count scan uses `index < 0x10`. It omits SRM-6 index
  `0x10` and can report completion while a destroyed SRM-6 remains.

These behaviors are preserved in the annotated C and recorded in
[`ORIGINAL_GAME_BUGS.md`](../investigations/ORIGINAL_GAME_BUGS.md).

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:056A-074D`;
- exact strings at `3EDB:1BB5`, `1BD3`, `1C0F`, `1C19`, and `1C35`;
- weapon records `3EDB:2FD7..30F7` with verified `0x11` stride;
- exact menu-build bound `<= 0x10` versus completion bound `< 0x10`;
- complete critical-slot scan and unbroken fall-through after every match.

The index conversions, menu mapping, prices, 32-bit charge, slot mutation,
completion logic, exits, and both original defects are directly verified. No
Astra review is required for this block. Review continues with
[Mech-Lube actuator repair](BTECH_11B8_MECHLUBE_ACTUATOR_REPAIR.md).
