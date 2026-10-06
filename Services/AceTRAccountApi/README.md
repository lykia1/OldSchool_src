# AceTR Account API

This service is the secure boundary between the Windows launcher and SQL Server.

The launcher must never contain SQL credentials. It authenticates against the existing PreServer first. After a successful game login, PreServer issues a short-lived HMAC-SHA256 signed launcher session token. The launcher sends that token in the `X-AceTR-Session` header. The signing secret exists only on PreServer and the API host.

## Endpoints

- `GET /health`
- `GET /api/account/profile`
- `GET /api/account/characters`
- `GET /api/launcher/characters` (legacy-launcher text transport)
- `POST /api/account/email`

Password change is intentionally not enabled yet. The existing game authentication/password storage must be confirmed before implementing writes to the password field.

## Setup

1. Copy `appsettings.example.json` to `appsettings.json`.
2. Put real SQL credentials only on the API host; do not commit them.
3. Set the same long random secret on both PreServer and the API using the `ACETR_LAUNCHER_API_SECRET` environment variable (or `launcher_api_secret.txt` beside PreServer only).
4. Install .NET 8 SDK.
5. Run:

   `dotnet restore`

   `dotnet run`

6. Put the API behind HTTPS (IIS, nginx, Caddy, or another TLS reverse proxy) before exposing it publicly.

## Security model

- PreServer remains the authority for username/password authentication.
- The launcher receives only a short-lived session token for account-management requests.
- The API validates the PreServer signature and expiry without trusting launcher-supplied account names.
- SQL credentials stay on the server.
- Account-management endpoints always resolve the account from the session token instead of trusting an arbitrary account name supplied by the client.