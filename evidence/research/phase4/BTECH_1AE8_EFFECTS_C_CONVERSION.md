# Sol: combat effects parent C conversion

Converted the complete `1AE8:12C7–1E45` parent into
`CrescentHawksInception/src/Original/BTECH_1AE8_EFFECTS.c`. This is the original
routine, not a replacement effects algorithm. It selects shared attack streams,
plays weapon sounds, draws beams or travelling missiles, plays target impacts,
restores Mech frames and removes casualties. Arena destruction retains its
original two-Mech replacement staging and shared movement-direction storage.

Weapon table indices are distinct from one-based Mech component identifiers.
The native missile range is 25–31, Mech beam range 15–18, and repeating-fire
selection is 23, 9 or 19–22. Corrected the corresponding annotated effects
comparisons, not unrelated combat fire predicates. Retained the carriage return
before `Killed him!` from EXE-owned text at `3EDB:41FC`.

The retained startup selects EGA adapter 2. Adapter 1's unassigned stack redraw
phase is outside this supported pipeline: it is not initialized as an invented
phase and its behaviour is not certified. The C parent retains exact endpoint
termination rather than adding projectile overshoot or timeout guards. The
original signed-WORD absolute-value helper retains the -32768 overflow result.
Valid original actor, weapon and direction indices remain call contracts.

Screen-coordinate globals moved from the drawing method translation unit into
the existing original combat-data translation unit. This changes no storage
type or game algorithm; isolated effects tests can access original data without
implicitly linking the viewport renderer.

`gameplay.original_combat_effects_parent` runs the actual parent, animation
interpreter, persistent-effect registration, Mech templates, octant helper and
WORD absolute-value routine. Presentation/audio boundaries are test-only fakes.
Checks cover all 33 weapon entries across eight directions with hidden actors,
disabled-graphics casualty/target cleanup, Jason death, wreck coordinate
boundaries, arena replacements, a visible personnel beam, a visible missile and
impact sequence, and every signed-WORD absolute input.

109 headless and 127 SDL/local-asset tests pass. This does not establish complete
headless combat, visible emulator parity, sound timing parity or a playable
game. The full `Combat_Mechanics` caller remains to be converted, including its
unassigned attack-result stack flag contract; this parent accepts that flag
without inventing its value.
