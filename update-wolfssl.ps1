param ([string]$version = 'latest')

Push-Location $PSScriptRoot
$ErrorActionPreference = "Stop"

$release_info = Invoke-RestMethod -Uri "https://api.github.com/repos/wolfSSL/wolfssl/releases/$version"
$tag = $release_info.tag_name

$source_archive_uri = $release_info.zipball_url
$archive_path = New-TemporaryFile
$output_dir = 'totp/lib/wolfssl'

Write-Host "Downloading WolfSSL $tag"

try {
    try {
        $process = Start-Process -FilePath "curl" -ArgumentList $source_archive_uri, "-L", "-o", $archive_path -Wait -NoNewWindow -PassThru
        if ($process.ExitCode -ne 0) {
            throw "cURL exited with non-zero code"
        }
    }
    catch {
        $ProgressPreference = 'SilentlyContinue'
        Invoke-WebRequest -Uri $source_archive_uri -OutFile $archive_path -UseBasicParsing
    }

    if (Test-Path -Path $output_dir -PathType Container) {
        Remove-Item -Path "$output_dir/*" -Recurse -Force
    }
    else {
        New-Item -Path $output_dir -ItemType Directory -Force
    }

    Write-Host "Extracting WolfSSL $tag"
    $zip_archive = [System.IO.Compression.ZipFile]::OpenRead($archive_path)
    $wolfssl_include_paths = "^wolfSSL-wolfssl-\w+\/(wolfssl\/|wolfcrypt\/|README$|LICENSING$|COPYING$|\.gitignore$)"
    $wolfssl_exclude_paths = "^wolfSSL-wolfssl-\w+\/(wolfcrypt\/(test\/|benchmark\/|src\/port\/|src\/.+\.i$|src\/.+\.S$|src\/.+\.asm$|src\/.+\.am$|user-crypto\/)|(wolfssl\/(openssl\/|wolfcrypt\/port\/|.+\.am$|.+\.rc$)))"
    try {
        $zip_archive.Entries | Where-Object { $_.FullName -match $wolfssl_include_paths -and $_.FullName -notmatch $wolfssl_exclude_paths } | ForEach-Object {
            if (-not($_.FullName.EndsWith('/') -or $_.FullName.EndsWith('\'))) {
                $file = Join-Path $output_dir ($_.FullName -replace '^wolfSSL-wolfssl-\w+\/', '')
                $parent = Split-Path -Parent $file
                if (-not (Test-Path -LiteralPath $parent)) {
                    New-Item -Path $parent -Type Directory | Out-Null
                }
                [System.IO.Compression.ZipFileExtensions]::ExtractToFile($_, $file, $true)
            }
        }
    }
    finally {
        $zip_archive.Dispose()
    }
    Set-Content -Value "DisableFormat: true`nSortIncludes: Never" -Path (Join-Path $output_dir '.clang-format')
    Write-Host "Done"
}
finally {
    Remove-Item -Path $archive_path -Force
}

Pop-Location
