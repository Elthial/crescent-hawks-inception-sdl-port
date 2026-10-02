# `BTECH_0DAB` random enemy 'Mech generation

## Reviewed range

- Address range: `0DAB:0F0B-0FBE`.
- Enemy-infantry placement state is initialized at `0FBF` and its scan begins
  at `0FD2`.

## Record and combatant IDs

The loop visits combined live 'Mech record IDs `4..7`, which are the four
enemy records at `3092:C918..CB0B`. Each destination's first name/status byte
is set to `FF` before the generation tests, making the slot unused by default.

An enemy 'Mech record ID becomes combatant ID `12..15` by adding eight. That
combatant ID selects its byte in the sprite-family table at `3092:D55E`.

## Friendly-strength gate

Each enemy slot is populated only if both conditions pass, in this order:

1. `Rand() & 1` is nonzero, giving a 50% chance;
2. the corresponding friendly 'Mech record's first byte is not exactly `FF`.

The assembly expresses the second address as:

```text
3092:C530 + enemyRecordId * 0x7D
```

For enemy record IDs `4..7`, that is algebraically identical to:

```text
3092:C724 + (enemyRecordId - 4) * 0x7D
```

It therefore addresses friendly records `0..3`, not an unknown array beginning
at `C530`. Random encounters can generate at most one enemy 'Mech opportunity
per occupied friendly lance slot. Because every opportunity also has its own
coin flip, zero enemy 'Mechs remains possible.

The comparison is specifically against the complete `FF` sentinel. The code
does not perform the broader probable high-bit status test here.

## Template choice and copy

For a permitted slot, another random value is divided by three and the signed
`IDIV` remainder selects template index `0..2`:

| Index | Reference address | Chassis |
|---:|---|---|
| 0 | `2FE8:02F0` | Locust |
| 1 | `2FE8:036D` | Wasp |
| 2 | `2FE8:03EA` | Stinger |

The selected 16:16 pointer comes from `3EDB:2DF8`. The routine copies exactly
`0x7D` bytes from that template into the enemy record, producing a complete
stock 'Mech record rather than initializing fields individually.

The initialized data proves that `3EDB:2DF8` contains five template pointers,
not the eight previously suggested by the scratch header:

```text
2FE8:02F0  Locust
2FE8:036D  Wasp
2FE8:03EA  Stinger
2FE8:0467  Commando
2FE8:0561  Jenner
```

The contiguous reference area in segment `2FE8` still contains eight records;
this five-pointer selection table simply does not reference all of them.

## Sprite family

After copying the record, Locust receives sprite-family base `00`. Wasp and
Stinger both receive `92`, the shared upright/humanoid 'Mech artwork family.
No sprite-family write occurs for an unused enemy slot, but its `FF` record
sentinel prevents stale family data from making that slot active.

## Confidence

Loop bounds, record arithmetic, conditional order, far-pointer values, copy
length, template choices, and sprite-family writes are directly verified from
the clean assembly and initialized data. No Astra review is required.
