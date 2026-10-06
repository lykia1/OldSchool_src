using AceTRAccountApi;
using Microsoft.Data.SqlClient;
using System.Data;

var builder = WebApplication.CreateBuilder(args);

var accountDb = builder.Configuration.GetConnectionString("AccountDb")
    ?? throw new InvalidOperationException("ConnectionStrings:AccountDb is missing.");
var gameDb = builder.Configuration.GetConnectionString("GameDb")
    ?? throw new InvalidOperationException("ConnectionStrings:GameDb is missing.");
var signingSecret = Environment.GetEnvironmentVariable("ACETR_LAUNCHER_API_SECRET");
if (string.IsNullOrWhiteSpace(signingSecret))
{
    signingSecret = builder.Configuration["LauncherApi:SigningSecret"];
}

if (string.IsNullOrWhiteSpace(signingSecret) ||
    signingSecret == "SET_A_LONG_RANDOM_SECRET_OR_USE_ACETR_LAUNCHER_API_SECRET")
{
    throw new InvalidOperationException(
        "LauncherApi signing secret is missing. Set ACETR_LAUNCHER_API_SECRET or configure LauncherApi:SigningSecret.");
}

builder.Services.AddSingleton(new DbOptions(accountDb, gameDb, signingSecret));

var app = builder.Build();

app.UseHttpsRedirection();

app.MapGet("/health", () => Results.Ok(new { ok = true, service = "AceTRAccountApi" }));

app.MapGet("/api/account/profile", async (HttpRequest request, DbOptions db) =>
{
    var session = LauncherSession.TryValidate(request, db.SigningSecret);
    if (session is null)
        return Results.Unauthorized();

    await using var con = new SqlConnection(db.AccountDb);
    await con.OpenAsync();

    const string sql = """
        SELECT TOP 1
            AccountName,
            Email,
            Active,
            CashPoint,
            RegisteredDate,
            LastLoginDate,
            IsBlocked
        FROM dbo.td_Account WITH (NOLOCK)
        WHERE AccountName = @accountName
        """;

    await using var cmd = new SqlCommand(sql, con);
    cmd.Parameters.Add("@accountName", SqlDbType.VarChar, 20).Value = session.AccountName;

    await using var reader = await cmd.ExecuteReaderAsync();
    if (!await reader.ReadAsync())
        return Results.NotFound();

    var profile = new AccountProfile(
        reader.GetString(0),
        reader.IsDBNull(1) ? null : reader.GetString(1),
        !reader.IsDBNull(2) && Convert.ToBoolean(reader.GetValue(2)),
        reader.IsDBNull(3) ? 0 : Convert.ToInt32(reader.GetValue(3)),
        reader.IsDBNull(4) ? null : reader.GetDateTime(4),
        reader.IsDBNull(5) ? null : reader.GetDateTime(5),
        !reader.IsDBNull(6) && Convert.ToBoolean(reader.GetValue(6)));

    return Results.Ok(profile);
});

app.MapGet("/api/account/characters", async (HttpRequest request, DbOptions db) =>
{
    var session = LauncherSession.TryValidate(request, db.SigningSecret);
    if (session is null)
        return Results.Unauthorized();

    await using var con = new SqlConnection(db.GameDb);
    await con.OpenAsync();

    const string sql = """
        SELECT
            UniqueNumber,
            CharacterName,
            Level,
            UnitKind,
            Race
        FROM dbo.td_Character WITH (NOLOCK)
        WHERE AccountName = @accountName
          AND Race < 128
        ORDER BY Level DESC, CharacterName ASC
        """;

    await using var cmd = new SqlCommand(sql, con);
    cmd.Parameters.Add("@accountName", SqlDbType.VarChar, 20).Value = session.AccountName;

    var result = new List<CharacterSummary>();
    await using var reader = await cmd.ExecuteReaderAsync();

    while (await reader.ReadAsync())
    {
        var unitKind = Convert.ToInt32(reader.GetValue(3));
        result.Add(new CharacterSummary(
            Convert.ToInt32(reader.GetValue(0)),
            reader.GetString(1),
            Convert.ToInt32(reader.GetValue(2)),
            unitKind,
            GearName(unitKind),
            Convert.ToInt32(reader.GetValue(4))));
    }

    return Results.Ok(result);
});

