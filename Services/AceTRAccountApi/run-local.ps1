$ErrorActionPreference = "Stop"

try {
    $repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
    $preServerDir = Join-Path $repoRoot "Server\GameServer\PreServer"
    $preServerOutputDir = Join-Path $repoRoot "BuildResult\Server\Release"
    $secretPath = Join-Path $preServerDir "launcher_api_secret.txt"
    $appSettingsPath = Join-Path $PSScriptRoot "appsettings.json"
    $appSettingsExample = Join-Path $PSScriptRoot "appsettings.example.json"

    if (-not (Test-Path $secretPath)) {
        $bytes = New-Object byte[] 48
        $rng = New-Object System.Security.Cryptography.RNGCryptoServiceProvider
        try {
            $rng.GetBytes($bytes)
        }
        finally {
            $rng.Dispose()
        }

        $secret = [Convert]::ToBase64String($bytes)
        [System.IO.File]::WriteAllText($secretPath, $secret, [System.Text.Encoding]::ASCII)
        Write-Host "Generated PreServer launcher API signing secret:"
        Write-Host "  $secretPath"
    }
    else {
        $secret = (Get-Content $secretPath -Raw).Trim()
        Write-Host "Using existing launcher API signing secret:"
        Write-Host "  $secretPath"
    }

    if ([string]::IsNullOrWhiteSpace($secret)) {
        throw "launcher_api_secret.txt is empty."
    }

    $env:ACETR_LAUNCHER_API_SECRET = $secret

    if (-not (Test-Path $preServerOutputDir)) {
        New-Item -ItemType Directory -Path $preServerOutputDir -Force | Out-Null
    }

    $outputSecretPath = Join-Path $preServerOutputDir "launcher_api_secret.txt"
    Copy-Item $secretPath $outputSecretPath -Force
    Write-Host "Synced launcher API secret beside PreServer output:"
    Write-Host "  $outputSecretPath"
    Write-Host ""

    if (-not (Test-Path $appSettingsPath)) {
        Copy-Item $appSettingsExample $appSettingsPath
        Write-Host "Created appsettings.json from template:"
        Write-Host "  $appSettingsPath"
        Write-Host ""
        Write-Host "Edit AccountDb and GameDb connection strings, then run this script again."
        Write-Host ""
        Read-Host "Press ENTER to close"
        exit 0
    }

    Write-Host "Starting AceTR Account API on http://127.0.0.1:5080"
    Write-Host "Keep this PowerShell window open while testing the launcher."
    Write-Host ""

    dotnet run --project (Join-Path $PSScriptRoot "AceTRAccountApi.csproj") --urls "http://127.0.0.1:5080"

    if ($LASTEXITCODE -ne 0) {
        throw "dotnet run failed with exit code $LASTEXITCODE."
    }
}
catch {
    Write-Host ""
    Write-Host "AceTR Account API startup failed:" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
    Write-Host ""
    Read-Host "Press ENTER to close"
    exit 1
}
