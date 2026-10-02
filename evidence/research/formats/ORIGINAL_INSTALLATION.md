# Original installation manifest

## Reference set

The ignored local `chinception/` directory is the current reference
installation. This manifest contains metadata only. It neither embeds nor
redistributes file contents.

Executable fingerprint:

| File | Bytes | SHA-256 |
|---|---:|---|
| `BTECH.EXE` | 152429 | `F2A9A023D79927B8072DE11DDD6E03DA6D3357D49181D8FBA6D12988BF8CC0EE` |
| `$VERIFY.EXE` | 16281 | `B410E7135DC48523452DFD14F2288A1374F9D26F6774337A554190811E1A856D` |

The executable hash identifies the binary against which current segment and
offset claims should be checked. Additional compatible releases may be added as
separate profiles rather than silently sharing offsets.

## Clean decompiler baseline

The ignored `BTech-Reko-expanded/` directory contains the executable used for
the pre-annotation Reko output:

| Profile | File | Bytes | SHA-256 |
|---|---|---:|---|
| `reko-expanded-2020` | `BTech-Reko-expanded/BTECH.EXE` | 260416 | `F224BA0670AAC22162EDCEACBFF3CF2AE2CEEB90921E9BF7EE9FFF489B7089FE` |

This does not match the `chinception` executable by size or hash because the
installed executable was packed/encrypted and the Reko input was unpacked for
analysis. The installed image has zero MZ relocations and contains a packed-file
corruption diagnostic; the expanded image has 3,292 relocations in a
reconstructed 13,200-byte header. Do not transfer packed file offsets directly
to the expanded image; use the unpacked segment addresses and document any
required mapping. Full profile rules are in
[../reference/EXECUTABLE_PROFILES.md](../reference/EXECUTABLE_PROFILES.md).

## Runtime data inventory

| Family | Files | Observed sizes |
|---|---:|---|
| BLD building scripts | 26 | 362–8501 bytes |
| MTP maps | 15 | 605, 768, 1565, or 4637 bytes |
| ANM animations | 22 (`O0`–`O21`) | 2176–16128 bytes; all multiples of 128 |
| CMP graphics | 7 | 287–27488 bytes |
| ICN graphics | 5 | 3613–29706 bytes |
| Save slots | 6 (`GAME1`–`GAME6`) | 3913 bytes each (`0x0F49`) |
| Attractor/self-play instructions | 1 (`DEMOFILE`) | 1023 bytes |
| Intro-song sound data | 1 (`WWOODBT.SIF`) | 4352 bytes |

The installation also contains player documentation/reference material:

- `BTchi.jpg` is box art.
- `CHI-MAP.GIF` is an annotated navigation map of the procedurally generated
  game world.
- `CHICODE1.GIF` is a labelled mech-component diagram for the historical
  training-centre copyright test.
- `BTECH.TXT` is installation documentation.

No game-executable reference to these filenames has been found; they are
consulted by the player rather than loaded as runtime assets. Another external
image supplied the seven-star answer for a late-game copyright-code puzzle, but
is absent from this installation. `$VERIFY.EXE` is a separate utility. All of
these remain external copyrighted material. See
[Copy protection and player references](../reference/COPY_PROTECTION_AND_PLAYER_REFERENCES.md).
Runtime evidence for `DEMOFILE` and `WWOODBT.SIF` is recorded in
[DEMO_AND_SOUND.md](DEMO_AND_SOUND.md).

## Compatibility rule

Future loaders should identify a supported installation using a small set of
file hashes and structural checks. Hash mismatch must report an unsupported or
different release, not imply that the user's copy is invalid. Where possible,
format parsing should remain version-tolerant while executable offsets remain
bound to a specific executable profile.
