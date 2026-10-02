# Asset and game identifiers

This file extracts stable identifier lists from `BTECH.h`. Spelling is normalized
for readability while original numeric values and noteworthy source misspellings
are retained in notes.

## BLD file index

The index is contiguous and matches the 26 filenames present in the original
installation. **Verified** from the 26 native 16:16 filename pointers beginning
at `3EDB:4EC2` and their target strings.

| ID | File | ID | File |
|---:|---|---:|---|
| `0x00` | TRAINING.BLD | `0x0D` | JAIL.BLD |
| `0x01` | CITADEL.BLD | `0x0E` | MAYOR.BLD |
| `0x02` | COMSTAR.BLD | `0x0F` | WEAPON2.BLD |
| `0x03` | WEAPON.BLD | `0x10` | THEATER.BLD |
| `0x04` | ARMOR.BLD | `0x11` | FROB.BLD |
| `0x05` | REPAIR.BLD | `0x12` | VIEWDISK.BLD |
| `0x06` | BARRACKS.BLD | `0x13` | BARRACK2.BLD |
| `0x07` | LOUNGE.BLD | `0x14` | ENTRANCE.BLD |
| `0x08` | GARAGE.BLD | `0x15` | HUT.BLD |
| `0x09` | HOSPITAL.BLD | `0x16` | ENDMECH.BLD |
| `0x0A` | ARENA.BLD | `0x17` | INSTRUCT.BLD |
| `0x0B` | PARTY.BLD | `0x18` | FINDIT.BLD |
| `0x0C` | CLOTHES.BLD | `0x19` | WINSCENE.BLD |

The old symbol `BLD_COMSTART` is a typo for COMSTAR. A late string note spells
`CLOTHESE.BLD`, while the actual file and index symbol use `CLOTHES.BLD`.

The original floppy selector groups these IDs as follows:

| Logical disk | BLD IDs | Files |
|---|---|---|
| Game Disk 1 | `0x02..0x10` | COMSTAR through THEATER |
| Disk 2 | `0x00..0x01`, `0x11..0x19` | TRAINING/CITADEL, then FROB through WINSCENE |

Hard-disk installations retain the logical disk number but do not switch the
DOS default drive.

## MTP map index

| ID | File | Current description | Confidence |
|---:|---|---|:---:|
| `0x01` | MAP1.MTP | Citadel/training center | P |
| `0x02` | MAP2.MTP | Starport/city | P |
| `0x03` | MAP3.MTP | Prison | P |
| `0x04`–`0x0A` | MAP4–MAP10.MTP | Village/local maps | H |
| `0x0B` | MAP11.MTP | Destroyed Citadel | P |
| `0x0C` | MAP12.MTP | Inventor's hut | P |
| `0x0D` | MAP13.MTP | Cache exterior | P |
| `0x0E` | MAP14.MTP | Star League cache | P |
| `0x0F` | MAP15.MTP | Star map | V |

## Tileset IDs

| ID | Name | Likely external source |
|---:|---|---|
| `0x00` | BattleTech | BTTLTECH.ICN, with ANIMATE.ICN overlays |
| `0x01` | Destruct | DESTRUCT.ICN |
| `0x02` | Star League cache | STARLEAG.ICN |
| `0x03` | Map room | MAP.ICN |
| `0xFF` | None | No tileset |

The numeric IDs are **Probable**. File associations must be verified at each
load branch rather than inferred from names alone.

## ANM scene IDs

These IDs match filenames `O0.ANM` through `O21.ANM`. Descriptions are the
hand-authored interpretations in `BTECH.h`, so confidence varies.

