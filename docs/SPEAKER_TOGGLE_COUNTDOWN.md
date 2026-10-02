# Sol: original software speaker gateways

Expanded ASM207F:007D..00D0 checked. Retained original entry points redirect
their hardware bodies to the separate SDL layer.007D stores its seed, XORs
only speaker-control bit1, adds9248 with WORD wrap, rotates right3 and returns
the native DX result explicitly.00A9 stores the mask/minimum, counts down
(DX AND mask) OR minimum and leaves the delay state unchanged. A zero initial
count still runs65536 iterations, rather than returning immediately.

Platform speaker state now distinguishes low control bits0 and1. Ordinary
tone/music enable sets both; off clears both. Software noise toggles bit1
without enabling the PIT gate: rendered output is a direct static high level
or silence, not a free-running PIT square wave. Divisor and phase are not
rewritten by toggle/countdown. These are idealized output levels, not a model
of physical speaker filtering or measured acoustic waveform.

The real SDL platform test verifies all65536 seeds against an independent
division/remainder rotation expression, alternation of bit1, static direct
samples, mask/minimum publication and unchanged delay state including zero
countdown. It also dispatches all18 original effects through the entire real
dispatcher/repetition/sweep/tone/noise/gateway chain and confirms final speaker
shutdown. Scale0 is used for that integration check only; it is not changed
in production and the test does not claim timing/audible fidelity.

Physical busy-wait duration, asynchronous callback sampling of fast changes,
actual waveforms and emulator comparison remain unverified. No pre-rendered
toolkit sounds replace original methods. All105 headless /123 SDL-local suites
pass. The real executable has six missing dependencies, none in the sound
effect chain. Preservation is still not playable/complete.
