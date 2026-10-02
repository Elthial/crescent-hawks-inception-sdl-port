# `BTECH_0800` combatant animation stream reader

## Review block (`0800:1732-17BA`)

`0800:1732` advances one combatant's compact animation stream and returns the
next sprite-frame ID in `AL`. The old maintained name
`Select_As_Target_Variable_1732` came from a bad Reko reconstruction: the
routine does not select a weapon target.

### Per-combatant stream cursors

`DS:01F6`, now named `CombatantAnimationCursor_01F6`, is a table of 25 mutable 16:16 far pointers, indexed as
`combatantId * 4`. Each invocation reads through that combatant's current
cursor and advances only its 16-bit offset word. The initialized table occupies
exactly `DS:01F6-0259`: the first four entries point to `2FE8:0290`, most later
combatant entries point to `2FE8:02D0`, and the final special slot (ID `18`)
points to `2FE8:0290`. Calls using ID `18` confirm that this is not merely a
24-combatant table.

Reko's 32-bit analysis confused this array of four-byte far pointers with a
single host pointer. The maintained pseudocode now keeps the four-byte stride
and 16-bit far-pointer meaning explicit.

### Bytecode

The stream reader sign-extends each byte before dispatching it:

| Raw byte | Meaning |
|---:|---|
| `00..7F` | Return this sprite-frame ID in `AL`. |
| `FD xx` | Set `3092:396C[combatantId]` to selector `xx`, then continue. |
| `FE dd` | Subtract signed byte `dd` from the cursor offset, then continue. |
| `FF` | Rewind one byte and poll the same location again; probable wait sentinel. |
| `80..FC` | Consume and skip; no more specific meaning is yet evidenced. |

`FE` does not consume its operand before applying it. For example, the shipped
sequence ending `... 08 09 0A 0B FE 05` reads distance five at the current
cursor, subtracts five, and returns to frame `08`. It therefore cycles those
four frame IDs indefinitely.

### Static animation families

The data at `2FE8:0270-02EF` consists of sixteen-byte families. Each half uses
`FD`, a selector, four ordinary frame IDs, then `FE 05`. Selectors `0..7` are
represented. Callers replace a combatant's stream cursor when its selector at
`3092:396C` changes; after this routine returns, they store `AL` in the
per-combatant sprite-frame table at `3092:409A`.

This establishes the roles of the three principal fields:

- `DS:01F6`: current animation-stream cursor per combatant.
- `3092:396C`: requested/current animation selector, likely direction or pose
  family depending on the calling context.
- `3092:409A`: current sprite-frame ID returned by the stream reader.

The movement writer at `0800:231D` confirms that selectors `0..7` are compass
directions in the order north, north-east, east, south-east, south, south-west,
west, and north-west. It selects separate eight-entry mech and infantry walk
tables at `DS:025A` and `DS:027A`.

### Open item

- Confirm the producer for an `FF` wait sentinel. Its rewind-and-reread behavior
  is exact, but none of the static loops at `2FE8:0270-02EF` contains `FF`.
- The formerly overlapping declarations at `3092:396C/3978` and
  `3092:409A/40A6` have now been reconciled as complete 24-byte combatant
  tables: `CombatantAnimationSelector_396C` and `CombatantSpriteFrame_409A`.

### Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:1732-17BA`.
- `BTech-Reko-expanded/BTECH.reko/BTECH_2FE8.asm`, `2FE8:0270-02EF`.
- Callers at `0800:2388`, `0800:23D9`, and `0800:276D`, which store returned
  `AL` into `3092:409A[combatantId]`.
