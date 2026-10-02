param(
    [Parameter(Mandatory = $true)]
    [string]$PackageDirectory,
    [Parameter(Mandatory = $true)]
    [string]$OutputPath
)

$ErrorActionPreference = 'Stop'
$resolvedPackage = (Resolve-Path -LiteralPath $PackageDirectory).Path
$files = @(Get-ChildItem -LiteralPath $resolvedPackage -File | Sort-Object Name)

if ($files.Count -eq 0)
{
    throw "No package files found in $resolvedPackage"
}

$lines = foreach ($file in $files)
{
    $hash = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
    "$hash *$($file.Name)"
}

Set-Content -LiteralPath $OutputPath -Value $lines -Encoding ascii
Write-Host "SHA-256 checksums: $OutputPath"
