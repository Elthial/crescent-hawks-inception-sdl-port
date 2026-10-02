# Segment 3092 mutable and saved state

Sol: Refreshed 2026-09-17. All explicit ranges here are half-open. Rows may
represent overlapping views, not exclusive allocations. V is instruction/byte
confirmation, not live gameplay validation; unknown gaps remain unassigned.

## Region map

| Address range | Size | Confidence | Meaning |
|---|---:|:---:|---|
| `0000..00A0` | `0x00A0` | H | General runtime/combat scratch, animation pointers, settings. |
| `0012..001B` | `0x0009` | V | Shared text scratch; BTSTATS copies a mech name at `0012` and writes a NUL at `001A`, limiting its displayed name to eight bytes. |
| `006E..0076` | `0x0008` | V | Eight signed byte mech heat levels. BTSTATS indexes without scaling, clamps the selected value to `0..30`, and uses it for the heat gauge. |
| `0076..0078` | `0x0002` | V | Signed WORD cost of the currently selected Mech-Lube modification package. Native purchase code sign-extends it with `CWD` while operating on the 32-bit C-bill balance. |
| `00A0..31FB` | `0x315B` | V | Minimum shared-buffer extent proved by the shipped `BTSTATS.CMP`: its 12,635-byte payload is loaded at `00A0`. This is a minimum usable extent, not proof that the native allocation ends at `31FB`. |
| `00A0..23C8` | `0x2328` | V | Exact byte span transformed in place by every original BLD load. The actual length-prefixed payload may be shorter. |
| `27B0..2BAF` | `0x03FF` | V | Byte-oriented DEMOFILE attractor/self-play instruction stream; the old `short[0x03FF]` declaration is wrong. |
| `3748..3780` | `0x38` | H | Text layout and combat scratch. |
| `3780..37FD` | `0x7D` | V | Complete mech-record backup used while an Arena entrant is staged in live mech slot zero. |
| `3928..3A00` | variable | H | Combatant presence, wreck/death flags, positions, and visibility arrays. |
| `3928..3938` | `0x0010` | V/P | Enemy display/spawn-state bytes indexed by enemy record ID `4..15`; initialization values mirror animation directions, but complete semantics remain probable. |
| `393C..396C` | `0x0030` | V | 24-WORD combatant wreck/death-state table. |
| `396C..3984` | `0x18` | V | Byte animation selector for each of 24 combatants; `3978` is the enemy-half interior address. |
| `3992..3994` | `0x0002` | V/U | WORD combat outcome/control value; width verified, exact meaning unresolved. |
| `3994..39A0` | `0x000C` | V | Twelve friendly-combatant action BYTEs, proven by1543; assigned-mech view at3998 overlaps it. |
| `39FA..3FDA` | `376 × 4` | V | Captured-sprite FAR pointers: Offset WORD then Segment WORD. Startup fills IDs0..375;0800 readers use this table, not a flat WORD array. |
| `3FE9..4100` | variable | H | Stored infantry IDs, BLD index, unit positions, visibility, and sprite state. |
| `3FE9..3FF1` | `0x0008` | V | Saved party-character Name IDs. Arena setup saves slots `1..7` before hiding them with `FF`; slot zero is not written there. |
| `3FF8..3FFA` | `0x0002` | V | WORD index of the BLD currently selected for the shared BLD/BTSTATS buffer. |
| `3FFA..3FFE` | `0x0004` | V | Saved Rider IDs for the four party mech records during own-mech Arena setup. |
| `4004..4034` | `0x0030` | V | 24-WORD live combatant X-position table. |
| `4036..4066` | `0x0030` | V | 24-WORD live combatant Y-position table. |
| `406A..409A` | `0x0030` | V | 24-WORD combatant active/on-map table. |
| `409A..40B2` | `0x18` | V | Current byte sprite frame for each of 24 combatants; `40A6` is the enemy-half interior address. |
| `40B4..42F4` | `24 × 24` | V | Combat movement plans,24 rows of24 BYTEs;1467 reset proves extent. |
| `42F6..4694` | variable | H | Combat/graphics scratch, interaction coordinates, and loaded-file pointer state. |
| `430E..4312` | `0x0004` | V | Saved Pilot IDs for the four party mech records during own-mech Arena setup. |
| `4584..4586` | `0x0002` | V | Original WORD building ID saved before map-specific BLD remapping; later used to restore the building's map-return coordinates. |
| `4594..4596` | `0x0002` | V | WORD cache flag: nonzero means the shared buffer currently holds BTSTATS; BLD loading clears it. |
| `4602..4612` | `0x0010` | V | Sixteen map-loaded alternate-BLD bytes. The interaction comparison admits indexes `10h/11h`, but all verified shipped call sites restrict remapped IDs to `0..0Bh`. |
| `C33C..C530` | `4 × 0x7D` | P | Four destroyed/scratch mech records. |
| `C60F` | — | V | Pre-biased address expression for character `MechAssignment`; not an independent array. |
| `C614..C724` | `16 × 0x11` | V | Eight player plus eight enemy infantry records. |
| `C724..CB0C` | `8 × 0x7D` | V | Four player plus four enemy mech records. |
| `CB0C..D30C` | `0x0800` | V | Saved 128×128 visibility bitmap: 16 bytes per row, eight horizontal cells per byte. |
| `D30C..D370` | `0x64` | V/P | New-game-cleared state span with dense named byte overlays; exact extent verified, individual meanings vary. |
| `D370..D380` | `0x10` | V | One 32-bit C-Bill balance plus three 32-bit stock values. |
| `D390..D450` | `8 × field view` | V/P | Eight roaming-map NPC field views at stride `0x1A`; each verified view uses its first ten bytes and the 16-byte gaps remain unresolved. |
| `D450..D452` | `0x02` | H | MedKit and Field Surgery Kit ownership bytes. |
| `D452..D456` | `0x04` | V | Stored first name character for four temporarily hidden party mech records; `FF` marks a free/absent slot. |
| `D456` | `0x01` | V | Next scripted recruit Name ID; initialized to `1`, incremented as Rex/agents join, and wrapped from `0A` to `02`. |
| `D457..D557` | `0x100` | V | Four parallel 64-byte persistent map-effect arrays: sprite, packed page, X low byte, and Y low byte. |
| `D557..D558` | 1 | V | Next map-effect insertion slot and final saved byte. |
| `D55E..D576` | `0x18` | V | Combatant sprite-family offsets,24 BYTEs. Frame-plus-family is a WORD sum; sorted-mech collection separately truncates to BYTE. |
| `D558..` | — | P | Runtime-only state outside the `0x0F44` save block. |
| `E484..E48F` | variable | H | Combat result, compass copy, animation frame, and party condition state. |
| `E48E..E490` | `0x0002` | V | WORD flag for the rental-Locust Arena mode. Nonzero enables the special spectator/enemy handling and selects rental cleanup; zero selects party-owned-mech writeback. |

