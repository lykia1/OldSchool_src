using AceTRAccountApi;
using Microsoft.Data.SqlClient;
using System.Data;

var builder = WebApplication.CreateBuilder(args);

var accountDb = builder.Configuration.GetConnectionString("AccountDb")
    ?? throw new InvalidOperationException("ConnectionStrings:AccountDb is missing.");
var gameDb = builder.Configuration.GetConnectionString("GameDb")
    ?? throw new InvalidOperationException("ConnectionStrings:GameDb is missing.");

builder.Services.AddSingleton(new DbOptions(accountDb, gameDb));

var app = builder.Build();

app.UseHttpsRedirection();

app.MapGet("/health", () => Results.Ok(new { ok = true, service = "AceTRAccountApi" }));

app.MapGet("/api/account/profile", async (HttpRequest request, DbOptions db) =>
{
    var session = await LauncherSession.TryValidateAsync(request, db.AccountDb);
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
    var session = await LauncherSession.TryValidateAsync(request, db.AccountDb);
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

app.MapPost("/api/account/email", async (HttpRequest request, UpdateEmailRequest body, DbOptions db) =>
{
    var session = await LauncherSession.TryValidateAsync(request, db.AccountDb);
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

public sealed record DbOptions(string AccountDb, string GameDb);

public sealed record LauncherSessionInfo(string AccountName, DateTime ExpiresAtUtc);

public static class LauncherSession
{
    public static async Task<LauncherSessionInfo?> TryValidateAsync(HttpRequest request, string accountDb)
    {
        var token = request.Headers["X-AceTR-Session"].ToString();
        if (string.IsNullOrWhiteSpace(token) || token.Length > 128)
            return null;

        await using var con = new SqlConnection(accountDb);
        await con.OpenAsync();

        const string sql = """
            SELECT TOP 1 AccountName, ExpiresAtUtc
            FROM dbo.td_LauncherSession WITH (NOLOCK)
            WHERE SessionToken = @token
              AND RevokedAtUtc IS NULL
              AND ExpiresAtUtc > SYSUTCDATETIME()
            """;

        await using var cmd = new SqlCommand(sql, con);
        cmd.Parameters.Add("@token", SqlDbType.VarChar, 128).Value = token;

        await using var reader = await cmd.ExecuteReaderAsync();
        if (!await reader.ReadAsync())
            return null;

        return new LauncherSessionInfo(reader.GetString(0), reader.GetDateTime(1));
    }
}
