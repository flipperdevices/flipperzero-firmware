[Richard Kuhnt](https://github.com/r15ch13) wrote the following:

I watched [your video about the CSV conversion](https://youtu.be/7pPN2Ph8eJ0) and that inspired me to write a small PowerShell function to do that.

```pwsh
# Converts Flipper SubGhz RAW Files to PSCustomObject[]
function ConvertFrom-SubGhzRAW {
    param(
        [Parameter(Mandatory = $true, Position = 0, ValueFromPipeline = $true)]
        [String] $Path
    )

    process {
        $data = Get-Content $Path
        if(!$data.Contains("Filetype: Flipper SubGhz RAW File")) {
            throw "$Path is not a Flipper SubGhz RAW File"
        }
        $data | Select-Object -Skip 5
        | ForEach-Object { $_.Replace("RAW_Data: ", "") }
        | Join-String -Separator " "
        | Select-String -Pattern '(\d+)\s(-\d+)' -AllMatches
        | ForEach-Object { $_.Matches }
        | ForEach-Object { [PSCustomObject]@{ Tone = $_.Groups[1]; Silence = $_.Groups[2] } }
    }
}
```


Copy and paste to console hit `<Enter>`, then run:

```pwsh
# convert one file
ConvertFrom-SubGhzRAW mysubfile.sub | ConvertTo-Csv | Out-File mycsvfile.csv
```

```pwsh
# convert every *.sub file in a directory
Get-ChildItem *.sub | ForEach-Object { ConvertFrom-Sub -Path $_ | ConvertTo-Csv | Out-File "$($_.BaseName).csv" }
```