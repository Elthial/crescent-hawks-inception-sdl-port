# Runtime and acceptance status

The preservation executable links and starts through original `Setup_Game`.
Connected automated coverage reaches original-asset startup, title flow, new
game setup, Citadel exploration, menus, services, training, Mech combat,
save/load and normal shutdown.

The latest recorded local baseline is:

- 113 passing asset-independent headless tests;
- 164 passing SDL/original-asset tests; and
- successful Debug and Release linkage of the game executable.

The SDL count includes opt-in tests using the owner's private installation;
the headless count is the public, asset-independent repository baseline.

## Known limits

- No complete manual playthrough has been certified.
- Video composition and PC-speaker/music output have connected tests but are
  not pixel-, cycle- or waveform-identical certifications.
- Several exceptional native paths depend on stack or residual storage that
  portable C cannot reproduce safely without more calling-context evidence.
- Original assets are required for normal play and are not distributed.

The detailed historic boundary list and acceptance checklist are retained in
[`history/PLAYABLE_BUILD_2026-09-18.md`](history/PLAYABLE_BUILD_2026-09-18.md).
Treat its old paths and build commands as a dated record.
