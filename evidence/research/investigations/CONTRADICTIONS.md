# Contradictions and open questions

This register prevents a convenient interpretation from silently becoming a
fact. Items close only when the resolution has address-level, byte-level, or
repeatable runtime evidence.

Global caution: the Reko project used a 32-bit model against a 16-bit C/assembly
program. Any decompiled `int`, `long`, or pointer width can be an extrapolation
and must be independently established before it is used to resolve an item.

## C-001 — Mech actuator field widths — resolved

- **Claim A:** two current bytes at `+0x24..+0x25` and two maximum bytes at
  `+0x69..+0x6A`.
- **Evidence A:** explicit `BTECH.h` offset constants; late `C724` address notes;
  existing InceptionTools indexes; all subsequent offsets and the `0x7D` record
  boundary align.
- **Claim B:** four current and four maximum actuator bytes.
- **Evidence B:** the older `BTECH.h` C typedef and imported UnBattletech format
  documents.
- **Resolution:** Claim A is correct. Clean repair code compares current bytes
  `+0x24/+0x25` with maximum bytes `+0x69/+0x6A`; combat and damage code perform
  byte-sized, independent nibble operations on the two current bytes. The next
  field is read at `+0x26`, leaving no room for four actuator bytes.
- **Later refinement:** BTSTATS at `0DAB:1F86..21BE` verifies byte `+0x24` as
  left, byte `+0x25` as right, low nibbles as legs, and high nibbles as arms.
- **Still open:** mapping each bit within those nibbles to a named joint or
  physical actuator.

## C-002 — Weapon name length and field offsets — resolved

- **Claim A:** 11-byte name; damage at `+0x0B`.
- **Evidence A:** old InceptionTools captured records and `BTECH.h` typedef.
- **Claim B:** 10-byte name; damage at `+0x0A`.
- **Evidence B:** imported UnBattletech `MEMORY_MAP.md`.
- **Resolution:** Claim A is correct. The clean `3EDB` segment dump contains 33
  contiguous `0x11`-byte records at `2ED8..3108`. Combat instructions use
  `2EE3..2EE8` for the first record's six post-name bytes, proving an 11-byte
  name and offsets `+0x0B..+0x10`.
- **Verified semantics:** `+0x0B` damage encoding; `+0x0C` personnel flag plus
  repeated-attack count or missile cluster column; low nibble of `+0x0D` heat;
  `+0x0E` packed short/medium range thresholds; `+0x0F` maximum range; `+0x10`
  skill index.
- **Still open:** the high nibble of `+0x0D`.
- **Canonical reference:** `docs/data-structures/WEAPON_RECORDS.md`.

## C-003 — Meaning of mech record byte `+0x7C` — resolved

- **Claim A:** unknown/padding.
- **Claim B:** mech upgrade level.
- **Evidence:** both names appear at different points in `BTECH.h`.
- **Resolution:** `UpgradeLevelFlags`, **Verified**. The upgrade workflow writes
  `1` for a first-stage modification, ORs in `2` for a second-stage
  modification, and treats `3` as fully upgraded.

## C-004 — UrbanMech reference index — resolved

- **Claim A:** symbolic reference ID `0x08`.
- **Claim B:** eighth contiguous template record, array index `0x07`, at
  `2FE8:065B`.
- **Resolution:** the contiguous template array index is `0x07`. The clean arena
  spawn loop copies `0x7D` bytes directly from `2FE8:065B`, which is
  `2FE8:02F0 + 7 × 0x7D`. No verified code uses `0x08` as an UrbanMech template
  index; the historical symbolic constant is stale or belongs to an unstated
  external domain.

## C-005 — Character offsets `+0x0C` and `+0x10` — resolved

- **Names seen:** Piloting, Riding, On Foot, On Foot or Piloting, and rider
  assignment.
- **Resolution for `+0x0C`:** `MechAssignment`, **Verified**. Values `0..7`
  index the eight live mech records and `8` means on foot. Pilot and rider
  characters receive the same value; their roles are held in mech bytes
  `+0x79/+0x7A`.
- **Resolution for `+0x10`:** `TrainingFlags`, **Verified**. Bit `0` gates and
  records Tech training; bit `1` gates and records Medical training. No rider
  use was found.
- **Annotation defect found:** annotated `BTECH_1CD3.c` case `0x2E` operates on
  `+0x10`, but the clean assembly operates on `+0x0F` (`Health`). This is queued
  for the later controlled annotation phase.
- **Still open:** meanings, if any, of training flag bits `2..7`.
- **Canonical reference:** `docs/data-structures/CHARACTER_RECORDS.md`.

## C-006 — Later save-file offsets

