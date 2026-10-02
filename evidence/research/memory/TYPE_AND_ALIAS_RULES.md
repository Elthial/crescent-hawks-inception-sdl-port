# Type and alias rules

## `BTECH.h` is an address catalogue

The segment `typedef struct` declarations in `BTECH.h` must not be compiled or
used to calculate offsets. They omit unknown padding, repeat the same address as
both an array and named elements, and sometimes assign a C width that conflicts
with adjacent addresses. Their useful content is the address in each comment.

Examples:

- `Mechs[8]` and the eight individually named mech declarations are two views of
  `3092:C724..CB0C`, not sixteen sequential records.
- `NewGameStateReset_D30C[0x64]` and the following `D30C`-named fields overlay
  the same 100 bytes through `D36F`; they are not sequential members.
- The historical `unsigned short Map_FogOfWar_Visibility[2048]` declaration
  contradicted the save boundary. The current header now uses BYTE[0x800],
  matching128x128 bit-packed visibility; it is not0x1000 bytes.
- `InfantryValueArray_C60F[8]` is not storage before the character records.
  Code uses `C60F + i*0x11` only for `i = 1..7`, making it a pre-biased alias of
  the preceding record's `+0x0C` field.

Canonical documentation should therefore use explicit address tables and
overlay groups rather than a monolithic C structure.

Sol: The current header also has canonical24-WORD X/Y/active combatant views
and376 four-byte sprite pointers. Historical split/flat aliases remain as
scratchpad history, not extra allocations. Sound uses scattered WORD scratch;
music has writable CS-relative fields. Neither implies one sequential save
structure. See the refreshed region/segment maps for current evidence.

## 16-bit widths

The executable was produced from 16-bit C with some inline assembly. Reko was
nevertheless configured/running with a 32-bit data model during this work and
often extrapolated a 32-bit `int` or pointer when it could not infer the original
type. Those rendered C types are placeholders, not evidence.

For new annotation checks, consult the unmodified views in
`BTech-Reko-expanded/BTECH.reko/` in this order: ASM instruction width, `.dis`
data-flow, then pseudo-C. All three derive from one Reko analysis and therefore
are not independent votes; the pseudo-C cannot overrule its own underlying
instructions. The executable profile caveat is recorded in
[`EXECUTABLE_PROFILES.md`](../reference/EXECUTABLE_PROFILES.md).

Use instruction width and address spacing as evidence:

| Stored concept | Expected size | Notes |
|---|---:|---|
| byte / flag / packed ID | 1 | Confirm with byte loads/stores where possible. |
| C `int` / `unsigned int` | 2 | Original 16-bit compiler default unless instructions prove otherwise. |
| word / near offset | 2 | Little-endian. |
| near pointer | 2 | Offset within an already established segment. |
| dword | 4 | Often represented as two 16-bit words. |
| far pointer | 4 | Conceptually `offset:word` plus `segment:word`; ordering must be checked at the use site. |

An `unsigned long*` in the decompiled scratch pad is especially dangerous: it
may mean “a four-byte far pointer stored by the DOS program,” not a pointer with
the host compiler's width and layout. Portable C# should initially represent it
as a decoded 16:16 address value, never as an unsafe native pointer.

Likewise, a Reko `int` must not become C# `int` mechanically. Until proven, the
portable representation should preserve a 16-bit value (`short`/`ushort`) or a
raw word with explicit signedness still under investigation.

## Evidence for pointer width

Prefer these observations over the rendered C declaration:

- a near pointer is passed/popped as one word and used with an already loaded
  segment register;
- a far pointer moves or stores both an offset and a segment word;
- `LDS`/`LES`, explicit segment loads, far calls/returns, or two-word pointer
  arithmetic support a 16:16 value;
- stack cleanup reveals the number of argument words;
- two-byte spacing between pointer-table entries supports near offsets, while
  four-byte spacing may support far pointers or dwords;
- byte/word/dword instruction selection establishes the accessed storage width.

Embedded assembly can also impose layouts or calling behaviour that the source
compiler's ordinary C model would not generate, so mixed C/assembly boundaries
should receive explicit notes.

## Naming prefixes

Existing names often use these hints:

| Prefix | Intended hint | Reliability |
|---|---|---|
| `b` | byte | Useful but not proof. |
| `w` | word | Useful but not proof. |
| `t` | typed/temporary object | Ambiguous; inspect access width. |
| `a` | array/base address | Does not establish element width or count. |
| `ptr` | pointer or pointer-like base | May be near, far, or a decompiler artefact. |

Preserve an address-based neutral name until access width, element stride, and
semantic ownership are all supported.

## Arrays versus strided records

Two nearby symbols do not necessarily begin two independent arrays. Expressions
such as `base[entity * 0x1A + fieldOffset]` can make several field bases look like
overlapping arrays. The `D390` region is a current example: proposed X/Y arrays
at offsets only two bytes apart cannot both be contiguous word arrays. Recover
the parent record stride before declaring collection shapes.

A compiler may also bias a field base before applying a loop index. For example,
`C60F + i*0x11`, where `i` begins at one, addresses exactly the same character
field sequence as `C620 + (i-1)*0x11`. Always establish the loop's actual index
range before treating the lowest rendered base as an allocated object.
