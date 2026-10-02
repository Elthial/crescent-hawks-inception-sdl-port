# `BTECH_0800` character and 'Mech inspection

## Reviewed block

- Address range: `0800:378D-3BCF`.
- Maintained name: `Inspect_Characters`.
- This covers the complete routine.

## Two inspection paths

The routine first constructs the common menu frame. If
`NumberOfActiveLanceMechs` at `3092:D31C` is nonzero, it asks whether the
player wants to inspect a 'Mech. A yes response:

1. lists the available 'Mechs;
2. clears the selected 'Mech's transient heat byte;
3. passes the selected record index to the `BTSTATS.CMP` inspection screen;
4. redraws the sidebars; and
5. returns without drawing a character sheet.

A no response, or having no active 'Mechs, enters the character path. The
dialog helpers leave the selected party-record index in byte `3092:D31A`.
That byte is also used elsewhere as a party count/highest-index scratch value,
so the existing `Party_NumOfMembers` field name has not yet been globally
changed.

## Character-record accesses

Every dynamic character value comes from the same selected 17-byte record at
`3092:C614 + selectedIndex * 0x11`:

| Displayed value | Record field | Lookup |
|---|---|---|
| Name | `+0x00 Name` | `DS:01CA`, eleven 16:16 text pointers |
| Weapon | `+0x0B Weapon` | Inline name at `3EDB:2ED8 + id * 0x11` |
| Armour name | `+0x0D ArmourType` | `3EDB:4E8A`, six 16:16 text pointers |
| Current armour | `+0x0E ArmourValue` | Compared with maximum table `3EDB:4DDB` |
| Health | selected record index | Passed to `1631:02E4` |
| Skills | contiguous bytes `+0x04..+0x0A` | `DS:0194`, five 16:16 text pointers |

The old pseudo-C accidentally reused resolved text pointers, weapon IDs, and
armour IDs as indexes into `Infantry`. Those expressions were decompiler
artefacts caused by split far pointers and lost temporary values; the assembly
always retains the selected record index at `[BP-0A]`.

Skill values above four are clamped for presentation. The exact compact labels
are `Unskilled`, `Amateur`, `Adequate`, `Good`, and `Excellent`.

## Traitor and armour messages

If the traitor is in the party and `Traitor_CharacterId` equals the selected
record index, the screen shows the yellow unease warning and sets
`TraitorWarning`. This comparison is against a party slot, not a name ID or
character-name pointer.

Otherwise, equipped armour receives a status line. Lost points are:

```text
ArmourTypeDurability[ArmourType] - ArmourValue
```

Zero loss prints that the armour has not been injured. A zero current value
prints `all`; other damage prints the numeric number of lost points, followed
by the armour type's maximum protective points.

## Equipment flags

The final lines report three global items as `Yes` or `No`:

| Label | State byte |
|---|---:|
| `MedKit:` | `3092:D450` |
| `Mapper:` | `3092:D33D` |
| `Field Surgery Kit:` | `3092:D451` |

The executable's label block proves that `D33D` is `HasMapper`. The main
exploration loop independently confirms the meaning: while this flag is set,
it reveals all eight fine rows in the current coarse map row unless the party
is inside the Star League Cache.

## Porting notes

- Model all character fields as bytes and indexes as 16-bit values when
  reproducing the original flow.
- Decode the original 16:16 name, skill-label, and armour-label pointers into
  host strings at load time; do not preserve Reko's split offset/segment
  expressions.
- Keep the compact skill labels distinct from the verbose five-entry table at
  `3EDB:4EA2`.
- Treat the `BTSTATS.CMP` screen as an original-asset dependency.

No Astra review is requested. The record stride, lookup addresses, argument
order, branch structure, exact labels, and flag comparisons are explicit in
the clean assembly and executable data.

## Evidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0800.asm`, `0800:378D-3BCF`.
- `BTech-Reko-expanded/BTECH.EXE`, original strings and pointer tables at
  `DS:0194`, `DS:01CA`, and `3EDB:06FF-0896`.
- `Btech/BTECH_0800.c`, `Inspect_Characters`.
- `docs/data-structures/CHARACTER_RECORDS.md`.
