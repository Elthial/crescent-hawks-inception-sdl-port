# Segment roles and code map

Sol: Refreshed 2026-09-17 from the completed audits. This is an address
catalogue, not a complete allocation map. V describes individual ASM/byte-backed
views, not gameplay certification or every byte of a segment. Ranges below
are half-open; analysis segments still require live relocation mapping.

## Data segments

| Segment | Confidence | Current role | Important areas |
|---|:---:|---|---|
| `246C` | P | Graphics and map workspace | Drawing scratch near `0220`; nine-map grid near `0564`; seed table `09FB`; tile buffer `101D`; decompressed graphics near `244B`; ANM data near `42C3`; party world position `A44B/A44D`; video state near `B764`. |
| `2FE8` | P | Static gameplay data/templates | Map-selection data near `0030`; current-map byte `00FC`; CGA tables; eight `0x7D` mech templates at `02F0..06D8`. |
| `3056` | V/U | Legacy adapter-0 colour source | BTSTATS passes FAR3056:0000 to the old colour conversion helper; full inventory unknown. Deleted graphics paths need not be restored. |
| `3058` | V/U | Ending palette | Ending code passes FAR3058:0000 as the EGA palette. Full inventory unknown. |
| `305B` | H | UI/menu scratch and settings | Menu geometry near `00C2`; party/menu counters near `0202`; movement settings near `0302`. `BTECH.h` defines this segment twice. |
| `3092` | P | Primary mutable state and large work buffers | BLD/image buffer near `00A0`; combat/map scratch; saved state `C614..D558`; post-save runtime state after `D558`. |
| `3EDB` | P | Strings, lookup tables, settings, and static data | UI globals near `014A`; text pointer tables; BLD/animation tables; mech template pointer table `2DF8`; weapon records `2ED8`; filenames/text around `47xx`; graphics/random globals near `4FB8`. |

These roles are organizational descriptions, not proof that a whole 64 KiB
segment has a single owner or lifetime.

## Major code segments

| Segment/file | Current responsibility | Confidence |
|---|---|:---:|
| `0800` | Startup/main loop, menus, save/load, maps, animation orchestration | P |
| `0D27` | Reconstructed EGA startup, installation flags, asset loading and 376 sprite captures | V/P |
| `0DAB` | Combat setup, enemy generation, salvage and combat utilities | H |
| `0FDC` | BLD load/decrypt/interpreter and room handling | P |
| `11B8` | Mech-Lube repair/upgrades/ammo, recruitment and scripted interactions | V/P |
| `135D` | Star League cache, security terminals, transmitter, map room, door/star puzzles | V/P |
| `1431` | Healing, medic selection and health/recovery calculations | V/P |
| `1467` | Crew assignment, written quiz, mech selection and movement-plan reset | V/P |
| `1543` | Weapon planning/targets, destruction support and numeric input | V/P |
| `1631` | Combat AI/target/range/path planning, occupancy/crushing, turning, heat and critical damage | V/P |
| `183B` | Large combat/encounter workflows | H |
| `1AE8` | Attack resolution, personnel/mech damage, hit locations, wrecks and audiovisual effects | V/P |
| `1CD3` | Building interaction dispatcher | P |
| `1E56` | Text/menu rendering and keyboard mapping | P |
| `1F3D` | Retrace/input, EGA text/geometric wrappers, file/allocation contracts and sprite capture | V/P |
| `1FC5` | Synchronous sound-effect dispatch, five repetition wrappers, tone/noise/divisor sweeps | V/P |
| `204B` | IRQ0/PIT binding and PC-speaker/Tandy music streams | V/P |
| `207F` | Low-level graphics, blitting, tiles, and DOS helpers | P |

The responsibility labels guide review order. Individual function findings must
still cite their own evidence.
The first-pass audits cover retained gameplay paths, not every original ASM
export. Dormant/invalid-selector music paths, DOS internals and deleted graphics
paths are not thereby reconstructed. See the bounded audits in the
[documentation index](../README.md), especially the
[startup/music audit](../phase4/BTECH_STARTUP_MUSIC_PLATFORM_AUDIT.md),
[1FC5 consistency](../phase4/BTECH_1FC5_FINAL_CONSISTENCY.md) and
[1631 review](../phase4/BTECH_1631_SEGMENT_REVIEW.md).

## High-value static tables in `3EDB`

| Address | Confidence | Table |
|---|:---:|---|
| `3EDB:01CA..01F6` | V | Eleven four-byte FAR character-name pointers; see the 0800 character-inspection audit. |
| `3EDB:141A` | P | 26 building-entry animation IDs. |
| `3EDB:21CE..21EF` | V | 33 BYTE security-terminal X positions. |
| `3EDB:21F0..2211` | V | 33 BYTE security-terminal Y positions; 33 colour BYTEs at2212..2233. |
| `3EDB:2DF8` | V | Five far pointers to the Locust, Wasp, Stinger, Commando, and Jenner templates in segment `2FE8`; random encounters use the first three. |
| `3EDB:2EBC` | H | Four weapon-range text references. |
| `3EDB:2ED8..3109` | V | 33 weapon records with `0x11` stride; most field meanings established. |
| `3EDB:4EC2..4F2A` | V | 26 four-byte FAR BLD filename pointers. |
| `3EDB:4FB8` | P | Graphics dimensions/adapter state. |
| `3EDB:4FC0..4FC3` | V | Three active RNG BYTEs; WORD seed writes overlap this view. 4FC3 is not a fourth RNG byte. |
| `3EDB:5008..527A` | V | 313-WORD sound-effect library, not a BYTE stream. |

The weapon base/stride and post-name offsets are established in the
[weapon reference](../data-structures/WEAPON_RECORDS.md); the high heat/effect
nibble remains unknown. Table evidence: character inspection, BLD load/decode,
135D final consistency, 207F RNG and 1FC5 dispatch audits under `docs/phase4`.

## Other address spaces

EGA display A000:0000, staging framebuffer A800:0000, tile source A400:0000
and map framebuffer AC00:0000 are presentation-address views, not host pointers.
A800 is not evidence of a separate VGA renderer. Music owns writable CS204B
views too: stream FAR0205/0207 and cadence BYTE0209/020A are not 3092 save data.
