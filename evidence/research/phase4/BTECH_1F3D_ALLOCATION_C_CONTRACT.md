# Sol: original working-buffer allocation C contract

`src/Original/BTECH_1F3D_ALLOCATE.c` translates the `1F3D:05BC–063A` wrapper.
`src/Original/BTECH_207F_ALLOCATE.c` retains the `207F:3835` runtime entry with a
redirect to `src/SDL/memory.c`. DOS heap arena lists and FAR allocation addresses
are platform internals, not copied into game rules. SDL allocation does not
clear buffers; original sprite-header padding remains unwritten.

Checked raw expanded ASM: the wrapper copies the request's low WORD first.
A signed-negative high WORD branches to allocation; a zero high WORD also
branches there because every low WORD is at most FFFF. Only signed-positive
high WORDs reach the oversize error. The original null result prints
`Alloc: Null pointer return!`, waits for keyboard input and returns null,
without retry or termination. The C wrapper retains that null behaviour.

Both original call sites fit the supported contract:

- `1E56:0AE5` shifts the tile count in a WORD then CWD-sign-extends it. Its high
  WORD is therefore zero or FFFF.
- `1F3D:070A` narrows sprite allocation bytes to a WORD, with zero high WORD.

The signed-positive high-WORD path remains **unresolved**, not preserved or
fixed: native code reads unassigned SS:BP-6/BP-4 after its warning/input. The
port performs that warning/input then explicitly aborts with a diagnostic.
This is a temporary unsupported-path guard, NOT the native result and NOT a
certificate for the complete wrapper on arbitrary DWORD inputs. No caller
was silently changed to make this branch disappear. A-009 remains open.
Original DOS heap exhaustion thresholds, heap address reuse and corrupted
arena metadata also are not reproduced by the SDL allocator.

`platform.original_allocation_contract` runs the actual wrapper with an
isolated heap entry to check low-WORD requests and forced null/error handling.
`platform.original_sdl_heap_redirect` runs the actual wrapper, original runtime
redirect and SDL heap. Requests cover zero, startup sprite/tile sizes, high-bit
WORDs, FFFF, CWD-negative requests and a negative non-FFFF high WORD. Only UI
message/input boundaries are isolated in the SDL case. Oversize residue is not
tested or claimed correct.

110 headless and 129 SDL/local-asset tests pass. Fresh real-executable link
still fails on four original methods: combat execution, salvage parent,
Mech-stats screen and weapon-selection parent. No production no-op stubs added.

Combat execution follow-up: `1AE8:000C` BP-56 persists across attacks within
one invocation, but its entry value is not initialized. Ordinary personnel
hit/miss and Mech hit/miss paths assign it. High-bit weapon encoding versus
a Mech can bypass those assignments. The effects parent does not consume
that flag when graphics are disabled or main characters are already dead;
otherwise it controls impact sound/animation. This observation does not
justify initializing it to false in the full combat parent. Entry residue and
inter-call stack writes still need evidence or an explicit wider stack model.
