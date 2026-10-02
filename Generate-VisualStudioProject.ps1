param()
$ErrorActionPreference = 'Stop'
$projectRoot = $PSScriptRoot
$projectGuid = '{7C65E83A-78EE-47C3-93DE-29D0434BA7CE}'
$original = Join-Path $projectRoot 'src/Original'
$sdl = Join-Path $projectRoot 'src/SDL'
$files = @(Get-ChildItem -LiteralPath $original,$sdl -File |
    Where-Object { $_.Extension -in '.c','.h' } |
    Sort-Object DirectoryName,Name)
$sourcePaths = @($files | Where-Object Extension -eq '.c' | ForEach-Object {
    'src/' + $_.Directory.Name + '/' + $_.Name
})
$cmake = [IO.File]::ReadAllText((Join-Path $projectRoot 'CMakeLists.txt'))
$cmakePaths = @([regex]::Matches($cmake,'src/(?:Original|SDL)/[A-Za-z0-9_]+\.c') |
    ForEach-Object Value | Sort-Object -Unique)
if (@(Compare-Object $sourcePaths $cmakePaths).Count) {
    throw 'CMake and src/ C-file lists differ; update CMakeLists.txt before generating Visual Studio files.'
}

$project = @'
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="Current" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64"><Configuration>Debug</Configuration><Platform>x64</Platform></ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64"><Configuration>Release</Configuration><Platform>x64</Platform></ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <VCProjectVersion>17.0</VCProjectVersion>
    <ProjectGuid>__PROJECT_GUID__</ProjectGuid>
    <Keyword>MakeFileProj</Keyword>
    <RootNamespace>CrescentHawksInception</RootNamespace>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration">
    <ConfigurationType>Makefile</ConfigurationType><UseDebugLibraries>true</UseDebugLibraries>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Makefile</ConfigurationType><UseDebugLibraries>false</UseDebugLibraries>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings" />
  <ImportGroup Label="Shared" />
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" />
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|x64'" />
  <PropertyGroup Label="UserMacros" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <NMakeBuildCommandLine>powershell.exe -NoProfile -ExecutionPolicy Bypass -File &quot;$(ProjectDir)Build.ps1&quot; -Configuration Debug -Target CrescentHawksInception</NMakeBuildCommandLine>
    <NMakeReBuildCommandLine>powershell.exe -NoProfile -ExecutionPolicy Bypass -File &quot;$(ProjectDir)Build.ps1&quot; -Configuration Debug -Target CrescentHawksInception</NMakeReBuildCommandLine>
    <NMakeOutput>$(ProjectDir)build\CrescentHawksInception.exe</NMakeOutput>
    <LocalDebuggerCommand>$(NMakeOutput)</LocalDebuggerCommand>
    <LocalDebuggerWorkingDirectory>$(ProjectDir)</LocalDebuggerWorkingDirectory>
    <LocalDebuggerEnvironment>CHI_ASSET_DIRECTORY=$(ProjectDir)assets\original-game</LocalDebuggerEnvironment>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <NMakeBuildCommandLine>powershell.exe -NoProfile -ExecutionPolicy Bypass -File &quot;$(ProjectDir)Build.ps1&quot; -Configuration Release -Target CrescentHawksInception</NMakeBuildCommandLine>
    <NMakeReBuildCommandLine>powershell.exe -NoProfile -ExecutionPolicy Bypass -File &quot;$(ProjectDir)Build.ps1&quot; -Configuration Release -Target CrescentHawksInception</NMakeReBuildCommandLine>
    <NMakeOutput>$(ProjectDir)build-release\CrescentHawksInception.exe</NMakeOutput>
    <LocalDebuggerCommand>$(NMakeOutput)</LocalDebuggerCommand>
    <LocalDebuggerWorkingDirectory>$(ProjectDir)</LocalDebuggerWorkingDirectory>
    <LocalDebuggerEnvironment>CHI_ASSET_DIRECTORY=$(ProjectDir)assets\original-game</LocalDebuggerEnvironment>
  </PropertyGroup>
  <PropertyGroup>
    <NMakeIncludeSearchPath>$(ProjectDir)src\Original;$(ProjectDir)src\SDL;$(ProjectDir)build\_deps\sdl3-src\include;$(ProjectDir)build-release\_deps\sdl3-src\include;$(ProjectDir)build\_deps\sdl3-build\include-revision;$(ProjectDir)build-release\_deps\sdl3-build\include-revision</NMakeIncludeSearchPath>
    <NMakePreprocessorDefinitions>CHI_SDL_PLATFORM=1;CHI_SDL_SOUND_TIMING=1</NMakePreprocessorDefinitions>
  </PropertyGroup>
  <ItemGroup>
