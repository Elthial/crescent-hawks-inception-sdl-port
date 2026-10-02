# Sol: component salvage parent C conversion

Converted complete `0DAB:0002–04F8` into
`CrescentHawksInception/src/Original/BTECH_0DAB_COMPONENT_SALVAGE.c`, using the
annotated method as the template. It chooses the best living party technician,
prints the native explanation, pools wreck armour, distributes repairs, performs
skill-gated structure/heat-sink/weapon-component repair and pays for scrap.

Retained signed Tech/name loads, strict first-wins technician ties, WORD pools,
descending armour/structure location priority and ascending friendly Mech slots.
Negative deficits reduce over-maximum armour/structure and increase the pool;
they are not clamped away. Excellent Tech enables structure repair, Average Tech
heat sinks, and Good Tech matching weapon-component repair. The original
`183B:273D` supporting-structure predicate is used, not a replacement salvage rule.
Heat-sink repair never checks its pool before decrementing: zero underflow and
repair without a recovered sink remain. Component repair does not refill ammo or
remove donated components from wreck records.

Scrap payout retains the original party-slot bug: no payment with technician
slot zero, otherwise `(90 + random&127)` per wreck multiplied by technician slot,
not Tech skill. The WORD result is sign-extended into the DWORD C-bill addition.
Native control bytes, plural suffix and prompt order remain. The three description
FAR strings at EXE-owned `3EDB:0FA2` were recovered from the expanded EXE:
` and heat sinks`, `, heat sinks and weapons`, and
`, heat sinks, weapons and structure`. No external assets are embedded.

## Unresolved SRM-6 stack bucket

Native initialization clears exactly 16 BYTE buckets for component IDs 10h–1Fh.
ID20h (SRM-6) is nevertheless accepted and reads/increments BP-14, an unassigned
native stack byte. The port represents only the initialized buckets and explicitly
stops before either unknown access. This temporary guard is NOT native behaviour,
not a zero-filled bucket, and not a certificate for every salvage workflow.
Intact friendly SRM-6 and destroyed enemy SRM-6 do not access the bucket and remain
supported. Entry stack residue or a wider native stack model is still required.
Critical-group anatomical labels remain uncertain; neutral contiguous record
offsets are retained. Native-valid character names and Tech levels remain contracts.

## Tests and current state

`rules.original_component_salvage_parent` runs the actual full parent and
structure-support predicate. Only presentation/input and random BYTE input are
isolated. It checks repair priority, negative deficits, structure skill threshold,
pool-free heat-sink repair, blocked critical support, one-for-one component use,
no ammo refilling, unsupported-bucket avoidance, technician ties/death/signed
skills, slot-index payout, singular/plural text, zero-wreck rejection and DWORD
money wrap. It does not validate unknown BP-14, emulator UI or full gameplay.

112 headless and 131 SDL/local-asset tests pass. Real-executable linkage now
exposes two missing original parents: `Examine_Screen_BTSTATS_CMP` and
`Combat_Weapon_UI_0004`. Additional runtime/fidelity gates still remain; a smaller
link error count is not proof the game is playable or preservation-complete.
