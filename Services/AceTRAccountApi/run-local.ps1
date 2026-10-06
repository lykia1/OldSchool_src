$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$preServerDir = Join-Path $repoRoot "Server\GameServer\PreServer"
$secretPath = Join-Path $preServerDir "launcher_api_secret.txt"
$appSettingsPath = Join-Path $PSScriptRoot "appsettings.json"
$appSettingsExample = Join-Path $PSScriptRoot "appsettings.example.json"

if (-not (Test-Path $secretPath)) {
    $bytes = New-Object byte[] 48
    [System.Security.Cryptography.RandomNumberGenerator]::Fill($bytes)
    $secret = [Convert]::ToBase64String($bytes)
    [System.IO.File]::WriteAllText($secretPath, $secret, [System.Text.Encoding]::ASCII)
    Write-Host "Generated PreServer launcher API signing secret."
} else {
    $secret = (Get-Content $secretPath -Raw).Trim()
}

if ([string]::IsNullOrWhiteSpace($secret)) {
    throw "launcher_api_secret.txt is empty."
}

$env:ACETR_LAUNCHER_API_SECRET = $secret

if (-not (Test-Path $appSettingsPath)) {
    Copy-Item $appSettingsExample $appSettingsPath
    Write-Host ""
    Write-Host "Created appsettings.json from template."
    Write-Host "Edit its AccountDb and GameDb connection strings, then run this script again."
    exit 1
}

Write-Host "Starting AceTR Account API on http://127.0.0.1:5080"
dotnet run --project (Join-Path $PSScriptRoot "AceTRAccountApi.csproj") --urls "http://127.0.0.1:5080"
