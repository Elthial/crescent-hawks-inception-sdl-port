# Sol: restore native animation/graphics workspace overlap

Native246C:244B is the beginning of both the general graphics file workspace
and animation frame workspace. Native246C:42C3 is the animation file view,
exactly1E78 bytes after244B. The frame and file occupy5D78 bytes within the
8000-byte graphics workspace.0800 scene/decode paths use these same native
addresses; they are not separate allocations or preserved copies.

The C port previously allocated SceneAnimation independently. That was a
preservation discrepancy: loading/clearing either view could not overwrite
the other as the original does. MapRuntime's workspace is now an anonymous
union containing both original address views. The old separate object is
removed. Original method bodies continue to use their existing named views;
there is no new gameplay algorithm, invented preservation backup or host
pointer cast. C17 union/character views keep the overlapping access defined.

Compile-time assertions check the shared start and frame/file offsets, the
full map-runtime extent and unchanged party-position offsets. A focused test
checks all5D78 overlapping bytes in both directions and confirms the remaining
workspace tail is untouched. Existing real scene decode/playback and local
original ANM frame tests also pass, including real external asset imports.
No assets are embedded or exported by this change.

All107 headless /125 SDL-local suites pass. The playable build remains unfinished
with the five original unresolved dependency families listed in PLAYABLE_BUILD.
This overlap correction does not resolve native stack starting phases in the
statistics screen, allocator or other residual-value cases.