## Saved-state boundary

The original save operation copies exactly `0x0F44` bytes beginning at
`3092:C614`. Therefore the copied half-open range is:

```text
3092:C614 .. 3092:D558
```

`3092:D557` is the final saved byte. `3092:D558` and later runtime fields are not
part of that direct block. See `docs/formats/SAVE.md` for file offsets.

## Persistent map-effect ring

`3092:D457..D557` holds 64 saved map-effect slots used for fires, impacts, and
mech wrecks. Each property occupies its own 64-byte array. `D557` selects the
next slot and wraps to zero after `0x3F`. See
[`BTECH_0800` persistent map effects](../phase4/BTECH_0800_PERSISTENT_MAP_EFFECTS.md)
for coordinate packing, rendering, and animation behaviour.

## Verified contiguous records

```text
C614  player infantry[0]       eight records, stride 0x11
C69C  enemy infantry[0]        eight records, stride 0x11
C724  player mech[0]           four records, stride 0x7D
C918  enemy mech[0]            four records, stride 0x7D
CB0C  visibility bytes         0x800 bytes
D30C  next state region
```

The arithmetic closes with no gaps and is stronger than the C element types in
the scratch header.

## `D30C` state overlay

`D30C..D370` is an exact 100-byte reset span: new-game initialization clears
it with a loop bounded by hexadecimal `64`. It is best represented as a raw
byte block plus named views, not as sequential C members. Selected
better-supported byte interpretations are:

