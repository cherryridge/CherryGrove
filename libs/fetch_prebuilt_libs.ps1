<#
.PARAMETER 7zPath
    Path to 7z.exe (or 7za/7z on *nix). If on PATH, just "7z".
#>

param(
    [string] $7zPath = '7z'
)

$osArch = [System.Runtime.InteropServices.RuntimeInformation,mscorlib]::ProcessArchitecture
if (-not $osArch) {
    $osArch = [System.Runtime.InteropServices.RuntimeInformation,mscorlib]::OSArchitecture
}
switch ($osArch) {
    'X64'   { $archTag = 'x64'; break }
    'Arm64' { $archTag = 'arm64'; break }
    'X86'   { throw "x86 is not supported." }
    default { throw "Architecture not detected." }
}

$headers = @{ 'User-Agent' = 'ps-github-latest-release' }

function Download-Library {
    param (
        [string]$name
    )
    Write-Host "Downloading $name for Windows $archTag..."
    $release = Invoke-RestMethod -Method GET -Uri https://api.github.com/repos/cherryridge/dep_$name/releases/latest -Headers $headers
    if(-not $release -or $release.assets.Count -eq 0) {
        Write-Error "No assets found on latest release of dep_$name, check it at https://api.github.com/repos/cherryridge/dep_$name/releases/latest."
        return
    }
    $assetDebug = $release.assets | Where-Object { $_.name -like "*windows_$archTag`_debug.7z" }
    $assetRelease = $release.assets | Where-Object { $_.name -like "*windows_$archTag`_release.7z" }
    if(-not $assetDebug -or -not $assetRelease) {
        Write-Error "Not all asset was found on latest release of dep_$name, check it manually at https://api.github.com/repos/cherryridge/dep_$name/releases/latest."
        return
    }
    Write-Host "Downloading $($assetDebug.name): $($assetDebug.browser_download_url)..."
    Invoke-WebRequest -Uri $assetDebug.browser_download_url -Headers $headers -OutFile $assetDebug.name
    Write-Host "Downloading $($assetRelease.name): $($assetRelease.browser_download_url)..."
    Invoke-WebRequest -Uri $assetRelease.browser_download_url -Headers $headers -OutFile $assetRelease.name
    Get-ChildItem -Path "$PSScriptRoot\$name\debug" -Force | Where-Object { $_.name -ne '.gitignore'} | Remove-Item -Recurse -Force
    Get-ChildItem -Path "$PSScriptRoot\$name\release" -Force | Where-Object { $_.name -ne '.gitignore'} | Remove-Item -Recurse -Force
    Write-Host "Extracting $($assetDebug.name) to $PSScriptRoot\$name\debug..."
    Start-Process -FilePath $7zPath -ArgumentList "x `"$($assetDebug.name)`" -o`"$PSScriptRoot\$name\debug`" -y" -NoNewWindow -PassThru -Wait
    Write-Host "Extracting $($assetRelease.name) to $PSScriptRoot\$name\release..."
    Start-Process -FilePath $7zPath -ArgumentList "x `"$($assetRelease.name)`" -o`"$PSScriptRoot\$name\release`" -y" -NoNewWindow -PassThru -Wait
    Remove-Item -Force $assetDebug.name
    Remove-Item -Force $assetRelease.name
}

Download-Library -name 'bgfx'
Download-Library -name 'luajit'
Download-Library -name 'soloud'
Download-Library -name 'v8'
Download-Library -name 'wasmtime'

Write-Host "All done."