USE [atum2_db_1]
GO

/****** Object:  Table [dbo].[td_Bonus]    Script Date: 21/02/2016 00:17:27 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[td_Bonus](
	[TargetItemUniqueNumber] [bigint] NULL,
	[TargetItemNum] [int] NULL,
	[BonusDesParam] [int] NULL,
	[BonusValue] [float] NULL,
	[SequenceNumber] [bigint] IDENTITY(1,1) NOT NULL
) ON [PRIMARY]

GO


