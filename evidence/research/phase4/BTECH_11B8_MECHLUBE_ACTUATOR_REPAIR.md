# `BTECH_11B8` Mech-Lube actuator repair

## Review boundary

This block covers `Mechlube_Repair_Mech` from `11B8:074D` through its far
return at `0809`. It handles actuator repair and the routine's shared final
message/input epilogue. `Mechlube_Modify_Mech` begins at `11B8:080A` and is the
next review block.

## Entry gate

The diagnostic stage compared each current packed actuator byte with its
corresponding maximum and recorded a two-bit WORD:

| Flag | Meaning |
|---:|---|
| `0x01` | current left packed byte differs from maximum left |
| `0x02` | current right packed byte differs from maximum right |

If neither bit is set, the routine returns immediately. The flags only decide
whether service is offered; they do not select individual bytes during repair.

## Offer and affordability

Actuator service has one flat price of 200 C-bills whether one or both packed
side bytes differ. The game compares both WORD halves of the 32-bit balance.
This check occurs after the armour, structure, heat-sink, and weapon stages, so
earlier work in the same visit may make actuator service unaffordable.

With sufficient funds, the shop prints the offer and asks for confirmation.
Declining returns without another message or mutation. With fewer than 200
C-bills, it displays the stored refusal through the shared message-and-input
epilogue.

## Accepted transaction

On acceptance, the executable first displays its completion message and waits
for one keyboard input. It then performs these mutations in order:

1. copy maximum right packed actuator byte `+0x6A` to current right `+0x25`;
2. copy maximum left packed actuator byte `+0x69` to current left `+0x24`;
3. subtract `0xC8` from the 32-bit balance with `SUB`/`SBB`;
4. redraw the balance and return.

Both packed side bytes are restored even when only one comparison failed. This
is harmless for the already-matching side and avoids interpreting the flag as
a field selector.

The restored values come from the chassis record. They are not universally
`0xFF`: intact arm configurations can have maximum high nibbles such as
`0xC0`, so filling every actuator bit would manufacture components the chassis
does not possess.

## Shared final epilogue

The no-repair-needed diagnostic path and the actuator insufficient-funds path
both load a 16-bit string offset into `AX`, branch to `11B8:07F6`, display the
data-segment text, consume one keyboard input at `0800`, and return. The cleaned
C uses a near text pointer to express this shared register-carried value.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:074D-0809`;
- exact strings at `3EDB:1C4B`, `1C70`, `1C97`, `1CA7`, and `1CCE`;
- verified packed actuator fields at mech offsets `+0x24/+0x25` and maxima at
  `+0x69/+0x6A`;
- native 32-bit comparison and `SUB`/`SBB` charge.

The gate, flag semantics, price, dialogue branches, byte-copy order, source
fields, charge timing, epilogue, and returns are directly verified. No Astra
review is required for this block. Review continues with
[Mech-Lube modification setup](BTECH_11B8_MECHLUBE_MODIFICATION_SETUP.md).
