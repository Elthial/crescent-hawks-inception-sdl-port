# 0800 sprite-pointer readers

Sol: Confirmed against the matching unpacked executable's `BTECH_0800.asm`
and the reviewed `207F:0377` renderer / `0D27:0410` startup captures.

`DS:39FA` contains 376 four-byte FAR entries: offset WORD followed by
segment WORD. Reader index `i` selects `39FA + i * 4`, not WORD `i`.
The table occupies `39FA–3FD9`; a sprite's header is in the allocation it
points to, not the next table entry.

| Reader | Native evidence | Maintained interpretation |
| --- | --- | --- |
| Exploration party | `069F` pointer load, `06E0` header edit, `056A/0591` draw/restore | Frame `409E[id]` and family `D562[id]` are the friendly-infantry slices of `409A/D55E` (combatant ID 4 + party ID). |
| Exploration NPC infantry | `0990`, draw `076B` | Two zero-extended BYTEs added as a WORD; renamed `bx_487` to `SpritePointerIndex_487`. |
| Exploration friendly mechs | `0AAF–0B40` | Same WORD index; terrain overlap remains 0/8/16 rows. |
| Exploration jailbreak overlay | Fixed `3C42/3C44`, `0D30–0E46` | Entry `0x92` (146), no temporary header edit. |
| Combat infantry | `111F`, draw `0EAD` | Same WORD frame-plus-family lookup, retaining calculated pixel positions. |
| Combat sorted mechs | `1556/15A0`, draw `150F` | Collection stores sprite ID in a BYTE; preserve its truncation and anchor minus (8,16). |
| Combat jailbreak overlay | Fixed `3C42/3C44`, `1616–172C` | Same entry 146; this second native overlay remains in place. |
| Persistent effects | EGA draw immediately before `2AF7` | BYTE `D457[slot]` selects an entry; no header edit. Visible fire alternation is unchanged. |

All retained draws pass destination **AC00:0000**, represented by
`EGA_MAP_FRAMEBUFFER_FAR`, rather than the legacy segment-only `EGA_Buffer`.
The renderer has five C arguments (six stack WORDs): packed destination FAR,
source offset, source segment, pixel X, pixel Y.

## Temporary clipping

The five terrain-clipped readers use `Draw_Terrain_Clipped_Sprite_0800`, a
research helper, **not** a claimed original routine. It snapshots both pointer
WORDs, subtracts overlap from the pointed-to header's height-minus-one BYTE at
offset +1, draws, then adds overlap back. BYTE subtraction/addition wraps;
offset +1 wraps as a WORD without carrying into the segment. Pointer-table
entries are never modified. This is transient rendering state, not permanent
sprite cropping or a gameplay-state change.

Original non-EGA branches are deliberately not restored. Reader ordering,
terrain decisions, depth sorting, visibility checks and fire-frame progression
are untouched. The reconciliation corrects Reko/maintained-C expressions; it
does not identify an original game bug.

## Verification and limits

`scripts/Verify-0800SpritePointerReaders.ps1` checks source contracts and
synthetic BYTE arithmetic, WORD index addition, pointer-entry stride and
segment-preserving offset wrap. It does not render original assets or execute
the DOS game. Out-of-table malformed indices remain unguarded, as in the
native readers; the 376-entry extent is not a promise that arbitrary BYTE sums
are valid. Live visual comparison remains useful before porting this boundary.

Validation: 132,772 sprite-reader checks, 187,656 startup/music regression
assertions and 39 sound caller/field checks passed; `git diff --check` passed.
