USE [atum2_db_1]
GO

/****** Object:  StoredProcedure [dbo].[atum_ItemBonusByUID]    Script Date: 21/02/2016 00:15:35 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO


CREATE PROCEDURE [dbo].[atum_ItemBonusByUID]
	@TargetUID			 BIGINT,
	@TargetItemNum			INT,
	@DesParam				INT,
	@Value					FLOAT
AS
		BEGIN
	INSERT INTO [dbo].[td_Bonus]
           ([TargetItemUniqueNumber]
           ,[TargetItemNum]
           ,[BonusDesParam]
           ,[BonusValue])
     VALUES
           (@TargetUID
           ,@TargetItemNum
           ,@DesParam
           ,@Value)
		END

	IF (@@ERROR <> 0)
	BEGIN
		SELECT 0;
		RETURN;
	END
GO
