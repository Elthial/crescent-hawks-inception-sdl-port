# `BTECH_0800` start-game initialization

## Reviewed block

- Address range: `0800:50C8-5125`.
- Parent routine: `Start_Game`.
- The title/input/attract loop begins at `0800:5126`.

This block allocates the native local frame, fills a 256-byte random workspace,
performs an obsolete adapter-0 colour-table setup, loads the animated map-tile
asset, and enters the title-screen loop.

## The `0x32` call is stack allocation

At entry the executable loads `AX=0032` and calls `207F:2FDC`. This helper pops
the return address, subtracts `AX` from `SP`, checks the result against the
runtime stack limit, and jumps to the stack-overflow handler on failure. It is
the compiler's dynamic local-frame allocator: `0x32` means 50 bytes of stack
locals. It is not an RNG seed value and needs no equivalent in managed C#.

## Random workspace

A 16-bit counter runs from zero through `00FF`. Each iteration calls the byte
PRNG at `207F:0BC0` and stores `AL` at `246C:09FB + index`. This proves that
`GameSeeds` is a 256-byte table, not a word array. Later `207F` routines use it
as a random lookup/permutation workspace.

## Adapter-0 colour conversion

The original then performs:

```text
GraphicsCompatibilityFlag_4FBC = 0
207F:00D1(2FE8:0130)
GraphicsCompatibilityFlag_4FBC = 1
```

`207F:00D1` copies exactly 32 source bytes and constructs two 256-byte colour
conversion tables, but only when graphics-adapter ID zero is active. It returns
immediately for the retained EGA adapter ID two. The maintained EGA path omits
that obsolete conversion while preserving and documenting the two flag stores.

The data region `2FE8:0130-026F` is ten adjacent 32-byte conversion maps. The
old scratch-header declarations used `short[20]`, making each view 40 bytes and
causing false overlaps. They are now represented as byte arrays of exact size.

## Transition to the title loop

Both native loop locals are initially assigned one, although the local at
`[bp-1A]` is never subsequently read. The compatibility flag is set and
`Load_And_Draw_ANIMATE_ICN` loads the three-frame animated terrain tiles plus
the base BattleTech tileset. Control then jumps to the intro-music call before
entering the input/attract loop.

The complete title/input loop has now been reconstructed through `5276`. The
interactive and attract branches are documented separately. No Astra
confirmation is required for this initialization:
the stack helper, byte-table width, adapter guard, and calls are explicit.
