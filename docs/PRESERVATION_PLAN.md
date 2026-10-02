# Preservation plan

## Objective

Reconstruct the original DOS program as readable C17. A reader should be able
to compare a C procedure with its annotated segment and assembly and recognize
the same inputs, state changes, control flow and calls.

## Rules

1. Original procedure boundaries, call relationships and observable ordering
   are retained unless evidence proves a decompiler-created boundary.
2. Names may explain known game concepts, records and state. Segment/address
   provenance remains in comments or evidence rather than obscuring meaning.
3. Original bugs and unusual arithmetic are preserved when observable. A host
   safety diagnostic must be identified as such; it is not evidence of native
   behavior.
4. DOS, BIOS and hardware access may redirect to `src/SDL` at the lowest
   practical boundary. Higher game logic must not depend directly on SDL types.
5. Serialized and executable-owned records retain their native widths, signed
   interpretation, packing and aliasing contracts. Host pointers do not enter
   those records.
6. Tests may isolate platform boundaries. Production source must not gain a
   replacement algorithm merely to satisfy a test.
7. The original executable and external game assets are evidence supplied by
   the user and are never committed or redistributed.

## Evidence order

When sources disagree, use this order:

1. observed original executable behavior;
2. assembly and executable-owned data;
3. annotated decompiler output;
4. contemporary format and audit notes;
5. tests and reconstructed source.

Tests demonstrate retained cases; they do not elevate an inference above the
original program.

## Platform substitutions

A procedure is still considered preserved when its game-visible behavior and
ordering match the original except for a documented DOS, BIOS or hardware call
implemented through SDL3. The substituted boundary must remain identifiable in
`docs/PLATFORM_REPLACEMENTS.md` and separated from reconstructed game logic.

## Completion standard

A successful link is necessary but insufficient. Release readiness requires:

- strict C17 Debug and Release builds;
- asset-independent tests passing;
- opt-in original-asset integration tests passing locally;
- representative manual play covering startup, exploration, services,
  training, combat, save/load and clean shutdown; and
- remaining unsupported native-residual-state paths documented rather than
  silently invented.