| ID | File | Current description | Confidence |
|---:|---|---|:---:|
| `0x00` | O0.ANM | Mech startup, including the eight-frame failed-start prefix | V |
| `0x01` | O1.ANM | Infantry/Manpack weapon versus mech | H |
| `0x02` | O2.ANM | Neurohelmet scene | H |
| `0x03` | O3.ANM | Cockpit hit | H |
| `0x04` | O4.ANM | Locust firing | H |
| `0x05` | O5.ANM | Neurohelmet smoking/failure | H |
| `0x06` | O6.ANM | Crescent Hawk card | H |
| `0x07` | O7.ANM | Wasp firing | H |
| `0x08` | O8.ANM | Siren/alarm | H |
| `0x09` | O9.ANM | Jenner step | H |
| `0x0A` | O10.ANM | Hyperpulse-generator transmission | V |
| `0x0B` | O11.ANM | DropShip arrival | V |
| `0x0C` | O12.ANM | Katrina | H |
| `0x0D` | O13.ANM | Jason bowing | H |
| `0x0E` | O14.ANM | DropShip departure / “THE END” | V |
| `0x0F` | O15.ANM | Lyran blast door | H |
| `0x10` | O16.ANM | Wasp loses arm | P |
| `0x11` | O17.ANM | Citadel secretary/banner | H |
| `0x12` | O18.ANM | Mech technicians/shop | H |
| `0x13` | O19.ANM | Draconis hall | H |
| `0x14` | O20.ANM | Arena poster | H |
| `0x15` | O21.ANM | Weapon-shop owner | H |

The three formerly unlabeled sequences are verified by both their complete
decoded imagery and their invocation order in `WINSCENE.BLD`: O10 is invoked at
decoded payload `+0x0005` immediately before the Hyperpulse-generator message;
O11 at `+0x00EB` immediately before the DropShip arrival/landing narration; and
O14 at `+0x0B87` after the Crescent Hawks board the DropShip. O14 visibly ends
with the words “THE END.”

## Sound-effect IDs

These are the historical labels attached to IDs passed through the sound path.
The numeric range is preserved; descriptions are **Hypothesis** until each call
site and the corresponding sound event are checked.

| ID | Scratch-pad label |
|---:|---|
| `0x00` | Unknown/default |
| `0x01` | Missile |
| `0x02` | Kick-related |
| `0x03` | Repeating projectile |
| `0x04` | Infantry-related |
| `0x05` | Vibroblade |
| `0x06` | Single-shot projectile |
| `0x07` | Arena destruction-related |
| `0x08` | Terrain damage-related |
| `0x09` | Laser-related |
| `0x0A` | Cache/grinding-door-related |
| `0x0B` | Bow string |
| `0x0C` | Mech startup |
| `0x0D` | Blade impact |
| `0x0E` | Mech startup failure |
| `0x0F` | Map interaction |
| `0x10` | Password accepted |
| `0x11` | Password rejected |
| `0x12` | Character crushed by mech |

## Gameplay enums

### Movement mode

| ID | Meaning |
|---:|---|
| `0x00` | Walk |
| `0x01` | Run |
| `0x02` | Jump |

### Skill level

| ID | Current concise name | Display wording found in executable notes |
|---:|---|---|
| `0x00` | Unskilled | completely unskilled |
| `0x01` | Amateur | an amateur |
| `0x02` | Competent | competent |
| `0x03` | Whiz | a whiz |
| `0x04` | Grand master | a grand master |

The older constants call levels 2–4 Average, Good, and Excellent. A future C#
enum should use neutral ordinal names or preserve display text separately.

### Range bracket

| ID | Meaning |
|---:|---|
| `0x00` | Short |
| `0x01` | Medium |
| `0x02` | Long |
| `0x03` | Out of range |

### Medical equipment/service level

| ID | Meaning |
|---:|---|
| `0x00` | None |
| `0x01` | Torn cloth |
| `0x02` | Medkit |
| `0x03` | Field surgery kit |
| `0x04` | Hospital facilities |

## Mission IDs

The scratch header labels mission IDs `0x00` through `0x09`, but only some names
are currently meaningful. Treat the entire mapping as **Hypothesis** pending
dispatch-table tracing:

| ID | Current label |
|---:|---|
| `0x00` | South-east corner/training start |
| `0x01` | Rubble pickup |
| `0x02` | Disabled Locust |
| `0x03` | Infantry robots |
| `0x04` | Locust without computer control |
| `0x05` | Two Locusts |
| `0x06` | Three Locusts |
| `0x07` | Kurita Jenners |
| `0x08` | Unknown |
| `0x09` | Jail break |