app.MapGet("/api/launcher/characters", async (HttpRequest request, DbOptions db) =>
{
    var session = LauncherSession.TryValidate(request, db.SigningSecret);
    if (session is null)
        return Results.Unauthorized();

    await using var con = new SqlConnection(db.GameDb);
    await con.OpenAsync();

    const string sql = """
        SELECT CharacterName, Level, UnitKind, Race
        FROM dbo.td_Character WITH (NOLOCK)
        WHERE AccountName = @accountName
          AND Race < 128
        ORDER BY Level DESC, CharacterName ASC
        """;

    await using var cmd = new SqlCommand(sql, con);
    cmd.Parameters.Add("@accountName", SqlDbType.VarChar, 20).Value = session.AccountName;

    var lines = new List<string>();
    await using var reader = await cmd.ExecuteReaderAsync();
    while (await reader.ReadAsync())
    {
        var name = reader.GetString(0).Replace("|", "").Replace("\r", "").Replace("\n", "");
        var level = Convert.ToInt32(reader.GetValue(1));
        var unitKind = Convert.ToInt32(reader.GetValue(2));
        var race = Convert.ToInt32(reader.GetValue(3));
        lines.Add($"{name}|Lv.{level}|{GearName(unitKind)}|Race {race}");
    }

    return Results.Text(string.Join("\n", lines), "text/plain; charset=utf-8");
});

app.MapPost("/api/account/email", async (HttpRequest request, UpdateEmailRequest body, DbOptions db) =>
{
    var session = LauncherSession.TryValidate(request, db.SigningSecret);
    if (session is null || !session.AccountName.Equals(body.AccountName, StringComparison.OrdinalIgnoreCase))
        return Results.Unauthorized();

    if (string.IsNullOrWhiteSpace(body.Email) || body.Email.Length > 254 || !body.Email.Contains('@'))
        return Results.BadRequest(new { error = "invalid_email" });

    await using var con = new SqlConnection(db.AccountDb);
    await con.OpenAsync();

    const string sql = """
        UPDATE dbo.td_Account
        SET Email = @email
        WHERE AccountName = @accountName
        """;

    await using var cmd = new SqlCommand(sql, con);
    cmd.Parameters.Add("@email", SqlDbType.VarChar, 254).Value = body.Email.Trim();
    cmd.Parameters.Add("@accountName", SqlDbType.VarChar, 20).Value = session.AccountName;

    return await cmd.ExecuteNonQueryAsync() == 1
        ? Results.Ok(new { ok = true })
        : Results.NotFound();
});

app.Run();

static string GearName(int unitKind)
{
    // ACE unit kinds are bit/variant based in the legacy source. These broad
    // values keep the API useful without duplicating game-client constants.
    return unitKind switch
    {
        1 => "B-Gear",
        16 => "I-Gear",
        256 => "A-Gear",
        4096 => "M-Gear",
        _ => $"Unit {unitKind}"
    };
}

public sealed record DbOptions(string AccountDb, string GameDb, string SigningSecret);

public sealed record LauncherSessionInfo(string AccountName, long ExpiresAtUnix);

public static class LauncherSession
{
    public static LauncherSessionInfo? TryValidate(HttpRequest request, string signingSecret)
    {
        var token = request.Headers["X-AceTR-Session"].ToString();
        if (string.IsNullOrWhiteSpace(token) || token.Length > 192)
            return null;

        var parts = token.Split('|');
        if (parts.Length != 4)
            return null;

        var accountName = parts[0];
        if (string.IsNullOrWhiteSpace(accountName) || accountName.Length > 20)
            return null;

        if (!long.TryParse(parts[1], out var expiresAt))
            return null;

        var now = DateTimeOffset.UtcNow.ToUnixTimeSeconds();
        if (expiresAt <= now || expiresAt > now + 3600)
            return null;

        var nonce = parts[2];
        var suppliedSignature = parts[3];
        if (nonce.Length != 32 || suppliedSignature.Length != 64)
            return null;

        var payload = $"{accountName}|{expiresAt}|{nonce}";
        using var hmac = new System.Security.Cryptography.HMACSHA256(
            System.Text.Encoding.UTF8.GetBytes(signingSecret));
        var expectedBytes = hmac.ComputeHash(System.Text.Encoding.UTF8.GetBytes(payload));
        var expected = Convert.ToHexString(expectedBytes).ToLowerInvariant();

        var expectedRaw = System.Text.Encoding.ASCII.GetBytes(expected);
        var suppliedRaw = System.Text.Encoding.ASCII.GetBytes(suppliedSignature.ToLowerInvariant());
        if (expectedRaw.Length != suppliedRaw.Length ||
            !System.Security.Cryptography.CryptographicOperations.FixedTimeEquals(expectedRaw, suppliedRaw))
            return null;

        return new LauncherSessionInfo(accountName, expiresAt);
    }
}