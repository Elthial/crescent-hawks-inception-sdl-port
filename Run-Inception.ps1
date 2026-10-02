param(
    [string]$OriginalAssetDirectory=(Join-Path $PSScriptRoot 'assets\original-game'),
    [ValidateSet('Debug','Release')][string]$Configuration='Release',
    [switch]$NoBuild,
    [switch]$Isolated,
    [ValidateSet(240,750,1510,3000)][int]$SoundCycles
)

$ErrorActionPreference='Stop'
$assetDirectory=(Resolve-Path -LiteralPath $OriginalAssetDirectory).Path
$buildName=if($Configuration -eq 'Release'){'build-release'}else{'build'}
$executable=Join-Path $PSScriptRoot "$buildName\CrescentHawksInception.exe"

if(-not $NoBuild)
{
    & (Join-Path $PSScriptRoot 'Build.ps1') -Configuration $Configuration -Target CrescentHawksInception
    if($LASTEXITCODE -ne 0){throw "$Configuration Inception build failed."}
}
if(-not (Test-Path -LiteralPath $executable -PathType Leaf))
{
    throw 'CrescentHawksInception.exe is not built.'
}
if(-not (Test-Path -LiteralPath (Join-Path $assetDirectory 'INFOCOM.CMP') -PathType Leaf))
{
    throw "The selected original asset directory does not contain INFOCOM.CMP: $assetDirectory"
}

if($Isolated)
{
    $name='inspection-'+[DateTime]::UtcNow.ToString('yyyyMMdd-HHmmss')+'-'+[Guid]::NewGuid().ToString('N').Substring(0,8)
    $privateDirectory=Join-Path (Split-Path $executable) "runs\$name"
    New-Item -ItemType Directory -Path $privateDirectory -Force | Out-Null
    Get-ChildItem -LiteralPath $assetDirectory -File | Where-Object {$_.Extension -ine '.exe'} |
        Copy-Item -Destination $privateDirectory
    $assetDirectory=$privateDirectory
    Write-Output "Isolated original-data and save copy: $assetDirectory"
}

$previousAssetDirectory=$env:CHI_ASSET_DIRECTORY
try
{
    $env:CHI_ASSET_DIRECTORY=$assetDirectory
    if($PSBoundParameters.ContainsKey('SoundCycles'))
    {
        & $executable --sound-cycles $SoundCycles
    }
    else
    {
        & $executable
    }
    if($LASTEXITCODE -ne 0){throw "Inception exited with code $LASTEXITCODE."}
}
finally
{
    $env:CHI_ASSET_DIRECTORY=$previousAssetDirectory
}
