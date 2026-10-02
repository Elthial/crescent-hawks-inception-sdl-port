param(
    [string]$OriginalAssetDirectory=(Join-Path $PSScriptRoot 'assets\original-game'),
    [ValidateSet('Debug','Release')][string]$Configuration='Release',
    [switch]$NoBuild,
    [switch]$Isolated,
    [ValidateSet(240,750,1510,3000)][int]$SoundCycles
)

# Compatibility name retained for existing local workflows.
$ErrorActionPreference='Stop'
& (Join-Path $PSScriptRoot 'Run-Inception.ps1') @PSBoundParameters
exit $LASTEXITCODE
