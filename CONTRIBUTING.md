# Contributing

This is a preservation reconstruction, not a redesign. Contributions should
make the original program more legible, verifiable or portable without
silently replacing its behavior.

## Preservation rules

1. Treat assembly and observed behavior from a supported original executable as
   authoritative. Annotations and reconstructed C are aids and must be corrected
   when they disagree with that evidence.
2. Keep original procedures, data flow and BattleTech concepts recognizable.
   Do not invent replacement game architecture or combine procedures merely for
   convenience.
3. Preserve original quirks and bugs by default. A compatibility fix needs a
   documented reason, a focused test and a clear host/preservation boundary.
4. Preserve DOS-facing wrappers. SDL and operating-system behavior belongs in
   `src/SDL`, beneath the lowest practical DOS, BIOS or hardware boundary.
5. Do not add original executables, assets, extracted media, save games or
   generated dumps. Use external ignored paths and identify binaries by hash.
   A documentation screenshot or title-derived image requires explicit review,
   must live under `docs/images`, and must be recorded as excluded material in
   `NOTICE.md` and `LICENSES/README.md`.
6. Cite segment:offset or other evidence in preservation-sensitive changes and
   update the relevant audit documentation and tests.

The detailed original-bug policy is in [`docs/ORIGINAL_BUGS.md`](docs/ORIGINAL_BUGS.md).

## Source style

The reconstructed C intentionally follows readable late-1980s/early-1990s C
conventions. Follow the existing file locally. Do not run a repository-wide
formatter or include drive-by whitespace changes. Prefer established C terms
and BattleTech vocabulary over address-only names, while retaining evidence
references where they aid comparison.

## Validation

Run before opening a pull request:

```powershell
./tools/Test-EvidenceLayout.ps1
./Build.ps1 -Headless -Configuration Release
./Build.ps1 -Configuration Release -Package
```

Use small, focused commits. Explain the original behavior, the evidence used,
and whether a changed boundary is original logic or an SDL/host substitution.
Contributed project-authored material is accepted under the scoped MIT grant in
`LICENSE`; that grant does not extend to reconstructed or original-rights data.
