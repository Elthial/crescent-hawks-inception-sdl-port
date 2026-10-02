# `BTECH_0FDC` party weapon distribution

## Review boundary

This block covers the complete function at `0FDC:15E6-17B8`, renamed
`Distribute_Weapon_To_Party`. It receives one native 16-bit stack WORD: the
weapon-table ID currently held for distribution.

Verified callers use it for:

- weapons recovered from fallen enemy infantry when Rex is present;
- weapons purchased in the Citadel shop;
- Laser Pistol or Laser Rifle awards made by Citadel BLD/event handling.

## Living-party menu

The routine initializes an eight-WORD local array to `FFFF`, then compacts the
record IDs of living party members (`Name != FF`) into that array. The former
pseudo-C mistranscribed these local stores as writes into the global infantry
array.

For two or more living party members, the menu is:

```text
Give <held weapon> to:
<character name>  <current weapon>
...
Drop it
```

The final choice clears the held ID and exits. Party records may contain gaps,
so the compact menu index must always be translated through the local map.

## Chained exchange

Choosing a character performs the exchange before any warning:

1. save the recipient's current weapon ID;
2. equip the held weapon;
3. if the two IDs match, display `He already has that weapon.` and wait for
   input;
4. make the displaced weapon the next held item.

The menu therefore repeats until the player drops the held item or the
displaced ID is zero. The same-weapon message is informational: the exchange
has already happened, although swapping identical IDs has no visible effect.

The executable sign-extends the displaced byte with `CBW` before storing it in
the WORD loop variable. All known valid weapon IDs are below `80`, so this has
no effect for normal data but is retained in the annotated C.

## Weapon zero is also the sentinel

Weapon-table ID zero is the Cudgel, but this routine also uses zero to mean
that distribution has finished. Consequently:

- passing a Cudgel returns immediately without showing a menu;
- replacing a Cudgel ends the exchange chain and discards it;
- the Rex-present post-battle caller may pass ID zero, but that call is a
  no-op;
- the Rex-absent post-battle path explicitly skips recovered ID zero before
  using its separate Jason-only prompt.

This appears to be an intentional low-value/default-weapon convention rather
than evidence that weapon ID zero is not a real table entry. A compatible port
should retain it unless inventory behaviour is deliberately redesigned.

## Single-member shortcut

When the living-party count is zero or one, no menu is drawn. The held weapon
is written directly to Jason's record and his previous weapon is discarded.
The zero-member case still writes Jason; ordinary game-state invariants are
expected to prevent that case.

## Corrected transcription errors

The previous pseudo-C had several structural errors:

- local menu-map initialization appeared to overwrite `Infantry[]`;
- the living-party scan and loop condition were assigned unrelated meanings;
- the shortcut wrote the piloting-skill field instead of `Weapon`;
- weapon-name indexing and the recipient address were malformed;
- the exchange and displaced-item loop were not recoverable as written.

The rewritten block follows the assembly's byte and WORD operations while
giving the menu map and exchange state explicit names.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:15E6-17B8`;
- call sites at `BTECH_0DAB.asm:0B4F`, `BTECH_1CD3.asm:0AE2`, and
  `BTECH_1CD3.asm:144A`;
- strings at `3EDB:16AE-16DC`;
- the `0x11`-byte weapon records beginning at `3EDB:2ED8`.

The argument, party filtering, menu, exchange order, shortcut, and zero-ID
termination are directly verified. No Astra review is required.

The next reviewed routine is
[`Talk_To_Building_Occupants`](BTECH_0FDC_BUILDING_OCCUPANT_CONVERSATIONS.md).
