# `BTECH_0DAB` post-battle scrap payout

## Reviewed block

- Address range: `0DAB:042A-04F2`.
- Parent routine: `Salvage_Armour_Dialog`.
- This block completes the routine; `Salvage_Mechs_Dialog` starts separately at
  `0DAB:04F9`.

This tail calculates a randomized C-Bill payment for scrap metal, reports it to
the player, and adds it to the 32-bit party balance.

## Entry gates

The payout is skipped when either:

- the earlier armour pass counted no enemy mech wrecks; or
- `BestTechMemberId` is zero.

The second test is explicitly against the selected technician's **party slot**,
not their Tech skill. Consequently Jason in slot zero receives no scrap payout
even if he is the selected technician and wrecks are available.

## Calculation

For every counted wreck, the routine adds:

```text
(RandomByte() & 0x007F) + 0x005A
```

Each wreck therefore contributes a uniformly derived value from 90 through
217 inclusive before multiplication. The 16-bit sum is then multiplied with a
signed `IMUL` by `BestTechMemberId`, again the party slot index. It is not
multiplied by `HighestTechSkill`.

With at most four wrecks and a nonzero slot index from 1 through 7, the observed
calculation ranges from 90 to 6076 C-Bills and remains positive within a signed
16-bit word. The existing C incorrectly narrowed this result to a byte.

Using the party slot both as an entry gate and multiplier appears to be an
original variable-selection bug: payout changes according to where the best
technician happens to be stored, while slot zero receives nothing. The clean
assembly leaves no ambiguity about which local is read, so preservation code
should reproduce it unless offering an explicit corrected-rules mode.

## Display and balance update

The routine displays:

```text
You are able to scrounge together <value> C-bills worth of scrap metal from the destroyed 'Mech[s].
```

The numeric value is printed in bright green. The suffix stored at
`3EDB:0F65` begins with control bytes `06 0F`; the text renderer interprets
these as a colour change to bright white. The earlier C omitted those bytes.
An `s` is appended only when more than one wreck was counted, followed by the
period in all cases.

Finally `CWD` sign-extends the 16-bit payout and adds it to the two-word C-Bill
balance at `3092:D370:D372`. This is a genuine 32-bit balance update performed
from a 16-bit calculated reward.

No Astra confirmation is required: both gate locals, the random expression,
the multiplication operand, text controls, and 32-bit addition are explicit in
the clean assembly.
