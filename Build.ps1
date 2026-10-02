param([switch]$Headless,[switch]$Game,[switch]$Package,[string[]]$Target=@(),[string]$TestFilter='',
    [string]$SDL3Directory='',[string]$OriginalAssetDirectory='',
    [ValidateSet('Debug','Release')][string]$Configuration='Debug')
$ErrorActionPreference='Stop'
if($Game -and $Headless){throw 'The preservation game executable requires the SDL build'}
if($Package -and $Headless){throw 'The playable package requires the SDL build'}
if($Package -and $Target.Count){throw 'Use either -Package or -Target, not both'}
if($Package){$Target=@('inception-package')}
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$installation=& $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if(-not $installation){throw 'Visual Studio C/C++ tools required'}
Import-Module (Join-Path $installation 'Common7/Tools/Microsoft.VisualStudio.DevShell.dll')
Enter-VsDevShell -VsInstallPath $installation -SkipAutomaticLocation -DevCmdArguments '-arch=x64 -host_arch=x64'
$cmake=Join-Path $installation 'Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe'
$ctest=Join-Path (Split-Path $cmake) 'ctest.exe'
$buildName=if($Headless){'build-headless'}else{'build'}
if($Configuration -eq 'Release'){$buildName+='-release'}
$build=Join-Path $PSScriptRoot $buildName
$options=@('-S',$PSScriptRoot,'-B',$build,'-G','Ninja',"-DCMAKE_BUILD_TYPE=$Configuration")
if($Headless){$options+='-DCHI_BUILD_SDL=OFF'}
if($SDL3Directory){$options+="-DSDL3_DIR=$SDL3Directory"}
$options+="-DCHI_ORIGINAL_ASSET_DIRECTORY=$OriginalAssetDirectory"
& $cmake @options
if($LASTEXITCODE -ne 0){throw 'C17 configuration failed'}
if($Target.Count){ & $cmake --build $build --target @Target }
else { & $cmake --build $build }
if($LASTEXITCODE -ne 0){throw 'C17 build failed'}
if($TestFilter){
    & $ctest --test-dir $build --output-on-failure --no-tests=error -R $TestFilter
    if($LASTEXITCODE -ne 0){throw 'Selected C17 tests failed'}
} elseif(-not $Target.Count){
    & $ctest --test-dir $build --output-on-failure
    if($LASTEXITCODE -ne 0){throw 'C17 tests failed'}
} else {
    Write-Output 'Selected targets built; no tests requested. Pass -TestFilter to run matching CTest cases.'
}
if($Game -and -not ($Target -contains 'CrescentHawksInception')){
    & $cmake --build $build --target CrescentHawksInception
    if($LASTEXITCODE -ne 0){throw 'Preservation executable link failed; remaining original routines must be converted, not stubbed'}
}
if($Package){
    $packageDirectory=Join-Path $build 'package'
    & (Join-Path $PSScriptRoot 'tools/Test-MinimalPackage.ps1') -PackageDirectory $packageDirectory
    $version=(Get-Content -LiteralPath (Join-Path $PSScriptRoot 'VERSION') -Raw).Trim()
    $checksumPath=Join-Path $build "CrescentHawksInception-$version-SHA256SUMS.txt"
    & (Join-Path $PSScriptRoot 'tools/Write-PackageChecksums.ps1') `
        -PackageDirectory $packageDirectory -OutputPath $checksumPath
    Write-Output "Runtime package: $packageDirectory"
    Write-Output "Package checksums: $checksumPath"
}