| Address | Confidence | Current name/meaning |
|---|:---:|---|
| `D30D` | V | Training-mission pass result; mission-specific teardown sets it. |
| `D30E` | H | Training mech selection/story variant. |
| `D30F` | V | Enemy mech pre-existing-damage severity. Valid values are `0..6`; positive values above six are clamped, but high-bit bytes are signed-negative and escape that clamp. |
| `D310` | P | Citadel destroyed state. |
| `D311` | V | Training mech survived a Kurita-interrupted mission. |
| `D313` | V | Citadel combat-school introduction seen; script state index `07`. |
| `D314` | V | Citadel selected combat skill, `0..2`, written by script state index `08`. |
| `D315` | V | Immediate combat-course purchase result; native action sets/clears it and script state index `09` tests it. Not an ongoing training timer. |
| `D316` | H | BLD interaction state. |
| `D317` | H | Payment/action result. |
| `D319` | V | NPC activity/building category set by BLD `Talk to others` branches; also indexes the activity-reason text table. |
| `D31A` | V | Party count, selected party slot, or specialist-training purchase result (`0`/`1`); workflow-local overlapping BYTE. |
| `D31B` | V | Selected character's tested specialist training mask: Tech `0`/`1`, Medical `0`/`2`, read by script state index `0F`. |
| `D31C` | H | Active lance mech count. |
| `D31D` | V | Selected mech modification package: `0..7` are real first/second-stage packages; `8` is the unsupported sentinel. |
| `D31E` | V | Modification workflow enabled; cleared only when selected mech upgrade-level flags equal `3`. |
| `D320` | V | Shared saturating world-tick cooldown: TRAINING.BLD sets `FF` after a mission; dormant copyright quiz also sets `FF` after failure. Script state index `14`. |
| `D321` | V | Citadel combat-course cooldown, script state index `15`; successful purchase sets `FF`, each processed world tick decrements toward zero. |
| `D322` | U | Saturating world-tick countdown; domain meaning remains unknown. |
| `D323` | H | Free-running ComStar finance/stock timer. The main loop branches on the pre-decrement value being zero, then leaves it wrapped to `FF`. |
| `D325` | V | Any living character with signed Health != signed Body*10; not an address comparison or merely Health below full. |
| `D326` | V | Medical service tier, scripted `0..7`; zero invokes own-party healing without a fee, nonzero indexes signed WORD fees. |
| `D32D` | H | Arena victory state. |
| `D330..D334` | H | Traitor probability, character, event, and party state. |
| `D335` | V | Party health-recovery countdown:1431 resets63 after treatment; main loop decrements each world tick. HOSPITAL02F2 clears it before injury/fee checks, allowing hospital treatment during recovery. |
| `D339` | V | Holds roaming NPC slot zero (Rick Atlas) at his waypoint pending the lounge conversation. |
| `D33A` | V | Signals the Rick Atlas selection to `LOUNGE.BLD`, which reads it as persistent-state index `2E`. |
| `D33C` | P | Has holoviewer. |
| `D33D` | V | Owns the Mapper. The character inspection sheet prints this byte beside `Mapper:`, and the exploration loop uses it to reveal the current coarse map row. |
| `D33E` | P | Has viewed holodisk. |
| `D33F..D341` | V | Little-endian 16-bit allowance total-wealth limit, initialized to 50. The native getter zero-extends it to 32 bits for comparison with cash plus all stock balances. |
| `D341` | U | Independent unknown byte. |
| `D342` | H | State tested after `ENTRANCE.BLD`; when nonzero, eight fog bytes at rows 96..103, byte-column 12 are cleared before the Star League scene controller runs. Exact story ownership remains open. |
| `D343` | H | Active/nonzero cache for the `D344:D345` countdown; refreshed as `D344 | D345`. |
| `D344..D346` | H | Little-endian 16-bit world-tick countdown (low byte first). Expiry can restore a saved 38-byte map-tile patch when the party is inside the relevant map rectangle. Earlier training-wait hypothesis is unconfirmed: the verified TRAINING.BLD entry gate uses D320 instead. This countdown's initializing write/story ownership remains unknown. |
| `D346` | P | Inside Star League cache. |
| `D347..D34A` | H | Current red, blue, and yellow security codes. |
| `D34A` | H | White-code accepted state. |
| `D34B` | P | Phoenix Hawk found. |
| `D35B` | V | Saved outtake-frequency selection: `0` very frequently, `1` frequently, or `2` infrequently. |

Names in this block may be context-sensitive temporaries rather than durable
domain properties. Bounded gameplay audits now establish many reads/writes, but this is not a
complete per-address cross-reference.

## Finance offsets in save files

The address-to-file formula yields:

| Memory | File offset | Width | Current meaning |
|---|---:|---:|---|
| `3092:D370` | `0x0D5D` | 4 | C-Bills. |
| `3092:D374` | `0x0D61` | 4 | Stock value 0. |
| `3092:D378` | `0x0D65` | 4 | Stock value 1. |
| `3092:D37C` | `0x0D69` | 4 | Stock value 2. |

Stock order is `DefHes`, `NasDiv`, `BakPhar`, verified from the far-pointer table
at `3EDB:4E7E`. Their main-loop change masks are `00FF`, `0007`, and `0001`;
their respective arithmetic factors are 96, 90, and 48.

## Shared buffers, sound and unresolved ownership

The BLD transformation covers9000 bytes beginning at00A0, but that is not the
allocation size: shipped `BTSTATS.CMP` loads12,635 payload bytes through31FA.
The C17 host buffer must therefore be at least12,635 bytes; allocating only the
BLD transform span caused a3,635-byte host buffer overrun. DEMOFILE uses the separate
27B0 view. Startup sprite entries point to separately captured EGA snapshots;
their header BYTE+1 is temporarily trimmed/restored during drawing. See
[reader reconciliation](../phase4/BTECH_0800_SPRITE_POINTER_READERS.md).

Sound-effect helpers reuse scattered WORD scratch:0000 (sweep centre),006C
(noise mask),3776 (minimum delay bits),398A (delay seed),4312 (toggle count),
4612 (command-or-repeat). These are named views, not an independent contiguous
sound structure. The [1FC5 final audit](../phase4/BTECH_1FC5_FINAL_CONSISTENCY.md)
and header catalogue contain all19 reviewed sound fields. Overlaps with other
workflow scratch must retain their original addresses/lifetimes.

Open: NPC stride gaps, D322/D341, D344 countdown's initializing writer/story
ownership, some D30C state meanings, exact3992 semantics, and DS/SS aliasing.
Do not fill these gaps by placing scratch-header C members sequentially.
