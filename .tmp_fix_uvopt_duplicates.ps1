$path = 'MDK-ARM/dm_mcV610.uvoptx'
$text = Get-Content -Raw $path
$seen = 0
$pattern = '(?ms)^    <File>.*?<PathWithFileName>\.\./LibSE/Algorithm/Control/ADRC/adrc\.c</PathWithFileName>.*?^    </File>'
$text = [regex]::Replace($text, $pattern, {
    param($match)
    $script:seen++
    if ($script:seen -eq 1) { return $match.Value }
    return ''
})
[System.IO.File]::WriteAllText($path, $text, (New-Object System.Text.UTF8Encoding($false)))
