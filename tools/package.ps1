# Package HUD Position Manager (The Witcher 3):
#   1. tools\gen_strings.py --check  (the settings page's labels are what the generator writes)
#   2. the built movie bundle exists (tools\build_flash.py writes it; it is not in git)
#   3. copy dist\ to <stage>\Witcher 3 - HUD Position Manager\HUD Position Manager <VERSION>\
#   4. stamp the version into the package README (rule 6: the version at the top of the README)
# The version comes from VERSION, which only version-gate.ps1 writes. The stage folder comes from the project's resolver
# (distro-names.ps1 Resolve-PackageRoot), never a hand-joined path. The same shape as Simple Loadout System W3's.
param(
    [string]$ProjectRoot = 'D:\Claude output',
    [string]$Stage = '7. current test builds',
    [string]$Label = ''
)
$ErrorActionPreference = 'Stop'
$repo = Split-Path -Parent $PSScriptRoot
$version = (Get-Content -LiteralPath (Join-Path $repo 'VERSION') -Raw).Trim()
if ($version -notmatch '^\d+\.\d+\.\d+$' -or $version -eq '0.0.0') { throw "VERSION is '$version' - issue a number with version-gate.ps1 -Action bump first" }

& python -I (Join-Path $PSScriptRoot 'gen_strings.py') --check
if ($LASTEXITCODE -ne 0) { throw 'gen_strings.py --check failed - the package is not built' }
# the AMF page's eleven languages (bin\x64_dx12\AMF\Translations, rule 66): every TR key in every language
& python -I (Join-Path $PSScriptRoot 'gen_translations.py') --check
if ($LASTEXITCODE -ne 0) { throw 'gen_translations.py --check failed - the package is not built' }
foreach ($f in 'bin\x64_dx12\HUDPositionManager.asi', 'bin\x64_dx12\AMF\Translations\HUDPositionManager_english.txt') {
    if (-not (Test-Path -LiteralPath (Join-Path $repo "dist\$f"))) { throw "dist\$f is missing - build the page (xmake) and run tools\gen_translations.py" }
}
$content = Join-Path $repo 'dist\Mods\modHUDPositionManager\content'
foreach ($f in 'blob0.bundle', 'metadata.store') {
    if (-not (Test-Path -LiteralPath (Join-Path $content $f))) { throw "$f is missing - run tools\build_flash.py first" }
}

. (Join-Path $ProjectRoot '.MD\scripts\distro-names.ps1')
$root = Resolve-PackageRoot -Root (Join-Path $ProjectRoot $Stage) -ModName 'HUD Position Manager' -Game 'Witcher 3' -ProjectRoot $ProjectRoot
$name = "HUD Position Manager $version"
if ($Label) { $name = "$name ($Label)" }
$pkg = Join-Path $root $name
if (Test-Path -LiteralPath $pkg) { throw "$pkg already exists - a version is packaged once" }
New-Item -ItemType Directory -Force -Path $pkg | Out-Null
Copy-Item -Path (Join-Path $repo 'dist\*') -Destination $pkg -Recurse
Copy-Item -LiteralPath (Join-Path $repo 'LICENSE') -Destination $pkg

$readme = Join-Path $pkg 'README.txt'
$text = [System.IO.File]::ReadAllText($readme)
$text = [regex]::Replace($text, '(?m)^Version \d+\.\d+\.\d+', "Version $version", 1)
[System.IO.File]::WriteAllText($readme, $text, (New-Object System.Text.UTF8Encoding($false)))

Write-Output "packaged $pkg"
