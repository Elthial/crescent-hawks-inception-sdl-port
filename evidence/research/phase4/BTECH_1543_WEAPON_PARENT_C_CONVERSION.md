# Sol: weapon selection parent C conversion

Converted complete `1543:0004–07CA` to
`CrescentHawksInception/src/Original/BTECH_1543_WEAPON_MENU.c`. Original weapon
collection, Mech/personnel branches, layout geometry, ammo/target/range rows,
shutdown/sensor warnings, unavailable popups, override prompts, target picking
and panel restoration remain in the parent. Native FAR data bind to original
global views rather than importing DOS selectors into host pointers.

Restored signed WORD ID/choice gates and infantry weapon CBW. Header text at
`3EDB:29BE` contains `Weapon`,09,0B,`Ammo Target  Range`; `29F9` contains
06,0F,`Done`. Corrected those literals and infantry CBW in the annotated
template too. Runtime geometry uses the actual panel/control tables: panel5
height/top at `305B:0056/0052`, menu5 base/width/count/colour at E2/E4/E6/EC.

Native behaviour retained: exact heat30 marks all twelve targets, sensor2 returns
without restoring panels, damaged weapons clear targets, ammo ordinal10/11 reads
walk/jump fields, numeric ammo is padded to three columns, target rendering clears
the fired bit, inactive Mech-panel targets clear, and cancellation after override
leaves the old target cleared. Range is informational, not a selection rejection.
The personnel branch still neither masks fired bits nor revalidates active state.
Its biased Mech-name lookup uses the whole original save-byte view; target6/7
resolve to character bytes preceding the Mech table, not negative host array
indices. Representable unusual target8 also retains the native wrong-kind name.

Sol: the same0387 biased lookup in the Mech panel now uses that byte view too.
The prior host `Mechs[target-8]` expression was invalid for target6/7 despite
their native addresses being represented. Regression cases cover both names
at independent native offsets16h/93h, including fired-bit clearing. Targets
below6 explicitly stop before an unrepresented name read; this is a conversion
boundary, not a new original-game rejection rule. ASM0694 personnel handling
and ordinary enemy name addresses are unchanged.

## Remaining contracts, not fixes

Sol: the personnel fired-bit TODO is resolved at the caller level, not by
adding a mask to this panel. `1AE8` finishes a surviving-party execution with
`1631:0C63`; `0C76..0CC9` scans all24 combatants and all12 target slots,
preserves FF, ANDs other targets with7F, then replaces inactive IDs with FF.
AI and target-picker assignments also write raw IDs. The existing heat test
checks all288 slots, including fired8C becoming12 and inactive13 becoming FF.
The C cleanup now uses `CombatTargetIdMask`, not the unrelated component-ID
mask with the same value. No instruction behaviour or infantry CBW is changed.
This proves the normal round boundary's bit cleanup, not universal name-view
safety: unusual friendly IDs can still address the original biased lookup
before the represented save window, and those separate guards remain.

The original 12-byte component list is zeroed before collection. With fewer than
12 entries, original strlen has a terminator and remains the geometry source.
With all12 filled, it reads unassigned BP-14 and subsequent stack bytes. The C
parent explicitly stops before that scan, rather than adding a terminator or
substituting the collector count. Shutdown/sensor-return paths which skip strlen
remain supported even with twelve components. Negative menu choices likewise
require native stack indexing and explicitly stop; ordinary menu choices are
not clamped or rewritten.

Personnel targets below6 or beyond the represented 24-actor coordinate tables
require a larger memory view and explicitly stop. This includes negative CBW
targets. They are not silently masked, normalized, or declared correct. Native
valid weapon IDs, Mech target indices and terminated strings remain contracts.
These guards are temporary unsupported-path diagnostics, NOT native behaviour
or certificates for all weapon workflows.

## Verification

`ui.original_weapon_selection_parent` runs actual parent, target picker, range,
menu layout, popup and text-formatting routines. Drawing/preview/camera and user
choices are isolated test boundaries. Checks include empty/ordinary/11-weapon
menus, twelve-weapon shutdown without strlen, sensor1/2 tails, both unavailable
messages, ordinal10 walk alias, padding, fired/inactive targets, override denial,
cancel/target selection, informational out-of-range selection, personnel target
override without active validation, and native target6/8 name aliases.

113 headless and132 SDL/local-asset tests pass. Fresh executable linkage now has
one missing parent: `Examine_Screen_BTSTATS_CMP`. This is not proof of complete
gameplay, visible SDL parity or native residual-state preservation. No production
test stubs or copyrighted external assets were added.
