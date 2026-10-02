# BTECH_11B8 complete Mech-Lube upgrade packages

## Scope

This note covers all eight package targets in `Mechlube_Upgrade_Mech`, native
addresses `11B8:09D3-0D30`, plus the common completion path at `0D41-0D57`.
The selector jump table and purchase ordering are documented separately in
[`BTECH_11B8_MECHLUBE_MODIFICATION_PURCHASE.md`](BTECH_11B8_MECHLUBE_MODIFICATION_PURCHASE.md).

Every package recalculates `Mech_Selected * 0x7D` and applies byte mutations to
that party-mech record. The cleaned source uses one `Mech *SelectedMech` for the
same address. This is a source-level pointer convenience, not evidence of a
32-bit original pointer; the native code uses a 16-bit record offset and a
separately loaded segment.

## Package summary

| Selector | Native block | Chassis/stage | Executable mutations |
|---:|---:|---|---|
| `0` | `09D3-0A0F` | Locust 1 | arm entries `+33/+41 = 11`; ammo states `0..1 = FF`; level `=1` |
| `1` | `0A10-0A97` | Wasp 1 | `+4F = 11`; ammo state `1 = FF`; both armour arrays copied from reference Locust; level `=1` |
| `2` | `0A98-0AF2` | Stinger 1 | `+33/+34/+42/+43 = 10`; ammo states `0..4 = FF`; level `=1` |
| `3` | `0AF3-0B75` | Commando 1 | `+41/+4A = 11`; old ammo state 2 moved to 3; states `1..2 = FF`; both armour arrays replaced; level `=1` |
| `4` | `0B76-0BBE` | Locust 2 | walk `=7`; engine hits `=0`; `+3B/+49 = 10`; ammo states `3..4 = FF`; level `|=2` |
| `5` | `0BBF-0C31` | Wasp 2 | jump `=0`; `+33/+53/+55 = 11`; ammo states `2..4 = FF`; level `|=2` |
| `6` | `0C32-0CB9` | Stinger 2 | jump `=0`; both armour arrays receive reference-Locust current armour; `+53/+54 = 11`; ammo states `5..6 = FF`; level `|=2` |
| `7` | `0CBA-0D30` | Commando 2 | `+4F..+54 = 10`; `+55 = 11`; all ten ammo states `=FF`; level `|=2` |

Component IDs `0x10` and `0x11` are small and medium lasers. An ammo state of
`0xFF` is the energy-weapon/infinite-ammo sentinel. The table deliberately gives
raw record offsets alongside the current field names because the anatomical
critical-group mapping remains queued as Astra candidate A-005.

## First-stage details

### Wasp

The Wasp case replaces raw critical byte `+0x4F` with a medium laser and changes
ammo-state slot one to `0xFF`. It then loops over record offsets `0x11..0x1B`.
For each location it copies the reference Locust's current armour to the Wasp's
current array and the reference Locust's maximum armour to its maximum array.

The source addresses are `2FE8:02F0 + offset` and
`2FE8:0335 + offset`; the latter bias reaches the Locust's `+0x56..+0x60`
maximum-armour bytes. Both reference profiles currently contain
`{04,08,08,08,0A,04,08,08,02,02,02}`.

### Stinger

Four arm-group bytes become small lasers. The loop uses an inclusive upper
bound and writes ammo-state slots zero through four—not merely four entries.
It then joins the Wasp block's final assignment of level one.

### Commando

Two critical bytes become medium lasers. Before ammo-state slot two becomes
`0xFF`, its current and maximum values are copied to slot three. This preserves
the remaining ammunition-using weapon after the earlier weapon is replaced.

The armour loop was substantially wrong in the old pseudo-C. Native code reads
through biased address `[3EDB:1E13 + recordOffset]` for offsets `0x11..0x1B`,
which resolves to the eleven bytes at `3EDB:1E24`:

```text
08 09 0C 09 0C 08 09 0C 03 04 03
```

Each value is written to both current and maximum armour. It is not a constant
`0x11`, nor does this loop have twelve iterations.

## Second-stage details

### Locust

The case lowers/stores walk movement as seven, clears record byte `+0x75`, adds
two small-laser component bytes, and makes ammo-state slots three and four
infinite. Byte `+0x75` is the verified `EngineHits` counter. The previous
annotation calling it an engine-model field was false; the executable contains
no separate engine-model write here.

### Wasp

Jump movement is set to zero. Three raw critical bytes become medium lasers and
ammo-state slots two, three, and four become infinite. The loop is exactly three
iterations.

### Stinger

Jump movement is set to zero. Both destination armour arrays are filled from
the reference Locust's **current** armour profile. Native code does not read the
Locust maximum array for this case, even though the pristine profiles currently
contain equal values. Two medium-laser bytes are installed, and the corresponding
ammo states are slots five and six. The old pseudo-C incorrectly used slots
four and five.

### Commando

The native loop writes small-laser ID `0x10` to every raw critical offset from
`+0x4F` through `+0x54` inclusive: six byte positions. It then writes medium
laser ID `0x11` to `+0x55` and changes all ten current/max ammo-state pairs to
`0xFF`. The old pseudo-C incorrectly wrote a small laser at `+0x55`.

The retained historical description says this package adds one medium and two
small lasers, whereas the executable performs six `0x10` writes. The exact
bytes are verified, but whether the runtime exposes six independent weapons or
interprets some repeated critical bytes as component occupancy belongs to the
broader weapon-linkage review in Astra candidate A-004. The port must preserve
the raw executable behavior until that semantic question is resolved.

## Upgrade state and completion

All first-stage paths assign `UpgradeLevelFlags = 1`. All second-stage paths OR
in `2`, preserving stage one and normally producing value `3`; modification
setup refuses further work at exactly that value.

Every accepted selector reaches the common text at `3EDB:1DEE`:

```text
\r\rYour 'Mech is done.  It's waiting for you outside.
```

The routine then waits for one keyboard input and returns. As documented in
BUG-010, unsupported charged selector eight skips mutation but still reaches
this same completion text.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_11B8.asm`, `11B8:09D3-0D57`;
- reference records in `BTECH_2FE8.asm`, especially Locust `2FE8:02F0-036C`;
- embedded Commando armour bytes in `BTECH_3EDB.asm`, `3EDB:1E24-1E2E`;
- verified 125-byte `Mech` stride and byte field offsets;
- component IDs and the ten paired current/max ammo-state bytes.

Instruction widths, loop bounds, raw offsets, copied data, state writes, and
completion flow are verified. Only the higher-level critical-slot/weapon
semantics called out under A-004/A-005 remain open; no new Astra candidate is
needed.
