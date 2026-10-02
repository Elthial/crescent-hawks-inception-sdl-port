$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot
$annotations=Join-Path $root 'evidence\annotations'
$assembly=Join-Path $root 'evidence\assembly'
$metadata=Join-Path $root 'evidence\metadata\BTECH.dcproject'
$evidenceRoot=Join-Path $root 'evidence'

if(-not (Test-Path -LiteralPath $metadata -PathType Leaf)){throw 'Decompiler metadata is missing.'}
$forbidden=@(Get-ChildItem -LiteralPath $evidenceRoot -Recurse -File | Where-Object {
    $_.Extension -in '.exe','.dll','.com','.bmp','.gif','.jpg','.jpeg','.png','.wav','.mid','.midi','.ogg','.mp3','.sav','.s01','.s02','.s03','.s04','.s05'
})
if($forbidden)
{
    throw "Executable or extracted media found in evidence: $($forbidden.FullName -join ', ')"
}

$annotationSegments=@(Get-ChildItem -LiteralPath $annotations -Filter 'BTECH_*.c' |
    ForEach-Object {$_.BaseName} | Sort-Object -Unique)
$assemblySegments=@(Get-ChildItem -LiteralPath $assembly -Filter 'BTECH_*.asm' |
    Where-Object {$_.BaseName -ne 'BTECH_PSP'} | ForEach-Object {$_.BaseName} |
    Sort-Object -Unique)
$missing=@(Compare-Object $annotationSegments $assemblySegments |
    Where-Object {$_.SideIndicator -eq '<='} | ForEach-Object InputObject)
if($missing){throw "Assembly evidence is missing for annotated segments: $($missing -join ', ')"}

Write-Output "Evidence layout verified: $($annotationSegments.Count) annotated segments and $($assemblySegments.Count) assembly segments."