'@.Replace('__PROJECT_GUID__',$projectGuid)
$filters = @'
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
    <Filter Include="Original"><UniqueIdentifier>{AA6AF09B-9C5F-4A22-A9AA-A53041369E28}</UniqueIdentifier></Filter>
    <Filter Include="SDL"><UniqueIdentifier>{A638B13F-481D-4E72-9E20-A3A5244B333F}</UniqueIdentifier></Filter>
    <Filter Include="Build"><UniqueIdentifier>{4892653F-94B6-4EA2-9C5F-28A8DD2A4A6D}</UniqueIdentifier></Filter>
  </ItemGroup>
  <ItemGroup>
'@
foreach ($file in $files) {
    $folder = $file.Directory.Name
    $relative = "src\$folder\$($file.Name)"
    $item = if ($file.Extension -eq '.c') { 'ClCompile' } else { 'ClInclude' }
    $project += "    <$item Include=`"$relative`" />`r`n"
    $filters += "    <$item Include=`"$relative`"><Filter>$folder</Filter></$item>`r`n"
}
foreach ($name in @('Build.ps1','CMakeLists.txt','README.md','Run.ps1','Run-Inception.ps1',
        'config\CrescentHawksInception.ini')) {
    $project += "    <None Include=`"$name`" />`r`n"
    $filters += "    <None Include=`"$name`"><Filter>Build</Filter></None>`r`n"
}
$project += @'
  </ItemGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets" />
</Project>
'@
$filters += "  </ItemGroup>`r`n</Project>`r`n"
$solution = @'
Microsoft Visual Studio Solution File, Format Version 12.00
# Visual Studio Version 17
VisualStudioVersion = 17.0.31903.59
MinimumVisualStudioVersion = 10.0.40219.1
Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "CrescentHawksInception", "CrescentHawksInception.vcxproj", "__PROJECT_GUID__"
EndProject
Global
    GlobalSection(SolutionConfigurationPlatforms) = preSolution
        Debug|x64 = Debug|x64
        Release|x64 = Release|x64
    EndGlobalSection
    GlobalSection(ProjectConfigurationPlatforms) = postSolution
        __PROJECT_GUID__.Debug|x64.ActiveCfg = Debug|x64
        __PROJECT_GUID__.Debug|x64.Build.0 = Debug|x64
        __PROJECT_GUID__.Release|x64.ActiveCfg = Release|x64
        __PROJECT_GUID__.Release|x64.Build.0 = Release|x64
    EndGlobalSection
    GlobalSection(SolutionProperties) = preSolution
        HideSolutionNode = FALSE
    EndGlobalSection
EndGlobal
'@.Replace('__PROJECT_GUID__',$projectGuid)
$encoding = [Text.UTF8Encoding]::new($false)
[IO.File]::WriteAllText((Join-Path $projectRoot 'CrescentHawksInception.vcxproj'),$project,$encoding)
[IO.File]::WriteAllText((Join-Path $projectRoot 'CrescentHawksInception.vcxproj.filters'),$filters,$encoding)
[IO.File]::WriteAllText((Join-Path $projectRoot 'CrescentHawksInception.sln'),$solution,$encoding)
Write-Output "Visual Studio solution updated: $($sourcePaths.Count) C files, $(@($files | Where-Object Extension -eq '.h').Count) headers."
