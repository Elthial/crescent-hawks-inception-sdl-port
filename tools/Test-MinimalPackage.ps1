param([Parameter(Mandatory=$true)][string]$PackageDirectory)

$ErrorActionPreference='Stop'
$expected=@('CrescentHawksInception.exe','CrescentHawksInception.ini')
$actual=@(Get-ChildItem -LiteralPath $PackageDirectory -File | Sort-Object Name | ForEach-Object Name)

if(Compare-Object ($expected | Sort-Object) $actual)
{
    throw "Runtime package must contain exactly: $($expected -join ', ')"
}

$configuration=Get-Content -LiteralPath (Join-Path $PackageDirectory 'CrescentHawksInception.ini') -Raw
if($configuration -notmatch '(?m)^asset_directory=')
{
    throw 'Runtime configuration does not declare asset_directory.'
}

Write-Output 'Minimal runtime package verified: executable and configuration only.'
