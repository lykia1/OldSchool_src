USE [atum2_db_1]
GO

/****** Object:  StoredProcedure [dbo].[atum_GetBonusBYItemUID]    Script Date: 21/02/2016 00:16:40 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

-- =============================================
-- Author:		<Author,,Name>
-- Create date: <Create Date,,>
-- Description:	<Description,,>
-- =============================================
CREATE PROCEDURE [dbo].[atum_GetBonusBYItemUID]
	@i_ItemUID		BIGINT
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TargetItemNum, BonusDesParam, BonusValue, SequenceNumber
	FROM td_Bonus WITH(NOLOCK)
	WHERE @i_ItemUID = TargetItemUniqueNumber
	
END

GO


