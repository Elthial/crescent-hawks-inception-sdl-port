# Sol: original exploration interactions

Converted complete 0800:1C12..218E into src/Original/BTECH_0800_INTERACTIONS.c,
using the annotated method as template and checking the complete expanded ASM.
The EXE-owned formation probes, cache row offsets and boundary-neighbour table
retain their original values. No external game assets are embedded.

The method rejects deep water at local map boundaries, probes active friendly
infantry before Mechs, offers building entry and dispatches cache puzzle tiles.
It accepts signed step deltas, not destination coordinates. Active flags are
WORDs; high-byte-only flags still qualify. Coordinates wrap as WORDs and the
projection halves negative values with arithmetic-shift rounding.

Original asymmetries retained: the two primary terrain comparisons are signed,
the Mech's adjacent footprint comparison is unsigned, blocked Mechs still offer
building entry, and Mech building prompts do not exclude the cache interior.
Infantry building scripts run before the live blocking threshold is tested.
Unknown blocking cache tiles still consume the attempted step. No new bounds
clamps or formation-footprint row restrictions have been introduced.

Focused tests exercise all 256 tile codes in both cache presentation modes,
tile dispatch arguments, high-byte active flags, Mech adjacent blocking,
doorway WORD wrapping, exterior infantry entry and all eight water boundaries.
These test the real parent with interaction/UI callees isolated. They are not
emulator gameplay or a certificate for invalid projection indices/unrepresented
native addresses. The ordinary suites pass 98 headless / 116 SDL-local tests;
the actual executable link reports ten remaining dependencies.
