Write-Output "Generating Installers..."

iscc /DArch=x64 pack_installer.iss
iscc /DArch=arm64 pack_installer.iss

Write-Output "Done Generating Installers."

Write-Output "Compressing Zips..."

function New-GamePackageZip {
    param(
        [Parameter(Mandatory = $true)]
        [string] $Preset,
        [Parameter(Mandatory = $true)]
        [string] $DestinationZip
    )
    $_1 = Join-Path -Path "..\..\out" -ChildPath $Preset
    $executablePath = Join-Path -Path $_1 -ChildPath "CherryGrove.exe"
    $mandatoryItems = @(
        "..\..\LICENSE",
        "..\..\assets",
        $executablePath
    )
    $mandatoryRootDirectory = "..\readmes"
    $optionalItems = @(
        "..\..\packs",
        "..\..\saves",
        "..\..\tests",
        "..\..\settings.json"
    )
    $stagingDirectory = Join-Path $env:TEMP ("zip-staging-" + [guid]::NewGuid())
    function Copy-ItemToZipRoot {
        param(
            [Parameter(Mandatory = $true)]
            [string] $SourcePath
        )
        $destinationPath = Join-Path -Path $stagingDirectory -ChildPath (Split-Path -Leaf $SourcePath)
        if (Test-Path -LiteralPath $destinationPath) {
            throw "Zip root conflict: '$destinationPath' already exists."
        }
        Copy-Item -LiteralPath $SourcePath -Destination $stagingDirectory -Recurse -Force
    }
    try {
        New-Item -ItemType Directory -Path $stagingDirectory | Out-Null
        foreach ($item in $mandatoryItems) {
            if (-not (Test-Path -LiteralPath $item)) {
                throw "Mandatory item does not exist: $item"
            }
            Copy-ItemToZipRoot -SourcePath $item
        }
        if (-not (Test-Path -LiteralPath $mandatoryRootDirectory -PathType Container)) {
            throw "Mandatory root directory does not exist: $mandatoryRootDirectory"
        }
        Get-ChildItem -LiteralPath $mandatoryRootDirectory -Force | ForEach-Object {
            Copy-ItemToZipRoot -SourcePath $_.FullName
        }
        foreach ($item in $optionalItems) {
            if (Test-Path -LiteralPath $item) {
                Copy-ItemToZipRoot -SourcePath $item
            }
        }
        Compress-Archive -Path (Join-Path $stagingDirectory "*") -DestinationPath $DestinationZip -Force
    }
    finally {
        Remove-Item -LiteralPath $stagingDirectory -Recurse -Force -ErrorAction SilentlyContinue
    }
}

New-GamePackageZip -Preset "windows-x64-release" -DestinationZip "CherryGrove_windows_x64.zip"
New-GamePackageZip -Preset "windows-arm64-release" -DestinationZip "CherryGrove_windows_arm64.zip"

Write-Output "Done Compressing Zips."