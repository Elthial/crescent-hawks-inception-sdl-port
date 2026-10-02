# Copy protection and player reference materials

The installation includes player-facing reference images that are not loaded as
game assets. Two of them support navigation or historical copy-protection
questions. They remain copyrighted external material and must not be copied
into the replacement executable or repository.

## Supplied reference files

| File | Role | Runtime-loaded by `BTECH.EXE`? |
|---|---|:---:|
| `BTchi.jpg` | Box art. | No reference found. |
| `CHI-MAP.GIF` | Annotated map of the procedurally generated world, supplied as a player navigation aid. | No reference found. |
| `CHICODE1.GIF` | Labelled mech-component diagram used to answer the training-centre copyright test. | No reference found. |
| `BTECH.TXT` | Installation documentation/readme material. | No reference found. |

The executable not referring to these filenames is expected: the game presents
an unlabelled screen and asks the player to consult material supplied outside
the program.

## Training-centre mech-component test

Remnants of the original test survive around `1467:08A8`, currently annotated
as `Copyright_Check_Mech_Quiz`:

1. Load and decompress `BTSTATS.CMP` for the on-screen mech diagram.
2. Explain that a written quiz is required before a training mission.
3. Display a list of 20 executable-embedded mech-part names.
4. Choose test locations and obtain their screen coordinates from tables near
   `3EDB:28C4` and `3EDB:28D8`.
5. Draw a box around the target and a line from the right-hand divider to it.
6. Ask the player to identify the part and compare the selection with the
   answer table near `3EDB:28EC`.
7. On failure, set the retry timer at `3092:D320`.

`CHICODE1.GIF` is the external labelled reference for this mechanism; the game
does not load that GIF. The user's executable was modified before this research
began so the test is bypassed/cut from ordinary play, although substantial code,
text, coordinate, and answer-table remnants remain. The bypass site is now
verified at `0FDC:0056`: an unconditional jump skips the dormant training-BLD
gate at `0058..0084`. That skipped block tests the retry cooldown and input
modes, calls `1467:08A8`, and suppresses the training interaction on a nonzero
quiz result. See [the BLD interaction entry review](../phase4/BTECH_0FDC_BLD_INTERACTION_ENTRY.md).

The complete [1467 segment review](../phase4/BTECH_1467_SEGMENT_REVIEW.md)
records all ten target coordinates, their correct part-name rows, the
three-distinct-question retry logic and the WORD pass/fail return. The clean
baseline initializes the question counter to zero; its bypass is at the caller,
not the previously suggested counter initialization inside the quiz.

## Star-map selection test

A second copyright-code mechanism survives at `135D:0980`, annotated as
`Cache_StarMap_CorrectPassword`. It belongs to a late Star League cache puzzle:

- the player toggles stars on the map;
- the code checks seven required map positions from the table near `246C:241E`;
- it rejects any additional selected star within the relevant tile range;
- success sets the white-code state at `3092:D34A`, enabling the HPG-related
  progression path.

The intended answer was supplied on another external image/reference card that
is missing from the current installation. It can be found independently online,
but is not required as repository material: the executable retains the seven
answer locations and validation behaviour needed for preservation work.

## Porting policy

- Do not redistribute the supplied or missing reference images.
- Preserve and document executable-resident prompts, answer tables, coordinates,
  and validation logic as part of reconstructing program behaviour.
- Keep historical copy-protection behaviour separate from the original-asset
  loader contract: these reference images are not files the game opens.
- Decide later, as an explicit compatibility choice, whether the replacement
  executable exposes, bypasses, or optionally emulates each test.

## Evidence

- Project-owner identification of the supplied images and the pre-existing
  training-test removal.
- `Btech/BTECH_1467.c`, `1467:08A8`: mech quiz presentation, line/box drawing,
  selection comparison, and failure timer.
- `Btech/BTECH_135D.c`, `135D:0980`: seven-star selection validation and
  white-code state transition.
- `BTech-Reko-expanded/BTECH.reko/`: clean assembly baseline for reachability
  checks and the verified bypass instructions.
