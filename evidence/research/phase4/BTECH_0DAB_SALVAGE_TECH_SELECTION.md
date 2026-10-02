# `BTECH_0DAB` salvage technician selection

## Reviewed block

- Address range: `0DAB:0002-00CA`.
- Parent routine: `Salvage_Armour_Dialog`.
- Enemy armour accumulation begins at `0DAB:00CB`.

This opening block selects the party member with the best Tech skill and prints
which classes of components that character is qualified to scavenge. Despite
its current narrow name, the complete parent routine continues beyond armour
into structure, heat-sink, weapon, and possibly whole-'Mech salvage.

## Technician selection

The executable visits all eight player character records at
`3092:C614 + partySlot * 0x11`. It skips a record when its byte at `+0x00`
(`Name`) is `0xFF`, then compares byte `+0x09` (`Skill_Tech`) against the best
skill seen so far.

All loop and selection locals are native 16-bit words. The record fields remain
bytes. The previous annotated C incorrectly applied address-of operators to
both fields, turning value comparisons into nonsensical pointer comparisons.

The comparison is strict: a later character replaces the selection only when
their Tech skill is greater. Equal scores retain the lower party slot. Both the
best skill and selected slot begin at zero. This creates an implicit gameplay
invariant: slot zero must remain a valid fallback, or at least one living party
member must have Tech above zero. The assembly contains no explicit all-dead or
all-unskilled fallback in this block.

## Result text and pointer tables

The selected record's Name byte indexes the original 16:16 character-name
pointer table at `3EDB:01CA`. The routine constructs this sentence:

```text
<name> uses his tech training to scavenge armor[skill suffix] from the enemy 'Mechs.
```

The skill suffix is omitted at Tech levels 0 and 1. Levels 2 through 4 select:

| Tech level | Far-pointer address | Target string |
|---:|---:|---|
| 2 | `3EDB:0FA2` | `3EDB:0EAE`, ` and heat sinks` |
| 3 | `3EDB:0FA6` | `3EDB:0EBE`, `, heat sinks and weapons` |
| 4 | `3EDB:0FAA` | `3EDB:0ED7`, `, heat sinks, weapons and structure` |

The assembly indexes these through the pre-biased expression
`3EDB:0F9A + TechSkill * 4`. The old scratch declaration incorrectly placed a
three-element pointer array at `0EAE`, where the first target string actually
resides. The maintained header now names the real table at `0FA2`, and the C
indexes it with `TechSkill - SkillLevel_Average`.

After displaying the sentence, the routine waits for a key and initializes the
armour-salvage pool for the next block.

No Astra confirmation is required for this block: record strides, byte fields,
far-pointer addresses, comparisons, and string bytes are explicit in the clean
assembly and data segment.
