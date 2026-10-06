# AceTR Account API

This service is the secure boundary between the Windows launcher and SQL Server.

The launcher must never contain SQL credentials. It authenticates against the existing PreServer first. After a successful game login, PreServer will issue a short-lived opaque launcher session token and persist it through `acetr_LauncherSession_Create`. The launcher then sends that token in the `X-AceTR-Session` header.

## Endpoints

- `GET /health`
- `GET /api/account/profile`
- `GET /api/account/characters`
- `POST /api/account/email`

Password change is intentionally not enabled yet. The existing game authentication/password storage must be confirmed before implementing writes to the password field.

## Setup

1. Run `sql/001_launcher_sessions.sql` against `atum2_db_account`.
2. Copy `appsettings.example.json` to `appsettings.json`.
3. Put real SQL credentials only on the API host; do not commit them.
4. Install .NET 8 SDK.
5. Run:

   `dotnet restore`

   `dotnet run`

6. Put the API behind HTTPS (IIS, nginx, Caddy, or another TLS reverse proxy) before exposing it publicly.

## Security model

- PreServer remains the authority for username/password authentication.
- The launcher receives only a short-lived session token for account-management requests.
- The API validates the session token from SQL Server.
- SQL credentials stay on the server.
- Account-management endpoints always resolve the account from the session token instead of trusting an arbitrary account name supplied by the client.
