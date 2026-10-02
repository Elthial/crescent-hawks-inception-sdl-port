# `BTECH_0800` new-game runtime-state reset

## Reviewed block

- Address range: `0800:4F6C-4FB5`.
- Parent routine: currently `Load_Game_Map_Data`.
- The starting-screen and Citadel-map setup begins at `0800:4FB6`.

This short block initializes the new game's allowance and medical equipment,
then invalidates the saved ring of persistent map effects.

## Allowance and medical equipment

The executable stores `32` at `3092:D33F` and zero at `D340`. These adjacent
bytes are the little-endian word `0032`, or 50 decimal. The value is the
allowance/total-wealth limit consumed by `0800:29F5`; it is not the player's
cash balance. Starting cash was separately initialized to 20 at `D370-D373`.

Bytes `D450` and `D451` are then cleared. Later inspection, BLD conditionals,
hospital logic, and shop writes establish them as ownership flags for the
ordinary MedKit and Field Surgery Kit respectively.

## Persistent map-effect reset

`D557`, the next insertion slot, is reset to zero. A 64-iteration loop then
clears three parallel byte arrays:

| Address | Maintained field | Reset purpose |
|---|---|---|
| `D497-D4D6` | `MapEffectPackedPage_D497` | removes packed X/Y page bits |
| `D4D7-D516` | `MapEffectPositionXLow_D4D7` | removes low world-X bits |
| `D517-D556` | `MapEffectPositionYLow_D517` | removes low world-Y bits |

The sprite-ID array at `D457-D496` is not cleared. This is intentional, not a
missing decompilation statement: with all coordinate components zeroed, those
stale sprite IDs cannot describe a valid retained effect. A producer will
overwrite the ID when it reuses a slot.

Every store is byte-sized and the loop counter is a 16-bit local. No Astra
confirmation is needed for this block because the widths, bounds, and omitted
sprite-array store are explicit in the clean assembly.
