# `BTECH_11B8` Mech-Lube armour repair

## Review boundary

This block covers `Mechlube_Repair_Mech` from `11B8:01E2` through `032E`.
It handles the zero-cash exit, quotes armour work, and installs as many armour
points as the player accepts and can afford. Internal-structure repair begins
at `11B8:032F` and is the next review block.

## Zero-balance gate

The game tests both WORD halves of the 32-bit C-bill balance. If their OR is
zero, it prints the stored no-money message, appends the common shop refusal,
waits for input, and returns from the complete repair routine. A nonzero high
WORD therefore counts correctly even when the low WORD happens to be zero.

## Quote

When armour is missing, the shop quotes:

```text
repair cost = total missing armour points * 4 C-bills
```

The missing-point total is the WORD accumulator calculated by the preceding
diagnostic block. Declining the prompt changes neither the mech nor the
balance; execution continues into the internal-structure stage.

## Per-point transaction

Accepted repairs visit all eleven armour bytes in their physical record order:
right arm, right leg, right torso, head, centre torso, left arm, left leg, left
torso, then right-, centre-, and left-torso rear armour. For each location the
routine calculates `MaximumArmour - CurrentArmour` and repeats this transaction
while points remain and at least four C-bills are available:

1. subtract four from the full 32-bit balance;
2. decrement the location's missing-point WORD;
3. decrement the overall missing-armour WORD;
4. increment that location's current-armour byte;
5. redraw the displayed C-bill balance.

The native insufficient-funds path clears only its temporary per-location
count. The outer loop still visits later locations, which immediately fail the
same four-C-bill check. This does not change any later armour bytes.

## Partial repairs

Each successful point is retained immediately. If funds run out before the
overall WORD reaches zero, the shop reports that it completed as much work as
the player could afford and waits for one keyboard input before continuing.
There is no rollback and no second confirmation.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:01E2-032E`;
- stored strings at `3EDB:1879`, `1899`, `18BF`, `1902`, `1919`, and `192A`;
- verified current/max armour fields at mech offsets `+0x11/+0x56`;
- the `SUB`/`SBB` pair used to charge the 32-bit balance.

The branch flow, field indexes, WORD temporaries, 32-bit balance operations,
price, repair order, and partial-repair behaviour are directly verified. No
Astra review is required for this block. Review continues with
[Mech-Lube internal-structure repair](BTECH_11B8_MECHLUBE_STRUCTURE_REPAIR.md).