- Imported documentation lists overlapping finance ranges and at least one
  reversed range (`0x0E30` to a smaller address).
- **Resolution progress:** `Load_Game`/`Save_Game` prove a one-byte header, direct
  `0x0F44`-byte copy from `3092:C614`, X at file `0x0F45`, and Y at `0x0F47`.
  All local saves are `0x0F49` bytes. The old InceptionTools final-position read
  is off by one.
- **Still open:** reconstruct semantic fields inside the later copied state block
  using the memory-to-file offset formula in `formats/SAVE.md`.

## C-007 — Exact compressed-image RLE semantics

- Existing source and imported documentation agree that multiple orientations
  or formats exist but use imprecise or potentially conflated descriptions of
  literal/repeat behaviour and EGA plane ordering.
- **Current decision:** format inventory is **Probable**, exact algorithm remains
  open.
- **Verification needed:** trace the annotated decompressor and compare decoded
  dimensions/hashes against known user-supplied samples.

## C-008 — ANM first word and frame count

- **Rejected claim:** the first two ANM bytes contain a general size.
- **Evidence:** values such as `41 42` and `45 46` do not match file or payload
  lengths; playback indexes these bytes by frame number for its timing lookup.
- **Resolution progress:** compressed data begins at `0x33`. The first `0x20`
  bytes form the observed playback-control region and the first zero terminates
  ordinary playback. O6 uses entries `0..30` and terminates at `+0x1F`. A
  separate 19-byte header trailer occupies `+0x20..+0x32` in every local file;
  it must not be counted as more playback controls.
- **Still open:** exact meanings of playback-control values and the 19-byte
  trailer, plus final confirmation of the `0x20` boundary from executable code.

## C-009 — BLD first two bytes

- **Rejected claim:** separate one-byte file type and paragraph count.
- **Evidence:** in all 26 local BLDs, the little-endian word at offset zero is
  exactly file length minus two.
- **Current decision:** two-byte stored encrypted-payload length, **Verified**.
  The generic loader consumes this word and loads the following payload at
  `3092:00A0`.

## C-010 — Visibility element width at `3092:CB0C` — resolved

- **Rejected declaration:** `unsigned short Map_FogOfWar_Visibility[2048]`.
- **Evidence:** eight mech records end at `CB0C`; the next known block begins at
  `D30C`; the difference is exactly `0x800` bytes and matches the save file's
  2048-byte visibility region.
- **Resolution:** the overhead renderer advances 16 bytes per row and consumes
  each byte most-significant-bit first across eight horizontal cells. The main
  exploration loop and new-game Citadel reveal use the same addressing.
- **Current decision:** `byte[0x800]` holding a 128×128 bit-packed visibility
  map is **Verified**.

## C-011 — `D30C` array and named fields — extent resolved

- **Problem:** the scratch header declared `PrimaryBoolList[64]` and then listed
  named members as though they followed it; the decimal-looking bound also hid
  the true extent.
- **Resolution:** new-game initialization at `0800:4E43-4E5D` compares its
  16-bit index with hexadecimal `0064`, clearing exactly 100 bytes from
  `D30C` through `D36F`. The named fields are overlays within this span, not
  members following an array.
- **Current decision:** use the address-view name
  `NewGameStateReset_D30C[0x64]`. Per-byte read/write cataloguing and some
  workflow-specific meanings remain open, but the storage extent is verified.

## C-012 — `D390` position arrays versus strided records — partially resolved

- **Problem:** proposed word arrays at `D390`, `D392`, `D394`, and `D396` overlap
  if interpreted as independent contiguous arrays. Other notes show accesses
  using an entity stride of `0x1A`.
- **Resolution:** `0800:24C2` and the map loader prove eight field views at
  `D390 + slot * 0x1A`. The first ten bytes are current X/Y words at `+00/+02`,
  destination X/Y words at `+04/+06`, a packed waypoint-pair byte at `+08`,
  and a movement-delay byte at `+09`.
- **Verification needed:** identify the remaining `0x10` bytes between these
  field views before treating the entire stride as one owned C structure. The
  final stride overlaps other named scratch/save variables if naively expanded.

## C-013 — `D33F` allowance width — resolved

- **Problem:** `BTECH.h` declares a four-byte `unsigned long` beginning at
  `D33F`, but also names independent fields at `D342` and later. A four-byte
  value would overlap `D342`.
- **Resolution:** `D33F..D340` is a little-endian 16-bit allowance total-wealth
  limit; `D341` is independent. New-game setup writes bytes `32 00`, and
  `0800:29F5` reconstructs those two bytes before returning the value in
  `DX:AX` with `DX=0`.

## C-014 — Input/display constants — resolved/retired

