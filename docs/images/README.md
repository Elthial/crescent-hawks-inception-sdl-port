# README image provenance

`title-banner.png` is an integer-scaled crop of the title lettering decoded
from `BTTITLE.CMP` in a legally owned original installation. It contains no raw
game resource file.

`gameplay.png` was exported on 2 October 2026 by the current Release build's
real Mech-statistics path, then enlarged from 320x200 to 960x600 using exact
3x nearest-neighbour pixel duplication. The source export was produced from a
private original installation by running the test with that installation as
its working directory:

```powershell
& <repository>/build-release/test_connected_combat.exe mech-statistics <output.bmp>
```

This fresh extraction supersedes the 18 September 2026 preview captured before
the 200-line RGBI scanout correction. The images remain original-rights
documentation material as described by `NOTICE.md` and `LICENSES/README.md`.
