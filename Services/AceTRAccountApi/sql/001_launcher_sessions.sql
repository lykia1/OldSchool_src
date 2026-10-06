IF OBJECT_ID('dbo.td_LauncherSession', 'U') IS NULL
BEGIN
    CREATE TABLE dbo.td_LauncherSession
    (
        SessionToken     VARCHAR(128) NOT NULL,
        AccountName     VARCHAR(20)  NOT NULL,
        CreatedAtUtc    DATETIME2(0) NOT NULL CONSTRAINT DF_td_LauncherSession_CreatedAtUtc DEFAULT SYSUTCDATETIME(),
        ExpiresAtUtc    DATETIME2(0) NOT NULL,
        RevokedAtUtc    DATETIME2(0) NULL,
        ClientIP        VARCHAR(45) NULL,
        CONSTRAINT PK_td_LauncherSession PRIMARY KEY (SessionToken)
    );

    CREATE INDEX IX_td_LauncherSession_AccountName
        ON dbo.td_LauncherSession(AccountName, ExpiresAtUtc);
END
GO

CREATE OR ALTER PROCEDURE dbo.acetr_LauncherSession_Create
    @AccountName VARCHAR(20),
    @SessionToken VARCHAR(128),
    @ClientIP VARCHAR(45) = NULL,
    @LifetimeMinutes INT = 30
AS
BEGIN
    SET NOCOUNT ON;

    DELETE FROM dbo.td_LauncherSession
    WHERE ExpiresAtUtc <= SYSUTCDATETIME()
       OR (AccountName = @AccountName AND RevokedAtUtc IS NULL);

    INSERT INTO dbo.td_LauncherSession
        (SessionToken, AccountName, ExpiresAtUtc, ClientIP)
    VALUES
        (@SessionToken, @AccountName,
         DATEADD(MINUTE, @LifetimeMinutes, SYSUTCDATETIME()),
         @ClientIP);
END
GO

CREATE OR ALTER PROCEDURE dbo.acetr_LauncherSession_Revoke
    @SessionToken VARCHAR(128)
AS
BEGIN
    SET NOCOUNT ON;

    UPDATE dbo.td_LauncherSession
    SET RevokedAtUtc = SYSUTCDATETIME()
    WHERE SessionToken = @SessionToken
      AND RevokedAtUtc IS NULL;
END
GO
