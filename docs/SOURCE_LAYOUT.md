# Source and repository layout

| Directory | Ownership |
| --- | --- |
| `src/Original` | Reconstructed original procedures, tables and globals |
| `src/SDL` | Host entry point, SDL video/audio/input, memory and file services |
| `src/Original/*.h`, `src/SDL/*.h` | Interfaces owned by their respective source domains |
| `tests` | Synthetic behavior tests and opt-in original-asset integration tests |
| `config` | Runtime configuration copied beside the executable |
| `assets/original-game` | Git-ignored user installation placeholder |
| `evidence/annotations` | Annotated decompiler output; never compiled |
| `evidence/assembly` | Segmented assembly listings; never compiled |
| `evidence/metadata` | Decompiler project metadata without the executable |
| `evidence/research` | Format, method, audit and investigation records |
| `docs` | Current policy/status and focused conversion documentation |
| `tools` | Repository, evidence and package validation utilities |

Files in `src/Original` retain segment-family prefixes because those names are
the stable connection to the executable. Topic suffixes make split procedures
readable without erasing provenance. Directory changes are organizational and
must not alter game behavior.

Do not add original binaries, artwork, maps, audio, saves or extracted media to
the repository. Local integration receives an external asset path through CMake
or the launcher.
