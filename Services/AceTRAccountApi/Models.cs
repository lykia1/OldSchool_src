namespace AceTRAccountApi;

public sealed record AccountProfile(
    string AccountName,
    string? Email,
    bool Active,
    int CashPoint,
    DateTime? RegisteredDate,
    DateTime? LastLoginDate,
    bool IsBlocked);

public sealed record CharacterSummary(
    int CharacterUniqueNumber,
    string CharacterName,
    int Level,
    int UnitKind,
    string Gear,
    int Race);

public sealed record ChangePasswordRequest(
    string AccountName,
    string CurrentPassword,
    string NewPassword);

public sealed record UpdateEmailRequest(
    string AccountName,
    string Email);
