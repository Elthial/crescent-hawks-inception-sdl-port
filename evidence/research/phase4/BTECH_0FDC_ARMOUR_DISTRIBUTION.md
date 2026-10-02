# `BTECH_0FDC` purchased-armour distribution

## Review boundary

This block covers the complete function at `0FDC:13DE-15E5`, renamed
`Distribute_Purchased_Armour`. Its only call occurs after a successful armour
purchase in `Citadel_Building_Dialogs`.

Both arguments are native 16-bit stack WORDs:

1. the armour type currently held for distribution;
2. that armour item's current durability/value.

The purchase call supplies the selected type and its full durability from the
byte table at `3EDB:4DDB`.

## Party-menu construction

The routine first initializes an eight-WORD local array to `FFFF`, then scans
party records `0..7`. Each record whose name/status byte is not `FF` is copied
into the next compact menu slot. Menu indexes therefore do not necessarily
equal party-record IDs when the party contains gaps.

For two or more living members, the menu displays:

- `Give <held armour name> to:`;
- one entry per living party member, showing their name and current armour;
- `Drop it`, followed three lines later by `Armor points left:` and the held
  armour value.

The menu choice count is the living-member count plus one. Selecting the final
entry clears the held type and exits.

The six armour types and full durability values are:

| Type | Name | Full value |
|---:|---|---:|
| 0 | None | 0 |
| 1 | Flak Vest | 25 |
| 2 | Flak Suit | 40 |
| 3 | Lt Env Suit | 30 |
| 4 | Hv Env Suit | 50 |
| 5 | Ablative | 50 |

## Chained exchange

Selecting a party member exchanges two complete byte pairs:

```text
recipient armour type/value = held type/value
held type/value             = recipient's previous type/value
```

The loop continues while the displaced type is nonzero. This lets the player
give the newly purchased armour to one character and then redistribute that
character's old armour to somebody else. Selecting a character who previously
had no armour naturally ends the chain because the displaced type is zero.

If the incoming and displaced types match, the exchange still happens first.
The game then displays `He already has that type of armor.`, waits for input,
and continues while holding the recipient's previous item. This can replace a
damaged item with a full item of the same type; the warning does not cancel or
roll back the exchange.

The assembly sign-extends both displaced bytes into WORD temporaries before
the next iteration. Valid armour types and durability values are below `0x80`,
so this does not alter ordinary state. A port should validate edited/corrupt
save values rather than use a negative result as a table index.

## Single-member shortcut

When the living-party count is zero or one, no menu is shown. The held armour
is written directly to Jason's record, the synthetic choice equals the party
count, and the common drop/finish path clears the held type.

This shortcut does not preserve Jason's previous armour for redistribution and
does not perform the same-type warning. The zero-living-member case also still
writes Jason's record; normal game invariants are expected to make that path
irrelevant.

## Corrected transcription errors

The earlier pseudo-C obscured nearly every central operation:

- its loop condition was reversed, running while armour type was zero;
- the armour-value argument became an uninitialized self-assignment;
- the recipient's old and incoming values collapsed into identical variables;
- the same-type comparison became an unconditional self-comparison;
- raw stack-array expressions hid the compact party-menu mapping.

The rewritten block keeps the executable's transfer order and menu behaviour
while naming the two-item exchange explicitly.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:13DE-15E5`;
- the call at `BTECH_1CD3.asm:0A43` and its preceding purchase flow;
- initialized armour durability bytes at `3EDB:4DDB-4DE0`;
- armour-name far-pointer table at `3EDB:4E8A` and its target strings.

The argument meanings, live-party mapping, menu layout, exchange order,
same-type warning timing, and single-member shortcut are directly verified.
No Astra review is required.

The next reviewed routine is
[`Distribute_Weapon_To_Party`](BTECH_0FDC_WEAPON_DISTRIBUTION.md).