- The project-owner correction fixes the game-action label `KEY_B` at `0x41`;
  it is not evidence for ASCII letter B.
- `KEY_W` is removed from the maintained model. The historical scratch header
  still contains `KEY_B = 0x42` and `KEY_W = 0x57` and must not be treated as
  authoritative for these labels.
- `SCREEN_RES_Y = 119` correctly expresses the zero-based end of a 120-line
  region. The symbol has no references in the retained EGA-only annotations and
  may be residue from a deleted graphics pipeline.
- **Current decision:** keyboard-definition defects are resolved. Preserve the Y
  constant only as historical context until an original use site identifies its
  owning viewport or graphics mode.

## C-015 — Expanded Reko executable versus installed executable — resolved

- **Profile A:** `BTech-Reko-expanded/BTECH.EXE`, 260,416 bytes, SHA-256
  `F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE`.
- **Profile B:** `chinception/BTECH.EXE`, 152,429 bytes, SHA-256
  `F2A9A023D79927B8072DE11DDD6E03DA6D3357D49181D8FBA6D12988BF8CC0EE`.
- **Observation:** the clean Reko project is explicitly configured for
  `x86-real-16` and its generated segment views line up structurally with the
  annotated research, but its source executable is not byte-identical to the
  installed binary. The installed image has zero MZ relocations and contains a
  packed-file corruption diagnostic; the expanded image has 3,292 relocations
  and a 13,200-byte relocation-bearing header.
- **Resolution:** the original distribution executable was packed/encrypted.
  Profile A is the unpacked/expanded analysis form produced so Reko could
  decompile the program; Profile B is the packed installation form.
- **Current decision:** use Profile A as the clean annotation baseline and
  Profile B as the runtime/distribution compatibility input. Never transfer raw
  packed file offsets directly to expanded-image offsets.
- **Remaining documentation task:** recover or record the unpacking/relocation
  mapping where a packed-file offset must be related to an expanded segment
  address. This is a provenance task, not an executable-version contradiction.

## C-016 — Alleged party array at `3092:C60F` — resolved

- **Old declaration:** `InfantryValueArray_C60F[8]` immediately precedes the
  character records.
- **Conflict:** only five bytes exist before `C614`, so an eight-byte array
  would overlap the first character record.
- **Resolution:** `C60F` is a pre-biased address used with `i × 0x11` for
  `i = 1..7`. It aliases `C620 + (i - 1) × 0x11`, the `MechAssignment` field of
  the preceding character. Paired `C60F` and `C620` stores cover all eight
  records; they do not describe an additional array.
- **Canonical reference:** `docs/data-structures/CHARACTER_RECORDS.md`.

## C-017 — BLD header, decode origin, and transform order — resolved

- **Imported claim:** file bytes `0x00..0x9F` form a header/metadata area;
  interpretation begins at file offset `0xA0`; decoding is
  `(stored XOR 0xE9) - 0x29`.
- **Resolution:** the first word is consumed as a payload length. The remaining
  bytes beginning at file offset `0x02` are loaded at `3092:00A0`, decoded using
  `((stored + 0x29) & 0xFF) XOR 0xE9`, and interpreted from payload offset zero.
- **Consequence:** raw `EE C6 EB EA` is an encrypted prologue, not a signature;
  raw `C0 xx` pairs are encrypted instructions, not content-type words.
- **Evidence:** `1F3D:063B`, `0FDC:1D30`, `0FDC:0008`, and decoded prefixes from
  all 26 local BLD files.
- **Canonical references:** `docs/formats/BLD.md` and
  `docs/formats/BLD_OPCODES.md`.

## C-018 — Alternate-BLD lookup length versus admitted indexes — resolved

- **Verified load:** the MTP loader reads exactly `0x10` bytes into
  `3092:4602..4611`.
- **Conflicting consumer:** `Interact_with_BLD` remaps every requested ID below
  `0x16` except `0x12`, admitting THEATER `0x10` and FROB `0x11`. Those two
  lookups read `3092:4612` and `4613`, beyond the loaded table and immediately
  before the graphics buffer beginning at `4614`.
- **Resolution:** both exterior interaction loops are bounded to building slots
  `0..0x0B`; recursion passes `VIEWDISK` (`0x12`), and direct story callers pass
  `ENDMECH` (`0x16`) or later IDs. No verified shipped caller can supply `0x10`
  or `0x11`, so the adjacent bytes are not read in the known game call graph.
  Portable APIs should nevertheless validate the 16-byte table if new callers
  are introduced rather than relying only on the historical call-site bounds.
- **Canonical reference:**
  `docs/phase4/BTECH_0FDC_BLD_INTERACTION_ENTRY.md`.
