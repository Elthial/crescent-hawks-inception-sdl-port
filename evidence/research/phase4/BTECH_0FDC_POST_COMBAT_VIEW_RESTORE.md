# `BTECH_0FDC` post-combat mission-view restore

## Review boundary

This block covers `0FDC:134B-13DD`, renamed
`Restore_Mission_Map_View_After_Combat`. Its two machine-code call sites
immediately follow `Combat_Parent_000A`: one shared path serves missions 2 and
3, and the other serves missions 4 through 7. Mission 8 and the jailbreak do
not call it. The structured pseudo-C represents the shared missions 2/3 path
as two equivalent source-level calls.

The routine restores the packed map-view anchor from Jason's combatant position
and then rebuilds and presents the exploration display.

## Mounted branch

The branch condition reads Jason's `MechAssignment` byte at `3092:C620`. Any
value other than the on-foot sentinel `8` is treated as mounted. In that case
the view anchor becomes:

```text
view X = combatant 0 X
view Y = combatant 0 Y + 2
```

The routine always reads friendly mech/combatant slot zero; it does not use the
assignment byte as an array index. This matches the reviewed training-mission
setup, where Jason's machine occupies slot zero, but it is an important caller
precondition for a port.

The two-unit Y adjustment is a view-framing offset whose presentation purpose
is probable; the arithmetic itself is verified. If the addition sets local
coordinate bit 7, the routine adds `0x0F80` to normalize the packed Y value into
the next coarse map row:

```text
707E + 2 = 7080;  7080 + 0F80 = 8000
707F + 2 = 7081;  7081 + 0F80 = 8001
```

Only the low byte's bit 7 is tested, exactly as in the assembly.

## On-foot branch

When `MechAssignment` is exactly `8`, the view anchor is copied directly from
on-foot combatant slot zero, corresponding to common combatant ID 4. No Y
offset or boundary normalization is needed because this branch performs no
coordinate arithmetic.

## View rebuild and presentation

Both branches converge on the same four-call sequence:

1. `PosXY_OffsetGrid` rebuilds the nine-block local terrain neighbourhood for
   the restored packed position.
2. `Copy_Data_To_GraphicsMemory` draws/copies the terrain view into the working
   EGA buffer.
3. `Draw_Infantry_And_Mechs` composites persistent effects and units and
   refreshes their display-position state.
4. `EGA_DrawBox_Wrapper` copies the completed game-view rectangle to EGA video
   memory.

This is therefore more than a coordinate assignment helper: it reconstructs
the complete exploration view after combat has returned.

## Evidence and confidence

- `BTech-Reko-expanded/BTECH.reko/BTECH_0FDC.asm`, `0FDC:134B-13DD`;
- the two executable call instructions at `0FDC:0741` and `07E0`;
- the verified packed-coordinate rules in
  [`PACKED_MAP_COORDINATES.md`](../data-structures/PACKED_MAP_COORDINATES.md);
- the separately reviewed exploration sprite compositor and map-cache rebuild.

The branch condition, source addresses, `+2` adjustment, normalization, and
call order are directly verified. The visual rationale for centring a mounted
Jason two coordinate units lower remains probable. No Astra review is required.

The following armour-distribution routine is documented in
[`BTECH_0FDC_ARMOUR_DISTRIBUTION.md`](BTECH_0FDC_ARMOUR_DISTRIBUTION.md).
