# `BTECH_0DAB` post-battle infantry loot

## Reviewed function

- Address range: `0DAB:094B-0B5D`.
- Annotated name: `Loot_Enemy_Soldiers_Dialog`.
- Called from the combat parent at `183B:1251`.

This function detects lootable infantry casualties, awards pocket money, and
offers recovered personal weapons to the party.

## Flag scan

The initial loop visits combatant IDs `4..11`. All its locals and both flag
accesses are 16-bit words, contrary to the old byte-array declarations.

Any nonzero entry in either of these views enables the cash dialog:

- `3092:393C + combatantId * 2`;
- `3092:3954 + combatantId * 2`.

For IDs `4..11`, the second expression addresses `395C..396A`. These are the
same eight words later accessed as `LootableInfantryFlags_395C[0..7]`. Each
nonzero second-table entry increments the casualty count.

The `3954` view overlaps the end of the sixteen-word `393C` table: indexes
`0..3` at base `3954` are exactly combatant entries `0C..0F` at base `393C`.
These must be documented as overlapping address views, not consecutive arrays
in a future strongly typed state object.

## Cash award

If either flag view contains a qualifying entry, the game displays the pocket-
search message. For every second-table casualty it adds:

```text
(random & 0x0F) + 3
```

giving `3..18` C-bills per flagged infantry record. The final total is forced
to at least `2`; therefore a casualty found only through the first table still
awards two C-bills. The value is added to the 32-bit C-bill balance.

If the weapon-presence flag is set, the exact clause is:

```text
, and confiscate their weapons
```

The comma is part of the stored string and there is no stored period. The
common sentence-period helper supplies the punctuation. The old annotated C
had omitted the comma and embedded a period, which would duplicate punctuation.

## Weapon offers

After the cash message, the routine processes all eight word flags at
`395C..396A`. The corresponding weapon ID is:

```text
Infantry[8 + lootSlot].Weapon
```

or raw address `3092:C6A7 + lootSlot * 0x11`.

When Rex's party record is absent (`Name == 0xFF`), Jason receives a simple
exchange prompt for every nonzero recovered weapon. Accepting replaces Jason's
current weapon immediately, so later offers compare against the latest choice.
The exact prompt is assembled as:

```text
Do you want to drop your <current> in exchange for a <recovered>?
```

When Rex is present, every flagged weapon ID—including zero—is instead passed
to `Distribute_Weapon_To_Party`, which owns the fuller inventory interaction.
Its zero ID is also the transfer loop's finished sentinel, so a recovered
Cudgel produces no menu. See
[`BTECH_0FDC_WEAPON_DISTRIBUTION.md`](BTECH_0FDC_WEAPON_DISTRIBUTION.md).

## Suspicious weapon-presence test

Before showing the confiscation clause or entering the weapon loop, the first
scan tests byte:

```text
3092:C6EB + combatantId * 0x11, combatantId = 4..11
```

Those effective addresses are `C72F, C740, ... C7A6`, crossing the documented
mech-record region rather than the later enemy-infantry weapon fields at
`C6A7 + lootSlot * 0x11`. The instructions are unambiguous, but intent is not.
This is recorded as probable original bug BUG-004 and Astra candidate A-006.
Compatibility code should reproduce the raw test until that review is resolved.
