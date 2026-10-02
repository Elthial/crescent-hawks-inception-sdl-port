# `BTECH_0DAB` BTSTATS actuator display

## Review boundary

This block covers `0DAB:1F86..21BE` within `Examine_Screen_BTSTATS_CMP`.
It draws the four actuator rows and classifies each packed limb state as
` OK`, `Hit`, or `Gone`. The following armour/structure redraw and input loop
is outside this block.

## Verified packed layout

The labels and comparisons establish the complete byte/nibble ownership:

| Record field | Mask | BTSTATS row |
|---|---:|---|
| `CurrentActuators[0]` (`+0x24`) | `0x0F` | Left Leg |
| `CurrentActuators[1]` (`+0x25`) | `0x0F` | Right Leg |
| `CurrentActuators[0]` (`+0x24`) | `0xF0` | Left Arm |
| `CurrentActuators[1]` (`+0x25`) | `0xF0` | Right Arm |

The matching maximum bytes are `MaxActuators[0]` at `+0x69` and
`MaxActuators[1]` at `+0x6A`. Individual bits within each nibble are not yet
assigned to joints such as hip, knee, shoulder, or hand.

## Status rules

Leg rows use an absolute intact value:

- current low nibble `== 0x0F`: ` OK`, bright white;
- current low nibble `== 0x00`: `Gone`, destroyed-status colour;
- otherwise: `Hit`, bright white.

Arm rows compare the current and chassis-maximum high nibbles:

- current high nibble `==` maximum high nibble: ` OK`, bright white;
- otherwise, current high nibble `== 0x00`: `Gone`, destroyed-status colour;
- otherwise: `Hit`, bright white.

Equality is tested before zero. Reference records contain intact arm maxima of
both `0xC0` and `0xF0`, so replacing the maximum comparison with a fixed
`0xF0` test would incorrectly mark some chassis as damaged.

## Recovered text

The clean executable confirms `DS:1265` is `" OK"` (including its alignment
space), `DS:1269` is `"Hit"`, `DS:126D` is `"Gone"`, and `DS:12E0` contains
the four labels separated by carriage returns. Earlier pseudo-C integers and
`SEQ(...)` expressions were decompiler failures, not encoded text values.

## Type notes

The mech record stores each actuator field as a byte. The 16-bit instructions
zero-extend and manipulate those bytes in word registers, and text pointers are
far pointers. No 32-bit integer or pointer is implied by Reko's former `int`
declarations.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0DAB.asm`, `1F86..21BE`;
- expanded executable strings at `3EDB:1265`, `1269`, `126D`, and `12E0`;
- reference mech templates at `2FE8:02F0` with actuator maxima `0xCF` or
  `0xFF`.
