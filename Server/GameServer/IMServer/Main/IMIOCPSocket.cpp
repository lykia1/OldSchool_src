// IMIOCPSocket.cpp: implementation of the CIMIOCPSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IMIOCPSocket.h"
#include "IMIOCP.h"
#include "IMGlobal.h"
#include "AtumIMDBManager.h"
#include "AtumProtocolVoIP.h"
#include "AtumSJ.h"
#include "AtumError.h"
#include "IMParty.h"
//#include "SecurityManager.h" // 2011-06-22 by hskim, »çĽł Ľ­ąö ąćÁö


CIMIOCP			*CIMIOCPSocket::ms_pIMIOCP	= NULL;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void findDuplicates(vector<char*> & vecOfElements, map<char*, int> & countMap)
{
	// Iterate over the vector and store the frequency of each element in map
	for (auto & elem : vecOfElements)
	{
		auto result = countMap.insert(std::pair<char*, int>(elem, 1));
		if (result.second == false)
			result.first->second++;
	}

	// Remove the elements from Map which has 1 frequency count
	for (auto it = countMap.begin(); it != countMap.end();)
	{
		if (it->second == 1)
			it = countMap.erase(it);
		else
			it++;

	}
}
CIMIOCPSocket::CIMIOCPSocket()
{
	nINETSendedCount = 0;
	m_nLoginType		= -1;

	m_PeerSocketType	= ST_INVALID_TYPE;
	this->SetIMParty(NULL);

	// ±ćµĺ °ü·Ă
	this->SetGuildUniqueNumber(INVALID_GUILD_UID);

	this->SetCurrentFieldServerSocket(NULL);
	// 2008-05-15 by dhjin, EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć
//	m_enableChatFlag8		= (BitFlag8_t)0xFF;
	m_enableChatFlag16		= (BitFlag16_t)0xFFFF;
	m_bSendMessgeTypeDBG	= FALSE;

	memset(&m_character, 0x00, sizeof(CHARACTER));
	memset(&m_MembershipInfo, 0x00, sizeof(MSG_FI_CASH_PREMIUM_CARD_INFO));
	
	m_mtvectorFriendList.reserve(10);
	m_mtvectorRejectList.reserve(10);
	m_nEachOtherFriendCnts	= 0;

	m_VoipType				= CHECK_TYPE_VOIP_NONE; // 2008-06-17 by dhjin, EP3 VOIP -
	m_bChatRoomUseing		= FALSE;				// 2008-06-17 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć Âüż© ÁßŔÎÁöżˇ ´ëÇŃ °Ş
	m_nChatRoomNum			= 0;					// 2008-06-18 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ąřČŁ
}

CIMIOCPSocket::~CIMIOCPSocket()
{
}

#define IS_VALID_CHARACTER_RETURN_FALSE if(FALSE == this->IsValidCharacter()){ g_pGlobal->WriteSystemLogEX(FALSE,"[ERROR] Not valid character trying to run packet! IP: %s", this->GetPeerIP()); }

BOOL CIMIOCPSocket::OnRecvdPacketIMServer(const char* pPacket, int nLength, BYTE nSeq, char* pPeerIP, int nPeerPort, SThreadInfo *i_pThreadInfo)
{
	int				nBytesUsed		= 0;
	MessageType_t	nRecvType		= 0;
	int				nRecvTypeSize	= 0;
	ProcessResult	procRes			= RES_BREAK;
	MessageType_t	prevMsgType		= 0;

	// UDP Ăł¸® ·çĆľ
	if(m_bUDPFlag == TRUE)
	{
		if(strncmp(m_szPeerIP, pPeerIP, SIZE_MAX_IPADDRESS) != 0 || m_nPeerPort != nPeerPort)
		{
			// Protocl Error Ăł¸®
			// - UDP·Î ąŢŔş ĆĐĹ¶ŔÇ IPżÍ Port°ˇ Ľ­ąöżˇ ĽłÁ¤µČ ClientŔÇ IP, PortżÍ °°Áö ľĘ´Ů
			// Error Code : ERR_PROTOCOL_INVALID_PEER
			SendErrorMessage(T_IM_IOCP, ERR_PROTOCOL_INVALID_PEER);
			Close(0x12000);
		}

		return TRUE;
	}

	while(this->IsUsing() && nBytesUsed < nLength)
	{
		nRecvType			= 0;

		nRecvType = *(MessageType_t*)(pPacket+nBytesUsed);
		nBytesUsed += SIZE_FIELD_TYPE_HEADER;

		if(i_pThreadInfo)
		{
			i_pThreadInfo->dwSocketIndex = m_nClientArrayIndex;
			i_pThreadInfo->dwMessageType = nRecvType;
			i_pThreadInfo->nParam1			= 0;			// 2007-02-21 by cmkwon
			i_pThreadInfo->nParam2			= 0;			// 2007-02-21 by cmkwon
		}

#ifdef _DEBUG
		PrintExchangeMsg(RECV_TYPE, nRecvType, m_szPeerIP, ST_IM_SERVER, GGetexchangeMsgPrintLevel());
#endif

#ifdef _DEBUG
		// ¸ŢĽĽÁö Âď±â
		if (m_PeerSocketType == ST_CLIENT_TYPE
			&& m_bSendMessgeTypeDBG && T0_IC_STRING != HIBYTE(nRecvType)
			&& !IS_ALIVE_MSG(nRecvType) && T0_IC_ADMIN != HIBYTE(nRecvType)
		)
		{
		//	SendString128(STRING_128_USER_NOTICE, "[C->I]%s", GetProtocolTypeString(nRecvType));
		}

		//dbg console (packets)
		if (nRecvType != T_FC_MOVE && nRecvType != T_FC_MOVE_WEAPON_VEL && nRecvType != T_FC_TIMER_TIMEOUT)
		{
			char p[1024];
			sprintf(p, "%s | %s(%s) | [%s]\n", GetProtocolTypeString(nRecvType), this->GetCharacter()->CharacterName, this->GetCharacter()->AccountName, this->GetPeerIP());
			AllocConsole();
			HANDLE myConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			DWORD cCharsWritten;
			WriteConsole(myConsoleHandle, p, strlen(p), &cCharsWritten, NULL);
		}

#endif

		switch(nRecvType)
		{
		////////////////////////////////////////////////////////////////////////
		// IMServer <-- Client, IMServer <-- Monitor, IMServer <-- FieldServer
		case T_IC_CONNECT_ALIVE:
		case T_FI_CONNECT_ALIVE:
		case T_PM_CONNECT_ALIVE:
		case T_IM_CONNECT_ALIVE:
		case T_FM_CONNECT_ALIVE:
		case T_NM_CONNECT_ALIVE:
			OnRecvdAlivePacket();
			procRes = RES_RETURN_TRUE;
			break;
		//////////////////////////////////////////////////////////////////
		// IMServer <-- AdminTool
		case T_IA_ADMIN_CONNECT:
			{
				if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
				{
					return FALSE;
				}
				MSG_IA_ADMIN_CONNECT* msgAdminConnect = (MSG_IA_ADMIN_CONNECT*)(pPacket + nBytesUsed);
				nBytesUsed += sizeof(MSG_IA_ADMIN_CONNECT);

				// ŔÎÁőÇĎ±â
				INIT_MSG_WITH_BUFFER(MSG_IA_ADMIN_CONNECT_OK, T_IA_ADMIN_CONNECT_OK, msgAdminConnectOK, SendBuf);
// 2006-04-15 by cmkwon
//				msgAdminConnectOK->AuthOK = bRet;
				msgAdminConnectOK->AccountType0 = g_pGlobalGameServer->AuthAdminToolUser(msgAdminConnect->UID, msgAdminConnect->PWD);
				SendAddData(SendBuf, MSG_SIZE(MSG_IA_ADMIN_CONNECT_OK));

				if (0 == msgAdminConnectOK->AccountType0)
				{
					procRes = RES_RETURN_FALSE;
					Close();
				}
				else
				{
					// ŔÎÁő Ľş°ř
					m_PeerSocketType = ST_ADMIN_TOOL;
					STRNCPY_MEMSET(m_character.AccountName, msgAdminConnect->UID, SIZE_MAX_ACCOUNT_NAME);	// 2007-10-02 by cmkwon, ĽöÁ¤
					m_character.ClientIndex		= this->GetClientArrayIndex();								// 2007-10-02 by cmkwon, ĽöÁ¤

					ms_pIMIOCP->InsertMonitorIOCPSocketPtr(this);
				}
			}
			break;
		case T_IA_ADMIN_GET_CHARACTER:
			{
				if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
				{
					return FALSE;
				}
				MSG_IA_ADMIN_GET_CHARACTER* msgAdminGetCharac = (MSG_IA_ADMIN_GET_CHARACTER*)(pPacket + nBytesUsed);
				nBytesUsed += sizeof(MSG_IA_ADMIN_GET_CHARACTER);

				CIMIOCPSocket *pSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(msgAdminGetCharac->CharacterName);

				INIT_MSG_WITH_BUFFER(MSG_IA_ADMIN_GET_CHARACTER_OK, T_IA_ADMIN_GET_CHARACTER_OK, msgAdminGetCharacOK, SendBuf);
				if (pSocket == NULL)
				{
					// Äł¸ŻĹÍ ľřŔ˝
					msgAdminGetCharacOK->IsOnline = FALSE;
				}
				else
				{
					// Äł¸ŻĹÍ ŔÖŔ˝
					msgAdminGetCharacOK->IsOnline = TRUE;
				}
				SendAddData(SendBuf, MSG_SIZE(MSG_IA_ADMIN_GET_CHARACTER_OK));
			}
			break;
		case T_IA_ADMIN_CHAT_ALL:
			{
				if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
				{
					return FALSE;
				}
				MSG_IA_ADMIN_CHAT_ALL* msgChatAll = (MSG_IA_ADMIN_CHAT_ALL*)(pPacket + nBytesUsed);
				nBytesUsed += sizeof(MSG_IA_ADMIN_CHAT_ALL) + msgChatAll->MessageLength;

				if(ms_pIMIOCP->m_bArenaServerCheck)
				{// 2008-04-28 by dhjin, ľĆ·ąłŞ ĹëÇŐ - ľĆ·ąłŞ Ľ­ąöżˇĽ­´Â ŔüĂĽ °řÁö¸¦ ş¸ł»Áö ľĘ´Â´Ů. ¸ŢŔÎĽ­ąöżˇĽ­ ş¸ł»±â ¶§ą®żˇ 2ąř ÂďÇŃ´Ů.
					break;
				}

				char *chatString = ((char*)(msgChatAll))+sizeof(MSG_IA_ADMIN_CHAT_ALL);
				if (strncmp(chatString, "/", 1) == 0)
				{
					// 2007-10-02 by cmkwon, COMMAND ·Î±× ł˛±â±â

					if( TRUE == g_pIMGlobal->CheckWriteLog(chatString) /*|| ms_pIMIOCP->m_pSecurityManager->CheckCommand(chatString)*/ )		// 2011-06-22 by hskim, »çĽł Ľ­ąö ąćÁö
					g_pIMGlobal->WriteSystemLogEX(TRUE, "[Notify] : %s, IP(%s), command(%s)\r\n", GetCharacterString(&m_character, string()), this->GetPeerIP(), chatString);

					// ¸í·ÉľîŔÓ
					BOOL ret = HandleAdminCommands(chatString);
					break;
				}

				// set message type
				SET_MSG_TYPE(((byte*)msgChatAll) - SIZE_FIELD_TYPE_HEADER, T_IC_CHAT_ALL);

				// send msg to all clients
				ms_pIMIOCP->SendMessageToAllClients(((BYTE*)msgChatAll) - SIZE_FIELD_TYPE_HEADER, MSG_SIZE(MSG_IC_CHAT_ALL) + msgChatAll->MessageLength);
			}
			break;
		case T_IA_ADMIN_GET_SERVER_INFO:
			{
				if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
				{
					return FALSE;
				}
				INIT_MSG_WITH_BUFFER(MSG_IA_ADMIN_GET_SERVER_INFO_OK, T_IA_ADMIN_GET_SERVER_INFO_OK, GetServerInfoOk, GetServerInfoOkBuf);
				GetServerInfoOk->UserCount = ms_pIMIOCP->m_MapChactarterUniqueNumber.size();
				GetServerInfoOk->ServerState = SERVER_STATE_NORMAL;
				SendAddData(GetServerInfoOkBuf, MSG_SIZE(MSG_IA_ADMIN_GET_SERVER_INFO_OK));
			}
			break;
		//////////////////////////////////////////////////////////////////
		// IMServer <-- Monitor
		case T_IM_CONNECT:
			{
				// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ ÇÔ
				//if(FALSE == IS_SCADMINTOOL_CONNECTABLE_IP(GetPeerIP()))
				if(FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
				{
					return FALSE;
				}
				char szSystemLog[256];
				sprintf(szSystemLog, "Monitor Client Connected, IP[%s]\r\n", GetPeerIP());
				DBGOUT(szSystemLog);
				g_pGlobal->WriteSystemLog(szSystemLog);


				m_PeerSocketType = ST_MONITOR_SERVER;
				ms_pIMIOCP->InsertMonitorIOCPSocketPtr(this);

				INIT_MSG_WITH_BUFFER(MSG_IM_CONNECT_OK, T_IM_CONNECT_OK, pSendConnectOK, SendBuf);
				STRNCPY_MEMSET(pSendConnectOK->IPAddress, g_pIMGlobal->GetPublicIPLocal(), SIZE_MAX_IPADDRESS);
				pSendConnectOK->NumOfClients = ms_pIMIOCP->m_MapAccountUniqueNumber.size();
				pSendConnectOK->Port = ms_pIMIOCP->GetListenerPort();
				pSendConnectOK->CalcBandwidth = ms_pIMIOCP->GetFlagCalcTrafficInfo();
				pSendConnectOK->StartedTime = ms_pIMIOCP->m_dwTimeStarted;
				STRNCPY_MEMSET(pSendConnectOK->ServerName, "IM Server", SIZE_MAX_SERVER_NAME);
				SendAddData(SendBuf, MSG_SIZE(MSG_IM_CONNECT_OK));

				INIT_MSG(MSG_IM_GET_TOTAL_USER_COUNTS_ACK, T_IM_GET_TOTAL_USER_COUNTS_ACK, pSeAck, SendBuf);
				pSeAck->AccumulatedTotalUserCounts	= ms_pIMIOCP->m_uiIMAccumulatedTotalUserCounts;
				pSeAck->CurrentTotalUserCounts		= ms_pIMIOCP->m_MapAccountUniqueNumber.size();
				pSeAck->MaxTotalUserCounts			= ms_pIMIOCP->m_uiIMMaxTotalUserCounts;				
				SendAddData(SendBuf, MSG_SIZE(MSG_IM_GET_TOTAL_USER_COUNTS_ACK));
			}
			break;
		case T_IM_GET_NUM_CLIENTS:
			{
				if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
				{
					return FALSE;
				}
				INIT_MSG_WITH_BUFFER(MSG_IM_GET_NUM_CLIENTS_OK, T_IM_GET_NUM_CLIENTS_OK, pSendGetNumClientsOK, SendBuf);
				pSendGetNumClientsOK->NumOfClients = ms_pIMIOCP->m_MapChactarterUniqueNumber.size();
				SendAddData(SendBuf, MSG_SIZE(MSG_IM_GET_NUM_CLIENTS_OK));
			}
			break;
		case T_IM_SHUTDOWN:
			{
				if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
				{
					return FALSE;
				}
				SendAddMessageType(T_IM_SHUTDOWN_OK);
				// 2011-11-18 by shcho, Ľ­ąö´Ůżî ÇÁ¸®ĆäľîĽ­ąö´Ůżî Á¦°Ĺ Ăł¸® - IMĽ­ąö·Î ÁÖ´Â ¸í·É
				// PostMessage(g_pGlobalGameServer->GetMainWndHandle(), WM_CLOSE, 0, 0);
			}
			break;
		case T_IM_PING:
			{
				if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
				{
					return FALSE;
				}
				SendAddMessageType(T_IM_PING_OK);
			}
			break;
		case T_IM_PRINTMAP:
			{
				if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
				{
					return FALSE;
				}
			}
			break;
		case T_IM_PRINT_DEBUG_MSG:
		{
			if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
			{
				return FALSE;
			}
			procRes = Process_IM_PRINT_DEBUG_MSG(pPacket, nLength, nBytesUsed);
			break;
		}
		case T_IM_CHANGE_BANDWIDTH_FLAG:
			{
				if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
				{
					return FALSE;
				}
				ms_pIMIOCP->SetFlagCalcTrafficInfo(!ms_pIMIOCP->GetFlagCalcTrafficInfo());

				INIT_MSG_WITH_BUFFER(MSG_IM_CHANGE_BANDWIDTH_FLAG_OK, T_IM_CHANGE_BANDWIDTH_FLAG_OK, pChangeFlag, SendBuf);
				pChangeFlag->bChagedFlag = ms_pIMIOCP->GetFlagCalcTrafficInfo();
				SendAddData(SendBuf, MSG_SIZE(MSG_IM_CHANGE_BANDWIDTH_FLAG_OK));
			}
			break;
		case T_IM_SET_MSG_PRINT_LEVEL:
			{
				if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
				{
					return FALSE;
				}
				BYTE tmpLevel = *(BYTE*)(pPacket + nBytesUsed);
				nBytesUsed += sizeof(MSG_IM_SET_MSG_PRINT_LEVEL);

				// set msg print level
				GSetexchangeMsgPrintLevel(tmpLevel);
			}
			break;
			//19-03-2017 by inet
		case T_FI_MULTICHAT_STEERING:
		{
			if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
			{
				return FALSE;
			}
			procRes = Process_FI_MULTICHAT_STEERING(pPacket, nLength, nBytesUsed);
		}
		break;
			//end
		case T_IM_GET_TOTAL_USER_COUNTS:
			{
				if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor Á˘±Ů °ˇ´É IP¸¦ server config file żˇ ĽłÁ¤ÇĎ±â - 
				{
					return FALSE;
				}
				INIT_MSG_WITH_BUFFER(MSG_IM_GET_TOTAL_USER_COUNTS_ACK, T_IM_GET_TOTAL_USER_COUNTS_ACK, pSeAck, SendBuf);
				pSeAck->AccumulatedTotalUserCounts	= ms_pIMIOCP->m_uiIMAccumulatedTotalUserCounts;
				pSeAck->CurrentTotalUserCounts		= ms_pIMIOCP->m_MapAccountUniqueNumber.size();
				pSeAck->MaxTotalUserCounts			= ms_pIMIOCP->m_uiIMMaxTotalUserCounts;
				
				ms_pIMIOCP->SendMessageToMonitor(SendBuf, MSG_SIZE(MSG_IM_GET_TOTAL_USER_COUNTS_ACK));
			}
			break;

		//////////////////////////////////////////////////////////////////
		// IMServer <-- Client, IMServer <-- FieldServer
		case T_IC_CONNECT_LOGIN:
			//·Î±×ŔÎ
			// Client·ÎşÎĹÍ AccountName, ServerName, CharacterName, Password(MD5),LoginType(°ÔŔÓÁß,¸Ţ˝ĹŔúÁ˘ĽÓ)Ŕ» ąŢ´Â´Ů.
			// Á˘ĽÓÁßŔÎ ID¶ó¸é ŔĚŔü Á˘ĽÓŔÚ¸¦ ˛÷´Â´Ů.
			// DB·ÎşÎĹÍ ŔÎÁőŔ» żäĂ»ÇŃ´Ů.
			procRes = Process_IC_CONNECT_LOGIN(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHAT_MAP:// ÁöżŞ Ă¤ĆĂ
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_MAP(pPacket, nLength, nBytesUsed, i_pThreadInfo);
			break;
		case T_IC_CHAT_REGION:// ÁöżŞ Ă¤ĆĂ
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_REGION(pPacket, nLength, nBytesUsed, i_pThreadInfo);
			break;
		case T_IC_CHAT_PARTY:// ĆÄĆĽ Ă¤ĆĂ
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_PARTY(pPacket, nLength, nBytesUsed, i_pThreadInfo);
			break;
		case T_IC_CHAT_GUILD://±ćµĺ Ă¤ĆĂ
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_GUILD(pPacket, nLength, nBytesUsed, i_pThreadInfo);
			break;
		case T_IC_CHAT_ALL:// ŔüĂĽ Ă¤ĆĂ
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_ALL(pPacket, nLength, nBytesUsed, i_pThreadInfo);
			break;
		case T_IC_CHAT_PTOP:
			// 1:1 Ă¤ĆĂ
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_PTOP(pPacket, nLength, nBytesUsed, i_pThreadInfo);
			break;
		case T_IC_CHAT_CHANGE_CHAT_FLAG:	// Ă¤ĆĂ ĽłÁ¤
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_CHANGE_CHAT_FLAG(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHAT_SELL_ALL:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_SELL_ALL(pPacket, nLength, nBytesUsed, i_pThreadInfo);
			break;
		case T_IC_CHAT_MULTI:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_MULTI(pPacket, nLength, nBytesUsed, i_pThreadInfo); // 2015-02-11 by silver chat multi
			break;
		case T_IC_CHAT_CASH_ALL:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_CASH_ALL(pPacket, nLength, nBytesUsed, i_pThreadInfo);
			break;
		case T_IC_CHAT_INFLUENCE_ALL:		// 2006-04-21 by cmkwon
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_INFLUENCE_ALL(pPacket, nLength, nBytesUsed, i_pThreadInfo);
			break;
		case T_IC_CHAT_ARENA:				// 2007-05-02 by dhjin
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_ARENA(pPacket, nLength, nBytesUsed, i_pThreadInfo);
			break;
		case T_IC_CHAT_WAR:				// 2008-05-19 by dhjin, EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć, ŔüŔď Ă¤ĆĂ
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_WAR(pPacket, nLength, nBytesUsed, i_pThreadInfo);
			break;
		case T_IC_CHAT_CHATROOM:				// 2008-06-18 by dhjin, EP3 Ă¤ĆĂąć - 
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_CHATROOM(pPacket, nLength, nBytesUsed, i_pThreadInfo);
			break;
		case T_IC_CHAT_INFINITY:			// 2009-09-09 ~ 2010 by dhjin, ŔÎÇÇ´ĎĆĽ - ŔÎÇÇ Ă¤ĆĂ
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_INFINITY(pPacket, nLength, nBytesUsed, i_pThreadInfo);
			break;	
		case T_IC_CHAT_FRIENDLIST_AND_REJECTLIST_LOADING:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_FRIENDLIST_AND_REJECTLIST_LOADING(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHAT_FRIENDLIST_INSERT:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_FRIENDLIST_INSERT(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHAT_FRIENDLIST_DELETE:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_FRIENDLIST_DELETE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHAT_FRIENDLIST_REFRESH:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_FRIENDLIST_REFRESH(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHAT_REJECTLIST_INSERT:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_REJECTLIST_INSERT(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHAT_REJECTLIST_DELETE:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHAT_REJECTLIST_DELETE(pPacket, nLength, nBytesUsed);
			break;

		case T_IC_CHAT_GET_GUILD:// ±ćµĺżř Á¤ş¸ żäĂ»
		// ±ćµĺŔĚ¸§Ŕ¸·Î DBżˇĽ­ ±ćµĺżř ŔüĂĽ¸¦ ąŢľĆżÍĽ­
		// Á˘ĽÓŔÚ,şńÁ˘ĽÓŔÚ,¸Ţ˝ĹŔúÁ˘ĽÓŔÚµéżˇ°Ô ŔüĽŰÇŃ´Ů.
			{
			}
			break;
		case T_IC_CHAT_CHANGE_GUILD_OK://±ćµĺżř Á¤ş¸ ąŮ˛ń
		// ±ćµĺżř Á¤ş¸ ąŮ˛ď ł»żëŔ» ąŢ´Â´Ů.
		// ±ćµĺżř ŔüĂĽżˇ°Ô ŔüĽŰÇŃ´Ů.
			{
			}
			break;
		case T_IC_CHAT_POSITION:
		// ÁÂÇĄ Á¤ş¸¸¦ ąŢ´Â´Ů.
		// (ÁöżŞ Ă¤ĆĂżˇ »çżëµČ´Ů.)
			{
			}
			break;
		case T_IC_PARTY_CREATE:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_CREATE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_PARTY_GET_MEMBER:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_GET_MEMBER(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_PARTY_GET_ALL_MEMBER:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_GET_ALL_MEMBER(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_PARTY_LEAVE:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_LEAVE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_PARTY_TRANSFER_MASTER:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_TRANSFER_MASTER(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_PARTY_BAN_MEMBER:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_BAN_MEMBER(pPacket, nLength, nBytesUsed);
			break;
// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - »çżëÇĎÁö ľĘ´Â ÇÁ·ÎĹäÄÝŔÓ
//		case T_IC_PARTY_DISMEMBER:
//			procRes = Process_IC_PARTY_DISMEMBER(pPacket, nLength, nBytesUsed);
//			break;
		case T_IC_PARTY_CHANGE_FLIGHT_FORMATION:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_CHANGE_FLIGHT_FORMATION(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_PARTY_GET_FLIGHT_POSITION:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_GET_FLIGHT_POSITION(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_PARTY_CHANGE_FLIGHT_POSITION:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_CHANGE_FLIGHT_POSITION(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_PARTY_CANCEL_FLIGHT_POSITION:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_CANCEL_FLIGHT_POSITION(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_PARTY_ALL_FLIGHT_POSITION:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_ALL_FLIGHT_POSITION(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_PARTY_REQUEST_PARTYINFO_FROM_A_TO_M:		// 2008-02-28 by dhjin, ľĆ·ąłŞ ĹëÇŐ - 
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_REQUEST_PARTYINFO_FROM_A_TO_M(pPacket, nLength, nBytesUsed);
			break;	
		case T_IC_PARTY_LEAVE_FROM_M_TO_A:					// 2008-02-28 by dhjin, ľĆ·ąłŞ ĹëÇŐ -
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_LEAVE_FROM_M_TO_A(pPacket, nLength, nBytesUsed);
			break;			
		case T_IC_PARTY_LEAVE_FROM_A_TO_M:					// 2008-02-28 by dhjin, ľĆ·ąłŞ ĹëÇŐ -
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_LEAVE_FROM_A_TO_M(pPacket, nLength, nBytesUsed);
			break;	
		case T_IC_PARTY_LIST_INFO:					// 2008-06-02 by dhjin, EP3 Ćí´ë ĽöÁ¤ - Ćí´ë Á¤ş¸ ¸®˝şĆ® żäĂ»
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_LIST_INFO(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_PARTY_JOIN_FREE:					// 2008-06-02 by dhjin, EP3 Ćí´ë ĽöÁ¤ - Ćí´ë ŔÚŔŻ Âüż©
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_JOIN_FREE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_PARTY_CHANGE_INFO:				// 2008-06-04 by dhjin, EP3 Ćí´ë ĽöÁ¤ - Ćí´ë Á¤ş¸ ĽöÁ¤
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_CHANGE_INFO(pPacket, nLength, nBytesUsed);
			break;	
		case T_IC_PARTY_RECOMMENDATION_MEMBER:				// 2008-06-04 by dhjin, EP3 Ćí´ë ĽöÁ¤ - ĂßĂµ ÄÉ¸ŻĹÍ żäĂ»
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_RECOMMENDATION_MEMBER(pPacket, nLength, nBytesUsed);
			break;	
		case T_IC_PARTY_GET_AUTO_PARTY_INFO:				// 2009-09-09 ~ 2010-01-26 by dhjin, ŔÎÇÇ´ĎĆĽ - ŔÚµż Ćí´ë
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_PARTY_GET_AUTO_PARTY_INFO(pPacket, nLength, nBytesUsed);
			break;	
			
		///////////////////////////////////////////////////////////////////////
		// IC_GUILD
		case T_IC_GUILD_CREATE:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_CREATE(pPacket, nLength, nBytesUsed);
			break;
// check: T_IC_GUILD_GET_GUILD_INFO·Î ĹëÇŐ, ÇĘżäÇĎ¸é »ě¸˛, 20040520, kelovon
//		case T_IC_GUILD_GET_MEMBER:
//			procRes = Process_IC_GUILD_GET_MEMBER(pPacket, nLength, nBytesUsed);
//			break;
		case T_IC_GUILD_GET_GUILD_INFO:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_GET_GUILD_INFO(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_REQUEST_INVITE:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_REQUEST_INVITE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_ACCEPT_INVITE:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_ACCEPT_INVITE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_REJECT_INVITE:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_REJECT_INVITE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_LEAVE:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_LEAVE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_BAN_MEMBER:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_BAN_MEMBER(pPacket, nLength, nBytesUsed);
			break;
// 2007-11-12 by dhjin, Process_IC_GUILD_DISMEMBER  ŔĚ¸§Ŕş łŞµÎ°í T_IC_GUILD_DISMEMBER¸¸ şŻ°ć
//		case T_IC_GUILD_DISMEMBER:
		case T_FI_GUILD_DISMEMBER:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_DISMEMBER(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_CANCEL_DISMEMBER:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_CANCEL_DISMEMBER(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_GET_DISMEMBER_DATE:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_GET_DISMEMBER_DATE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_CHANGE_GUILD_NAME:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_CHANGE_GUILD_NAME(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_GET_GUILD_MARK:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_GET_GUILD_MARK(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_SET_GUILD_MARK:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_SET_GUILD_MARK(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_SET_RANK:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_SET_RANK(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_GET_OTHER_GUILD_INFO:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_GET_OTHER_GUILD_INFO(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_SURRENDER_GUILD_WAR:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_SURRENDER_GUILD_WAR(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_NEW_COMMANDER:		// 2008-05-20 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´ÜŔĺ Ŕ§ŔÓ
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_NEW_COMMANDER(pPacket, nLength, nBytesUsed);
			break;	
		case T_IC_GUILD_NOTICE_WRITE:		// 2008-05-20 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´Ü °řÁö
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_NOTICE_WRITE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_GET_APPLICANT:		// 2008-05-27 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ÁöżřŔÚ °ü¸®
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_GET_APPLICANT(pPacket, nLength, nBytesUsed);
			break;	
		case T_IC_GUILD_GET_INTRODUCTION:		// 2008-05-27 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´Ü ĽŇ°ł
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_GET_INTRODUCTION(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_GET_SELF_INTRODUCTION:		// 2008-05-27 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´Ü ÁöżřŔÚ ĽŇ°łĽ­ 
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_GET_SELF_INTRODUCTION(pPacket, nLength, nBytesUsed);
			break;	
		case T_IC_GUILD_SEARCH_INTRODUCTION:		// 2008-05-27 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´Ü ĽŇ°ł °Ë»ö
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_SEARCH_INTRODUCTION(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_UPDATE_INTRODUCTION:		// 2008-05-27 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´Ü ĽŇ°ł ŔŰĽş 
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_UPDATE_INTRODUCTION(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_DELETE_INTRODUCTION:		// 2008-05-27 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´Ü ĽŇ°ł Áöżě±â
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_DELETE_INTRODUCTION(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_UPDATE_SELFINTRODUCTION:		// 2008-05-27 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ŔÚ±â ĽŇ°ł ŔŰĽş 
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_UPDATE_SELFINTRODUCTION(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_DELETE_SELFINTRODUCTION:		// 2008-05-27 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ŔÚ±â ĽŇ°ł Áöżě±â  
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_DELETE_SELFINTRODUCTION(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_APPLICANT_INVITE:				// 2008-06-12 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ÁöżřŔÚ °ü¸®żˇĽ­ ż©´Üżř ĂĘ´ë
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_APPLICANT_INVITE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_GUILD_APPLICANT_REJECT_INVITE:		// 2008-06-12 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ÁöżřŔÚ °ü¸®żˇĽ­ ż©´Üżř ĂĘ´ë °ĹşÎ 
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_GUILD_APPLICANT_REJECT_INVITE(pPacket, nLength, nBytesUsed);
			break;
		///////////////////////////////////////////////////////////////////////
		// Admin
		case T_IC_ADMIN_CALL_CHARACTER:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_ADMIN_CALL_CHARACTER(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_ADMIN_CALL_GUILD:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_ADMIN_CALL_GUILD(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_ADMIN_GET_SERVER_STAT:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_ADMIN_GET_SERVER_STAT(pPacket, nLength, nBytesUsed);
			break;
		///////////////////////////////////////////////////////////////////////
		// Countdown
		case T_IC_COUNTDOWN_DONE:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_COUNTDOWN_DONE(pPacket, nLength, nBytesUsed);
			break;
			
		case T_IC_VOIP_SET:			// C -> I, 2008-06-17 by dhjin, EP3 VOIP - ĽłÁ¤ 
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_VOIP_SET(pPacket, nLength, nBytesUsed);
			break;

		// T0_IC_CHATROOM
		case T_IC_CHATROOM_CREATE:	// C -> I, 2008-06-16 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć »ýĽş
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHATROOM_CREATE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHATROOM_LIST_INFO:		// C -> I, 2008-06-16 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ¸ń·Ď °ˇÁ®żŔ±â		
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHATROOM_LIST_INFO(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHATROOM_REQUEST_INVITE:		// C -> I, 2008-06-16 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ĂĘ´ë żäĂ» 	
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHATROOM_REQUEST_INVITE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHATROOM_JOIN:				// C -> I, 2008-06-16 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć Âüż©	
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHATROOM_JOIN(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHATROOM_ACCEPT_INVITE:		// C -> I, 2008-06-16 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ĂĘ´ë Ľö¶ô	
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHATROOM_ACCEPT_INVITE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHATROOM_REJECT_INVITE:		// C -> I, 2008-06-16 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ĂĘ´ë °ĹŔý	
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHATROOM_REJECT_INVITE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHATROOM_LEAVE:				// C -> I, 2008-06-16 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć łŞ°ˇ±â	
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHATROOM_LEAVE(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHATROOM_BAN:					// C -> I, 2008-06-16 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć Ăßąć	
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHATROOM_BAN(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHATROOM_CHANGE_NAME:			// C -> I, 2008-06-16 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ŔĚ¸§ şŻ°ć	
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHATROOM_CHANGE_NAME(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHATROOM_CHANGE_MASTER:		// C -> I, 2008-06-16 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ąćŔĺ şŻ°ć	
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHATROOM_CHANGE_MASTER(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHATROOM_CHANGE_LOCK_PW:		// C -> I, 2008-06-16 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć şńąĐąřČŁ şŻ°ć	
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHATROOM_CHANGE_LOCK_PW(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHATROOM_CHANGE_MAX_MEMBER:	// C -> I, 2008-06-16 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ŔÎżřĽö şŻ°ć	
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHATROOM_CHANGE_MAX_MEMBER(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_CHATROOM_OTHER_MEMBER_INFO:			// I -> C, 2008-06-25 by dhjin, EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ´Ů¸Ą ¸Éąö Á¤ş¸ ŔüĽŰ
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_CHATROOM_OTHER_MEMBER_INFO(pPacket, nLength, nBytesUsed);
			break;
		///////////////////////////////////////////////////////////////////////
		// VOIP
		case T_IC_VOIP_GET_ACCOUNTUNIQUENUMBER:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_VOIP_GET_ACCOUNTUNIQUENUMBER(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_VOIP_1to1_DIAL:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_VOIP_1to1_DIAL(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_VOIP_1to1_DIAL_OK:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_VOIP_1to1_DIAL_OK(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_VOIP_1to1_END:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_VOIP_1to1_END(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_VOIP_NtoN_CREATEROOM_REQUEST:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_VOIP_NtoN_CREATEROOM_REQUEST(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_VOIP_NtoN_INVITE_ALL:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_VOIP_NtoN_INVITE_ALL(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_VOIP_NtoN_INVITE_USER:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_VOIP_NtoN_INVITE_USER(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_VOIP_NtoN_GOOUT_ALL:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_VOIP_NtoN_GOOUT_ALL(pPacket, nLength, nBytesUsed);
			break;
		case T_IC_VOIP_ERROR:
			IS_VALID_CHARACTER_RETURN_FALSE
			procRes = Process_IC_VOIP_ERROR(pPacket, nLength, nBytesUsed);
			break;
		///////////////////////////////////////////////////////////////////////
		// Field Server
		case T_FI_CONNECT:
			procRes = Process_FI_CONNECT(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_CONNECT_NOTIFY_FIELDSERVER_IP:
			procRes = Process_FI_CONNECT_NOTIFY_FIELDSERVER_IP(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_CONNECT_NOTIFY_GAMEEND:
			procRes = Process_FI_CONNECT_NOTIFY_GAMEEND(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_CONNECT_NOTIFY_GAMESTART:
			procRes = Process_FI_CONNECT_NOTIFY_GAMESTART(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_CONNECT_NOTIFY_DEAD_GAMESTART:
			procRes = Process_FI_CONNECT_NOTIFY_DEAD_GAMESTART(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_EVENT_NOTIFY_WARP:
			procRes = Process_FI_EVENT_NOTIFY_WARP(pPacket, nLength, nBytesUsed);
			break;
// 2004-10-29 by cmkwon, FieldServer´Â ÇĎłŞ·Î ÇČ˝şÇŃ´Ů
//		case T_FI_EVENT_GET_WARP_INFO:
//			procRes = Process_FI_EVENT_GET_WARP_INFO(pPacket, nLength, nBytesUsed);
//			break;
		case T_FI_EVENT_CHAT_BLOCK:		// 2008-12-30 by cmkwon, ÁöµµŔÚ Ă¤ĆĂ Á¦ÇŃ Ä«µĺ ±¸Çö - 
			procRes = Process_FI_EVENT_CHAT_BLOCK(pPacket, nLength, nBytesUsed);
			break;

		case T_FI_CONNECT_NOTIFY_DEAD:
			procRes = Process_FI_CONNECT_NOTIFY_DEAD(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_PARTY_ACCEPT_INVITE_OK:
			procRes = Process_FI_PARTY_ACCEPT_INVITE_OK(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_PARTY_UPDATE_ITEM_POS:
			procRes = Process_FI_PARTY_UPDATE_ITEM_POS(pPacket, nLength, nBytesUsed);
			break;
//		case T_FI_PARTY_GET_PARTY_INFO:
//			procRes = Process_FI_PARTY_GET_PARTY_INFO(pPacket, nLength, nBytesUsed);
//			break;
		case T_FI_PARTY_AUTO_CREATE:	// 2009-09-09 ~ 2010-01-26 by dhjin, ŔÎÇÇ´ĎĆĽ - ŔÚµż Ćí´ë 
			procRes = Process_FI_PARTY_AUTO_CREATE(pPacket, nLength, nBytesUsed);
			break;	
		case T_FI_PARTY_UPDATE_ITEM_TRANSFORMER_OK:	// 2010-03-18 by cmkwon, ¸ó˝şĹÍşŻ˝Ĺ ±¸Çö - 
			procRes = Process_FI_PARTY_UPDATE_ITEM_TRANSFORMER_OK(pPacket, nLength, nBytesUsed);
			break;	

		case T_FI_CHARACTER_DELETE_CHARACTER:
			procRes = Process_FI_CHARACTER_DELETE_CHARACTER(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_CHARACTER_CHANGE_LEVEL:
			procRes = Process_FI_CHARACTER_CHANGE_LEVEL(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_CHARACTER_UPDATE_MAP_CHANNEL:
			procRes = Process_FI_CHARACTER_UPDATE_MAP_CHANNEL(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_CHARACTER_CHANGE_INFLUENCE_TYPE:
			procRes = Process_FI_CHARACTER_CHANGE_INFLUENCE_TYPE(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_UPDATE_SUBLEADER:			// 2007-02-14 by dhjin
			procRes = Process_FI_UPDATE_SUBLEADER(pPacket, nLength, nBytesUsed);
			break;

		case T_FI_GUILD_NOTIFY_START_WAR:
			procRes = Process_FI_GUILD_NOTIFY_START_WAR(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_GUILD_DELETE_GUILD:
			procRes = Process_FI_GUILD_DELETE_GUILD(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_GUILD_ADD_GUILD_FAME:
			procRes = Process_FI_GUILD_ADD_GUILD_FAME(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_GUILD_OUTPOST:	 // 2008-05-20 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ŔüÁř±âÁö °ü·Ă
			procRes = Process_FI_GUILD_OUTPOST(pPacket, nLength, nBytesUsed);
			break;

		case T_FI_CASH_USING_GUILD:
			procRes = Process_FI_CASH_USING_GUILD(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_CASH_USING_CHANGE_CHARACTERNAME:
			procRes = Process_FI_CASH_USING_CHANGE_CHARACTERNAME(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_CASH_PREMIUM_CARD_INFO:
			procRes = Process_FI_CASH_PREMIUM_CARD_INFO(pPacket, nLength, nBytesUsed);
			break;
// 2004-12-09 by cmkwon, ż©´ÜŔüŔÇ Áľ·á´Â IMServerżˇĽ­¸¸ ąß»ýÇŃ´Ů. ±×·ŻąÇ·Î ľË¸˛Ŕş IMServer -> Field·Î ąß»ýÇŃ´Ů
//		case T_FI_GUILD_NOTIFY_END_WAR:
//			procRes = Process_FI_GUILD_NOTIFY_END_WAR(pPacket, nLength, nBytesUsed);
//			break;
		case T_FI_ADMIN_GET_CHARACTER_INFO_OK:
			procRes = Process_FI_ADMIN_GET_CHARACTER_INFO_OK(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_GET_FIELD_USER_COUNTS_ACK:
			procRes = Process_FI_GET_FIELD_USER_COUNTS_ACK(pPacket, nLength, nBytesUsed);
			break;
		case T_FI_INFO_DECLARATION_MSWAR_SET_OK:	// 2009-01-12 by dhjin, Ľ±Ŕü Ć÷°í - Ľ±ŔüĆ÷°í ˝Ă°Ł ą× Ć÷±â ĽłÁ¤ F->I
			procRes = Process_FI_INFO_DECLARATION_MSWAR_SET_OK(pPacket, nLength, nBytesUsed);
			break;		

		case T_ERROR:
			{
				MSG_ERROR	*pMsgRecvErr;
				pMsgRecvErr = (MSG_ERROR*)(pPacket + nBytesUsed);
				nBytesUsed += (sizeof(MSG_ERROR) + pMsgRecvErr->StringLength);

				//  Ăł¸®
				Err_t error = pMsgRecvErr->ErrorCode;

				g_pGlobal->WriteSystemLogEX(TRUE, "  T_ERROR %s(%#04X) received from %s[%s]\r\n",
					GetErrorString(pMsgRecvErr->ErrorCode), pMsgRecvErr->ErrorCode, 
					GGetENServerTypeString(m_PeerSocketType), m_szPeerIP);
			}
			break;
		default:
			{
				// Protocol Error Ăł¸®(Close Ăł¸®ÇÔ)
				// - Client·Î şÎĹÍ ąŢŔş Field TypeŔĚ ŔŻČżÇĎÁö ľĘ´Ů
				// Error Code : ERR_PROTOCOL_INVALID_PROTOCOL_TYPE
				SendErrorMessage(T_IM_IOCP, ERR_PROTOCOL_INVALID_PROTOCOL_TYPE, 0, 0, NULL, TRUE);

				char	szSystemLog[256];
				sprintf(szSystemLog, "[Error] CIMIOCPSocket::OnRecvdPacketIMServer invalid protocol type, RecvType[%s(%#04X)]\r\n", GetProtocolTypeString(nRecvType), nRecvType);
				g_pGlobal->WriteSystemLog(szSystemLog);
				DBGOUT(szSystemLog);
				return FALSE;
			}
		} // end - switch

		// żˇ·Ż °ü¸®żë ŔĚŔü ¸ŢĽĽÁö ŔúŔĺ
		prevMsgType = nRecvType;

		// MSG °á°ú Ăł¸®
		if (procRes == RES_BREAK)
		{
			// °ćąĚÇŃ żˇ·Żµé. ż¬°áŔ» ˛÷Áö ľĘ´Â´Ů.
			// do nothing, ±×łÉ ł˛ľĆŔÖ´Â ´ŮŔ˝ packetŔ» Ăł¸®ÇŃ´Ů
		}
		else if (procRes == RES_PACKET_ERROR)
		{
			// ĆĐĹ¶ŔĚ ĽŐ»óµČ °ćżě. ł˛Ŕş packetŔ» Ăł¸®ÇĎÁö ľĘ°í ąŮ·Î ¸®ĹĎÇŃ´Ů. Ľ­ąö°Ł ż¬°áżˇ¸¸ »çżë.
			return TRUE;
		}
		else if (procRes == RES_RETURN_TRUE)
		{
			// Á¤»ó
			// do nothing, ±×łÉ ł˛ľĆŔÖ´Â ´ŮŔ˝ packetŔ» Ăł¸®ÇŃ´Ů
		}
		else if (procRes == RES_RETURN_FALSE)
		{
			return FALSE;
		}

//#ifdef _DEBUG
//		if (HIBYTE(nRecvType) == T0_IC_PARTY || nRecvType == T_FI_PARTY_ACCEPT_INVITE_OK)
//		{
//			char *dpm;
//			( (CIMIOCPSocket*)(ms_pIMIOCP->GetIOCPSocket(CLIENT_INDEX_START_NUM)) )->PRINT_DEBUG_MESSAGE(dpm);
//		}
//#endif
	}	// end - while

	return TRUE;
}

void CIMIOCPSocket::OnConnect(void)
{
	memset(&m_character, 0x00, sizeof(CHARACTER));

	char szSystemLog[256];
	sprintf(szSystemLog, "AtumIMServer Index[%3d] Client[%15s] Connected\r\n", this->GetClientArrayIndex(), m_szPeerIP);
	g_pGlobal->WriteSystemLog(szSystemLog);
	DBGOUT(szSystemLog);

	CIOCPSocket::OnConnect();	
	this->SetIMParty(NULL);

	// ±ćµĺ °ü·Ă
	this->SetGuildUniqueNumber(INVALID_GUILD_UID);

	this->SetCurrentFieldServerSocket(NULL);
	m_PeerSocketType			= ST_INVALID_TYPE;
	// 2008-05-15 by dhjin, EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć
//	m_enableChatFlag8			= (BitFlag8_t)0xFF;
	m_enableChatFlag16			= (BitFlag16_t)0xFFFF;
	m_bGameEndRoutineFlag		= TRUE;
	m_bFirstTimeLoadFriendList	= TRUE;		// 2009-03-05 by cmkwon, ĹëÇŐ ľĆ·ąłŞ ŔĚżë˝Ă ÄŁ±¸ ¸®˝şĆ® ş¸łĘ˝ş °ćÇčÄˇ °ü·Ă ąö±× ĽöÁ¤ - 
}

void CIMIOCPSocket::OnClose(int reason)
{
	char szSystemLog[256];
	sprintf(szSystemLog, "AtumIMServer Index[%3d] SocketType[%d] MaxWriteBufCounts[%4d] ClientIP[%15s] Closed ==> reason %d[%#08X]\r\n"
		, this->GetClientArrayIndex(), m_PeerSocketType, m_nMaxWriteBufCounts, m_szPeerIP, reason, reason);
	g_pGlobal->WriteSystemLog(szSystemLog);
	DBGOUT(szSystemLog);

	if (m_PeerSocketType == ST_CLIENT_TYPE)
	{
		GameEndRoutineIM();
	}

	if (m_PeerSocketType == ST_FIELD_SERVER)
	{
		// 2009-03-19 by cmkwon, ˝Ă˝şĹŰ ·Î±× Ăß°ˇ - 
		g_pGlobal->WriteSystemLogEX(TRUE, "  [Notify] Socket for FieldServer was closed !! \r\n");

		ms_pIMIOCP->SetFieldServerSocket(NULL);
		ms_pIMIOCP->m_FieldServerInfo.Reset();
		ms_pIMIOCP->m_mCharacterName.clearLock();
		ms_pIMIOCP->m_MapChactarterUniqueNumber.clearLock();
		ms_pIMIOCP->m_mAccountName.clearLock();
		ms_pIMIOCP->m_MapAccountUniqueNumber.clearLock();
	}

	if(m_PeerSocketType == ST_MONITOR_SERVER)
	{
		ms_pIMIOCP->DeleteMonitorIOCPSocketPtr(this);
	}

	if(m_PeerSocketType == ST_ADMIN_TOOL)
	{
		ms_pIMIOCP->DeleteMonitorIOCPSocketPtr(this);
	}

	CIOCPSocket::OnClose(30);
}

CIMParty *CIMIOCPSocket::GetIMParty(void)
{
	CIMParty *ptParty = this->m_pIMParty;
	if(NULL == ptParty || FALSE == ptParty->IsValidIMParty())
	{
		this->SetIMParty(NULL);
	}

	return ptParty;
}

void CIMIOCPSocket::SetIMParty(CIMParty *i_pParty)
{
	this->m_pIMParty = i_pParty;
}

BOOL CIMIOCPSocket::ResAuthChatLogin(CHARACTER *pCharacter, int nLoginType, int nErr)
{
	BYTE	buffer[SIZE_MAX_PACKET];
	int		nUsedBytes	= 0;

	if(IsUsing() == FALSE){	return FALSE;}

	if(0 != nErr)
	{	// Error
		return FALSE;
	}

	m_nLoginType = nLoginType;
	memcpy(&m_character, pCharacter, sizeof(CHARACTER));	
	m_character.ClientIndex = this->GetClientArrayIndex();

	//////////////////////////////////////////////////////////////////////////
	// 2006-12-08 by dhjin, ÁöµµŔÚ, şÎÁöµµŔÚ ĽłÁ¤
	ms_pIMIOCP->SetInflAllLeader(&m_character);

	memset(buffer, 0x00, SIZE_MAX_PACKET);
	*(MessageType_t*)buffer = T_IC_CONNECT_LOGIN_OK;
	nUsedBytes += SIZE_FIELD_TYPE_HEADER;
	SendAddData(buffer, nUsedBytes);

	/* check: Č®ŔÎ ÇĘżä
	if(m_nLoginType == CHAT_LOGIN_TYPE_MESSENGER)
	{
		memcpy(buffer+nUsedBytes, &m_character, sizeof(MSG_IC_CONNECT_LOGIN_OK));
		nUsedBytes += sizeof(MSG_IC_CONNECT_LOGIN_OK);

		SendAddData(buffer, nUsedBytes);
	}
	*/

	// CharacterNameŔĚ Ĺ°°ŞŔ¸·Î Mapżˇ ĽĽĆĂ
	char tmCharacterName[SIZE_MAX_CHARACTER_NAME];
	ms_pIMIOCP->m_mCharacterName.insertLock(GGetLowerCase(tmCharacterName, m_character.CharacterName, SIZE_MAX_CHARACTER_NAME), this);
	ms_pIMIOCP->m_MapChactarterUniqueNumber.insertLock(m_character.CharacterUniqueNumber, this);
	ms_pIMIOCP->m_mAccountName.insertLock(m_character.AccountName, this);
	ms_pIMIOCP->m_MapAccountUniqueNumber.insertLock(m_character.AccountUniqueNumber, this);

	ms_pIMIOCP->m_uiIMAccumulatedTotalUserCounts++;
	if(ms_pIMIOCP->m_uiIMMaxTotalUserCounts < ms_pIMIOCP->m_MapChactarterUniqueNumber.size())
	{
		ms_pIMIOCP->m_uiIMMaxTotalUserCounts = ms_pIMIOCP->m_MapChactarterUniqueNumber.size();
	}

	return TRUE;
}

FRIENDINFO *CIMIOCPSocket::FindFriendInfoByCharacterName(char *i_szCharacterName)
{
	FRIENDINFO *pRet = NULL;
	if(0 == strcmp(i_szCharacterName, "")
		|| 0 == m_mtvectorFriendList.size())
	{
		return pRet;
	}

	m_mtvectorFriendList.lock();
	mtvectorFRIENDINFO::iterator	itr(m_mtvectorFriendList.begin());
	for(; itr != m_mtvectorFriendList.end(); itr++)
	{
		if(0 == strnicmp(itr->szCharacterName, i_szCharacterName, SIZE_MAX_CHARACTER_NAME))
		{
			pRet = &*itr; //inetpub added '&*' operators for 'itr'
			break;
		}
	}
	m_mtvectorFriendList.unlock();
	
	return pRet;
}
mtvectorFRIENDINFO::iterator CIMIOCPSocket::FindFriendInfoByCharacterNameItr(char *i_szCharacterName) //inetpub fix for find friend info
{
	mtvectorFRIENDINFO::iterator pRet = m_mtvectorFriendList.end();
	if (0 == strcmp(i_szCharacterName, "")
		|| 0 == m_mtvectorFriendList.size())
	{
		return m_mtvectorFriendList.end();
	}

	m_mtvectorFriendList.lock();
	mtvectorFRIENDINFO::iterator	itr(m_mtvectorFriendList.begin());
	for (; itr != m_mtvectorFriendList.end(); itr++)
	{
		if (0 == strnicmp(itr->szCharacterName, i_szCharacterName, SIZE_MAX_CHARACTER_NAME))
		{
			pRet = itr;
		}
	}
	m_mtvectorFriendList.unlock();

	return pRet;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CIMIOCPSocket::IsExistFriendList(char *i_szCharacterName)
/// \brief		
/// \author		cmkwon
/// \date		2006-07-19 ~ 2006-07-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CIMIOCPSocket::IsExistFriendList(char *i_szCharacterName)
{
	if(NULL == this->FindFriendInfoByCharacterName(i_szCharacterName))
	{
		return FALSE;
	}

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CIMIOCPSocket::IncreaseOnlineEachOtherFriendCnts(char *i_szOnlineCharacterName)
/// \brief		
/// \author		cmkwon
/// \date		2006-07-18 ~ 2006-07-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CIMIOCPSocket::IncreaseOnlineEachOtherFriendCnts(char *i_szOnlineCharacterName)
{
	if(0 == strcmp(i_szOnlineCharacterName, "")
		|| 0 == m_mtvectorFriendList.size())
	{
		return FALSE;
	}

	mt_auto_lock mtA(&m_mtvectorFriendList);
	FRIENDINFO *pTmp = this->FindFriendInfoByCharacterName(i_szOnlineCharacterName);
	if(NULL == pTmp)
	{
		return FALSE;
	}

	m_nEachOtherFriendCnts++;		// 2006-07-19 by cmkwon
	m_nEachOtherFriendCnts = min(m_nEachOtherFriendCnts, SIZE_MAX_FRIENDLIST_COUNT);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CIMIOCPSocket::DecreaseOnlineEachOtherFriendCnts(char *i_szOfflineCharacterName)
/// \brief		
/// \author		cmkwon
/// \date		2006-07-19 ~ 2006-07-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CIMIOCPSocket::DecreaseOnlineEachOtherFriendCnts(char *i_szOfflineCharacterName)
{
	if(0 == strcmp(i_szOfflineCharacterName, "")
		|| 0 == m_mtvectorFriendList.size())
	{
		return FALSE;
	}

	mt_auto_lock mtA(&m_mtvectorFriendList);
	FRIENDINFO *pTmp = this->FindFriendInfoByCharacterName(i_szOfflineCharacterName);
	if(NULL == pTmp)
	{
		return FALSE;
	}

	m_nEachOtherFriendCnts--;		// 2006-07-19 by cmkwon
	m_nEachOtherFriendCnts = max(m_nEachOtherFriendCnts, 0);
	return TRUE;
}

BOOL CIMIOCPSocket::InsertToFriendList(FRIENDINFO *i_pFriendInfo, BOOL i_bEachOtherFriend/*=FALSE*/)
{
	if(0 == strncmp(i_pFriendInfo->szCharacterName, "", SIZE_MAX_CHARACTER_NAME))
	{
		return FALSE;
	}

	mt_auto_lock mtA(&m_mtvectorFriendList);

	if(i_bEachOtherFriend
		&& i_pFriendInfo->byIsOnline)
	{// 2006-07-19 by cmkwon
		m_nEachOtherFriendCnts++;
		m_nEachOtherFriendCnts = min(m_nEachOtherFriendCnts, SIZE_MAX_FRIENDLIST_COUNT);
	}

	FRIENDINFO *pTmp = this->FindFriendInfoByCharacterName(i_pFriendInfo->szCharacterName);
	if(pTmp)
	{
		pTmp->byIsOnline = i_pFriendInfo->byIsOnline;
		return FALSE;
	}

	m_mtvectorFriendList.push_back(*i_pFriendInfo);
	
	return TRUE;
}

BOOL CIMIOCPSocket::DeleteFromFriendList(char *i_szCharacterName, BOOL i_bEachOtherFriend/*=FALSE*/)
{
	if(0 == strncmp(i_szCharacterName, "", SIZE_MAX_CHARACTER_NAME))
	{
		return FALSE;
	}

	mt_auto_lock mtA(&m_mtvectorFriendList);

	mtvectorFRIENDINFO::iterator pTmpItr = this->FindFriendInfoByCharacterNameItr(i_szCharacterName);//inetpub fix for find friend info
	FRIENDINFO *pTmp = &*pTmpItr;//inetpub fix for find friend info
	//FRIENDINFO *pTmp = this->FindFriendInfoByCharacterName(i_szCharacterName);
	if(NULL == pTmp)
	{
		return FALSE;
	}

	if(i_bEachOtherFriend
		&& pTmp->byIsOnline)
	{// 2006-07-19 by cmkwon
		m_nEachOtherFriendCnts--;
		m_nEachOtherFriendCnts = max(m_nEachOtherFriendCnts, 0);
	}

	m_mtvectorFriendList.erase(pTmpItr); //inetpub erase fix
	return TRUE;
}

FRIENDINFO *CIMIOCPSocket::IsCharacterNameFromRejectList(char *i_szCharacterName)
{
	FRIENDINFO *pRet = NULL;
	if(0 == stricmp(i_szCharacterName, "")
		|| 0 == m_mtvectorRejectList.size())
	{
		return pRet;
	}

	m_mtvectorRejectList.lock();
	mtvectorFRIENDINFO::iterator	itr(m_mtvectorRejectList.begin());
	for(; itr != m_mtvectorRejectList.end(); itr++)
	{
		if(0 == strnicmp(itr->szCharacterName, i_szCharacterName, SIZE_MAX_CHARACTER_NAME))
		{
			pRet = &*itr; //inetpub added '&*' operators for 'itr'
			break;
		}
	}
	m_mtvectorRejectList.unlock();
	
	return pRet;
}
mtvectorFRIENDINFO::iterator CIMIOCPSocket::IsCharacterNameFromRejectListItr(char *i_szCharacterName) //inetpub fix for reject list
{
	mtvectorFRIENDINFO::iterator pRet = m_mtvectorRejectList.end();
	if (0 == stricmp(i_szCharacterName, "")
		|| 0 == m_mtvectorRejectList.size())
	{
		return m_mtvectorRejectList.end();
	}

	m_mtvectorRejectList.lock();
	mtvectorFRIENDINFO::iterator	itr(m_mtvectorRejectList.begin());
	for (; itr != m_mtvectorRejectList.end(); itr++)
	{
		if (0 == strnicmp(itr->szCharacterName, i_szCharacterName, SIZE_MAX_CHARACTER_NAME))
		{
			pRet = itr;
		}
	}
	m_mtvectorRejectList.unlock();

	return pRet;
}


BOOL CIMIOCPSocket::InsertToRejectList(FRIENDINFO *i_pFriendInfo)
{
	if(0 == strncmp(i_pFriendInfo->szCharacterName, "", SIZE_MAX_CHARACTER_NAME))
	{
		return FALSE;
	}

	m_mtvectorRejectList.lock();
	FRIENDINFO *pTmp = this->IsCharacterNameFromRejectList(i_pFriendInfo->szCharacterName);
	if(pTmp)
	{
		m_mtvectorRejectList.unlock();
		return FALSE;
	}

	m_mtvectorRejectList.push_back(*i_pFriendInfo);
	m_mtvectorRejectList.unlock();
	
	return TRUE;
}

BOOL CIMIOCPSocket::DeleteFromRejectList(char *i_szCharacterName)
{
	if(0 == strncmp(i_szCharacterName, "", SIZE_MAX_CHARACTER_NAME))
	{
		return FALSE;
	}

	m_mtvectorRejectList.lock();
	mtvectorFRIENDINFO::iterator pTmpItr = this->IsCharacterNameFromRejectListItr(i_szCharacterName);  //inetpub fix for reject list
	FRIENDINFO *pTmp = &*pTmpItr;
	//FRIENDINFO *pTmp = this->IsCharacterNameFromRejectList(i_szCharacterName);
	if(NULL == pTmp)
	{
		m_mtvectorRejectList.unlock();
		return FALSE;
	}
	m_mtvectorRejectList.erase(pTmpItr); //inetpub fix for reject list
	m_mtvectorRejectList.unlock();
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CIMIOCPSocket::SetEachOtherFriendCnts(int i_nFriendCnts)
/// \brief		
/// \author		cmkwon
/// \date		2006-07-19 ~ 2006-07-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CIMIOCPSocket::SetEachOtherFriendCnts(int i_nFriendCnts)
{
	mt_auto_lock mtA(&m_mtvectorFriendList);
	m_nEachOtherFriendCnts		= i_nFriendCnts;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CIMIOCPSocket::ChangedCharacterInfluenceType(BYTE i_byInfluenceTy)
/// \brief		
/// \author		cmkwon
/// \date		2005-12-03 ~ 2005-12-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CIMIOCPSocket::ChangedCharacterInfluenceType(BYTE i_byInfluenceTy)
{
	m_character.InfluenceType	= i_byInfluenceTy;
}

// 2009-03-05 by cmkwon, ĹëÇŐ ľĆ·ąłŞ ŔĚżë˝Ă ÄŁ±¸ ¸®˝şĆ® ş¸łĘ˝ş °ćÇčÄˇ °ü·Ă ąö±× ĽöÁ¤ - ResLoadingFriendsAndRejects() ÇÔĽöżˇ ŔÎŔÚĂß°ˇ, BOOL i_bFirstTimeLoadFriendList
BOOL CIMIOCPSocket::ResLoadingFriendsAndRejects(mtvectorFRIENDINFO *i_pvectorFriendList, mtvectorFRIENDINFO *i_pvectorRejectList, BOOL i_bFirstTimeLoadFriendList)
{
	///////////////////////////////////////////////////////////////////////////////
	// 1. ÄŁ±¸¸®˝şĆ® Ăł¸®
	if(false == i_pvectorFriendList->empty())
	{
		CIMIOCPSocket *arrIISocketPtr[SIZE_MAX_FRIENDLIST_COUNT];
		MEMSET_ZERO(arrIISocketPtr, sizeof(arrIISocketPtr[0])*SIZE_MAX_FRIENDLIST_COUNT);

// 2008-10-24 by cmkwon, ÄŁ±¸¸®˝şĆ® ŔüĽŰ ąö±× ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ ÇÔ.
//		INIT_MSG_WITH_BUFFER(MSG_IC_CHAT_FRIENDLIST_LOADING_OK, T_IC_CHAT_FRIENDLIST_LOADING_OK, pSendFriendList, tmSendBuf);
//		pSendFriendList->nFriendListCounts = 0;
//
//		m_mtvectorFriendList.lock();
//		m_mtvectorFriendList.clear();	
//		m_mtvectorFriendList.insert(m_mtvectorFriendList.begin()
//			, i_pvectorFriendList->begin(), i_pvectorFriendList->end());
// 		this->MakeMsg_MSG_IC_CHAT_FRIENDLIST_LOADING_OK(pSendFriendList
// 			, (FRIENDINFO *)(tmSendBuf + MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_LOADING_OK)), (SIZE_MAX_PACKET-MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_LOADING_OK))/sizeof(FRIENDINFO), TRUE, arrIISocketPtr);
// 		m_mtvectorFriendList.unlock();
// 
// 		this->SendAddData(tmSendBuf
// 			, MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_LOADING_OK) + sizeof(FRIENDINFO)*pSendFriendList->nFriendListCounts);
		mt_auto_lock mtF(&m_mtvectorFriendList);
		m_mtvectorFriendList.clear();	
		m_nEachOtherFriendCnts	= 0;		// 2009-03-05 by cmkwon, ĹëÇŐ ľĆ·ąłŞ ŔĚżë˝Ă ÄŁ±¸ ¸®˝şĆ® ş¸łĘ˝ş °ćÇčÄˇ °ü·Ă ąö±× ĽöÁ¤ - ˛Ŕ ÇĘżäÇĎÁö´Â ľĘÁö¸¸ şüÁř°Í °°ľĆĽ­ Ăß°ˇ ÇÔ.
		m_mtvectorFriendList.insert(m_mtvectorFriendList.begin(), i_pvectorFriendList->begin(), i_pvectorFriendList->end());
		this->SendFriendListLoadingOK(arrIISocketPtr);		// 2008-10-24 by cmkwon, ÄŁ±¸¸®˝şĆ® ŔüĽŰ ąö±× ĽöÁ¤ - ľĆ·ˇŔÇ ÇÔĽö·Î şŻ°ćÇÔ
 		mtF.auto_unlock_cancel();


		///////////////////////////////////////////////////////////////////////////
		// µŃ´Ů ÄŁ±¸·Î µî·ĎµÇľî ŔÖ´Ů¸é Á˘ĽÓÁ¤ş¸¸¦ ŔüĽŰÇŃ´Ů
		char szTemp[128];
		int nEachOtherFriendCnts = 0;
		sprintf(szTemp, STRMSG_S_I2NOTIFY_0004, m_character.CharacterName);
		for(int i=0; i < SIZE_MAX_FRIENDLIST_COUNT; i++)
		{
			if(NULL == arrIISocketPtr[i] 
				|| FALSE == arrIISocketPtr[i]->IsValidCharacter())
			{
				continue;
			}

			if(FALSE == COMPARE_RACE(this->GetCharacter()->Race, RACE_OPERATION|RACE_GAMEMASTER)
				&& COMPARE_RACE(arrIISocketPtr[i]->GetCharacter()->Race, RACE_OPERATION|RACE_GAMEMASTER))
			{// 2006-11-14 by cmkwon, ŔĎąÝ ŔŻŔú´Â °ü¸®ŔÚ,żîżµŔÚ¸¦ ÄŁ±¸·Î µî·ĎÇŘµµ Online/Offline Č®ŔÎ şŇ°ˇ, Ăß°ˇ °ćÇčÄˇ şŇ°ˇ
				continue;
			}

			//////////////////////////////////////////////////////////////////////////
			// 2009-03-05 by cmkwon, ĹëÇŐ ľĆ·ąłŞ ŔĚżë˝Ă ÄŁ±¸ ¸®˝şĆ® ş¸łĘ˝ş °ćÇčÄˇ °ü·Ă ąö±× ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĂłŔ˝ ·Îµů˝ĂżÍ ľĆ·ąłŞ ŔĚµż˝Ă ·Îµů˝Ă Ăł¸®¸¦ ´Ů¸Ł°Ô ÇŃ´Ů.
			if(i_bFirstTimeLoadFriendList)
			{
				if(FALSE == arrIISocketPtr[i]->IncreaseOnlineEachOtherFriendCnts(m_character.CharacterName))
				{
					continue;
				}

				arrIISocketPtr[i]->SendString128(STRING_128_USER_NOTICE, szTemp);
				nEachOtherFriendCnts++;
				
				///////////////////////////////////////////////////////////////////////////////
				// 2006-07-18 by cmkwon, Ľ­·Î µî·ĎÇŃ żÂ¶óŔÎ ÄŁ±¸ Ä«żîĆ® FieldServer·Î ŔüĽŰ
				arrIISocketPtr[i]->SendOnlineEachOtherFriendCntsToFieldServer();
			}
			else
			{
				//////////////////////////////////////////////////////////////////////////
				// 2009-03-05 by cmkwon, ĹëÇŐ ľĆ·ąłŞ ŔĚżë˝Ă ÄŁ±¸ ¸®˝şĆ® ş¸łĘ˝ş °ćÇčÄˇ °ü·Ă ąö±× ĽöÁ¤ - ĂłŔ˝ ·ÎµůŔĚ ľĆ´Ď¸é ş»ŔÎŔÇ Ä«żîĆ®¸¸ Áő°ˇ ˝ĂÄŃľß ÇŃ´Ů.
				if(FALSE == arrIISocketPtr[i]->IsExistFriendList(m_character.CharacterName))
				{
					continue;
				}

				nEachOtherFriendCnts++;
			}
		}
		this->SetEachOtherFriendCnts(nEachOtherFriendCnts);		// 2006-07-19 by cmkwon

		///////////////////////////////////////////////////////////////////////////////
		// 2006-07-18 by cmkwon, Ľ­·Î µî·ĎÇŃ żÂ¶óŔÎ ÄŁ±¸ Ä«żîĆ® FieldServer·Î ŔüĽŰ
		this->SendOnlineEachOtherFriendCntsToFieldServer();
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2. °ĹşÎ¸®˝şĆ® Ăł¸®	
	if(false == i_pvectorRejectList->empty())
	{
// 2008-10-24 by cmkwon, ÄŁ±¸¸®˝şĆ® ŔüĽŰ ąö±× ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ ÇÔ
//		INIT_MSG_WITH_BUFFER(MSG_IC_CHAT_REJECTLIST_LOADING_OK, T_IC_CHAT_REJECTLIST_LOADING_OK, pSendRejectList, tmSendBuf);
//		pSendRejectList->nRejectListCounts = 0;
//
//		m_mtvectorRejectList.lock();
//		m_mtvectorRejectList.clear();
//		m_mtvectorRejectList.insert(m_mtvectorRejectList.begin()
//			, i_pvectorRejectList->begin(), i_pvectorRejectList->end());
// 		this->MakeMsg_MSG_IC_CHAT_REJECTLIST_LOADING_OK(pSendRejectList
// 			, (FRIENDINFO*)(tmSendBuf + MSG_SIZE(MSG_IC_CHAT_REJECTLIST_LOADING_OK)), (SIZE_MAX_PACKET-MSG_SIZE(MSG_IC_CHAT_REJECTLIST_LOADING_OK))/sizeof(FRIENDINFO));
// 		m_mtvectorRejectList.unlock();
// 
// 		this->SendAddData(tmSendBuf
// 			, MSG_SIZE(MSG_IC_CHAT_REJECTLIST_LOADING_OK) + sizeof(FRIENDINFO)*pSendRejectList->nRejectListCounts);
		mt_auto_lock mtR(&m_mtvectorRejectList);
		m_mtvectorRejectList.clear();
		m_mtvectorRejectList.insert(m_mtvectorRejectList.begin(), i_pvectorRejectList->begin(), i_pvectorRejectList->end());
		this->SendRejectListLoadingOK();		// 2008-10-24 by cmkwon, ÄŁ±¸¸®˝şĆ® ŔüĽŰ ąö±× ĽöÁ¤ - 
 		mtR.auto_unlock_cancel();
	}
	
	return TRUE;
}

// 2008-10-24 by cmkwon, ÄŁ±¸¸®˝şĆ® ŔüĽŰ ąö±× ĽöÁ¤ - ľĆ·ˇŔÇ ÇÔĽö·Î şŻ°ćÇÔ
// ///////////////////////////////////////////////////////////////////////////////
// /// \fn			BOOL CIMIOCPSocket::MakeMsg_MSG_IC_CHAT_FRIENDLIST_LOADING_OK(MSG_IC_CHAT_FRIENDLIST_ALL *io_pFiendListAll, FRIENDINFO *io_pFriendInfo, int i_nMaxCount, BOOL i_bAllFlag/*=FALSE*/)
// /// \brief		
// /// \author		cmkwon
// /// \date		2004-10-14 ~ 2004-10-14
// /// \warning	
// ///
// /// \param		
// /// \return		
// ///////////////////////////////////////////////////////////////////////////////
// BOOL CIMIOCPSocket::MakeMsg_MSG_IC_CHAT_FRIENDLIST_LOADING_OK(MSG_IC_CHAT_FRIENDLIST_LOADING_OK *io_pFriendListAll
// 															  , FRIENDINFO *io_pFriendInfo, int i_nMaxCount															  
// 															  , BOOL i_bAllFlag/*=FALSE*/
// 															  , CIMIOCPSocket **o_ArrOnlineIISocketPtr/*=NULL*/)
// {
// 
// // 2007-06-01 by cmkwon, autolock Ŕ¸·Î ĽöÁ¤ÇÔ
// //	m_mtvectorFriendList.lock();	
// 	mt_auto_lock mtA(&m_mtvectorFriendList);
// 
// 	if(i_bAllFlag)
// 	{// ¸đµç ŔŻŔú Á¤ş¸ ŔüĽŰ
// 
// 		int nOnlineCounts = 0;
// 		mtvectorFRIENDINFO::iterator itr(m_mtvectorFriendList.begin());
// 		for(; itr != m_mtvectorFriendList.end(); itr++)
// 		{
// 			if(i_nMaxCount <= io_pFriendListAll->nFriendListCounts)
// 			{// 2007-01-24 by cmkwon, ĂÖ´ë °łĽö ąö±× ĽöÁ¤
// 
// 				return TRUE;
// 			}
// 
// 			if(strnicmp(itr->szCharacterName, "", SIZE_MAX_CHARACTER_NAME))
// 			{
// 				CIMIOCPSocket *pIISocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(itr->szCharacterName);
// 				if(NULL == pIISocket
// 					|| FALSE == pIISocket->IsValidCharacter())
// 				{
// 					itr->byIsOnline			= FALSE;
// 				}
// 				else
// 				{
// 					if(FALSE == COMPARE_RACE(this->GetCharacter()->Race, RACE_OPERATION|RACE_GAMEMASTER)
// 						&& COMPARE_RACE(pIISocket->GetCharacter()->Race, RACE_OPERATION|RACE_GAMEMASTER))
// 					{// 2006-11-14 by cmkwon, ŔĎąÝ ŔŻŔú´Â °ü¸®ŔÚ,żîżµŔÚ¸¦ ÄŁ±¸·Î µî·ĎÇŘµµ Online/Offline Č®ŔÎ şŇ°ˇ, Ăß°ˇ °ćÇčÄˇ şŇ°ˇ
// 						itr->byIsOnline		= FALSE;
// 					}
// 					else
// 					{
// 						itr->byIsOnline		= TRUE;
// 						if(o_ArrOnlineIISocketPtr 
// 							&& nOnlineCounts < SIZE_MAX_FRIENDLIST_COUNT)
// 						{
// 							o_ArrOnlineIISocketPtr[nOnlineCounts] = pIISocket;
// 							nOnlineCounts++;
// 						}
// 					}
// 				}
// 				
// 				STRNCPY_MEMSET(io_pFriendInfo->szCharacterName, itr->szCharacterName, SIZE_MAX_CHARACTER_NAME);
// 				io_pFriendInfo->byIsOnline = itr->byIsOnline;
// 
// 				io_pFriendListAll->nFriendListCounts++;
// 				io_pFriendInfo++;
// 			}
// 		}
// 	}
// 	else
// 	{// şŻ°ćµČ ŔŻŔú Á¤ş¸¸¸ ŔüĽŰ
// 
// 		mtvectorFRIENDINFO::iterator itr(m_mtvectorFriendList.begin());
// 		for(; itr != m_mtvectorFriendList.end(); itr++)
// 		{
// 			if(i_nMaxCount <= io_pFriendListAll->nFriendListCounts)
// 			{// 2007-01-24 by cmkwon, ĂÖ´ë °łĽö ąö±× ĽöÁ¤
// 
// 				return TRUE;
// 			}
// 
// 			if(strnicmp(itr->szCharacterName, "", SIZE_MAX_CHARACTER_NAME))
// 			{
// 				BYTE tmIsOnline = itr->byIsOnline;
// 				itr->byIsOnline = (ms_pIMIOCP->GetIMIOCPSocketByCharacterName(itr->szCharacterName))?TRUE:FALSE;
// 				
// 				if(tmIsOnline != itr->byIsOnline)
// 				{
// 					STRNCPY_MEMSET(io_pFriendInfo->szCharacterName, itr->szCharacterName, SIZE_MAX_CHARACTER_NAME);
// 					io_pFriendInfo->byIsOnline = itr->byIsOnline;
// 
// 					io_pFriendListAll->nFriendListCounts++;
// 					io_pFriendInfo++;
// 				}			
// 			}
// 		}
// 	}	
// // 2007-06-01 by cmkwon, autolock Ŕ¸·Î ĽöÁ¤ÇÔ
// //	m_mtvectorFriendList.unlock();
// 	return TRUE;
// }
// 
// 
// ///////////////////////////////////////////////////////////////////////////////
// /// \fn			BOOL CIMIOCPSocket::MakeMsg_MSG_IC_CHAT_REJECTLIST_LOADING_OK(MSG_IC_CHAT_REJECTLIST_ALL *io_pRejectListAll, FRIENDINFO *io_pFriendInfo, int i_nMaxCount)
// /// \brief		
// /// \author		cmkwon
// /// \date		2004-10-14 ~ 2004-10-14
// /// \warning	
// ///
// /// \param		
// /// \return		
// ///////////////////////////////////////////////////////////////////////////////
// BOOL CIMIOCPSocket::MakeMsg_MSG_IC_CHAT_REJECTLIST_LOADING_OK(MSG_IC_CHAT_REJECTLIST_LOADING_OK *io_pRejectListAll, FRIENDINFO *io_pFriendInfo, int i_nMaxCount)
// {
// 	m_mtvectorRejectList.lock();
// 	mtvectorFRIENDINFO::iterator itr(m_mtvectorRejectList.begin());
// 	for(; itr != m_mtvectorRejectList.end(); itr++)
// 	{
// 		if(i_nMaxCount <= io_pRejectListAll->nRejectListCounts)
// 		{// 2007-01-24 by cmkwon, ĂÖ´ë °łĽö ąö±× ĽöÁ¤
// 			m_mtvectorRejectList.unlock();				// 2008-07-17 by cmkwon, µĄµĺ¶ô ąö±× ĽöÁ¤
// 			return TRUE;
// 		}
// 		
// 		if(strncmp(itr->szCharacterName, "", SIZE_MAX_CHARACTER_NAME))
// 		{
// 			STRNCPY_MEMSET(io_pFriendInfo->szCharacterName, itr->szCharacterName, SIZE_MAX_CHARACTER_NAME);
// 			io_pFriendInfo->RegDate	= itr->RegDate;			// 2008-04-11 by dhjin, EP3 °ĹşÎ¸ń·Ď -
// 			io_pRejectListAll->nRejectListCounts++;
// 			io_pFriendInfo++;
// 		}
// 	}
// 	m_mtvectorRejectList.unlock();
// 	return TRUE;
// }


///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CIMIOCPSocket::SendFriendListLoadingOK(CIMIOCPSocket **o_ArrOnlineIISocketPtr/*=NULL*/)
/// \brief		// 2008-10-24 by cmkwon, ÄŁ±¸¸®˝şĆ® ŔüĽŰ ąö±× ĽöÁ¤ - MakeMsg_MSG_IC_CHAT_FRIENDLIST_LOADING_OK() ÇÔĽö¸¦ şŻ°ć ÇÔ
/// \author		cmkwon
/// \date		2008-10-24 ~ 2008-10-24
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CIMIOCPSocket::SendFriendListLoadingOK(CIMIOCPSocket **o_ArrOnlineIISocketPtr/*=NULL*/)
{
	INIT_MSG_WITH_BUFFER(MSG_IC_CHAT_FRIENDLIST_LOADING_OK, T_IC_CHAT_FRIENDLIST_LOADING_OK, pSendFriendList, tmSendBuf);
	pSendFriendList->nFriendListCounts	= 0;
	FRIENDINFO *pFriendInfo				= (FRIENDINFO*)(tmSendBuf + MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_LOADING_OK));

	mt_auto_lock mtA(&m_mtvectorFriendList);

	int nOnlineCounts = 0;
	mtvectorFRIENDINFO::iterator itr(m_mtvectorFriendList.begin());
	for(; itr != m_mtvectorFriendList.end(); itr++)
	{
		if(0 == strnicmp(itr->szCharacterName, "", SIZE_MAX_CHARACTER_NAME))
		{
			continue;
		}

		if( SIZE_MAX_PACKET <= MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_LOADING_OK) + (pSendFriendList->nFriendListCounts+1)*sizeof(FRIENDINFO) )
		{// 2008-10-24 by cmkwon, ĆĐĹ¶ ĂÖ´ë »çŔĚÁî żŔąö ĂĽĹ©
			this->SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_LOADING_OK) + pSendFriendList->nFriendListCounts*sizeof(FRIENDINFO));
			pSendFriendList->nFriendListCounts	= 0;
			pFriendInfo							= (FRIENDINFO*)(tmSendBuf + MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_LOADING_OK));
		}

		CIMIOCPSocket *pIISocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(itr->szCharacterName);
		if(NULL == pIISocket
			|| FALSE == pIISocket->IsValidCharacter())
		{
			itr->byIsOnline			= FALSE;
		}
		else
		{
			if(FALSE == COMPARE_RACE(this->GetCharacter()->Race, RACE_OPERATION|RACE_GAMEMASTER)
				&& COMPARE_RACE(pIISocket->GetCharacter()->Race, RACE_OPERATION|RACE_GAMEMASTER))
			{// 2006-11-14 by cmkwon, ŔĎąÝ ŔŻŔú´Â °ü¸®ŔÚ,żîżµŔÚ¸¦ ÄŁ±¸·Î µî·ĎÇŘµµ Online/Offline Č®ŔÎ şŇ°ˇ, Ăß°ˇ °ćÇčÄˇ şŇ°ˇ
				itr->byIsOnline		= FALSE;
			}
			else
			{
				itr->byIsOnline		= TRUE;
				if(o_ArrOnlineIISocketPtr 
					&& nOnlineCounts < SIZE_MAX_FRIENDLIST_COUNT)
				{
					o_ArrOnlineIISocketPtr[nOnlineCounts] = pIISocket;
					nOnlineCounts++;
				}
			}
		}
		
		STRNCPY_MEMSET(pFriendInfo->szCharacterName, itr->szCharacterName, SIZE_MAX_CHARACTER_NAME);
		pFriendInfo->byIsOnline = itr->byIsOnline;
		pFriendInfo->LastLogin = itr->LastLogin;
		pFriendInfo->RegDate = itr->RegDate;
		pSendFriendList->nFriendListCounts++;
		pFriendInfo++;
	}

	if(0 < pSendFriendList->nFriendListCounts)
	{
		this->SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_LOADING_OK) + pSendFriendList->nFriendListCounts*sizeof(FRIENDINFO));
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CIMIOCPSocket::SendFriendListRefreshOK(void)
/// \brief		// 2008-10-24 by cmkwon, ÄŁ±¸¸®˝şĆ® ŔüĽŰ ąö±× ĽöÁ¤ - 
/// \author		cmkwon
/// \date		2008-10-24 ~ 2008-10-24
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CIMIOCPSocket::SendFriendListRefreshOK(void)
{
	INIT_MSG_WITH_BUFFER(MSG_IC_CHAT_FRIENDLIST_REFRESH_OK, T_IC_CHAT_FRIENDLIST_REFRESH_OK, pSendFriendList, tmSendBuf);
	pSendFriendList->nFriendListCounts	= 0;
	FRIENDINFO *pFriendInfo				= (FRIENDINFO*)(tmSendBuf + MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_REFRESH_OK));
	
	mt_auto_lock mtA(&m_mtvectorFriendList);
			
	mtvectorFRIENDINFO::iterator itr(m_mtvectorFriendList.begin());
	for(; itr != m_mtvectorFriendList.end(); itr++)
	{
		if(0 == strnicmp(itr->szCharacterName, "", SIZE_MAX_CHARACTER_NAME))
		{
			continue;
		}

		if( SIZE_MAX_PACKET <= MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_REFRESH_OK) + (pSendFriendList->nFriendListCounts+1)*sizeof(FRIENDINFO) )
		{// 2008-10-24 by cmkwon, ĆĐĹ¶ ĂÖ´ë »çŔĚÁî żŔąö ĂĽĹ©
			this->SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_REFRESH_OK) + pSendFriendList->nFriendListCounts*sizeof(FRIENDINFO));
			pSendFriendList->nFriendListCounts	= 0;
			pFriendInfo							= (FRIENDINFO*)(tmSendBuf + MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_REFRESH_OK));
		}

		BYTE tmIsOnline = itr->byIsOnline;
		itr->byIsOnline = (ms_pIMIOCP->GetIMIOCPSocketByCharacterName(itr->szCharacterName))?TRUE:FALSE;

		if(tmIsOnline != itr->byIsOnline)
		{
			STRNCPY_MEMSET(pFriendInfo->szCharacterName, itr->szCharacterName, SIZE_MAX_CHARACTER_NAME);
			pFriendInfo->byIsOnline = itr->byIsOnline;
			pFriendInfo->LastLogin = itr->LastLogin;
			pFriendInfo->RegDate = itr->RegDate;
			pSendFriendList->nFriendListCounts++;
			pFriendInfo++;
		}
	}

	if(0 < pSendFriendList->nFriendListCounts)
	{
		this->SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_REFRESH_OK) + pSendFriendList->nFriendListCounts*sizeof(FRIENDINFO));
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CIMIOCPSocket::SendRejectListLoadingOK(void)
/// \brief		// 2008-10-24 by cmkwon, ÄŁ±¸¸®˝şĆ® ŔüĽŰ ąö±× ĽöÁ¤ - MakeMsg_MSG_IC_CHAT_REJECTLIST_LOADING_OK() ÇÔĽö¸¦ şŻ°ćÇÔ
/// \author		cmkwon
/// \date		2008-10-24 ~ 2008-10-24
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CIMIOCPSocket::SendRejectListLoadingOK(void)
{
	INIT_MSG_WITH_BUFFER(MSG_IC_CHAT_REJECTLIST_LOADING_OK, T_IC_CHAT_REJECTLIST_LOADING_OK, pSendRejectList, tmSendBuf);
	pSendRejectList->nRejectListCounts	= 0;
	FRIENDINFO *pFriendInfo				= (FRIENDINFO*)(tmSendBuf + MSG_SIZE(MSG_IC_CHAT_REJECTLIST_LOADING_OK));

	mt_auto_lock mtA(&m_mtvectorRejectList);
	mtvectorFRIENDINFO::iterator itr(m_mtvectorRejectList.begin());
	for(; itr != m_mtvectorRejectList.end(); itr++)
	{
		if(0 == strncmp(itr->szCharacterName, "", SIZE_MAX_CHARACTER_NAME))
		{
			continue;
		}

		if( SIZE_MAX_PACKET <= MSG_SIZE(MSG_IC_CHAT_REJECTLIST_LOADING_OK) + (pSendRejectList->nRejectListCounts+1)*sizeof(FRIENDINFO) )
		{// 2008-10-24 by cmkwon, ĆĐĹ¶ ĂÖ´ë »çŔĚÁî żŔąö ĂĽĹ©
			this->SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_CHAT_REJECTLIST_LOADING_OK) + pSendRejectList->nRejectListCounts*sizeof(FRIENDINFO));
			pSendRejectList->nRejectListCounts	= 0;
			pFriendInfo							= (FRIENDINFO*)(tmSendBuf + MSG_SIZE(MSG_IC_CHAT_REJECTLIST_LOADING_OK));
		}

		STRNCPY_MEMSET(pFriendInfo->szCharacterName, itr->szCharacterName, SIZE_MAX_CHARACTER_NAME);
		pFriendInfo->RegDate	= itr->RegDate;			// 2008-04-11 by dhjin, EP3 °ĹşÎ¸ń·Ď -
		pSendRejectList->nRejectListCounts++;
		pFriendInfo++;
	}

	if(0 < pSendRejectList->nRejectListCounts)
	{
		this->SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_CHAT_REJECTLIST_LOADING_OK) + pSendRejectList->nRejectListCounts*sizeof(FRIENDINFO));
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CIMIOCPSocket::IsValidCharacter(void)
/// \brief		
/// \author		cmkwon
/// \date		2006-07-18 ~ 2006-07-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CIMIOCPSocket::IsValidCharacter(void)
{
	if(FALSE == IsUsing()
		|| ST_CLIENT_TYPE != m_PeerSocketType
		|| FALSE == IS_VALID_UNIQUE_NUMBER(m_character.CharacterUniqueNumber))
	{
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CIMIOCPSocket::SetChatCharacter(CHARACTER *pCharacter)
/// \brief		
/// \author		cmkwon
/// \date		2004-10-14 ~ 2004-10-14
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CIMIOCPSocket::SetChatCharacter(CHARACTER *pCharacter)
{
	memcpy(&m_character, pCharacter, sizeof(CHARACTER));
	return TRUE;
}
ProcessResult CIMIOCPSocket::Process_FI_MULTICHAT_STEERING(const char* pPacket, int nLength, int &nBytesUsed)
{
	int						nRecvTypeSize = 0;
	CIMIOCPSocket			*pIOCPSocket = NULL;
	MSG_FI_MULTICHAT_STEERING	*pRecvCommand = NULL;

	nRecvTypeSize = sizeof(MSG_FI_MULTICHAT_STEERING);
	if (nLength - nBytesUsed < nRecvTypeSize) {
		SendErrorMessage(T_FI_MULTICHAT_STEERING, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12003);
		return RES_RETURN_FALSE;
	}

	pRecvCommand = new MSG_FI_MULTICHAT_STEERING;
	memcpy(pRecvCommand, pPacket + nBytesUsed, nRecvTypeSize);
	nBytesUsed += nRecvTypeSize;

	ms_pIMIOCP->m_bMultiChatStatues = pRecvCommand->nEnableChats;
	ms_pIMIOCP->m_bINETTradeChatCmdState = pRecvCommand->nEnableChats;

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// ProcessResult Process_IC_CONNECT_LOGIN(const char* pPacket, int nLength, int &nBytesUsed)
//	: IM Serverżˇ Á˘ĽÓ, ŔÎÁőŔ» °ĹÄŁ´Ů
ProcessResult CIMIOCPSocket::Process_IC_CONNECT_LOGIN(const char* pPacket, int nLength, int &nBytesUsed)
{
	int						nRecvTypeSize = 0;
	CIMIOCPSocket			*pIOCPSocket = NULL;
	MSG_IC_CONNECT_LOGIN	*pRecvLogin = NULL;

	nRecvTypeSize = sizeof(MSG_IC_CONNECT_LOGIN);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{	// ąŢľĆľßÇŇ ¸ŢĽĽÁö Ĺ¸ŔÔŔÇ »çŔĚÁîş¸´Ů ł˛Ŕş µĄŔĚĹ¸ŔÇ »çŔĚÁî°ˇ ŔŰ´Ů¸é żˇ·Ż
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		SendErrorMessage(T_IC_CONNECT_LOGIN, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12002);
		return RES_RETURN_FALSE;
	}
	pRecvLogin = new MSG_IC_CONNECT_LOGIN;
	memcpy(pRecvLogin, pPacket+nBytesUsed, nRecvTypeSize);
	nBytesUsed += nRecvTypeSize;

	// 2009-01-23 by cmkwon, ĹÍĹ° i,I ą®ŔÚ ą®Á¦ Ăł¸® - T_IC_CONNECT_LOGIN
	LowercaseOnly_i_forTurkish(pRecvLogin->AccountName, SIZE_MAX_ACCOUNT_NAME);
	LowercaseOnly_i_forTurkish(pRecvLogin->CharacterName, SIZE_MAX_CHARACTER_NAME);

	m_PeerSocketType = ST_CLIENT_TYPE;
	if(ms_pIMIOCP->IsConnectedUser(pRecvLogin->AccountName, &pIOCPSocket))
	{
		SendErrorMessage(T_IC_CONNECT_LOGIN, ERR_PROTOCOL_DUPLICATE_LOGIN, 0, 0, pRecvLogin->AccountName);
		Close(0x12003);
		SAFE_DELETE(pRecvLogin);
		return RES_RETURN_FALSE;
	}

	STRNCPY_MEMSET(m_character.AccountName, pRecvLogin->AccountName, SIZE_MAX_ACCOUNT_NAME);
	STRNCPY_MEMSET(m_character.CharacterName, pRecvLogin->CharacterName, SIZE_MAX_CHARACTER_NAME);
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_AuthChatLogin, this, pRecvLogin);	// DB·Î ŔÎÁő ŔýÂ÷¸¦ łŃ±ä´Ů.
	pRecvLogin = NULL;

	char szSystemLog[256];
	sprintf(szSystemLog, "  IM Server Logined Account[%10s], Character[%10s]\r\n", m_character.AccountName, m_character.CharacterName);
	g_pGlobal->WriteSystemLog(szSystemLog);

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// ProcessResult Process_IC_CHAT_ALL(const char* pPacket, int nLength, int &nBytesUsed)
//	:
// 2007-02-21 by cmkwon, ŔÎŔÚĂß°ˇ(SThreadInfo *i_pThreadInfo)
ProcessResult CIMIOCPSocket::Process_IC_CHAT_ALL(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
{
	int				nRecvTypeSize;
	MSG_IC_CHAT_ALL	*pRecvChatAll;
	char			*chatString;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_ALL) + ((MSG_IC_CHAT_ALL*)(pPacket+nBytesUsed))->MessageLength;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_ALL, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12005);
		return RES_RETURN_FALSE;
	}
	pRecvChatAll = (MSG_IC_CHAT_ALL*)(pPacket+nBytesUsed);
	chatString = (char*)(pPacket+nBytesUsed+sizeof(MSG_IC_CHAT_ALL));
	nBytesUsed += nRecvTypeSize;

	if (!COMPARE_RACE(m_character.Race, RACE_OPERATION|RACE_GAMEMASTER))
	{
		SendErrorMessage(T_IC_CHAT_ALL, ERR_CHAT_PERMISSION_DENIED);
		return RES_BREAK;
	}

	if (strnicmp(m_character.CharacterName, pRecvChatAll->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_ALL, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		Close(0x12006);
		return RES_RETURN_FALSE;
	}

	// Ă¤ĆĂ ÇĘĹÍ¸µ
	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_ALL, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	char szChatting[1024];
	sprintf(szChatting, "Chat_All|%s|%d|%d|%s\r\n",
			pRecvChatAll->FromCharacterName, m_character.MapChannelIndex.MapIndex,
			m_character.MapChannelIndex.ChannelIndex, (char*)pRecvChatAll + sizeof(MSG_IC_CHAT_ALL));
	// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
	g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);
#ifdef _INET_AT_ALL_CHATS
	INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
	msgATChat->PrintType = 0x00;
	sprintf(szChatting,"[%s]: %s", pRecvChatAll->FromCharacterName, chatString);
	STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
	msgATChat->SecurityNumber = m_character.InfluenceType;
	ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif
	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}
	pRecvChatAll->DonatorRank = m_character.PCBangTotalPlayTime;
	ms_pIMIOCP->SendChatMsgToAllClients(CHAT_TYPE_UNCONDITIONAL, ((BYTE*)pRecvChatAll) - SIZE_FIELD_TYPE_HEADER, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize);

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}
	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// ProcessResult Process_IC_CHAT_MAP(const char* pPacket, int nLength, int &nBytesUsed)
//	:
// 2007-02-21 by cmkwon, ŔÎŔÚĂß°ˇ(SThreadInfo *i_pThreadInfo)
ProcessResult CIMIOCPSocket::Process_IC_CHAT_MAP(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
{
	int				nRecvTypeSize;
	MSG_IC_CHAT_MAP	*pRecvChatMap;
	char			*chatString;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_MAP) + ((MSG_IC_CHAT_MAP*)(pPacket+nBytesUsed))->MessageLength;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_MAP, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12007);
		return RES_RETURN_FALSE;
	}
	pRecvChatMap = (MSG_IC_CHAT_MAP*)(pPacket+nBytesUsed);
	chatString = (char*)(pPacket+nBytesUsed+sizeof(MSG_IC_CHAT_MAP));
	nBytesUsed += nRecvTypeSize;

	if (strnicmp(m_character.CharacterName, pRecvChatMap->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_MAP, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		Close(0x12008);
		return RES_RETURN_FALSE;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	int nRemainMinute = 0;
	if(ms_pIMIOCP->m_chatBlockManager.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName)
		|| ms_pIMIOCP->m_chatBlockManagerByLeader.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName))	// 2008-12-30 by cmkwon, ÁöµµŔÚ Ă¤ĆĂ Á¦ÇŃ Ä«µĺ ±¸Çö - T_IC_CHAT_MAP, ¸Ę
	{
		SendErrorMessage(T_IC_CHAT_MAP, ERR_CHAT_CHAT_BLOCKED, nRemainMinute);
		return RES_BREAK;
	}

	// Ă¤ĆĂ ÇĘĹÍ¸µ
	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_MAP, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	char szChatting[1024];
	sprintf(szChatting, "Chat_Map|%s|%d|%d|%s\r\n",
			pRecvChatMap->FromCharacterName, m_character.MapChannelIndex.MapIndex,
			m_character.MapChannelIndex.ChannelIndex, (char*)pRecvChatMap + sizeof(MSG_IC_CHAT_MAP));
	// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
	g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);
#ifdef _INET_AT_ALL_CHATS
	INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
	msgATChat->PrintType = 0x01;
	sprintf(szChatting, "[%s]: %s", pRecvChatMap->FromCharacterName, chatString);
	STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
	msgATChat->SecurityNumber = m_character.InfluenceType;
	ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif
	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	INIT_MSG_WITH_BUFFER(MSG_FI_CHAT_MAP, T_FI_CHAT_MAP, msgChatMap, SendBuf);
	msgChatMap->CharacterUniqueNumber = m_character.CharacterUniqueNumber;
	msgChatMap->MessageLength = pRecvChatMap->MessageLength;
	STRNCPY_MEMSET((char*)SendBuf + MSG_SIZE(MSG_FI_CHAT_MAP), chatString, SIZE_MAX_CHAT_MESSAGE);

	this->SendMsgToCurrentFieldServerSocket(SendBuf, MSG_SIZE(MSG_FI_CHAT_MAP) + msgChatMap->MessageLength);

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}
	
	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// ProcessResult Process_IC_CHAT_REGION(const char* pPacket, int nLength, int &nBytesUsed)
//	:
// 2007-02-21 by cmkwon, ŔÎŔÚĂß°ˇ(SThreadInfo *i_pThreadInfo)
ProcessResult CIMIOCPSocket::Process_IC_CHAT_REGION(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
{
	int					nRecvTypeSize;
	MSG_IC_CHAT_REGION	*pRecvChatRegion;
	char				*chatString;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_REGION) + ((MSG_IC_CHAT_REGION*)(pPacket+nBytesUsed))->MessageLength;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_REGION, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1200A);
		return RES_RETURN_FALSE;
	}
	pRecvChatRegion = (MSG_IC_CHAT_REGION*)(pPacket+nBytesUsed);
	chatString = (char*)(pPacket+nBytesUsed+sizeof(MSG_IC_CHAT_REGION));
	nBytesUsed += nRecvTypeSize;

	if (strnicmp(m_character.CharacterName, pRecvChatRegion->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_REGION, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		Close(0x1200B);
		return RES_RETURN_FALSE;
	}

	// Ă¤ĆĂ ÇĘĹÍ¸µ
	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_REGION, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	///////////////////////////////////////////////////////////////////////////////
	// Ă¤ĆĂ ·Î±× ł˛±â±â
	char szChatting[1024];
	sprintf(szChatting, "Chat_Region|%s|%d|%d|%s\r\n",
			pRecvChatRegion->FromCharacterName, m_character.MapChannelIndex.MapIndex,
			m_character.MapChannelIndex.ChannelIndex, (char*)pRecvChatRegion + sizeof(MSG_IC_CHAT_REGION));
	// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
#ifdef _INET_AT_ALL_CHATS
	INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
	msgATChat->PrintType = 0x02;
	sprintf(szChatting, "[%s]: %s", pRecvChatRegion->FromCharacterName, chatString);
	STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
	msgATChat->SecurityNumber = m_character.InfluenceType;
	ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif
	if( TRUE == g_pIMGlobal->CheckWriteLog(chatString)/* && FALSE == ms_pIMIOCP->m_pSecurityManager->CheckCommand(chatString)*/ )		// 2011-06-22 by hskim, »çĽł Ľ­ąö ąćÁö
	g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	if (strncmp(chatString, "/", 1) == 0)
	{
		// 2007-10-02 by cmkwon, COMMAND ·Î±× ł˛±â±â
		if( TRUE == g_pIMGlobal->CheckWriteLog(chatString) /*&& FALSE == ms_pIMIOCP->m_pSecurityManager->CheckCommand(chatString)*/ )		// 2011-06-22 by hskim, »çĽł Ľ­ąö ąćÁö
		g_pIMGlobal->WriteSystemLogEX(FALSE, "[Notify] : %s, IP(%s), command(%s)\r\n", GetCharacterString(&m_character, string()), this->GetPeerIP(), chatString);

		// ¸í·ÉľîŔÓ
		BOOL ret = HandleAdminCommands(chatString);
		if (ret)
		{
			return RES_RETURN_TRUE;
		}
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	if(0 != strncmp(chatString, "/", 1))
	{// 2008-12-30 by cmkwon, ¸í·Éľî´Â Ă¤±Ý ĂĽĹ©ÇĎÁö ľĘ´Â´Ů.
		int nRemainMinute = 0;
		if(ms_pIMIOCP->m_chatBlockManager.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName)
			|| ms_pIMIOCP->m_chatBlockManagerByLeader.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName))	// 2008-12-30 by cmkwon, ÁöµµŔÚ Ă¤ĆĂ Á¦ÇŃ Ä«µĺ ±¸Çö - T_IC_CHAT_REGION, ÁÖşŻ
		{
			SendErrorMessage(T_IC_CHAT_REGION, ERR_CHAT_CHAT_BLOCKED, nRemainMinute);
			return RES_BREAK;
		}
	}


	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	INIT_MSG_WITH_BUFFER(MSG_FI_CHAT_REGION, T_FI_CHAT_REGION, msgChatRegion, SendBuf);
	msgChatRegion->CharacterUniqueNumber = m_character.CharacterUniqueNumber;
	msgChatRegion->MessageLength = pRecvChatRegion->MessageLength;
	STRNCPY_MEMSET((char*)SendBuf + MSG_SIZE(MSG_FI_CHAT_REGION), chatString, SIZE_MAX_CHAT_MESSAGE);

	if (FALSE == this->SendMsgToCurrentFieldServerSocket(SendBuf, MSG_SIZE(MSG_FI_CHAT_REGION) + msgChatRegion->MessageLength))
	{
		// current field server socketŔĚ NULLŔĚ¸é, Á˘ĽÓŔ» ˛÷´Â´Ů.
		char	szSystemLog[1024];
		sprintf(szSystemLog, "  Fatal Error: CurrentFieldServerSocket is NULL, %s at %s\r\n",
			GetCharacterString(&m_character, string()), GetProtocolTypeString(T_IC_CHAT_REGION));
		DBGOUT(szSystemLog);
		g_pGlobal->WriteSystemLog(szSystemLog);

		return RES_RETURN_FALSE;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// ProcessResult Process_IC_CHAT_GUILD(const char* pPacket, int nLength, int &nBytesUsed)
//	:
// 2007-02-21 by cmkwon, ŔÎŔÚĂß°ˇ(SThreadInfo *i_pThreadInfo)
ProcessResult CIMIOCPSocket::Process_IC_CHAT_GUILD(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
{
	int					nRecvTypeSize;
	MSG_IC_CHAT_GUILD	*pRecvChatGuild;
	char				*chatString;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_GUILD) + ((MSG_IC_CHAT_GUILD*)(pPacket+nBytesUsed))->MessageLength;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_GUILD, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1200C);
		return RES_RETURN_FALSE;
	}
	pRecvChatGuild = (MSG_IC_CHAT_GUILD*)(pPacket+nBytesUsed);
	chatString = (char*)(pPacket+nBytesUsed+sizeof(MSG_IC_CHAT_GUILD));
	nBytesUsed += nRecvTypeSize;

	if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
	{
		SendErrorMessage(T_IC_CHAT_GUILD, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}

	if (strnicmp(m_character.CharacterName, pRecvChatGuild->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_GUILD, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		Close(0x1200D);
		return RES_RETURN_FALSE;
	}

	int nRemainMinute = 0;
	if(
		ms_pIMIOCP->m_chatBlockManager.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName) || //01-04-2017 by Inetpub - guild chat restricted after mute
		ms_pIMIOCP->m_chatBlockManagerByLeader.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName))	// 2008-12-30 by cmkwon, ÁöµµŔÚ Ă¤ĆĂ Á¦ÇŃ Ä«µĺ ±¸Çö - T_IC_CHAT_GUILD, ż©´Ü
	{
		SendErrorMessage(T_IC_CHAT_GUILD, ERR_CHAT_CHAT_BLOCKED, nRemainMinute);
		return RES_BREAK;
	}

	// Ă¤ĆĂ ÇĘĹÍ¸µ
	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_GUILD, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	char szChatting[1024];
	sprintf(szChatting, "Chat_GUILD|%s|%d|%d|%s\r\n",
			pRecvChatGuild->FromCharacterName, m_character.MapChannelIndex.MapIndex,
			m_character.MapChannelIndex.ChannelIndex, (char*)pRecvChatGuild + sizeof(MSG_IC_CHAT_GUILD));
	//////////////////////////////////////////////////////////////////////////
	// 2009-03-03 by cmkwon, ľĆ·ˇżÍ °°ŔĚ Gameforge4D ¸đµç Ľ­şń˝ş´Â ±Ó¸», ż©´Ü, Ćí´ë Ă¤ĆĂ ·Î±×¸¦ ł˛±âÁö ľĘ´Â´Ů.
	// 2009-01-06 by dhjin, Gameforge4D żäĂ»Ŕ¸·Î żµ±ą, µ¶ŔĎŔş ±Ó¸», ż©´Ü, Ćí´ë Ă¤ĆĂ ·Î±×¸¦ ł˛±âÁö ľĘ´Â´Ů.
	// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
//	g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);
#if !defined(SERVICE_TYPE_ENGLISH_SERVER_2) && !defined(SERVICE_TYPE_GERMAN_SERVER_1) && !defined(SERVICE_TYPE_TURKISH_SERVER_1) && !defined(SERVICE_TYPE_ITALIAN_SERVER_1) && !defined(SERVICE_TYPE_FRENCH_SERVER_1)
	g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);
#endif // END - SERVICE_TYPE_ENGLISH_SERVER_2
#ifdef _INET_AT_ALL_CHATS
	INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
	msgATChat->PrintType = 0x03;
	sprintf(szChatting, "BrigID: %d [%s]: %s", m_uidGuildUniqueNumber, pRecvChatGuild->FromCharacterName, chatString);
	STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
	msgATChat->SecurityNumber = m_character.InfluenceType;
	ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif

// 2007-01-31 by cmkwon, ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ÇÔ
//	ms_pIMIOCP->m_MapGuild.lock();
//	{
//		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
//		if(ptmGuild)
//		{
//			ptmGuild->SendChatMsgToGuildMembers(((BYTE*)pRecvChatGuild) - SIZE_FIELD_TYPE_HEADER
//				, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize, CHAT_TYPE_GUILD);
//		}
//	}
//	ms_pIMIOCP->m_MapGuild.unlock();
	
	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	vectCIMIOCPSocketPtr vectIISockList;
	vectIISockList.reserve(SIZE_MAX_GUILD_CAPACITY);			// 2008-05-27 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ż©´Üżř Áő°ˇ Äł˝¬ ľĆŔĚĹŰ
	
	mt_auto_lock mtAG(&ms_pIMIOCP->m_MapGuild);
	CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
	if(NULL == ptmGuild)
	{
		return RES_BREAK;
	}
	ptmGuild->GetGuildMemberIISocket(&vectIISockList);
	mtAG.auto_unlock_cancel();
	
	if(vectIISockList.empty())
	{
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}
	pRecvChatGuild->DonatorRank = m_character.PCBangTotalPlayTime;
	ms_pIMIOCP->SendChatMsg2MultiUser(&vectIISockList, ((BYTE*)pRecvChatGuild) - SIZE_FIELD_TYPE_HEADER
				, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize, CHAT_TYPE_GUILD);

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}
	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// ProcessResult Process_IC_CHAT_PARTY(const char* pPacket, int nLength, int &nBytesUsed)
//	:
// 2007-02-21 by cmkwon, ŔÎŔÚĂß°ˇ(SThreadInfo *i_pThreadInfo)
ProcessResult CIMIOCPSocket::Process_IC_CHAT_PARTY(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
{
	int					nRecvTypeSize;
	MSG_IC_CHAT_PARTY	*pRecvChatParty;
	char				*chatString;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_PARTY) + ((MSG_IC_CHAT_PARTY*)(pPacket+nBytesUsed))->MessageLength;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_PARTY, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12010);
		return RES_RETURN_FALSE;
	}
	pRecvChatParty = (MSG_IC_CHAT_PARTY*)(pPacket+nBytesUsed);
	chatString = (char*)(pPacket+nBytesUsed+sizeof(MSG_IC_CHAT_PARTY));
	nBytesUsed += nRecvTypeSize;

	if (strnicmp(m_character.CharacterName, pRecvChatParty->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_PARTY, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		Close(0x12011);
		return RES_RETURN_FALSE;
	}

	int nRemainMinute = 0;
	if (ms_pIMIOCP->m_chatBlockManager.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName) || //01-04-2017 by inetpub - formation chat restricted when muted
		ms_pIMIOCP->m_chatBlockManagerByLeader.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName))	// 2008-12-30 by cmkwon, ÁöµµŔÚ Ă¤ĆĂ Á¦ÇŃ Ä«µĺ ±¸Çö - T_IC_CHAT_PARTY, Ćí´ë
	{
		SendErrorMessage(T_IC_CHAT_PARTY, ERR_CHAT_CHAT_BLOCKED, nRemainMinute);
		return RES_BREAK;
	}

	// Ă¤ĆĂ ÇĘĹÍ¸µ
	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_PARTY, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	CIMParty *ptmParty = this->GetIMParty();
	if (NULL == ptmParty)
	{
		SendErrorMessage(T_IC_CHAT_PARTY, ERR_PROTOCOL_NOT_HAVE_PARTY);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	ptmParty->LockIMParty();
	{
		if(i_pThreadInfo)
		{// 2007-02-21 by cmkwon
			i_pThreadInfo->nParam1	= 10;
		}
		char szChatting[1024];
		sprintf(szChatting, "Chat_Party|%s|%d|%d|%s\r\n",
				pRecvChatParty->FromCharacterName, m_character.MapChannelIndex.MapIndex,
				m_character.MapChannelIndex.ChannelIndex, (char*)pRecvChatParty + sizeof(MSG_IC_CHAT_PARTY));
		//////////////////////////////////////////////////////////////////////////
		// 2009-03-03 by cmkwon, ľĆ·ˇżÍ °°ŔĚ Gameforge4D ¸đµç Ľ­şń˝ş´Â ±Ó¸», ż©´Ü, Ćí´ë Ă¤ĆĂ ·Î±×¸¦ ł˛±âÁö ľĘ´Â´Ů.
		// 2009-01-06 by dhjin, Gameforge4D żäĂ»Ŕ¸·Î żµ±ą, µ¶ŔĎŔş ±Ó¸», ż©´Ü, Ćí´ë Ă¤ĆĂ ·Î±×¸¦ ł˛±âÁö ľĘ´Â´Ů.
		// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
//		g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);
#if !defined(SERVICE_TYPE_ENGLISH_SERVER_2) && !defined(SERVICE_TYPE_GERMAN_SERVER_1) && !defined(SERVICE_TYPE_TURKISH_SERVER_1) && !defined(SERVICE_TYPE_ITALIAN_SERVER_1) && !defined(SERVICE_TYPE_FRENCH_SERVER_1)
		g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);
#endif // END - SERVICE_TYPE_ENGLISH_SERVER_2
#ifdef _INET_AT_ALL_CHATS
		INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
		msgATChat->PrintType = 0x04;
		sprintf(szChatting, "[%s]: %s", pRecvChatParty->FromCharacterName, chatString);
		STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
		msgATChat->SecurityNumber = m_character.InfluenceType;
		ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif
		if(i_pThreadInfo)
		{// 2007-02-21 by cmkwon
			i_pThreadInfo->nParam1++;
		}
		pRecvChatParty->DonatorRank = m_character.PCBangTotalPlayTime;
		ptmParty->SendChatMsgToMembers(((BYTE*)pRecvChatParty) - SIZE_FIELD_TYPE_HEADER
			, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize, CHAT_TYPE_PARTY);

		if(i_pThreadInfo)
		{// 2007-02-21 by cmkwon
			i_pThreadInfo->nParam1++;
		}
	}
	ptmParty->UnlockIMParty();

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1	= 20;
	}

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// ProcessResult Process_IC_CHAT_PTOP(const char* pPacket, int nLength, int &nBytesUsed)
//	:
// 2007-02-21 by cmkwon, ŔÎŔÚĂß°ˇ(SThreadInfo *i_pThreadInfo)
ProcessResult CIMIOCPSocket::Process_IC_CHAT_PTOP(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
{
	int					nRecvTypeSize;
	MSG_IC_CHAT_PTOP	*pRecvChatPTOP;
	char				*chatString;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_PTOP) + ((MSG_IC_CHAT_PTOP*)(pPacket+nBytesUsed))->MessageLength;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_PTOP, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12012);
		return RES_RETURN_FALSE;
	}
	pRecvChatPTOP = (MSG_IC_CHAT_PTOP*)(pPacket+nBytesUsed);
	chatString = (char*)(pPacket+nBytesUsed+sizeof(MSG_IC_CHAT_PTOP));
	nBytesUsed += nRecvTypeSize;

	// 2009-01-23 by cmkwon, ĹÍĹ° i,I ą®ŔÚ ą®Á¦ Ăł¸® - T_IC_CHAT_PTOP
	LowercaseOnly_i_forTurkish(pRecvChatPTOP->FromCharacterName, SIZE_MAX_CHARACTER_NAME);
	LowercaseOnly_i_forTurkish(pRecvChatPTOP->ToCharacterName, SIZE_MAX_CHARACTER_NAME);

	if (strnicmp(m_character.CharacterName, pRecvChatPTOP->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_PTOP, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		Close(0x12013);
		return RES_RETURN_FALSE;
	}

	int nRemainMinute = 0;
	if (ms_pIMIOCP->m_chatBlockManager.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName) || //01-04-2017 by Inetpub - whiseperchat restricted while mute
		ms_pIMIOCP->m_chatBlockManagerByLeader.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName))	// 2008-12-30 by cmkwon, ÁöµµŔÚ Ă¤ĆĂ Á¦ÇŃ Ä«µĺ ±¸Çö - T_IC_CHAT_PTOP, ±Ó¸»
	{
		SendErrorMessage(T_IC_CHAT_PTOP, ERR_CHAT_CHAT_BLOCKED, nRemainMinute);
		return RES_BREAK;
	}

	if(1 >= pRecvChatPTOP->MessageLength)
	{// 2006-09-27 by cmkwon, Ă¤ĆĂ ł»żëŔĚ ľř´Ů
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	///////////////////////////////////////////////////////////////////////////////
	// »ó´ëŔÇ ±ÓĽÓ¸» °ĹşÎ ż©şÎ Č®ŔÎ
	CIMIOCPSocket *pToSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(pRecvChatPTOP->ToCharacterName);
	if (NULL == pToSocket
		|| FALSE == pToSocket->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHAT_PTOP, ERR_COMMON_NO_SUCH_CHARACTER, 0, 0, pRecvChatPTOP->ToCharacterName);
		return RES_BREAK;
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2007-11-19 by cmkwon, ÁřÁ¤˝Ă˝şĹŰ ľ÷µĄŔĚĆ® - ş¸ł˝Äł¸ŻĹÍŔÇ ĽĽ·ÂŔ» ĽłÁ¤ ÇŘÁŘ´Ů
	pRecvChatPTOP->FromInflTy	= m_character.InfluenceType;

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

#ifdef _INET_WHISPER_WHEN_INTERNATIONAL	
	if (ms_pIMIOCP->m_bINETTradeChatCmdState == FALSE)
	{
		if (pRecvChatPTOP->FromInflTy != pToSocket->GetCharacter()->InfluenceType)
			SendString128(STRING_128_USER_NOTICE, "\\yThe cross nation whisper chat is \\rdisabled\\y at this moment. Try again later.\\y");
	}
#endif
	///////////////////////////////////////////////////////////////////////////////
	// 2005-12-07 by cmkwon, ĽĽ·ÂĂĽĹ©
	if(FALSE == GCheckRaceAndInfluenceType(CHECK_TYPE_CHAT_PTOP, m_character.Race, m_character.InfluenceType,
		pToSocket->GetCharacter()->Race, pToSocket->GetCharacter()->InfluenceType))
	{
#ifdef _INET_WHISPER_WHEN_INTERNATIONAL	
		if (ms_pIMIOCP->m_bINETTradeChatCmdState == FALSE)
		{
#endif
			if (FALSE == COMPARE_RACE(m_character.Race, RACE_INFLUENCE_LEADER)
				|| FALSE == COMPARE_RACE(pToSocket->GetCharacter()->Race, RACE_INFLUENCE_LEADER))
			{// 2006-04-20 by cmkwon, µŃ´Ů ĽĽ·ÂÁöµµŔÚŔĚ¸é 1:1 Ă¤ĆĂ °ˇ´É
				SendErrorMessage(T_IC_CHAT_PTOP, ERR_PROTOCOL_REQ_INFLUENCE_NOT_MATCHED);
				return RES_BREAK;
			}
#ifdef _INET_WHISPER_WHEN_INTERNATIONAL	
		}
#endif
	}

	// 2008-05-15 by dhjin, EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć
//	if (FALSE == COMPARE_CHATTYPE_BIT(pToSocket->m_enableChatFlag8, CHAT_TYPE_PTOP))
	if (FALSE == COMPARE_CHATTYPE_BIT(pToSocket->m_enableChatFlag16, CHAT_TYPE_PTOP))
	{
		if(COMPARE_RACE(pToSocket->GetCharacter()->Race, RACE_OPERATION|RACE_GAMEMASTER))
		{// 2006-11-14 by cmkwon, °ü¸®ŔÚ,żîżµŔÚ´Â ľËĽö ľřµµ·Ď ľĆ·ˇŔÇ żˇ·Ż¸Ţ˝ĂÁö¸¦ ŔüĽŰÇŃ´Ů.
			SendErrorMessage(T_IC_CHAT_PTOP, ERR_COMMON_NO_SUCH_CHARACTER, 0, 0, pRecvChatPTOP->ToCharacterName);
		}
		else
		{
			SendErrorMessage(T_IC_CHAT_PTOP, ERR_CHAT_CHAT_NOT_TRANSFERED);
		}
		return RES_BREAK;
	}
	
	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	if(pToSocket->IsCharacterNameFromRejectList(this->m_character.CharacterName))
	{// »ó´ëąć °ĹşÎ¸®˝şĆ®żˇ µî·ĎµÇľî ŔÖŔ˝

		SendErrorMessage(T_IC_CHAT_PTOP, ERR_FRIEND_REGISTERED_PEER_REJECTLIST);	
		return RES_BREAK;
	}

	// Ă¤ĆĂ ÇĘĹÍ¸µ
	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_PTOP, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	char szChatting[1024];
	sprintf(szChatting, "Chat_PtoP|%s, %s|%d|%d|%s\r\n",
			pRecvChatPTOP->FromCharacterName, pRecvChatPTOP->ToCharacterName,
			m_character.MapChannelIndex.MapIndex, m_character.MapChannelIndex.ChannelIndex,
			(char*)pRecvChatPTOP + sizeof(MSG_IC_CHAT_PTOP));
	// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
	USHORT usRace = m_character.Race | pToSocket->m_character.Race;	// 2007-11-13 by cmkwon, 1:1 Ă¤ĆĂŔş µŃÁßżˇ ÇŃ¸íŔĚ °ü¸®ŔÚ Č¤Ŕş żîżµŔÚŔĚ¸é Ă¤ĆĂ·Î±× ŔúŔĺ
	//////////////////////////////////////////////////////////////////////////
	// 2009-03-03 by cmkwon, ľĆ·ˇżÍ °°ŔĚ Gameforge4D ¸đµç Ľ­şń˝ş´Â ±Ó¸», ż©´Ü, Ćí´ë Ă¤ĆĂ ·Î±×¸¦ ł˛±âÁö ľĘ´Â´Ů.
	// 2009-01-06 by dhjin, Gameforge4D żäĂ»Ŕ¸·Î żµ±ą, µ¶ŔĎŔş ±Ó¸», ż©´Ü, Ćí´ë Ă¤ĆĂ ·Î±×¸¦ ł˛±âÁö ľĘ´Â´Ů.
	// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
//	g_pIMGlobal->WriteChattingLog(szChatting, usRace);
#if !defined(SERVICE_TYPE_ENGLISH_SERVER_2) && !defined(SERVICE_TYPE_GERMAN_SERVER_1) && !defined(SERVICE_TYPE_TURKISH_SERVER_1) && !defined(SERVICE_TYPE_ITALIAN_SERVER_1) && !defined(SERVICE_TYPE_FRENCH_SERVER_1)
	g_pIMGlobal->WriteChattingLog(szChatting, usRace);
#endif // END - SERVICE_TYPE_ENGLISH_SERVER_2

#ifdef _INET_AT_ALL_CHATS
	INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
	msgATChat->PrintType = 0x05;
	sprintf(szChatting, "[%s <> %s]: %s", pRecvChatPTOP->FromCharacterName, pRecvChatPTOP->ToCharacterName, chatString);
	STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
	msgATChat->SecurityNumber = m_character.InfluenceType;
	ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif
	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}
	pRecvChatPTOP->DonatorRank = m_character.PCBangTotalPlayTime;
	pToSocket->SendAddData(((BYTE*)pRecvChatPTOP) - SIZE_FIELD_TYPE_HEADER, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize);

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	SendAddData(((BYTE*)pRecvChatPTOP) - SIZE_FIELD_TYPE_HEADER, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize);

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_CHAT_CHANGE_CHAT_FLAG(const char* pPacket, int nLength, int &nBytesUsed)
{
	int						nRecvTypeSize;
	MSG_IC_CHAT_CHANGE_CHAT_FLAG	*pRecvChatOnOff;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_CHANGE_CHAT_FLAG);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_CHANGE_CHAT_FLAG, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12012);
		return RES_RETURN_FALSE;
	}
	pRecvChatOnOff = (MSG_IC_CHAT_CHANGE_CHAT_FLAG*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	SET_CHATTYPE_BIT(pRecvChatOnOff->bitChatType, CHAT_TYPE_UNCONDITIONAL);		// 
// 2008-05-15 by dhjin, EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć
//	m_enableChatFlag8			= pRecvChatOnOff->bitChatType;					//
	m_enableChatFlag16			= pRecvChatOnOff->bitChatType;					//

	this->SendToFieldServerChatBitFlag();
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_CHAT_MULTI(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
{
	int						nRecvTypeSize;
	MSG_IC_CHAT_MULTI	*pRecvChatAll;
	char					*chatString;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_MULTI) + ((MSG_IC_CHAT_MULTI*)(pPacket + nBytesUsed))->MessageLength;
	if (nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_CHAT_MULTI, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12005);
		return RES_RETURN_FALSE;
	}
	pRecvChatAll = (MSG_IC_CHAT_MULTI*)(pPacket + nBytesUsed);
	chatString = (char*)(pPacket + nBytesUsed + sizeof(MSG_IC_CHAT_MULTI));
	nBytesUsed += nRecvTypeSize;
	//g_pGlobal->WriteSystemLog("Process_IC_CHAT_MULTI");
	if (strnicmp(m_character.CharacterName, pRecvChatAll->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_MULTI, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		Close(0x12006);
		return RES_RETURN_FALSE;
	}

	if (i_pThreadInfo)
	{
		i_pThreadInfo->nParam1++;
	}
//#ifdef COMMAND_MULTI_CHAT_SILVER
	if (ms_pIMIOCP->m_bMultiChatStatues == FALSE)
	{
		SendString128(STRING_128_USER_NOTICE, "\\yThe multi chat is disabled for the moment.\\y");
		return RES_BREAK;
	}
//#endif // COMMAND_MULTI_CHAT_SILVER

	int nRemainMinute = 0;
	if (ms_pIMIOCP->m_chatBlockManager.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName)
		|| ms_pIMIOCP->m_chatBlockManagerByLeader.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName))
	{
		SendErrorMessage(T_IC_CHAT_MULTI, ERR_CHAT_CHAT_BLOCKED, nRemainMinute);
		return RES_BREAK;
	}

	if (FALSE == COMPARE_INFLUENCE(m_character.InfluenceType, INFLUENCE_TYPE_VCN | INFLUENCE_TYPE_ANI))
	{
		SendErrorMessage(T_IC_CHAT_MULTI, ERR_REQ_CHOOSE_INFLUENCE_TYPE);
		return RES_BREAK;
	}

	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_MULTI, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	if (i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	char szChatting[1024];
	sprintf(szChatting, "Chat_Multi|%s|%d|%d|%s\r\n",
		pRecvChatAll->FromCharacterName, m_character.MapChannelIndex.MapIndex,
		m_character.MapChannelIndex.ChannelIndex, (char*)pRecvChatAll + sizeof(MSG_IC_CHAT_MULTI));
	g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);

	///////////////////////////////////////////////////////////////////////////////
	// 2005-12-07 by cmkwon, 거래는 동일한 세력에게만 전송한다
#ifdef _INET_AT_ALL_CHATS
	INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
	msgATChat->PrintType = 0x06;
	sprintf(szChatting, "[%s]: %s", pRecvChatAll->FromCharacterName, chatString);
	STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
	msgATChat->SecurityNumber = m_character.InfluenceType;
	ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif
	pRecvChatAll->DonatorRank = m_character.PCBangTotalPlayTime;
	if (COMPARE_RACE(m_character.Race, RACE_OPERATION | RACE_GAMEMASTER))
	{
		ms_pIMIOCP->SendChatMsgToAllClients(CHAT_TYPE_MULTI, ((BYTE*)pRecvChatAll) - SIZE_FIELD_TYPE_HEADER
			, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize,/* m_character.InfluenceType = */INFLUENCE_TYPE_ALL_MASK); // 2015-02-11 by silver chat multi
	}
	else
	{
		ms_pIMIOCP->SendChatMsgToAllClients(CHAT_TYPE_MULTI, ((BYTE*)pRecvChatAll) - SIZE_FIELD_TYPE_HEADER
			, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize, INFLUENCE_TYPE_ALL_MASK); // 2015-02-11 by silver chat multi
	}
	if (i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}
	return RES_RETURN_TRUE;
}

// 2007-02-21 by cmkwon, ŔÎŔÚ Ăß°ˇ(SThreadInfo *i_pThreadInfo)
ProcessResult CIMIOCPSocket::Process_IC_CHAT_SELL_ALL(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
{
	int						nRecvTypeSize;
	MSG_IC_CHAT_SELL_ALL	*pRecvChatAll;
	char					*chatString;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_SELL_ALL) + ((MSG_IC_CHAT_SELL_ALL*)(pPacket+nBytesUsed))->MessageLength;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_SELL_ALL, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12005);
		return RES_RETURN_FALSE;
	}
	pRecvChatAll = (MSG_IC_CHAT_SELL_ALL*)(pPacket+nBytesUsed);
	chatString = (char*)(pPacket+nBytesUsed+sizeof(MSG_IC_CHAT_SELL_ALL));
	nBytesUsed += nRecvTypeSize;

	if (strnicmp(m_character.CharacterName, pRecvChatAll->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_SELL_ALL, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		Close(0x12006);
		return RES_RETURN_FALSE;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	int nRemainMinute = 0;
	if(ms_pIMIOCP->m_chatBlockManager.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName)
		|| ms_pIMIOCP->m_chatBlockManagerByLeader.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName))	// 2008-12-30 by cmkwon, ÁöµµŔÚ Ă¤ĆĂ Á¦ÇŃ Ä«µĺ ±¸Çö - T_IC_CHAT_SELL_ALL, ¸Ĺ¸Ĺ
	{
		SendErrorMessage(T_IC_CHAT_SELL_ALL, ERR_CHAT_CHAT_BLOCKED, nRemainMinute);
		return RES_BREAK;
	}

	if(FALSE == COMPARE_INFLUENCE(m_character.InfluenceType, INFLUENCE_TYPE_VCN|INFLUENCE_TYPE_ANI))
	{// 2006-06-13 by cmkwon, ĽĽ·Â Ľ±ĹĂ ŔŻŔú¸¸ŔĚ °Ĺ·ˇ Ă¤ĆĂ °ˇ´É
		SendErrorMessage(T_IC_CHAT_SELL_ALL, ERR_REQ_CHOOSE_INFLUENCE_TYPE);
		return RES_BREAK;
	}

	// 2008-05-19 by dhjin, EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć, ŔüŔď Ă¤ĆĂ - °Ĺ·ˇ Ă¤ĆĂŔş ĽĽ·ÂŔ¸·Î¸¸ ĂĽĹ© ÇŃ´Ů.
//	if(FALSE == GCheckLimitLevel(CHECK_TYPE_CHAT_SELL_ALL, m_character.Level))
//	{
//		SendErrorMessage(T_IC_CHAT_SELL_ALL, ERR_PROTOCOL_REQ_LEVEL_NOT_MATCHED);
//		return RES_BREAK;
//	}

	// Ă¤ĆĂ ÇĘĹÍ¸µ
	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_SELL_ALL, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	char szChatting[1024];
	sprintf(szChatting, "Chat_Sell_All|%s|%d|%d|%s\r\n",
			pRecvChatAll->FromCharacterName, m_character.MapChannelIndex.MapIndex,
			m_character.MapChannelIndex.ChannelIndex, (char*)pRecvChatAll + sizeof(MSG_IC_CHAT_SELL_ALL));
	// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
	g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);
#ifdef _INET_AT_ALL_CHATS
	INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
	msgATChat->PrintType = 0x07;
	sprintf(szChatting, "[%s]: %s", pRecvChatAll->FromCharacterName, chatString);
	STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
	msgATChat->SecurityNumber = m_character.InfluenceType;
	ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif
	///////////////////////////////////////////////////////////////////////////////
	// 2005-12-07 by cmkwon, °Ĺ·ˇ´Â µżŔĎÇŃ ĽĽ·Âżˇ°Ô¸¸ ŔüĽŰÇŃ´Ů
	pRecvChatAll->DonatorRank = m_character.PCBangTotalPlayTime;
#ifdef _INET_TRADE_INTERNATIONAL
	if (ms_pIMIOCP->m_bINETTradeChatCmdState == FALSE)
	{
		pRecvChatAll->InfluenceType = NULL;
#endif	
		ms_pIMIOCP->SendChatMsgToAllClients(CHAT_TYPE_SELL_ALL, ((BYTE*)pRecvChatAll) - SIZE_FIELD_TYPE_HEADER
			, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize, m_character.InfluenceType);
#ifdef _INET_TRADE_INTERNATIONAL
	}
	else
	{
		if (COMPARE_RACE(m_character.Race, RACE_OPERATION | RACE_GAMEMASTER))
		{
			ms_pIMIOCP->SendChatMsgToAllClients(CHAT_TYPE_SELL_ALL, ((BYTE*)pRecvChatAll) - SIZE_FIELD_TYPE_HEADER
				, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize, INFLUENCE_TYPE_ALL_MASK);
		}
		else
		{
			ms_pIMIOCP->SendChatMsgToAllClients(CHAT_TYPE_SELL_ALL, ((BYTE*)pRecvChatAll) - SIZE_FIELD_TYPE_HEADER
				, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize, INFLUENCE_TYPE_ALL_MASK);
			/*if (nINETSendedCount == 0) //fresh init
			{
				nINETSendedCount++;
				INETserverTime = INETserverTime.GetCurrentDateTime();
				ms_pIMIOCP->SendChatMsgToAllClients(CHAT_TYPE_SELL_ALL, ((BYTE*)pRecvChatAll) - SIZE_FIELD_TYPE_HEADER
					, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize, INFLUENCE_TYPE_ALL_MASK);
			}
			else
			{
				ATUM_DATE_TIME tempTime = tempTime.GetCurrentDateTime();
				tempTime.GetCurrentDateTime();

				if (tempTime.GetTimeDiffTimeInSeconds(INETserverTime) >= 120)
				{
					tempTime.Reset();
					INETserverTime.Reset();
					ms_pIMIOCP->SendChatMsgToAllClients(CHAT_TYPE_SELL_ALL, ((BYTE*)pRecvChatAll) - SIZE_FIELD_TYPE_HEADER
						, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize, INFLUENCE_TYPE_ALL_MASK);
				}
				else
				{
					//wait message
					char szTemp1[1024];
					int secLeft = 120 - tempTime.GetTimeDiffTimeInSeconds(INETserverTime);
					if (secLeft > 0)
						sprintf(szTemp1, "\\rYou have to wait \\e%d\\r seconds more to send next message !!!\\r", secLeft);
					SendString128(STRING_128_USER_NOTICE, szTemp1);
				}
			}*/
		}
	}
#endif
	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}
	return RES_RETURN_TRUE;
}

// 2007-02-21 by cmkwon, ŔÎŔÚĂß°ˇ(SThreadInfo *i_pThreadInfo)
ProcessResult CIMIOCPSocket::Process_IC_CHAT_CASH_ALL(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
{
	int						nRecvTypeSize;
	MSG_IC_CHAT_CASH_ALL	*pRecvChatMap;
	char					*chatString;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_CASH_ALL) + ((MSG_IC_CHAT_CASH_ALL*)(pPacket+nBytesUsed))->MessageLength;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_CASH_ALL, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12007);
		return RES_RETURN_FALSE;
	}
	pRecvChatMap = (MSG_IC_CHAT_CASH_ALL*)(pPacket+nBytesUsed);
	chatString = (char*)(pPacket+nBytesUsed+sizeof(MSG_IC_CHAT_CASH_ALL));
	nBytesUsed += nRecvTypeSize;

	if (strnicmp(m_character.CharacterName, pRecvChatMap->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_MAP, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		Close(0x12008);
		return RES_RETURN_FALSE;
	}

	// Ă¤ĆĂ ÇĘĹÍ¸µ
	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_MAP, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	char szChatting[1024];
	sprintf(szChatting, "Chat_Cash_All|%s|%d|%d|%s\r\n",
			pRecvChatMap->FromCharacterName, m_character.MapChannelIndex.MapIndex,
			m_character.MapChannelIndex.ChannelIndex, (char*)pRecvChatMap + sizeof(MSG_IC_CHAT_MAP));
	// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
	g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);
#ifdef _INET_AT_ALL_CHATS
	INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
	msgATChat->PrintType = 0x08;
	sprintf(szChatting, "[%s]: %s", pRecvChatMap->FromCharacterName, chatString);
	STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
	msgATChat->SecurityNumber = m_character.InfluenceType;
	ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif
	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	INIT_MSG_WITH_BUFFER(MSG_FI_CHAT_CASH_ALL, T_FI_CHAT_CASH_ALL, msgChatMap, SendBuf);
	msgChatMap->CharacterUniqueNumber	= m_character.CharacterUniqueNumber;
	msgChatMap->MessageLength			= pRecvChatMap->MessageLength;
	STRNCPY_MEMSET((char*)SendBuf + MSG_SIZE(MSG_FI_CHAT_CASH_ALL), chatString, SIZE_MAX_CHAT_MESSAGE);
	this->SendMsgToCurrentFieldServerSocket(SendBuf, MSG_SIZE(MSG_FI_CHAT_CASH_ALL) + msgChatMap->MessageLength);
	
	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	return RES_RETURN_TRUE;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHAT_INFLUENCE_ALL(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		// 2007-02-21 by cmkwon, ŔÎŔÚĂß°ˇ(SThreadInfo *i_pThreadInfo)
/// \author		cmkwon
/// \date		2006-04-21 ~ 2006-04-21
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHAT_INFLUENCE_ALL(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
{
	int						nRecvTypeSize;
	char					*chatString;
	MSG_IC_CHAT_INFLUENCE_ALL	*pRInflAll;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_INFLUENCE_ALL) + ((MSG_IC_CHAT_INFLUENCE_ALL*)(pPacket+nBytesUsed))->MessageLength;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_INFLUENCE_ALL, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12007);
		return RES_RETURN_FALSE;
	}
	pRInflAll = (MSG_IC_CHAT_INFLUENCE_ALL*)(pPacket+nBytesUsed);
	chatString = (char*)(pPacket+nBytesUsed+sizeof(MSG_IC_CHAT_INFLUENCE_ALL));
	nBytesUsed += nRecvTypeSize;

	if (strnicmp(m_character.CharacterName, pRInflAll->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_INFLUENCE_ALL, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		return RES_BREAK;
	}

	if(FALSE == COMPARE_INFLUENCE(this->m_character.InfluenceType, INFLUENCE_TYPE_VCN|INFLUENCE_TYPE_ANI))
	{// 2007-10-06 by cmkwon, ±âş» ĽĽ·Â ĂĽĹ©
		SendErrorMessage(T_IC_CHAT_INFLUENCE_ALL, ERR_REQ_CHOOSE_INFLUENCE_TYPE);
		return RES_BREAK;
	}

	if(FALSE == COMPARE_RACE(m_character.Race, RACE_INFLUENCE_LEADER|RACE_INFLUENCE_SUBLEADER_MASK))
	{// 2007-10-06 by cmkwon, ŔüÁř ±âÁö ĽŇŔŻ ż©´ÜŔĺ ĽĽ·Â Ă¤ĆĂ

		if(FALSE == IS_VALID_UNIQUE_NUMBER(this->GetCharacter()->GuildUniqueNumber))
		{// 2007-10-06 by cmkwon, ż©´Ü °ˇŔÔ ŔŻą« ĂĽĹ©
			SendErrorMessage(T_IC_CHAT_INFLUENCE_ALL, ERR_PROTOCOL_NOT_HAVE_GUILD);
			return RES_BREAK;
		}

		if(FALSE == ms_pIMIOCP->IsGuildCommanderW(this->GetCharacter()->GuildUniqueNumber, this->GetCharacter()->CharacterUniqueNumber))
		{// 2007-10-06 by cmkwon, ż©´ÜŔĺŔ» ĂĽĹ©
			SendErrorMessage(T_IC_CHAT_INFLUENCE_ALL, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
			return RES_BREAK;
		}
	}

//	if(FALSE == COMPARE_RACE(m_character.Race, RACE_INFLUENCE_LEADER))
//	{
//		SendErrorMessage(T_IC_CHAT_INFLUENCE_ALL, ERR_CHAT_PERMISSION_DENIED);
//		return RES_BREAK;
//	}

// 2007-10-06 by cmkwon, ŔüÁř ±âÁö ĽŇŔŻÇŃ ż©´ÜŔĺ ĽĽ·Â Ă¤ĆĂ °ˇ´É - ÁöµµŔÚ/şÎÁöµµŔÚ°ˇ ľĆ´Ď¸é Ă¤ĆĂŔ» FieldServer·Î ŔüĽŰÇĎż© FieldServerżˇĽ­ ŔüÁř±âÁö ĽŇŔŻ ż©´ÜŔĺŔ» ĂĽĹ©ÇŃ´Ů.
//	// 2006-12-08 by dhjin, ÁöµµŔÚ, şÎÁöµµŔÚ´Â °°Ŕş ĽĽ·Â ŔüĂĽ °řÁöĂ¤ĆĂ°ˇ´É
//	// 2007-10-06 by cmkwon, şÎÁöµµŔÚ 2¸íŔÇ ČŁÄŞŔ» ´Ů¸Ł°Ô ĽłÁ¤ -
//	//if(FALSE == COMPARE_RACE(m_character.Race, RACE_INFLUENCE_LEADER|RACE_INFLUENCE_SUBLEADER))
//	if(FALSE == COMPARE_RACE(m_character.Race, RACE_INFLUENCE_LEADER|RACE_INFLUENCE_SUBLEADER_MASK))
//	{
//		SendErrorMessage(T_IC_CHAT_INFLUENCE_ALL, ERR_CHAT_PERMISSION_DENIED);
//		return RES_BREAK;
//	}

	// Ă¤ĆĂ ÇĘĹÍ¸µ
	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_INFLUENCE_ALL, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2008-03-07 by cmkwon, ĽĽ·ÂĂ¤ĆĂ˝Ă Ă¤ĆĂ şí·° ĂĽĹ© Ăß°ˇ
	int nRemainMinute = 0;
	if(ms_pIMIOCP->m_chatBlockManager.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName))
	{
		SendErrorMessage(T_IC_CHAT_INFLUENCE_ALL, ERR_CHAT_CHAT_BLOCKED, nRemainMinute);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	char szChatting[1024];
	sprintf(szChatting, "Chat_Influence_All|%s|%d|%d|%s\r\n",
			pRInflAll->FromCharacterName, m_character.MapChannelIndex.MapIndex,
			m_character.MapChannelIndex.ChannelIndex, (char*)pRInflAll + sizeof(MSG_IC_CHAT_MAP));
	// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
	g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);
#ifdef _INET_AT_ALL_CHATS
	INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
	msgATChat->PrintType = 0x09;
	sprintf(szChatting, "[%s]: %s", pRInflAll->FromCharacterName, chatString);
	STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
	msgATChat->SecurityNumber = m_character.InfluenceType;
	ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif
	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	// 2007-10-06 by cmkwon, ŔüÁř ±âÁö ĽŇŔŻÇŃ ż©´ÜŔĺ ĽĽ·Â Ă¤ĆĂ °ˇ´É - ÁöµµŔÚ/şÎÁöµµŔÚ°ˇ ľĆ´Ď¸é Ă¤ĆĂŔ» FieldServer·Î ŔüĽŰÇĎż© FieldServerżˇĽ­ ŔüÁř±âÁö ĽŇŔŻ ż©´ÜŔĺŔ» ĂĽĹ©ÇŃ´Ů.
	if(FALSE == COMPARE_RACE(m_character.Race, RACE_INFLUENCE_LEADER|RACE_INFLUENCE_SUBLEADER_MASK))
	{
		INIT_MSG_WITH_BUFFER(MSG_FI_CHAT_OUTPOST_GUILD, T_FI_CHAT_OUTPOST_GUILD, pSOutpost, SendBuf);
		pSOutpost->CharacterUniqueNumber	= m_character.CharacterUniqueNumber;
		pSOutpost->MessageLength			= min(SIZE_MAX_CHAT_MESSAGE, pRInflAll->MessageLength);
		STRNCPY_MEMSET((char*)SendBuf + MSG_SIZE(MSG_FI_CHAT_OUTPOST_GUILD), chatString, SIZE_MAX_CHAT_MESSAGE);
		this->SendMsgToCurrentFieldServerSocket(SendBuf, MSG_SIZE(MSG_FI_CHAT_OUTPOST_GUILD)+pSOutpost->MessageLength);
	}
	else
	{
		INIT_MSG_WITH_BUFFER(MSG_IC_CHAT_INFLUENCE_ALL, T_IC_CHAT_INFLUENCE_ALL, pSInflAll, SendBuf);
		STRNCPY_MEMSET(pSInflAll->FromCharacterName, GetLeader8SubLeaderString(m_character.InfluenceType, m_character.Race), SIZE_MAX_CHARACTER_NAME);
		STRNCPY_MEMSET(pSInflAll->FromCharacterNameLead, m_character.CharacterName, SIZE_MAX_CHARACTER_NAME); //19-09-2023 by Inet - add lead name on hoover
	// 2007-10-06 by cmkwon, şÎÁöµµŔÚ 2¸íŔÇ ČŁÄŞŔ» ´Ů¸Ł°Ô ĽłÁ¤ -
	//// 2006-12-13 by cmkwon, ľĆ·ˇżÍ °°ŔĚ ŔüĽŰ Äł¸ŻĹÍ¸íŔ» ÁöµµŔÚ/şÎÁöµµŔÚ·Î ĽłÁ¤ÇŘĽ­ ŔüĽŰÇŃ´Ů.
	////	STRNCPY_MEMSET(pSInflAll->FromCharacterName, pRInflAll->FromCharacterName, SIZE_MAX_CHARACTER_NAME);
	//	if(COMPARE_RACE(m_character.Race, RACE_INFLUENCE_LEADER))
	//	{
	//		STRNCPY_MEMSET(pSInflAll->FromCharacterName, STRCMD_CS_COMMON_INFLUENCE_LEADER, SIZE_MAX_CHARACTER_NAME);
	//	}
	//	else
	//	{
	//		STRNCPY_MEMSET(pSInflAll->FromCharacterName, STRCMD_CS_COMMON_INFLUENCE_SUBLEADER, SIZE_MAX_CHARACTER_NAME);
	//	}
		pSInflAll->MessageLength	= min(SIZE_MAX_CHAT_MESSAGE, pRInflAll->MessageLength);
		pSInflAll->DonatorRank	= m_character.PCBangTotalPlayTime;
		STRNCPY_MEMSET((char*)SendBuf + MSG_SIZE(MSG_IC_CHAT_INFLUENCE_ALL), chatString, SIZE_MAX_CHAT_MESSAGE);
		ms_pIMIOCP->SendChatMsgToAllClients(CHAT_TYPE_INFLUENCE, SendBuf, MSG_SIZE(MSG_IC_CHAT_INFLUENCE_ALL)+pSInflAll->MessageLength, m_character.InfluenceType);
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}
	return RES_RETURN_TRUE;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHAT_ARENA(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
/// \brief		ľĆ·ąłŞ Ă¤ĆĂŔ» ąŢľĆ FieldServer·Î ş¸ł˝´Ů
/// \author		dhjin
/// \date		2007-05-02 ~ 2007-05-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHAT_ARENA(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
{
	int					nRecvTypeSize;
	MSG_IC_CHAT_REGION	*pRecvChatRegion;
	char				*chatString;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_ARENA) + ((MSG_IC_CHAT_ARENA*)(pPacket+nBytesUsed))->MessageLength;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_ARENA, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1200A);
		return RES_RETURN_FALSE;
	}
	pRecvChatRegion = (MSG_IC_CHAT_ARENA*)(pPacket+nBytesUsed);
	chatString = (char*)(pPacket+nBytesUsed+sizeof(MSG_IC_CHAT_ARENA));
	nBytesUsed += nRecvTypeSize;

	if (strnicmp(m_character.CharacterName, pRecvChatRegion->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_ARENA, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		Close(0x1200B);
		return RES_RETURN_FALSE;
	}

	// Ă¤ĆĂ ÇĘĹÍ¸µ
	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_ARENA, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	///////////////////////////////////////////////////////////////////////////////
	// Ă¤ĆĂ ·Î±× ł˛±â±â
	char szChatting[1024];
	sprintf(szChatting, "Chat_Arena|%s|%d|%d|%s\r\n",
			pRecvChatRegion->FromCharacterName, m_character.MapChannelIndex.MapIndex,
			m_character.MapChannelIndex.ChannelIndex, (char*)pRecvChatRegion + sizeof(MSG_IC_CHAT_ARENA));
	// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
	g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);
#ifdef _INET_AT_ALL_CHATS
	INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
	msgATChat->PrintType = 0x10;
	sprintf(szChatting, "[%s]: %s", pRecvChatRegion->FromCharacterName, chatString);
	STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
	msgATChat->SecurityNumber = m_character.InfluenceType;
	ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif
	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	int nRemainMinute = 0;
	if(ms_pIMIOCP->m_chatBlockManager.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName)
		|| ms_pIMIOCP->m_chatBlockManagerByLeader.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName))	// 2008-12-30 by cmkwon, ÁöµµŔÚ Ă¤ĆĂ Á¦ÇŃ Ä«µĺ ±¸Çö - T_IC_CHAT_ARENA, ľĆ·ąłŞ
	{
		SendErrorMessage(T_IC_CHAT_ARENA, ERR_CHAT_CHAT_BLOCKED, nRemainMinute);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	INIT_MSG_WITH_BUFFER(MSG_FI_CHAT_ARENA, T_FI_CHAT_ARENA, msgChatRegion, SendBuf);
	msgChatRegion->CharacterUniqueNumber = m_character.CharacterUniqueNumber;
	msgChatRegion->MessageLength = pRecvChatRegion->MessageLength;
	STRNCPY_MEMSET((char*)SendBuf + MSG_SIZE(MSG_FI_CHAT_ARENA), chatString, SIZE_MAX_CHAT_MESSAGE);

	if (FALSE == this->SendMsgToCurrentFieldServerSocket(SendBuf, MSG_SIZE(MSG_FI_CHAT_ARENA) + msgChatRegion->MessageLength))
	{
		// current field server socketŔĚ NULLŔĚ¸é, Á˘ĽÓŔ» ˛÷´Â´Ů.
		char	szSystemLog[1024];
		sprintf(szSystemLog, "  Fatal Error: CurrentFieldServerSocket is NULL, %s at %s\r\n",
			GetCharacterString(&m_character, string()), GetProtocolTypeString(T_IC_CHAT_ARENA));
		DBGOUT(szSystemLog);
		g_pGlobal->WriteSystemLog(szSystemLog);

		return RES_RETURN_FALSE;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	return RES_RETURN_TRUE;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHAT_WAR(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
/// \brief		EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć, ŔüŔď Ă¤ĆĂ
/// \author		dhjin
/// \date		2008-05-19 ~ 2008-05-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHAT_WAR(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
{
	int						nRecvTypeSize;
	MSG_IC_CHAT_WAR			*pRecvChatWar;
	char					*chatString;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_WAR) + ((MSG_IC_CHAT_WAR*)(pPacket+nBytesUsed))->MessageLength;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_CHAT_WAR, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12005);
		return RES_RETURN_FALSE;
	}
	pRecvChatWar = (MSG_IC_CHAT_WAR*)(pPacket+nBytesUsed);
	chatString = (char*)(pPacket+nBytesUsed+sizeof(MSG_IC_CHAT_WAR));
	nBytesUsed += nRecvTypeSize;

	if (strnicmp(m_character.CharacterName, pRecvChatWar->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_WAR, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		Close(0x12006);
		return RES_RETURN_FALSE;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	int nRemainMinute = 0;
	if(ms_pIMIOCP->m_chatBlockManager.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName)
		|| ms_pIMIOCP->m_chatBlockManagerByLeader.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName))	// 2008-12-30 by cmkwon, ÁöµµŔÚ Ă¤ĆĂ Á¦ÇŃ Ä«µĺ ±¸Çö - T_IC_CHAT_WAR, ŔüŔď
	{
		SendErrorMessage(T_IC_CHAT_WAR, ERR_CHAT_CHAT_BLOCKED, nRemainMinute);
		return RES_BREAK;
	}

	if(FALSE == COMPARE_INFLUENCE(m_character.InfluenceType, INFLUENCE_TYPE_VCN|INFLUENCE_TYPE_ANI))
	{// 2006-06-13 by cmkwon, ĽĽ·Â Ľ±ĹĂ ŔŻŔú¸¸ŔĚ °Ĺ·ˇ Ă¤ĆĂ °ˇ´É
		SendErrorMessage(T_IC_CHAT_WAR, ERR_REQ_CHOOSE_INFLUENCE_TYPE);
		return RES_BREAK;
	}

	if(FALSE == GCheckLimitLevel(CHECK_TYPE_CHAT_WAR, m_character.Level))
	{
		SendErrorMessage(T_IC_CHAT_WAR, ERR_PROTOCOL_REQ_LEVEL_NOT_MATCHED);
		return RES_BREAK;
	}

	// Ă¤ĆĂ ÇĘĹÍ¸µ
	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_WAR, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	char szChatting[1024];
	sprintf(szChatting, "Chat_War|%s|%d|%d|%s\r\n",
			pRecvChatWar->FromCharacterName, m_character.MapChannelIndex.MapIndex,
			m_character.MapChannelIndex.ChannelIndex, (char*)pRecvChatWar + sizeof(MSG_IC_CHAT_SELL_ALL));
	// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
	g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);
#ifdef _INET_AT_ALL_CHATS
	INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
	msgATChat->PrintType = 0x11;
	sprintf(szChatting, "[%s]: %s", pRecvChatWar->FromCharacterName, chatString);
	STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
	msgATChat->SecurityNumber = m_character.InfluenceType;
	ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif
	pRecvChatWar->DonatorRank = m_character.PCBangTotalPlayTime;
	///////////////////////////////////////////////////////////////////////////////
	// 2005-12-07 by cmkwon, °Ĺ·ˇ´Â µżŔĎÇŃ ĽĽ·Âżˇ°Ô¸¸ ŔüĽŰÇŃ´Ů
	ms_pIMIOCP->SendChatMsgToAllClients(CHAT_TYPE_WAR, ((BYTE*)pRecvChatWar) - SIZE_FIELD_TYPE_HEADER
		, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize, m_character.InfluenceType);

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}
	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHAT_CHATROOM(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
/// \brief		EP3 Ă¤ĆĂąć - 
/// \author		dhjin
/// \date		2008-06-18 ~ 2008-06-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHAT_CHATROOM(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
{
	int						nRecvTypeSize;
	MSG_IC_CHAT_CHATROOM	*pRecvChat;
	char					*chatString;

	nRecvTypeSize = sizeof(MSG_IC_CHAT_CHATROOM) + ((MSG_IC_CHAT_CHATROOM*)(pPacket+nBytesUsed))->MessageLength;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_CHATROOM, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12010);
		return RES_RETURN_FALSE;
	}
	pRecvChat = (MSG_IC_CHAT_CHATROOM*)(pPacket+nBytesUsed);
	chatString = (char*)(pPacket+nBytesUsed+sizeof(MSG_IC_CHAT_CHATROOM));
	nBytesUsed += nRecvTypeSize;

	if (strnicmp(m_character.CharacterName, pRecvChat->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_CHATROOM, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		Close(0x12011);
		return RES_RETURN_FALSE;
	}

	int nRemainMinute = 0;
	if (ms_pIMIOCP->m_chatBlockManager.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName) || //01-04-2017 by Inetpub - chatrom chat restricted when mute
		ms_pIMIOCP->m_chatBlockManagerByLeader.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName))	// 2008-12-30 by cmkwon, ÁöµµŔÚ Ă¤ĆĂ Á¦ÇŃ Ä«µĺ ±¸Çö - T_IC_CHAT_CHATROOM, Ă¤ĆĂąć
	{
		SendErrorMessage(T_IC_CHAT_CHATROOM, ERR_CHAT_CHAT_BLOCKED, nRemainMinute);
		return RES_BREAK;
	}

	// Ă¤ĆĂ ÇĘĹÍ¸µ
	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_CHATROOM, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	if(this->m_bChatRoomUseing)
	{
		mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));
		mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
		for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
		{
			if(this->m_nChatRoomNum == (*itr)->GetChatRoomNum())
			{
				char szChatting[1024];
				sprintf(szChatting, "Chat_ChatRoom|%s|%d|%d|%s\r\n",
						pRecvChat->FromCharacterName, m_character.MapChannelIndex.MapIndex,
						m_character.MapChannelIndex.ChannelIndex, (char*)pRecvChat + sizeof(MSG_IC_CHAT_CHATROOM));
				// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
				g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);
#ifdef _INET_AT_ALL_CHATS
				INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
				msgATChat->PrintType = 0x12;
				sprintf(szChatting, "[%s]: %s", pRecvChat->FromCharacterName, chatString);
				STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
				msgATChat->SecurityNumber = m_character.InfluenceType;
				ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif
				pRecvChat->DonatorRank = m_character.PCBangTotalPlayTime;
				(*itr)->SendChatMessagetoChatRoomMember(((BYTE*)pRecvChat) - SIZE_FIELD_TYPE_HEADER
					, SIZE_FIELD_TYPE_HEADER + nRecvTypeSize, CHAT_TYPE_CHATROOM);
				break;
			}
		}
	}
	else
	{
		SendErrorMessage(T_IC_CHAT_CHATROOM, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHAT_INFINITY(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo)
/// \brief		ŔÎÇÇ´ĎĆĽ - ŔÎÇÇ Ă¤ĆĂ
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHAT_INFINITY(const char* pPacket, int nLength, int &nBytesUsed, SThreadInfo *i_pThreadInfo) {
	int						nRecvTypeSize;
	MSG_IC_CHAT_INFINITY	*pRecvChat;
	char					*chatString;
	
	nRecvTypeSize = sizeof(MSG_IC_CHAT_INFINITY) + ((MSG_IC_CHAT_INFINITY*)(pPacket+nBytesUsed))->MessageLength;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_INFINITY, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12010);
		return RES_RETURN_FALSE;
	}
	pRecvChat = (MSG_IC_CHAT_INFINITY*)(pPacket+nBytesUsed);
	chatString = (char*)(pPacket+nBytesUsed+sizeof(MSG_IC_CHAT_INFINITY));
	nBytesUsed += nRecvTypeSize;
	
	if (strnicmp(m_character.CharacterName, pRecvChat->FromCharacterName, SIZE_MAX_CHARACTER_NAME) != 0)
	{
		SendErrorMessage(T_IC_CHAT_INFINITY, ERR_CHAT_CHARACTER_NAME_NOT_MATCHED);
		Close(0x1200B);
		return RES_RETURN_FALSE;
	}

	// Ă¤ĆĂ ÇĘĹÍ¸µ
	if (!FilterChattingString(chatString))
	{
		SendErrorMessage(T_IC_CHAT_INFINITY, ERR_CHAT_NOT_ALLOWED_STRING);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	///////////////////////////////////////////////////////////////////////////////
	// Ă¤ĆĂ ·Î±× ł˛±â±â
	char szChatting[1024];
	sprintf(szChatting, "Chat_INFINITY|%s|%d|%d|%s\r\n",
			pRecvChat->FromCharacterName, m_character.MapChannelIndex.MapIndex,
			m_character.MapChannelIndex.ChannelIndex, (char*)pRecvChat + sizeof(MSG_IC_CHAT_INFINITY));
	// 2007-11-13 by cmkwon, °ü¸®ŔÚ, żîżµŔÚ Ă¤ĆĂ ·Î±× ŔúŔĺÇĎ±â - CIMGlobal::WriteChattingLog() ÇÔĽö ŔÎŔÚ Ăß°ˇ
	g_pIMGlobal->WriteChattingLog(szChatting, m_character.Race);
#ifdef _INET_AT_ALL_CHATS
	INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, msgATChat, SendBuff);
	msgATChat->PrintType = 0x13;
	sprintf(szChatting, "[%s]: %s", pRecvChat->FromCharacterName, chatString);
	STRNCPY_MEMSET(msgATChat->String, szChatting, 128);
	msgATChat->SecurityNumber = m_character.InfluenceType;
	ms_pIMIOCP->SendMessageToAdminTool(SendBuff, MSG_SIZE(MSG_IC_STRING_128));
#endif
	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	int nRemainMinute = 0;
	if(ms_pIMIOCP->m_chatBlockManager.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName)
		|| ms_pIMIOCP->m_chatBlockManagerByLeader.IsExistChatBlockList(&nRemainMinute, m_character.CharacterName))	// 2008-12-30 by cmkwon, ÁöµµŔÚ Ă¤ĆĂ Á¦ÇŃ Ä«µĺ ±¸Çö - T_IC_CHAT_ARENA, ľĆ·ąłŞ
	{
		SendErrorMessage(T_IC_CHAT_INFINITY, ERR_CHAT_CHAT_BLOCKED, nRemainMinute);
		return RES_BREAK;
	}

	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}

	INIT_MSG_WITH_BUFFER(MSG_FI_CHAT_INFINITY, T_FI_CHAT_INFINITY, msgChatRegion, SendBuf);
	msgChatRegion->CharacterUniqueNumber = m_character.CharacterUniqueNumber;
	msgChatRegion->MessageLength = pRecvChat->MessageLength;
	STRNCPY_MEMSET((char*)SendBuf + MSG_SIZE(MSG_FI_CHAT_INFINITY), chatString, SIZE_MAX_CHAT_MESSAGE);

	if (FALSE == this->SendMsgToCurrentFieldServerSocket(SendBuf, MSG_SIZE(MSG_FI_CHAT_INFINITY) + msgChatRegion->MessageLength))
	{
		// current field server socketŔĚ NULLŔĚ¸é, Á˘ĽÓŔ» ˛÷´Â´Ů.
		char	szSystemLog[1024];
		sprintf(szSystemLog, "  Fatal Error: CurrentFieldServerSocket is NULL, %s at %s\r\n",
			GetCharacterString(&m_character, string()), GetProtocolTypeString(T_IC_CHAT_INFINITY));
		DBGOUT(szSystemLog);
		g_pGlobal->WriteSystemLog(szSystemLog);
		
		return RES_RETURN_FALSE;
	}
	
	if(i_pThreadInfo)
	{// 2007-02-21 by cmkwon
		i_pThreadInfo->nParam1++;
	}
	
	return RES_RETURN_TRUE;	

}

ProcessResult CIMIOCPSocket::Process_IC_CHAT_FRIENDLIST_AND_REJECTLIST_LOADING(const char* pPacket, int nLength, int &nBytesUsed)
{// No Body
	
	///////////////////////////////////////////////////////////////////////////
	// ÄŁ±¸, °ĹşÎ ¸ń·Ď ·Îµů
	DB_FRIEND_INFO	*pDBFriendInfo = new DB_FRIEND_INFO;
	memset(pDBFriendInfo, 0x00, sizeof(DB_FRIEND_INFO));
	STRNCPY_MEMSET(pDBFriendInfo->CharacterName, this->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);
// 2009-03-05 by cmkwon, ĹëÇŐ ľĆ·ąłŞ ŔĚżë˝Ă ÄŁ±¸ ¸®˝şĆ® ş¸łĘ˝ş °ćÇčÄˇ °ü·Ă ąö±× ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĂłŔ˝ ·Îµů ŔÎŔÚ¸¦ Ŕü´Ţ ÇÔ.
//	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_FriendLoadFriends, this, pDBFriendInfo);
	
	// 2009-03-05 by cmkwon, ĹëÇŐ ľĆ·ąłŞ ŔĚżë˝Ă ÄŁ±¸ ¸®˝şĆ® ş¸łĘ˝ş °ćÇčÄˇ °ü·Ă ąö±× ĽöÁ¤ - ŔÎŔÚ¸¦ Ŕü´ŢÇĎ°í FALSE¸¦ ĽłÁ¤ÇŃ´Ů.
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_FriendLoadFriends, this, pDBFriendInfo, NULL, this->m_bFirstTimeLoadFriendList);
	this->m_bFirstTimeLoadFriendList		= FALSE;

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_CHAT_FRIENDLIST_INSERT(const char* pPacket, int nLength, int &nBytesUsed)
{
	MSG_IC_CHAT_FRIENDLIST_INSERT	*pNewMsg = NULL;

	int			nRecvTypeSize = sizeof(MSG_IC_CHAT_FRIENDLIST_INSERT);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_FRIENDLIST_INSERT, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12012);
		return RES_RETURN_FALSE;
	}
	pNewMsg = (MSG_IC_CHAT_FRIENDLIST_INSERT*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	// 2009-01-23 by cmkwon, ĹÍĹ° i,I ą®ŔÚ ą®Á¦ Ăł¸® - T_IC_CHAT_FRIENDLIST_INSERT
	LowercaseOnly_i_forTurkish(pNewMsg->szCharacterName, SIZE_MAX_CHARACTER_NAME);

	if(0 == strncmp(pNewMsg->szCharacterName, "", SIZE_MAX_CHARACTER_NAME)
		|| 0 == strnicmp(pNewMsg->szCharacterName, this->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME))
	{// Äł¸ŻĹÍ¸íŔĚ Ŕß¸řµÇľî ŔÖŔ˝

		SendErrorMessage(T_IC_CHAT_FRIENDLIST_INSERT, ERR_FRIEND_INVALID_CHARACTER_NAME);
		return RES_BREAK;
	}

	// 2009-02-26 by cmkwon, ÄŁ±¸¸®˝şĆ® °řąé °ü·Ă ąö±× ĽöÁ¤ - ÄŁ±¸Äł¸ŻĹÍ¸íżˇ °řąéŔĚ ŔÖ´ÂÁö ĂĽĹ©
	if(NULL != strstr(pNewMsg->szCharacterName, " "))
	{
		SendErrorMessage(T_IC_CHAT_FRIENDLIST_INSERT, ERR_FRIEND_INVALID_CHARACTER_NAME);
		return RES_BREAK;
	}

	if(COMPARE_INFLUENCE(m_character.InfluenceType, INFLUENCE_TYPE_NORMAL))
	{// 2006-10-14 by cmkwon, ŔĎąÝ ĽĽ·ÂŔş ÄŁ±¸ µî·Ď şŇ°ˇ
		SendErrorMessage(T_IC_CHAT_FRIENDLIST_INSERT, ERR_REQ_CHOOSE_INFLUENCE_TYPE);
		return RES_BREAK;
	}

	CIMIOCPSocket *pSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(pNewMsg->szCharacterName);
	if(pSocket
		&& pSocket->IsValidCharacter())
	{
		if(FALSE == COMPARE_RACE(this->GetCharacter()->Race, RACE_OPERATION|RACE_GAMEMASTER)
			&& COMPARE_RACE(pSocket->GetCharacter()->Race, RACE_OPERATION|RACE_GAMEMASTER))
		{// 2006-11-14 by cmkwon, ŔĎąÝ ŔŻŔú´Â °ü¸®ŔÚ,żîżµŔÚ¸¦ ÄŁ±¸·Î µî·ĎşŇ°ˇ

			SendErrorMessage(T_IC_CHAT_FRIENDLIST_INSERT, ERR_FRIEND_INVALID_CHARACTER_NAME);
			return RES_BREAK;
		}
		
		if(m_character.InfluenceType != pSocket->GetCharacter()->InfluenceType)
		{// 2006-10-17 by cmkwon, Ľ­·Î ´Ů¸Ą ĽĽ·ÂŔş ÄŁ±¸ Ăß°ˇ şŇ°ˇ

			SendErrorMessage(T_IC_CHAT_FRIENDLIST_INSERT, ERR_PROTOCOL_REQ_INFLUENCE_NOT_MATCHED);
			return RES_BREAK;
		}

		if(pSocket->IsCharacterNameFromRejectList(this->m_character.CharacterName))
		{// »ó´ëąć °ĹşÎ¸®˝şĆ®żˇ µî·ĎµÇľî ŔÖŔ˝
	
			SendErrorMessage(T_IC_CHAT_FRIENDLIST_INSERT, ERR_FRIEND_REGISTERED_PEER_REJECTLIST, 0, 0, pNewMsg->szCharacterName);	
			return RES_BREAK;
		}

		///////////////////////////////////////////////////////////////////////////////
		// 2009-01-13 by cmkwon, ÄŁ±¸ µî·Ď˝Ă »ó´ëąćżˇ°Ô ľË¸˛ ˝Ă˝şĹŰ Ŕűżë - »ó´ëąćżˇ°Ô ÄŁ±¸ µî·ĎµĘŔ» ľË¸°´Ů. Ĺ¬¶óŔĚľđĆ®żˇĽ­ µî·ĎµČ »ó´ëąćŔĚ µî·Ď ż©şÎ ĂĽĹ© 
		INIT_MSG_WITH_BUFFER(MSG_IC_CHAT_FRIENDLIST_INSERT_NOTIFY, T_IC_CHAT_FRIENDLIST_INSERT_NOTIFY, pNotify, SendBuf);
		STRNCPY_MEMSET(pNotify->szDoerCharacName, this->GetCharacter()->CharacterName, SIZE_MAX_CHARACTER_NAME);
		pSocket->SendAddData(SendBuf, MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_INSERT_NOTIFY));		
	}

	if(this->FindFriendInfoByCharacterName(pNewMsg->szCharacterName))
	{
		SendErrorMessage(T_IC_CHAT_FRIENDLIST_INSERT, ERR_FRIEND_ALREADY_REGISTERED, 0, 0, pNewMsg->szCharacterName);		
		return RES_BREAK;
	}

	if(SIZE_MAX_FRIENDLIST_COUNT <= m_mtvectorFriendList.size())
	{
		SendErrorMessage(T_IC_CHAT_FRIENDLIST_INSERT, ERR_FRIEND_ALREADY_MAX_LIST, m_mtvectorFriendList.size());
		return RES_BREAK;
	}
	
	///////////////////////////////////////////////////////////////////////////
	// ÄŁ±¸żˇ Ăß°ˇ Äő¸®
	DB_FRIEND_INFO	*pDBFriendInfo = new DB_FRIEND_INFO;
	STRNCPY_MEMSET(pDBFriendInfo->CharacterName, this->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);
	STRNCPY_MEMSET(pDBFriendInfo->FriendName, pNewMsg->szCharacterName, SIZE_MAX_CHARACTER_NAME);
	pDBFriendInfo->FriendType = FRIEND_TYPE_FRIEND;
	pDBFriendInfo->RegDate.SetCurrentDateTime();		// 2008-04-11 by dhjin, EP3 °ĹşÎ¸ń·Ď -
	pDBFriendInfo->LastLogin.SetCurrentDateTime();
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_FriendInsertFriend, this, pDBFriendInfo);

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_CHAT_FRIENDLIST_DELETE(const char* pPacket, int nLength, int &nBytesUsed)
{
	MSG_IC_CHAT_FRIENDLIST_DELETE	*pNewMsg = NULL;

	int			nRecvTypeSize = sizeof(MSG_IC_CHAT_FRIENDLIST_DELETE);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_FRIENDLIST_DELETE, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12012);
		return RES_RETURN_FALSE;
	}
	pNewMsg = (MSG_IC_CHAT_FRIENDLIST_DELETE*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	// 2009-01-23 by cmkwon, ĹÍĹ° i,I ą®ŔÚ ą®Á¦ Ăł¸® - T_IC_CHAT_FRIENDLIST_DELETE
	LowercaseOnly_i_forTurkish(pNewMsg->szCharacterName, SIZE_MAX_CHARACTER_NAME);

	if(strncmp(pNewMsg->szCharacterName, "", SIZE_MAX_CHARACTER_NAME) == 0)
	{// Äł¸ŻĹÍ¸í ŔĚ Ŕß¸řµÇľî ŔÖŔ˝

		SendErrorMessage(T_IC_CHAT_FRIENDLIST_DELETE, ERR_FRIEND_INVALID_CHARACTER_NAME);
		return RES_BREAK;
	}
	
	if(FALSE == this->FindFriendInfoByCharacterName(pNewMsg->szCharacterName))
	{// ÄŁ±¸¸®˝şĆ®żˇ Á¸ŔçÇĎÁö ľĘŔ˝

		SendErrorMessage(T_IC_CHAT_FRIENDLIST_DELETE, ERR_FRIEND_NO_SUCH_CHARACTER_IN_LIST, 0, 0, pNewMsg->szCharacterName);
		return RES_BREAK;
	}

	///////////////////////////////////////////////////////////////////////////
	// ÄŁ±¸¸®˝şĆ®żˇĽ­ »čÁ¦ Äő¸®
	DB_FRIEND_INFO	*pDBFriendInfo = new DB_FRIEND_INFO;
	STRNCPY_MEMSET(pDBFriendInfo->CharacterName, this->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);
	STRNCPY_MEMSET(pDBFriendInfo->FriendName, pNewMsg->szCharacterName, SIZE_MAX_CHARACTER_NAME);
	pDBFriendInfo->FriendType = FRIEND_TYPE_FRIEND;
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_FriendDeleteFriend, this, pDBFriendInfo);
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_CHAT_FRIENDLIST_REFRESH(const char* pPacket, int nLength, int &nBytesUsed)
{// No Body

	if(0 == m_mtvectorFriendList.size())
	{
		return RES_BREAK;
	}

// 2008-10-24 by cmkwon, ÄŁ±¸¸®˝şĆ® ŔüĽŰ ąö±× ĽöÁ¤ - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤ ÇÔ.
// 	INIT_MSG_WITH_BUFFER(MSG_IC_CHAT_FRIENDLIST_REFRESH_OK, T_IC_CHAT_FRIENDLIST_REFRESH_OK, pSendMsg, tmSendBuf);
// 	pSendMsg->nFriendListCounts = 0;
// 
// 	this->m_mtvectorFriendList.lock();
// 	this->MakeMsg_MSG_IC_CHAT_FRIENDLIST_LOADING_OK(pSendMsg
// 		, (FRIENDINFO *)(tmSendBuf + MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_REFRESH_OK)), (SIZE_MAX_PACKET-MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_REFRESH_OK))/sizeof(FRIENDINFO));
// 	this->m_mtvectorFriendList.unlock();
// 
// 	if(pSendMsg->nFriendListCounts > 0)
// 	{
// 		this->SendAddData(tmSendBuf
// 			, MSG_SIZE(MSG_IC_CHAT_FRIENDLIST_REFRESH_OK) + sizeof(FRIENDINFO)*pSendMsg->nFriendListCounts);
// 	}	
	this->SendFriendListRefreshOK();		// 2008-10-24 by cmkwon, ÄŁ±¸¸®˝şĆ® ŔüĽŰ ąö±× ĽöÁ¤ - 
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_CHAT_REJECTLIST_INSERT(const char* pPacket, int nLength, int &nBytesUsed)
{
	MSG_IC_CHAT_REJECTLIST_INSERT	*pNewMsg = NULL;

	int			nRecvTypeSize = sizeof(MSG_IC_CHAT_REJECTLIST_INSERT);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_REJECTLIST_INSERT, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12012);
		return RES_RETURN_FALSE;
	}
	pNewMsg = (MSG_IC_CHAT_REJECTLIST_INSERT*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	// 2009-01-23 by cmkwon, ĹÍĹ° i,I ą®ŔÚ ą®Á¦ Ăł¸® - T_IC_CHAT_REJECTLIST_INSERT
	LowercaseOnly_i_forTurkish(pNewMsg->szCharacterName, SIZE_MAX_CHARACTER_NAME);

	if(0 == strncmp(pNewMsg->szCharacterName, "", SIZE_MAX_CHARACTER_NAME))	
	{// Äł¸ŻĹÍ¸íŔĚ Ŕß¸řµÇľî ŔÖŔ˝
		SendErrorMessage(T_IC_CHAT_REJECTLIST_INSERT, ERR_FRIEND_INVALID_CHARACTER_NAME);
		return RES_BREAK;
	}

	if(0 == strnicmp(pNewMsg->szCharacterName, this->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME))
	{// Äł¸ŻĹÍ¸íŔĚ Ŕß¸řµÇľî ŔÖŔ˝

		SendErrorMessage(T_IC_CHAT_REJECTLIST_INSERT, ERR_FRIEND_INVALID_CHARACTER_NAME, 0, 0, pNewMsg->szCharacterName);
		return RES_BREAK;
	}

	// 2009-02-26 by cmkwon, ÄŁ±¸¸®˝şĆ® °řąé °ü·Ă ąö±× ĽöÁ¤ - °ĹşÎÄł¸ŻĹÍ¸íżˇ °řąéŔĚ ŔÖ´ÂÁö ĂĽĹ©
	if(NULL != strstr(pNewMsg->szCharacterName, " "))
	{
		SendErrorMessage(T_IC_CHAT_FRIENDLIST_INSERT, ERR_FRIEND_INVALID_CHARACTER_NAME);
		return RES_BREAK;
	}

	if(this->IsCharacterNameFromRejectList(pNewMsg->szCharacterName))
	{// ŔĚąĚ °ĹşÎ¸®˝şĆ®żˇ Á¸ŔçÇÔ

		SendErrorMessage(T_IC_CHAT_REJECTLIST_INSERT, ERR_FRIEND_ALREADY_REGISTERED, 0, 0, pNewMsg->szCharacterName);		
		return RES_BREAK;
	}

	if(SIZE_MAX_REJECTLIST_COUNT <= m_mtvectorRejectList.size())
	{// ĂÖ´ë ŔÎżř

		SendErrorMessage(T_IC_CHAT_REJECTLIST_INSERT, ERR_FRIEND_ALREADY_MAX_LIST, m_mtvectorRejectList.size());
		return RES_BREAK;
	}
// 2004-11-02 by cmkwon, °ĹşÎ ¸®˝şĆ®µµ On/Offline »óĹÂżÍ »ó°üľřŔĚ µî·Ď °ˇ´É
//	CIMIOCPSocket *pSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(pNewMsg->szCharacterName);
//	if(NULL == pSocket)
//	{
//		SendErrorMessage(T_IC_CHAT_REJECTLIST_INSERT, ERR_COMMON_NO_SUCH_CHARACTER, 0, 0, pNewMsg->szCharacterName);
//		return RES_BREAK;
//	}
//
//	if(pSocket->FindFriendInfoByCharacterName(this->m_character.CharacterName))
//	{// »ó´ëąć ÄŁ±¸¸®˝şĆ®żˇ µî·ĎµÇľî ŔÖŔ¸ąÇ·Î »čÁ¦ÇŘľßÇŃ´Ů.
//
//		///////////////////////////////////////////////////////////////////////////
//		// »ó´ëąć ÄŁ±¸ ¸ń·ĎżˇĽ­ ŔÚ˝ĹŔ» »čÁ¦ÇĎ´Â Äő¸®
//		DB_FRIEND_INFO	*pDBFriendInfo = new DB_FRIEND_INFO;
//		strncpy(pDBFriendInfo->CharacterName, pSocket->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);
//		strncpy(pDBFriendInfo->FriendName, this->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);
//		pDBFriendInfo->FriendType = FRIEND_TYPE_FRIEND;
//		ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_FriendDeleteFriend, pSocket, pDBFriendInfo);
//	}

	///////////////////////////////////////////////////////////////////////////
	// »ó´ëąć ÄŁ±¸ ¸ń·ĎżˇĽ­ ŔÚ˝ĹŔ» »čÁ¦ÇĎ´Â Äő¸®
	DB_FRIEND_INFO	*pDBFriendInfo = new DB_FRIEND_INFO;
	STRNCPY_MEMSET(pDBFriendInfo->CharacterName, pNewMsg->szCharacterName, SIZE_MAX_CHARACTER_NAME);
	STRNCPY_MEMSET(pDBFriendInfo->FriendName, this->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);
	pDBFriendInfo->FriendType = FRIEND_TYPE_FRIEND;
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_FriendDeleteFriend, this, pDBFriendInfo);
	
	///////////////////////////////////////////////////////////////////////////
	// °ĹşÎ¸®˝şĆ® Ăß°ˇ Äő¸®
	DB_FRIEND_INFO	*pDBFriendInfo1 = new DB_FRIEND_INFO;
	STRNCPY_MEMSET(pDBFriendInfo1->CharacterName, this->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);
	STRNCPY_MEMSET(pDBFriendInfo1->FriendName, pNewMsg->szCharacterName, SIZE_MAX_CHARACTER_NAME);
	pDBFriendInfo1->FriendType = FRIEND_TYPE_REJECT;
	pDBFriendInfo1->RegDate.SetCurrentDateTime();		// 2008-04-11 by dhjin, EP3 °ĹşÎ¸ń·Ď -
	pDBFriendInfo1->LastLogin.SetCurrentDateTime(); //not used in reject
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_FriendInsertFriend, this, pDBFriendInfo1);

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_CHAT_REJECTLIST_DELETE(const char* pPacket, int nLength, int &nBytesUsed)
{
	MSG_IC_CHAT_REJECTLIST_DELETE	*pNewMsg = NULL;

	int			nRecvTypeSize = sizeof(MSG_IC_CHAT_REJECTLIST_DELETE);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IC_CHAT_REJECTLIST_DELETE, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12012);
		return RES_RETURN_FALSE;
	}
	pNewMsg = (MSG_IC_CHAT_REJECTLIST_DELETE*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	// 2009-01-23 by cmkwon, ĹÍĹ° i,I ą®ŔÚ ą®Á¦ Ăł¸® - T_IC_CHAT_REJECTLIST_DELETE
	LowercaseOnly_i_forTurkish(pNewMsg->szCharacterName, SIZE_MAX_CHARACTER_NAME);

	if(strncmp(pNewMsg->szCharacterName, "", SIZE_MAX_CHARACTER_NAME) == 0)
	{// Äł¸ŻĹÍ¸í ŔĚ Ŕß¸řµÇľî ŔÖŔ˝

		SendErrorMessage(T_IC_CHAT_REJECTLIST_DELETE, ERR_FRIEND_INVALID_CHARACTER_NAME);
		return RES_BREAK;
	}
	
	if(FALSE == this->IsCharacterNameFromRejectList(pNewMsg->szCharacterName))
	{// °ĹşÎ¸®˝şĆ®żˇ Á¸ŔçÇĎÁö ľĘŔ˝

		SendErrorMessage(T_IC_CHAT_REJECTLIST_DELETE, ERR_FRIEND_NO_SUCH_CHARACTER_IN_LIST, 0, 0, pNewMsg->szCharacterName);
		return RES_BREAK;
	}
	
	///////////////////////////////////////////////////////////////////////////
	// ÄŁ±¸¸®˝şĆ®żˇĽ­ »čÁ¦ Äő¸®
	DB_FRIEND_INFO	*pDBFriendInfo = new DB_FRIEND_INFO;
	STRNCPY_MEMSET(pDBFriendInfo->CharacterName, this->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);
	STRNCPY_MEMSET(pDBFriendInfo->FriendName, pNewMsg->szCharacterName, SIZE_MAX_CHARACTER_NAME);
	pDBFriendInfo->FriendType = FRIEND_TYPE_REJECT;
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_FriendDeleteFriend, this, pDBFriendInfo);
	return RES_RETURN_TRUE;
}


BOOL CIMIOCPSocket::SendToFieldServerChatBitFlag(void)
{
	INIT_MSG_WITH_BUFFER(MSG_FI_CHAT_CHANGE_CHAT_FLAG, T_FI_CHAT_CHANGE_CHAT_FLAG, pOnOff, pOnOffBuf);
	pOnOff->CharacterUniqueNumber	= m_character.CharacterUniqueNumber;
// 2008-05-15 by dhjin, EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć
//	pOnOff->ChatFlag				= m_enableChatFlag8;
	pOnOff->ChatFlag				= m_enableChatFlag16;

	this->SendMsgToCurrentFieldServerSocket(pOnOffBuf, MSG_SIZE(MSG_FI_CHAT_CHANGE_CHAT_FLAG));
	return TRUE;
}

void CIMIOCPSocket::SendString128(BYTE string128_type, char* pFormat, ...)
{
	if (string128_type == STRING_128_DEBUG_L1
		|| string128_type == STRING_128_DEBUG_L2)
	{
		return;
	}

	char szStrTemp[1024];
	int nHeaderSize = 0;
	MEMSET_ZERO(szStrTemp, 1024);
	if (string128_type == STRING_128_DEBUG_L1)
	{
		strcpy(szStrTemp, "[D1]");
		nHeaderSize = 4;
	}
	else if (string128_type == STRING_128_DEBUG_L2)
	{
		strcpy(szStrTemp, "[D2]");
		nHeaderSize = 4;
	}
	else if (string128_type == STRING_128_DEBUG_L3)
	{
		strcpy(szStrTemp, "[D3]");
		nHeaderSize = 4;
	}
	else if (string128_type == STRING_128_ADMIN_CMD)
	{
		strcpy(szStrTemp, "[A]");
		nHeaderSize = 3;
	}

	va_list args;
	va_start(args, pFormat);
	vsprintf(szStrTemp+nHeaderSize, pFormat, args);
    va_end(args);

	// start 2011-06-22 by hskim, »çĽł Ľ­ąö ąćÁö
	//WORD wSecurityNumber = ms_pIMIOCP->m_pSecurityManager->GetSecurityNumber();
	// end 2011-06-22 by hskim, »çĽł Ľ­ąö ąćÁö

	int nStrLen = strlen(szStrTemp);
	if(128 > nStrLen)
	{
		INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, MsgStr, SendBuf);
		MsgStr->PrintType = STR128_PRINT_TYPE_CHAT;
		STRNCPY_MEMSET(MsgStr->String, szStrTemp, 128);
		MsgStr->SecurityNumber = 0;		// 2011-06-22 by hskim, »çĽł Ľ­ąö ąćÁö
		SendAddData(SendBuf, MSG_SIZE(MSG_IC_STRING_128));
	}
	else if(256 > nStrLen)
	{
		INIT_MSG_WITH_BUFFER(MSG_IC_STRING_256, T_IC_STRING_256, MsgStr, SendBuf);
		MsgStr->PrintType = STR128_PRINT_TYPE_CHAT;
		STRNCPY_MEMSET(MsgStr->String, szStrTemp, 256);
		MsgStr->SecurityNumber = 0;		// 2011-06-22 by hskim, »çĽł Ľ­ąö ąćÁö
		SendAddData(SendBuf, MSG_SIZE(MSG_IC_STRING_256));
	}
	else if(512 > nStrLen)
	{
		INIT_MSG_WITH_BUFFER(MSG_IC_STRING_512, T_IC_STRING_512, MsgStr, SendBuf);
		MsgStr->PrintType = STR128_PRINT_TYPE_CHAT;
		STRNCPY_MEMSET(MsgStr->String, szStrTemp, 512);
		MsgStr->SecurityNumber = 0;		// 2011-06-22 by hskim, »çĽł Ľ­ąö ąćÁö
		SendAddData(SendBuf, MSG_SIZE(MSG_IC_STRING_512));
	}
	else
	{
		INIT_MSG_WITH_BUFFER(MSG_IC_STRING_512, T_IC_STRING_512, MsgStr, SendBuf);
		MsgStr->PrintType = STR128_PRINT_TYPE_CHAT;
		STRNCPY_MEMSET(MsgStr->String, szStrTemp, 512);
		MsgStr->SecurityNumber = 0;		// 2011-06-22 by hskim, »çĽł Ľ­ąö ąćÁö
		SendAddData(SendBuf, MSG_SIZE(MSG_IC_STRING_512));

		char szErrStr[1024];
 		wsprintf(szErrStr, "  Error: String Too Long For STRING_XXX(Length:%d, %s)\r\n", nStrLen, szStrTemp);
		g_pIMGlobal->WriteSystemLog(szErrStr);
		DbgOut(szErrStr);
	}

	if (string128_type == STRING_128_ADMIN_CMD)
	{
		DBGOUT2(DO2_SYSLOG, "	SendString128 %s: %s\r\n",
			GetCharacterString(&m_character, string()),
			szStrTemp);
	}

// 2006-04-12 by cmkwon, ĽöÁ¤ŔŰľ÷Ŕ¸·Î ŔÓ˝Ă ÁÖĽ®Ăł¸®ÇÔ
// 	int nHeaderSize = 0;
// 	INIT_MSG_WITH_BUFFER(MSG_IC_STRING_128, T_IC_STRING_128, MsgStr, SendBuf);
// 	MsgStr->PrintType = STR128_PRINT_TYPE_CHAT;
// 	if (string128_type == STRING_128_DEBUG_L1)
// 	{
// 		strcpy(MsgStr->String, "[D1]");
// 		nHeaderSize = 4;
// 	}
// 	else if (string128_type == STRING_128_DEBUG_L2)
// 	{
// 		strcpy(MsgStr->String, "[D2]");
// 		nHeaderSize = 4;
// 	}
// 	else if (string128_type == STRING_128_DEBUG_L3)
// 	{
// 		strcpy(MsgStr->String, "[D3]");
// 		nHeaderSize = 4;
// 	}
// 	else if (string128_type == STRING_128_ADMIN_CMD)
// 	{
// 		strcpy(MsgStr->String, "[A]");
// 		nHeaderSize = 3;
// 	}
// 
// 	va_list args;
// 	va_start(args, pFormat);
// 	vsprintf(MsgStr->String+nHeaderSize, pFormat, args);
// 	int ret = GDeleteNewLine(MsgStr->String);	// delete newline character(s)
// 	if (ret < 128)
// 	{
// 		SendAddData(SendBuf, MSG_SIZE(MSG_FC_STRING_128));
// 	}
// 	else
// 	{
// 		MsgStr->String[127] = '\0';
// 		DBGOUT("  Error: String Too Long For STRING_128(ret: %d): %s\r\n", ret, MsgStr->String);
// 		SendAddData(SendBuf, MSG_SIZE(MSG_FC_STRING_128));
// 	}
//     va_end(args);
// 
// 	if (string128_type == STRING_128_ADMIN_CMD)
// 	{
// 		DBGOUT2(DO2_SYSLOG, "	SendString128 %s: %s\r\n",
// 			GetCharacterString(&m_character, string()),
// 			MsgStr->String);
// 	}
}

BOOL CIMIOCPSocket::SendChatMessage(BitFlag16_t i_CHAT_TYPE, BYTE *i_pChatData, int i_nSize, BYTE i_byInfluenceMask/*=INFLUENCE_TYPE_ALL_MASK*/)
{
	if(FALSE == IsUsing()
		|| FALSE == COMPARE_CHATTYPE_BIT(m_enableChatFlag16, i_CHAT_TYPE)
		|| FALSE == COMPARE_INFLUENCE(m_character.InfluenceType, i_byInfluenceMask))
	{
		return FALSE;
	}

	return SendAddData(i_pChatData, i_nSize);
}



ProcessResult CIMIOCPSocket::Process_FI_CONNECT(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_CONNECT, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	int					nRecvTypeSize = 0;
	MSG_FI_CONNECT		*pMsgConn;

	nRecvTypeSize = sizeof(MSG_FI_CONNECT)
		+ ((MSG_FI_CONNECT*)(pPacket+nBytesUsed))->NumOfMapIndex * sizeof(MapIndex_t);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		SendErrorMessage(T_FI_CONNECT, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12016);
		return RES_RETURN_FALSE;
	}
	pMsgConn = (MSG_FI_CONNECT*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	if(ms_pIMIOCP->m_pFieldServerSocket)
	{
		// ŔĚąĚ FieldServer°ˇ Á˘ĽÓÇŘ ŔÖŔ˝
		SendErrorMessage(T_FI_CONNECT, ERR_PROTOCOL_FIELD_SERVER_ALREADY_REGISTERD);
		return RES_BREAK;
	}

	m_PeerSocketType	= ST_FIELD_SERVER;
	m_FieldServerID		= pMsgConn->FieldServerID;

	// set field server ID
	FieldServerInfo *pFieldServerInfo = &ms_pIMIOCP->m_FieldServerInfo;
	pFieldServerInfo->Reset();
	pFieldServerInfo->serverID						= pMsgConn->FieldServerID;
	pFieldServerInfo->IsActive						= TRUE;
	pFieldServerInfo->ServerType					= ST_FIELD_SERVER;
	pFieldServerInfo->pSocket						= this;
	pFieldServerInfo->AccumulatedFieldUserCounts	= 0;
	pFieldServerInfo->CurrentFieldUserCounts		= 0;
	pFieldServerInfo->MaxFieldUserCounts			= 0;
	pFieldServerInfo->AccumulatedFieldMonsterCounts	= 0;
	pFieldServerInfo->CurrentFieldMonsterCounts		= 0;	
	for (int i = 0; i < pMsgConn->NumOfMapIndex; i++)
	{
		MapIndex_t pTmpMapIndex = *(MapIndex_t*)(((char*)pMsgConn) + sizeof(MSG_FI_CONNECT) + sizeof(MapIndex_t)*i);
		pFieldServerInfo->MapIndexVector.push_back(pTmpMapIndex);
#ifdef _DEBUG
		DBGOUT("FieldServer(%s) -> %04d\n", pFieldServerInfo->serverID.GetString(string()), pTmpMapIndex);
#endif
	}
	ms_pIMIOCP->SetFieldServerSocket(this);

	if(pMsgConn->ArenaServerCheck)
	{// 2008-02-28 by dhjin, ľĆ·ąłŞ ĹëÇŐ - Arena Ľ­ąöŔĚ¸é TRUE
		ms_pIMIOCP->m_bArenaServerCheck	= TRUE;
	}

	this->SendAddMessageType(T_FI_CONNECT_OK);

	// 2009-03-26 by cmkwon, ˝Ă˝şĹŰ ·Î±× Ăß°ˇ - 
	//DBGOUT("Connection from Field Server(%s) successfully established...\n", pFieldServerInfo->serverID.GetString(string()));
	g_pGlobal->WriteSystemLogEX(TRUE, "  [Notify] Connected from FieldServer(%s) !! IsArenaServerFlag(%d) \r\n", pFieldServerInfo->serverID.GetString(string()), pMsgConn->ArenaServerCheck);

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_FI_CONNECT_NOTIFY_DEAD(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_CONNECT_NOTIFY_DEAD, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_RETURN_FALSE;
	}
	//end of 10-08-2015 by inetpub

	int							nRecvTypeSize = 0;
	MSG_FI_CONNECT_NOTIFY_DEAD	*pMsgNotifyDead;

	nRecvTypeSize = sizeof(MSG_FI_CONNECT_NOTIFY_DEAD);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		SendErrorMessage(T_FI_CONNECT_NOTIFY_DEAD, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12017);
		return RES_RETURN_FALSE;
	}
	pMsgNotifyDead = (MSG_FI_CONNECT_NOTIFY_DEAD*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMIOCPSocket *pIMSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pMsgNotifyDead->CharacterUniqueNumber);
	if (pIMSocket
		&& pIMSocket->IsUsing())
	{
		pIMSocket->CharacterDeadRoutine(pMsgNotifyDead->AttackerGuildUID);
	}

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_FI_CONNECT_NOTIFY_GAMEEND(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_CONNECT_NOTIFY_GAMEEND, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	int								nRecvTypeSize = 0;
	MSG_FI_CONNECT_NOTIFY_GAMEEND	*pMsgNotifyGameEnd;

	nRecvTypeSize = sizeof(MSG_FI_CONNECT_NOTIFY_GAMEEND);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		SendErrorMessage(T_FI_CONNECT_NOTIFY_GAMEEND, ERR_PROTOCOL_INVALID_FIELD_DATA);
		return RES_RETURN_FALSE;
	}
	pMsgNotifyGameEnd = (MSG_FI_CONNECT_NOTIFY_GAMEEND*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMIOCPSocket *pIMSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pMsgNotifyGameEnd->CharacterUniqueNumber);
	if (pIMSocket
		&& pIMSocket->IsUsing()
		&& pIMSocket->m_character.CharacterUniqueNumber == pMsgNotifyGameEnd->CharacterUniqueNumber)
	{
		if(COMPARE_RACE(pIMSocket->GetCharacter()->Race, RACE_OPERATION|RACE_GAMEMASTER))
		{// 2006-05-08 by cmkwon
			ms_pIMIOCP->DeleteFromAdminCharacter(pIMSocket->GetCharacter()->CharacterUniqueNumber);
		}

		char szTemp[1024];
		sprintf(szTemp, "%s's GameEnd Notified...\r\n", GetCharacterString(&pIMSocket->m_character, string()));
		g_pIMGlobal->WriteSystemLog(szTemp);
		DBGOUT(szTemp);
		pIMSocket->GameEndRoutineIM();
	}

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_FI_CONNECT_NOTIFY_GAMESTART(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_CONNECT_NOTIFY_GAMESTART, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	int								nRecvTypeSize = 0;
	MSG_FI_CONNECT_NOTIFY_GAMESTART	*pMsgNotifyGameStart;

	nRecvTypeSize = sizeof(MSG_FI_CONNECT_NOTIFY_GAMESTART);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		SendErrorMessage(T_FI_CONNECT_NOTIFY_GAMESTART, ERR_PROTOCOL_INVALID_FIELD_DATA);
		return RES_PACKET_ERROR;
	}
	pMsgNotifyGameStart = (MSG_FI_CONNECT_NOTIFY_GAMESTART*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	// »çżëŔÚ Č®ŔÎ
	CIMIOCPSocket *pCharacterSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pMsgNotifyGameStart->CharacterUniqueNumber);
	if (pCharacterSocket == NULL 
		|| FALSE == pCharacterSocket->IsUsing())
	{
		SendErrorMessage(T_FI_CONNECT_NOTIFY_GAMESTART, ERR_COMMON_NO_SUCH_CHARACTER, pMsgNotifyGameStart->CharacterUniqueNumber);
		return RES_BREAK;
	}

	pCharacterSocket->GameStartRoutineIM(pMsgNotifyGameStart, this);

// 2006-05-09 by cmkwon, GameStartRoutineIM()ÇÔĽöżˇĽ­ Ăł¸®
// 	if(COMPARE_RACE(pCharacterSocket->m_character.Race, RACE_OPERATION|RACE_GAMEMASTER))
// 	{// 2006-05-08 by cmkwon, °ü¸®ŔÚ/żîżµŔÚ´Â ±Ó¸»˛ű »óĹÂ·Î ˝ĂŔŰµĘ
// 		CLEAR_CHATTYPE_BIT(pCharacterSocket->m_enableChatFlag8, CHAT_TYPE_PTOP);
// 
// 		///////////////////////////////////////////////////////////////////////////////
// 		// 2006-05-08 by cmkwon
// 		ms_pIMIOCP->AddToAdminCharacter(pCharacterSocket->GetCharacter()->CharacterUniqueNumber);
// 		
// 		///////////////////////////////////////////////////////////////////////////////
// 		// 2006-05-08 by cmkwon
// 		int nCallGMCnt = ms_pIMIOCP->m_mtvectSCallGMList.size();
// 		if(0 < nCallGMCnt)
// 		{
// 			INIT_MSG_WITH_BUFFER(MSG_IC_ADMIN_CALLGM_INFO_OK, T_IC_ADMIN_CALLGM_INFO_OK, pCallGMInfo, SendBuf);
// 			pCallGMInfo->nCallGMTotalCount	= ms_pIMIOCP->m_mtvectSCallGMList.size();
// 			pCharacterSocket->SendAddData(SendBuf, MSG_SIZE(MSG_IC_ADMIN_CALLGM_INFO_OK));
// 		}
// 	}
// 
// 	pCharacterSocket->m_bGameEndRoutineFlag			= TRUE;
// 
// 	///////////////////////////////////////////////////////////////////////////
// 	// MAP_CHANNEL_INDEX ÇŇ´ç	
// 	pCharacterSocket->m_character.MapChannelIndex	= pMsgNotifyGameStart->MapChannelIndex;
// 
// 	///////////////////////////////////////////////////////////////////////////
// 	// ±ćµĺ Á¤ş¸ ·Îµů	
// 	if(INVALID_GUILD_UID != pCharacterSocket->m_character.GuildUniqueNumber)
// 	{
// 		ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildLoadGuild, this, NULL, NULL, pMsgNotifyGameStart->CharacterUniqueNumber);
// 	}	
// 
// 	///////////////////////////////////////////////////////////////////////////
// 	// ŔĚŔü ĆÄĆĽ °Ë»ç
// 	///////////////////////////////////////////////////////////////////////////
// 	if (pCharacterSocket->m_character.LastPartyID != 0)
// 	{
// #ifdef _DEBUG
// 		pCharacterSocket->SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0005);
// #endif
// 		// ĆÄĆĽ °Ë»ö
// 		CIMParty *pParty = ms_pIMIOCP->FindParty(pCharacterSocket->m_character.LastPartyID);
// 		if (pParty == NULL)
// 		{
// 			// ĆÄĆĽ°ˇ ŔĚąĚ ÇŘ»ęµČ °ćżě
// 			pCharacterSocket->SendErrorMessage(T_FI_CONNECT_NOTIFY_GAMESTART, ERR_PROTOCOL_NO_SUCH_PARTY, pCharacterSocket->m_character.LastPartyID);
// 
// 			pCharacterSocket->SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0006);
// 
// 			// Update DB
// 			QPARAM_SAVELASTPARTYID *pQSavePartyID = new QPARAM_SAVELASTPARTYID;
// 			pQSavePartyID->CharacterUniqueNumber = pCharacterSocket->m_character.CharacterUniqueNumber;
// 			pQSavePartyID->LastPartyID = 0;
// 			ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_SaveLastPartyID, this, pQSavePartyID);
// 
// 			// Update CHARACTER
// 			pCharacterSocket->m_character.LastPartyID = 0;
// 			return RES_BREAK;
// 		}
// 		
// 		pParty->LockIMParty();
// 		{
// 			IMPartyMember *pMember = pParty->GetMemberPtrByCharacterUniqueNumber(pMsgNotifyGameStart->CharacterUniqueNumber);
// 			if (pMember == NULL)
// 			{
// 				// ĆÄĆĽżřŔĚ ľř´Â °ćżě
// 				pParty->UnlockIMParty();
// 				SendErrorMessage(T_FI_CONNECT_NOTIFY_GAMESTART, ERR_PROTOCOL_NO_SUCH_PARTY_MEMBER, pMsgNotifyGameStart->CharacterUniqueNumber);
// 				return RES_BREAK;
// 			}
// 
// 			// »çżëŔÚżˇ°Ô ĆÄĆĽ Á¤ş¸ ÇŇ´ç
// 			pCharacterSocket->SetIMParty(pParty);
// 
// 			// validate party member
// 			pMember->SetOnlineMember(pCharacterSocket);
// 
// 			// ŔÚ±â ŔÚ˝Ĺżˇ°Ô ş¸łż
// 			INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_PUT_LAST_PARTY_INFO, T_IC_PARTY_PUT_LAST_PARTY_INFO, pLastPartyInfo, pLastPartyInfoBuf);
// 			pLastPartyInfo->PartyID = pParty->m_PartyID;
// 			pCharacterSocket->SendAddData(pLastPartyInfoBuf, MSG_SIZE(MSG_IC_PARTY_PUT_LAST_PARTY_INFO));
// 
// 			pCharacterSocket->SendString128(STRING_128_USER_NOTICE, STRMSG_S_I2NOTIFY_0007);
// 		}
// 		pParty->UnlockIMParty();
// 	}

	// 2008-03-03 by dhjin, ľĆ·ąłŞ ĹëÇŐ - ľĆ·ąłŞ Ľ­ąö ŔĎ¶§¸¸ ŔŻŔúżˇ°Ô FM <-> IMĽ­ąö Á˘ĽÓ Ľş°řŔ» ş¸ł˝´Ů
	if(ms_pIMIOCP->m_bArenaServerCheck)
	{
		pCharacterSocket->SendAddMessageType(T_IC_CONNECT_FM_TO_IM_OK);
	}

	return RES_RETURN_TRUE;
}


ProcessResult CIMIOCPSocket::Process_FI_CONNECT_NOTIFY_DEAD_GAMESTART(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_CONNECT_NOTIFY_DEAD_GAMESTART, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	int										nRecvTypeSize = 0;
	MSG_FI_CONNECT_NOTIFY_DEAD_GAMESTART	*pDeadGameStart;

	nRecvTypeSize = sizeof(MSG_FI_CONNECT_NOTIFY_DEAD_GAMESTART);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		SendErrorMessage(T_FI_CONNECT_NOTIFY_DEAD_GAMESTART, ERR_PROTOCOL_INVALID_FIELD_DATA);
		return RES_PACKET_ERROR;
	}
	pDeadGameStart = (MSG_FI_CONNECT_NOTIFY_DEAD_GAMESTART*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	// »çżëŔÚ Č®ŔÎ
	CIMIOCPSocket *pCharacterSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pDeadGameStart->CharacterUniqueNumber);
	if (pCharacterSocket == NULL || FALSE == pCharacterSocket->IsUsing())
	{
		SendErrorMessage(T_FI_CONNECT_NOTIFY_DEAD_GAMESTART, ERR_COMMON_NO_SUCH_CHARACTER, pDeadGameStart->CharacterUniqueNumber);
		return RES_BREAK;
	}

	CLEAR_BODYCON_BIT(pCharacterSocket->m_character.BodyCondition, BODYCON_DEAD_MASK);
	return RES_RETURN_TRUE;
}


ProcessResult CIMIOCPSocket::Process_FI_EVENT_NOTIFY_WARP(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_EVENT_NOTIFY_WARP, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	int							nRecvTypeSize = 0;
	MSG_FI_EVENT_NOTIFY_WARP	*pMsgNotifyWarp;

	nRecvTypeSize = sizeof(MSG_FI_EVENT_NOTIFY_WARP);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		SendErrorMessage(T_FI_EVENT_NOTIFY_WARP, ERR_PROTOCOL_INVALID_FIELD_DATA);
		return RES_RETURN_FALSE;
	}
	pMsgNotifyWarp = (MSG_FI_EVENT_NOTIFY_WARP*)(pPacket+nBytesUsed);
	nRecvTypeSize += pMsgNotifyWarp->nNumOfTimer*sizeof(TIMER_EVENT_4_EXCHANGE);
	nBytesUsed += nRecvTypeSize;
	
	CIMIOCPSocket *pCharacterSocket
		= ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pMsgNotifyWarp->CharacterUniqueNumber);
	if (pCharacterSocket == NULL || FALSE == pCharacterSocket->IsUsing())
	{
		DBGOUT("[Error]ERR_COMMON_NO_SUCH_CHARACTER: %d\n", pMsgNotifyWarp->CharacterUniqueNumber);
		SendErrorMessage(T_FI_EVENT_NOTIFY_WARP, ERR_COMMON_NO_SUCH_CHARACTER, pMsgNotifyWarp->CharacterUniqueNumber);
		return RES_BREAK;
	}

	if (NULL == ms_pIMIOCP->m_pFieldServerSocket 
		|| FALSE == ms_pIMIOCP->m_pFieldServerSocket->IsUsing()
		|| FALSE == ms_pIMIOCP->m_FieldServerInfo.IsExistMapIndex(pMsgNotifyWarp->MapChannelIndex.MapIndex))
	{
		DBGOUT("[Error]ERR_PROTOCOL_NO_SUCH_MAP_SERVED: %d\n", pMsgNotifyWarp->MapChannelIndex.MapIndex);
		SendErrorMessage(T_FI_EVENT_NOTIFY_WARP, ERR_PROTOCOL_NO_SUCH_MAP_SERVED, pMsgNotifyWarp->CharacterUniqueNumber, pMsgNotifyWarp->MapChannelIndex.MapIndex);
		return RES_BREAK;
	}

	///////////////////////////////////////////////////////////////////////////////
	// ż©´ÜŔü ĂĽĹ©
	if (IS_VALID_UNIQUE_NUMBER(pCharacterSocket->m_uidGuildUniqueNumber))
	{
		ms_pIMIOCP->GuildWarCheck(pCharacterSocket->m_uidGuildUniqueNumber, pCharacterSocket->m_character.MapChannelIndex);
	}

	pCharacterSocket->m_character.MapChannelIndex = pMsgNotifyWarp->MapChannelIndex;
	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_FI_EVENT_CHAT_BLOCK(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		// 2008-12-30 by cmkwon, ÁöµµŔÚ Ă¤ĆĂ Á¦ÇŃ Ä«µĺ ±¸Çö - 
/// \author		cmkwon
/// \date		2008-12-30 ~ 2008-12-30
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_FI_EVENT_CHAT_BLOCK(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_EVENT_CHAT_BLOCK, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_FI_EVENT_CHAT_BLOCK,
		MSG_FI_EVENT_CHAT_BLOCK, pRMsg);
	
	ms_pIMIOCP->m_chatBlockManagerByLeader.InsertChatBlockList(pRMsg->szBlockedCharacterName, pRMsg->nBlockedMinutes);
	return RES_RETURN_TRUE;
}

// 2004-10-29 by cmkwon, FieldServer´Â ÇĎłŞ·Î ÇČ˝şÇŃ´Ů
//ProcessResult CIMIOCPSocket::Process_FI_EVENT_GET_WARP_INFO(const char* pPacket, int nLength, int &nBytesUsed)
//{
//	int							nRecvTypeSize = 0;
//	MSG_FI_EVENT_GET_WARP_INFO	*pMsgGetWarpInfo = NULL;
//
//	nRecvTypeSize = sizeof(MSG_FI_EVENT_GET_WARP_INFO);
//	if(nLength - nBytesUsed < nRecvTypeSize)
//	{
//		SendErrorMessage(T_FI_EVENT_GET_WARP_INFO, ERR_PROTOCOL_INVALID_FIELD_DATA);
//		Close(0x1201C);
//		return RES_RETURN_FALSE;
//	}
//	pMsgGetWarpInfo = (MSG_FI_EVENT_GET_WARP_INFO*)(pPacket + nBytesUsed);
//	nBytesUsed += nRecvTypeSize;
//
//	CIMIOCPSocket *pSock = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pMsgGetWarpInfo->CharacterUniqueNumber);
//	if (pSock == NULL)
//	{
//		SendErrorMessage(T_FI_EVENT_GET_WARP_INFO, ERR_COMMON_NO_SUCH_CHARACTER, pMsgGetWarpInfo->CharacterUniqueNumber);
//		return RES_BREAK;
//	}
//
//	// żˇ·Ż°ˇ łŞ¸é ąŮ·Î ş¸ł»±â Ŕ§ÇŘ ąĚ¸® ¸¸µéľîµŇ
//	INIT_MSG_WITH_BUFFER(MSG_FI_EVENT_GET_WARP_INFO_OK, T_FI_EVENT_GET_WARP_INFO_OK, pGetOK, pGetOKBuf);
//	pGetOK->AccountUniqueNumber = pSock->m_character.AccountUniqueNumber;
//	pGetOK->CharacterUniqueNumber = pSock->m_character.CharacterUniqueNumber;
//	pGetOK->PartyID = 0;
//	pGetOK->MasterCharacterUniqueNumber = INVALID_UNIQUE_NUMBER;
//	pGetOK->nPartyMembers = 0;
//	pGetOK->nTimerEvents = 0;
//
//	int offset = MSG_SIZE(MSG_FI_EVENT_GET_WARP_INFO_OK);
//
//	// ĽŇĽÓ ĆÄĆĽ°ˇ ŔÖŔ¸¸é ÇŇ´çÇÔ
//	if (pSock->m_pIMParty != NULL)
//	{
//		pSock->m_pIMParty->m_ListPartyMember.lock();
//		pGetOK->PartyID						= pSock->m_pIMParty->m_PartyID;
//		pGetOK->MasterCharacterUniqueNumber	= pSock->m_pIMParty->GetMasterCharacterUniqueNumber();
//		pGetOK->nPartyMembers				= pSock->m_pIMParty->GetMemberCount(FALSE);
//
//		mt_list<IMPartyMember>::iterator itr = pSock->m_pIMParty->m_ListPartyMember.begin();
//		while(itr != pSock->m_pIMParty->m_ListPartyMember.end())
//		{
//			IMPartyMember &tmpPartyMember = *itr;
//			FI_PARTY_MEMBER_INFO *pPartyMember = (FI_PARTY_MEMBER_INFO*)(pGetOKBuf + offset);
//			pPartyMember->CharacterUniqueNumber = tmpPartyMember.PM_CharacterUniqueNumber;
//
//			offset += sizeof(FI_PARTY_MEMBER_INFO);
//			itr++;
//		}
//		pSock->m_pIMParty->m_ListPartyMember.unlock();
//#ifdef _DEBUG
//		// offset Č®ŔÎ
//		assert(offset
//			== (MSG_SIZE(MSG_FI_EVENT_GET_WARP_INFO_OK) + sizeof(FI_PARTY_MEMBER_INFO)*pSock->m_pIMParty->GetMemberCount(FALSE))
//		);
//#endif
//	}
//
//
//#ifdef _DEBUG
//	// offset Č®ŔÎ
//	assert(offset == (MSG_SIZE(MSG_FI_EVENT_GET_WARP_INFO_OK) +
//						sizeof(FI_PARTY_MEMBER_INFO)*pGetOK->nPartyMembers +
//						sizeof(TIMER_EVENT_4_EXCHANGE)*pGetOK->nTimerEvents)
//	);
//#endif
//
//	// ŔüĽŰ
//	SendAddData(pGetOKBuf, offset);
//
//	return RES_RETURN_TRUE;
//}

ProcessResult CIMIOCPSocket::Process_FI_CONNECT_NOTIFY_FIELDSERVER_IP(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_CONNECT_NOTIFY_FIELDSERVER_IP, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	int								nRecvTypeSize = 0;
	MSG_FI_CONNECT_NOTIFY_FIELDSERVER_IP	*pMsgNotifyIP;

	nRecvTypeSize = sizeof(MSG_FI_CONNECT_NOTIFY_FIELDSERVER_IP);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		SendErrorMessage(T_FI_CONNECT_NOTIFY_FIELDSERVER_IP, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12018);
		return RES_RETURN_FALSE;
	}
	pMsgNotifyIP = (MSG_FI_CONNECT_NOTIFY_FIELDSERVER_IP*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMIOCPSocket *pCharacterSocket
		= ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pMsgNotifyIP->CharacterUniqueNumber);
	if (pCharacterSocket == NULL)
	{
		DBGOUT("[Error]ERR_COMMON_NO_SUCH_CHARACTER: %d\n", pMsgNotifyIP->CharacterUniqueNumber);
		SendErrorMessage(T_FI_CONNECT_NOTIFY_FIELDSERVER_IP, ERR_COMMON_NO_SUCH_CHARACTER, pMsgNotifyIP->CharacterUniqueNumber);
		return RES_BREAK;
	}

	if(NULL == ms_pIMIOCP->m_pFieldServerSocket
		|| FALSE == ms_pIMIOCP->m_pFieldServerSocket->IsUsing()
		|| ms_pIMIOCP->m_FieldServerInfo.serverID != pMsgNotifyIP->FieldServerID)
	{
		DBGOUT("[Error]ERR_PROTOCOL_NO_SUCH_FIELD_SERVER: %s\n", pMsgNotifyIP->FieldServerID.GetString(string()));
		SendErrorMessage(T_FI_CONNECT_NOTIFY_FIELDSERVER_IP, ERR_PROTOCOL_NO_SUCH_FIELD_SERVER, pMsgNotifyIP->FieldServerID.port, 0, pMsgNotifyIP->FieldServerID.IP);
		return RES_BREAK;
	}

	if ( pCharacterSocket->m_pCurrentFieldServerSocket != this)
	{
		pCharacterSocket->SetCurrentFieldServerSocket(this);
	}

	DBGOUT("%s's FieldServer IP Notified: %s\n", pCharacterSocket->m_character.AccountName, pMsgNotifyIP->FieldServerID.GetString(string()));
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_PARTY_CREATE(const char* pPacket, int nLength, int &nBytesUsed)
{
	int					nRecvTypeSize;
	MSG_IC_PARTY_CREATE	*pMsgPartyCreate;

	nRecvTypeSize = sizeof(MSG_IC_PARTY_CREATE);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		SendErrorMessage(T_IC_PARTY_CREATE, ERR_PROTOCOL_INVALID_FIELD_DATA);
		return RES_RETURN_FALSE;
	}
	pMsgPartyCreate = (MSG_IC_PARTY_CREATE*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	if (NULL != this->GetIMParty())
	{
		// error
		SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0008);
		SendErrorMessage(T_IC_PARTY_CREATE, ERR_PROTOCOL_PARTY_MEMBER_ALREADY_EXISTS);
		return RES_BREAK;
	}
	
	//////////////////////////////////////////////////////////////////////////
	// 2008-06-02 by dhjin, EP3 Ćí´ë ĽöÁ¤ - Ćí´ë Á¤ş¸
	if(pMsgPartyCreate->PartyInfo.PartyInfluence != this->m_character.InfluenceType)
	{// 2008-06-02 by dhjin, ĽĽ·ÂŔĚ żŔ·ů
		SendErrorMessage(T_IC_PARTY_CREATE, ERR_PROTOCOL_REQ_INFLUENCE_NOT_MATCHED, pMsgPartyCreate->PartyInfo.PartyInfluence, this->m_character.InfluenceType);	// 2008-12-04 by cmkwon, Á¤ş¸ Ăß°ˇ
		return RES_BREAK;		
	}

	// 2008-06-02 by dhjin, EP3 Ćí´ë ĽöÁ¤ - Ćí´ë Á¤ş¸
	// 2009-12-29 by cmkwon, Äł¸ŻĹÍ ĂÖ´ë ·ąş§ »óÇâ(110Ŕ¸·Î) - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
	//if(pMsgPartyCreate->PartyInfo.MinLevel > pMsgPartyCreate->PartyInfo.MaxLevel
	//	|| pMsgPartyCreate->PartyInfo.MinLevel <= 0
	//	|| pMsgPartyCreate->PartyInfo.MaxLevel > 100
	//	)
	///////////////////////////////////////////////////////////////////////////////
	// 2009-12-29 by cmkwon, Äł¸ŻĹÍ ĂÖ´ë ·ąş§ »óÇâ(110Ŕ¸·Î) - 
	if(pMsgPartyCreate->PartyInfo.MinLevel > pMsgPartyCreate->PartyInfo.MaxLevel
		|| FALSE == IS_IN_RANGE(1, pMsgPartyCreate->PartyInfo.MinLevel, CHARACTER_LEVEL_110)
		|| FALSE == IS_IN_RANGE(1, pMsgPartyCreate->PartyInfo.MaxLevel, CHARACTER_LEVEL_110)
		)
	{
		SendErrorMessage(T_IC_PARTY_CREATE, ERR_PROTOCOL_REQ_LEVEL_NOT_MATCHED, pMsgPartyCreate->PartyInfo.MinLevel, pMsgPartyCreate->PartyInfo.MaxLevel);
		return RES_BREAK;
	}

	CIMParty *ptmParty = new CIMParty(ms_pIMIOCP, m_character.CharacterUniqueNumber, timeGetTime(), ms_pIMIOCP->m_UID32GenPartyID.GetNext(), &(pMsgPartyCreate->PartyInfo) );	// 2008-06-02 by dhjin, EP3 Ćí´ë ĽöÁ¤ - Ćí´ë Á¤ş¸
	IMPartyMember tmMember(m_character.CharacterName, m_character.CharacterUniqueNumber, this, ptmParty);
	ptmParty->PushBackMember(tmMember);

	this->SetIMParty(ptmParty);

	// 2008-02-28 by dhjin,	ľĆ·ąłŞ ĹëÇŐ - LastPartyIDżˇ °ŞŔ» ŔúŔĺÇŃ´Ů.
	this->m_character.LastPartyID	= ptmParty->m_PartyID;

	// Make msg
	INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_CREATE_OK, T_FI_PARTY_CREATE_OK, pMsgPartyCreateOK, pMsgPartyCreateOKBuf);
	pMsgPartyCreateOK->CharacterUniqueNumber = pMsgPartyCreate->CharacterUniqueNumber;
	pMsgPartyCreateOK->PartyID = ptmParty->m_PartyID;
	pMsgPartyCreateOK->FieldPartyInfo.nTotalPartyMember = 1;					// ĂĘ±âżŁ 1¸í
	pMsgPartyCreateOK->FieldPartyInfo.lowestMemberLevel = m_character.Level;	// ĂĘ±âżŁ ŔÚ˝ĹŔÇ level
	pMsgPartyCreateOK->ExpDistributeType	= pMsgPartyCreate->PartyInfo.ExpDistributeType;			// 2008-06-03 by dhjin, EP3 Ćí´ë ĽöÁ¤
	pMsgPartyCreateOK->ItemDistributeType	= pMsgPartyCreate->PartyInfo.ItemDistributeType;		// 2008-06-03 by dhjin, EP3 Ćí´ë ĽöÁ¤


	// ĆÄĆĽµéŔÇ listżˇ Ăß°ˇ
	ms_pIMIOCP->InsertParty(ptmParty);

	if (FALSE == this->SendMsgToCurrentFieldServerSocket(pMsgPartyCreateOKBuf, MSG_SIZE(MSG_FI_PARTY_CREATE_OK)))
	{
		// żˇ·ŻŔĚąÇ·Î ĆÄĆĽµéŔÇ listżˇĽ­ »čÁ¦
		ms_pIMIOCP->DeleteParty(ptmParty->m_PartyID);

		// current field server socketŔĚ NULLŔĚ¸é, Á˘ĽÓŔ» ˛÷´Â´Ů.
		char	szSystemLog[1024];
		sprintf(szSystemLog, "  Fatal Error: CurrentFieldServerSocket is NULL, %s at %s\r\n",
			GetCharacterString(&m_character, string()), GetProtocolTypeString(T_IC_PARTY_CREATE));
		DBGOUT(szSystemLog);
		g_pGlobal->WriteSystemLog(szSystemLog);

		return RES_RETURN_FALSE;
	}

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_PARTY_GET_MEMBER(const char* pPacket, int nLength, int &nBytesUsed)
{
	int						nRecvTypeSize;
	MSG_IC_PARTY_GET_MEMBER	*pGetMember;

	nRecvTypeSize = sizeof(MSG_IC_PARTY_GET_MEMBER);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		SendErrorMessage(T_IC_PARTY_GET_MEMBER, ERR_PROTOCOL_INVALID_FIELD_DATA);
		return RES_RETURN_FALSE;
	}
	pGetMember = (MSG_IC_PARTY_GET_MEMBER*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMParty *ptmParty = this->GetIMParty();
	if (NULL == ptmParty)
	{
		SendErrorMessage(T_IC_PARTY_GET_MEMBER, ERR_PROTOCOL_NO_SUCH_PARTY);
		return RES_BREAK;
	}

	ptmParty->LockIMParty();
	// init msg
	INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_PUT_MEMBER, T_IC_PARTY_PUT_MEMBER, pPutMember, pPutMemberBuf);
	{
		IMPartyMember *pMember = ptmParty->GetMemberPtrByCharacterUniqueNumber(pGetMember->CharacterUniqueNumber);
		if (NULL == pMember)
		{
			ptmParty->UnlockIMParty();
			SendErrorMessage(T_IC_PARTY_GET_MEMBER, ERR_PROTOCOL_NO_SUCH_PARTY_MEMBER);
			return RES_BREAK;
		}

		if (pMember->IsOnlineMember())
		{
			
			if (FALSE == pMember->PM_pIMIOCPSocket->IsUsing()
				|| pMember->PM_CharacterUniqueNumber != pMember->PM_pIMIOCPSocket->m_character.CharacterUniqueNumber)
			{
				ptmParty->UnlockIMParty();
				SendErrorMessage(T_IC_PARTY_GET_MEMBER, ERR_COMMON_NO_SUCH_CHARACTER);
				g_pGlobal->WriteSystemLogEX(TRUE, STRMSG_S_I2NOTIFY_0009);
				return RES_BREAK;
			}

			pPutMember->IMPartyMemberInfo = pMember->PM_pIMIOCPSocket->m_character;
			pPutMember->IMPartyMemberInfo.VoipType = pMember->PM_pIMIOCPSocket->m_VoipType;		// 2008-07-16 by dhjin, EP3 - Voip Á¤ş¸
		}
		else
		{
			pPutMember->IMPartyMemberInfo = *pMember;
			pPutMember->IMPartyMemberInfo.VoipType = CHECK_TYPE_VOIP_NONE;		// 2008-07-16 by dhjin, EP3 - Voip Á¤ş¸	
		}
	}
	ptmParty->UnlockIMParty();


	// µĄŔĚĹÍ ŔüĽŰ
	SendAddData(pPutMemberBuf, MSG_SIZE(MSG_IC_PARTY_PUT_MEMBER));

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_PARTY_GET_ALL_MEMBER(const char* pPacket, int nLength, int &nBytesUsed)
{
	int							nRecvTypeSize;
	MSG_IC_PARTY_GET_ALL_MEMBER	*pGetAllMember;

	nRecvTypeSize = sizeof(MSG_IC_PARTY_GET_ALL_MEMBER);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		SendErrorMessage(T_IC_PARTY_GET_ALL_MEMBER, ERR_PROTOCOL_INVALID_FIELD_DATA);
		return RES_RETURN_FALSE;
	}
	pGetAllMember = (MSG_IC_PARTY_GET_ALL_MEMBER*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMParty *ptmParty = this->GetIMParty();
	if (NULL == ptmParty)
	{
		SendErrorMessage(T_IC_PARTY_GET_ALL_MEMBER, ERR_PROTOCOL_NO_SUCH_PARTY);
		return RES_BREAK;
	}

	// init msg
	INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_PUT_ALL_MEMBER, T_IC_PARTY_PUT_ALL_MEMBER, pPutAllMember, pPutAllMemberBuf);
	
	ptmParty->LockIMParty();	
	{
		pPutAllMember->PartyID				= ptmParty->m_PartyID;
		pPutAllMember->MasterUniqueNumber	= ptmParty->GetMasterCharacterUniqueNumber();

		// µĄŔĚĹÍ ÇŇ´ç
		ptmParty->MakeMsg_IC_PARTY_PUT_ALL_MEMBER(&pPutAllMember->nNumOfPartyMembers, (IM_PARTY_MEMBER_INFO*)(pPutAllMemberBuf+MSG_SIZE(MSG_IC_PARTY_PUT_ALL_MEMBER)), m_character.CharacterUniqueNumber);

// 2004-10-29 by cmkwon, Ŕ§ŔÇ ÇÔĽöżˇĽ­ Ăł¸®
// µĄŔĚĹÍ ÇŇ´ç
//		pPutAllMember->nNumOfPartyMembers = m_pIMParty->GetMemberCount(FALSE) - 1;	// ŔÚ±â ŔÚ˝ĹŔş Á¦żÜÇÔ
//		IM_PARTY_MEMBER_INFO *ArrIMPartyInfo = (IM_PARTY_MEMBER_INFO*)(pPutAllMemberBuf+MSG_SIZE(MSG_IC_PARTY_PUT_ALL_MEMBER));
//		list<IMPartyMember>::iterator itr = m_pIMParty->m_ListPartyMember.begin();
//		int count = 0;
//		while(m_pIMParty->m_ListPartyMember.end() != itr)
//		{
//			IMPartyMember &tmpPartyMember = *itr;
//
//			// ŔÚ˝ĹŔÇ Á¤ş¸´Â ş¸ł»Áö ľĘŔ˝
//			if (tmpPartyMember.PM_CharacterUniqueNumber != m_character.CharacterUniqueNumber)
//			{
//				if (tmpPartyMember.IsOnlineMember())
//				{
//					CIMIOCPSocket *pSocket = tmpPartyMember.PM_pIMIOCPSocket;
//					if (NULL == pSocket 
//						|| FALSE == pSocket->IsUsing()
//						|| tmpPartyMember.PM_CharacterUniqueNumber != pSocket->m_character.CharacterUniqueNumber)
//					{
//						m_pIMParty->UnlockIMParty();	// unlock
//
//						SendErrorMessage(T_IC_PARTY_GET_ALL_MEMBER, ERR_COMMON_NO_SUCH_CHARACTER);
//						g_pGlobal->WriteSystemLogEX(TRUE, "  Fatal Error: ˝É°˘ÇŃ ĆÄĆĽ żˇ·Ż! at T_IC_PARTY_GET_MEMBER");
//						return RES_BREAK;
//					}
//
//					ArrIMPartyInfo[count] = pSocket->m_character;
//				}
//				else
//				{
//					ArrIMPartyInfo[count] = tmpPartyMember;
//				}
//
//				count++;
//			}
//
//			itr++;
//		}
	}// LockIMParty_end
	ptmParty->UnlockIMParty();	// unlock

	// µĄŔĚĹÍ ŔüĽŰ
	SendAddData(pPutAllMemberBuf,
		MSG_SIZE(MSG_IC_PARTY_PUT_ALL_MEMBER) +
		pPutAllMember->nNumOfPartyMembers*sizeof(IM_PARTY_MEMBER_INFO));

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_FI_PARTY_ACCEPT_INVITE_OK(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_PARTY_ACCEPT_INVITE_OK, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	int								nRecvTypeSize = 0;
	MSG_FI_PARTY_ACCEPT_INVITE_OK	*pMsgAcceptOK;

	nRecvTypeSize = sizeof(MSG_FI_PARTY_ACCEPT_INVITE_OK);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		SendErrorMessage(T_FI_PARTY_ACCEPT_INVITE_OK, ERR_PROTOCOL_INVALID_FIELD_DATA);
		return RES_PACKET_ERROR;
	}
	pMsgAcceptOK = (MSG_FI_PARTY_ACCEPT_INVITE_OK*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMIOCPSocket *pInviteeSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pMsgAcceptOK->CharacterUniqueNumber);
	if(NULL == pInviteeSocket || FALSE == pInviteeSocket->IsUsing())
	{
		return RES_BREAK;
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2009-12-03 by cmkwon, Ćí´ë 2Áß ĂĘ´ë ąö±× ĽöÁ¤(E014153) - 
	if(pInviteeSocket->GetIMParty())
	{
		SendErrorMessage(T_FI_PARTY_ACCEPT_INVITE_OK, ERR_PROTOCOL_PARTY_MEMBER_ALREADY_EXISTS);	
		return RES_BREAK;
	}
	
	if(this->IsCharacterNameFromRejectList(pInviteeSocket->m_character.CharacterName)
		|| pInviteeSocket->IsCharacterNameFromRejectList(this->m_character.CharacterName))
	{// 2008-07-03 by dhjin, EP3 °ĹşÎ¸ń·Ď -
		SendErrorMessage(T_FI_PARTY_ACCEPT_INVITE_OK, ERR_FRIEND_REGISTERED_PEER_REJECTLIST);	
		return RES_BREAK;
	}
	
	CIMParty *ptmParty = ms_pIMIOCP->FindParty(pMsgAcceptOK->PartyID);
	if (ptmParty == NULL)
	{
		SendErrorMessage(T_FI_PARTY_ACCEPT_INVITE_OK, ERR_PROTOCOL_NO_SUCH_PARTY, pMsgAcceptOK->PartyID, pMsgAcceptOK->CharacterUniqueNumber);
		return RES_BREAK;
	}
	
	ptmParty->LockIMParty();

	if ( ptmParty->GetMemberCount(FALSE) >= SIZE_MAX_PARTY_MEMBER )
	{	// ż©±âĽ­ ÇŃąř´ő Á¦ÇŃ ŔÎżř ĂĽĹ© ÇĎµµ·Ď ĽöÁ¤.
		SendErrorMessage(T_FI_PARTY_ACCEPT_INVITE_OK , ERR_PROTOCOL_ALREADY_MAX_PARTY_MEMBER , pMsgAcceptOK->PartyID , pMsgAcceptOK->CharacterUniqueNumber );

		pInviteeSocket->SendErrorMessage ( T_FI_PARTY_ACCEPT_INVITE_OK , ERR_PROTOCOL_ALREADY_MAX_PARTY_MEMBER );

		ptmParty->UnlockIMParty();

		return RES_BREAK;
	}
	else
	{
		// party ÇŇ´ç
		pInviteeSocket->SetIMParty(ptmParty);

		// partyżř »đŔÔ
		IMPartyMember tmMember(pInviteeSocket->m_character.CharacterName, pInviteeSocket->m_character.CharacterUniqueNumber, pInviteeSocket, ptmParty);
		ptmParty->PushBackMember(tmMember);

		// 2008-02-28 by dhjin,	ľĆ·ąłŞ ĹëÇŐ - LastPartyIDżˇ °ŞŔ» ŔúŔĺÇŃ´Ů.
		pInviteeSocket->m_character.LastPartyID	= pMsgAcceptOK->PartyID;

		// send to SERVER
		INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_ACCEPT_INVITE_OK, T_FI_PARTY_ACCEPT_INVITE_OK, pInviteOKFI, pInviteOKFIBuf);
		pInviteOKFI->PartyID				= pMsgAcceptOK->PartyID;
		pInviteOKFI->CharacterUniqueNumber	= pMsgAcceptOK->CharacterUniqueNumber;
		pInviteOKFI->FieldPartyInfo.nTotalPartyMember = ptmParty->GetMemberCount(FALSE);		// ÁÖŔÇ: m_ListPartyMemberżˇ »đŔÔ ŔĚČÄż©ľß ÇÔ!
		pInviteOKFI->FieldPartyInfo.lowestMemberLevel = ptmParty->GetLowestMemberLevel();		// ÁÖŔÇ: m_ListPartyMemberżˇ »đŔÔ ŔĚČÄż©ľß ÇÔ!
		pInviteeSocket->SendMsgToCurrentFieldServerSocket(pInviteOKFIBuf, MSG_SIZE(MSG_FI_PARTY_ACCEPT_INVITE_OK));

		// clientµéżˇ°Ô ŔüĽŰÇĎ±â
		INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_ACCEPT_INVITE_OK, T_IC_PARTY_ACCEPT_INVITE_OK, pInviteOKIC, pInviteOKICBuf);
		pInviteOKIC->PartyID			= pMsgAcceptOK->PartyID;
		pInviteOKIC->IMPartyMemberInfo	= pInviteeSocket->m_character;
		ptmParty->SendMsgToMembers(pInviteOKICBuf, MSG_SIZE(MSG_IC_PARTY_ACCEPT_INVITE_OK));

		// 2008-06-10 by dhjin, EP3 Ćí´ë ĽöÁ¤ - Ćí´ë Á¤ş¸ ŔüĽŰ
		INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_INFO, T_IC_PARTY_INFO, pSendMsg, pSendBuf);
		pSendMsg->PartyInfo = ptmParty->m_SPartyInfo;
		pInviteeSocket->SendAddData(pSendBuf, MSG_SIZE(MSG_IC_PARTY_INFO));
	}
	ptmParty->UnlockIMParty();

//#ifdef _DEBUG
//	char *pdm = NULL;
//	pInviteeSocket->PRINT_DEBUG_MESSAGE(pdm);
//#endif
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_FI_PARTY_UPDATE_ITEM_POS(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_PARTY_UPDATE_ITEM_POS, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	int								nRecvTypeSize = 0;
	MSG_FI_PARTY_UPDATE_ITEM_POS	*pFIUpdateItemPos;

	nRecvTypeSize = sizeof(MSG_FI_PARTY_UPDATE_ITEM_POS);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		SendErrorMessage(T_FI_PARTY_UPDATE_ITEM_POS, ERR_PROTOCOL_INVALID_FIELD_DATA);
		return RES_PACKET_ERROR;
	}
	pFIUpdateItemPos = (MSG_FI_PARTY_UPDATE_ITEM_POS*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	// »çżëŔÚ Č®ŔÎ
	CIMIOCPSocket *pCharacterSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pFIUpdateItemPos->CharacterUniqueNumber);
	if (pCharacterSocket == NULL || FALSE == pCharacterSocket->IsUsing())
	{
		SendErrorMessage(T_FI_PARTY_UPDATE_ITEM_POS, ERR_COMMON_NO_SUCH_CHARACTER, pFIUpdateItemPos->CharacterUniqueNumber);
		return RES_BREAK;
	}

	// ĆÄĆĽ Č®ŔÎ
	CIMParty *pParty = pCharacterSocket->GetIMParty();
	if (pParty == NULL)
	{
		SendErrorMessage(T_FI_PARTY_UPDATE_ITEM_POS, ERR_PROTOCOL_NO_SUCH_PARTY, pFIUpdateItemPos->CharacterUniqueNumber);
		return RES_BREAK;
	}
	pParty->LockIMParty();
	{
		INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_UPDATE_ITEM_POS, T_IC_PARTY_UPDATE_ITEM_POS, pICUpdateItemPos, pICUpdateItemPosBuf);
		pICUpdateItemPos->CharacterUniqueNumber = pFIUpdateItemPos->CharacterUniqueNumber;
		pICUpdateItemPos->ItemPosition			= pFIUpdateItemPos->ItemPosition;
		pICUpdateItemPos->ItemNum				= pFIUpdateItemPos->ItemNum;
		// 2009-08-26 by cmkwon, ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö - 
		//pICUpdateItemPos->ColorCode				= pFIUpdateItemPos->ColorCode;		// 2005-12-08 by cmkwon, ľĆ¸Ó »ö»ó Ć©´× Á¤ş¸
		pICUpdateItemPos->nShapeItemNum			= pFIUpdateItemPos->nShapeItemNum;		// 2009-08-26 by cmkwon, ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö - 
		pICUpdateItemPos->nEffectItemNum		= pFIUpdateItemPos->nEffectItemNum;		// 2009-08-26 by cmkwon, ±×·ˇÇČ ¸®ĽŇ˝ş şŻ°ć ˝Ă˝şĹŰ ±¸Çö - 
		pParty->SendMsgToMembers(pICUpdateItemPosBuf, MSG_SIZE(MSG_IC_PARTY_UPDATE_ITEM_POS));
	}
	pParty->UnlockIMParty();

	return RES_RETURN_TRUE;
}

///*
//ProcessResult CIMIOCPSocket::Process_FI_PARTY_GET_PARTY_INFO(const char* pPacket, int nLength, int &nBytesUsed)
//{
//	int							nRecvTypeSize = 0;
//	MSG_FI_PARTY_GET_PARTY_INFO	*pMsgGetPartyInfo = NULL;
//
//	nRecvTypeSize = sizeof(MSG_FI_PARTY_GET_PARTY_INFO);
//	if(nLength - nBytesUsed < nRecvTypeSize)
//	{
//		SendErrorMessage(T_FI_PARTY_GET_PARTY_INFO, ERR_PROTOCOL_INVALID_FIELD_DATA);
//		Close(0x1201C);
//		return RES_RETURN_FALSE;
//	}
//	pMsgGetPartyInfo = (MSG_FI_PARTY_GET_PARTY_INFO*)(pPacket + nBytesUsed);
//	nBytesUsed += nRecvTypeSize;
//
//	CIMIOCPSocket *pSock = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pMsgGetPartyInfo->CharacterUniqueNumber);
//	if (pSock == NULL)
//	{
//		SendErrorMessage(T_FI_PARTY_GET_PARTY_INFO, ERR_COMMON_NO_SUCH_CHARACTER, pMsgGetPartyInfo->CharacterUniqueNumber);
//		return RES_BREAK;
//	}
//
//	// żˇ·Ż°ˇ łŞ¸é ąŮ·Î ş¸ł»±â Ŕ§ÇŘ ąĚ¸® ¸¸µéľîµŇ
//	INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_GET_PARTY_INFO_OK, T_FI_PARTY_GET_PARTY_INFO_OK, pGetOK, pGetOKBuf);
//	pGetOK->AccountUniqueNumber = pSock->m_character.AccountUniqueNumber;
//	pGetOK->CharacterUniqueNumber = pSock->m_character.CharacterUniqueNumber;
//	pGetOK->MasterCharacterUniqueNumber = INVALID_UNIQUE_NUMBER;
//	pGetOK->NumOfPartyMembers = 0;
//
//	if (pSock->m_pIMParty == NULL)
//	{
//		// ĽŇĽÓ ĆÄĆĽ°ˇ ľřŔ¸ąÇ·Î ±×łÉ ş¸łż
//		SendAddData(pGetOKBuf, MSG_SIZE(MSG_FI_PARTY_GET_PARTY_INFO_OK));
//		return RES_BREAK;
//	}
//
//	// ĆÄĆĽ°ˇ ŔÖŔ¸ąÇ·Î Ă¤żöĽ­ ş¸łż
//	pGetOK->MasterCharacterUniqueNumber = pSock->m_pIMParty->GetMasterCharacterUniqueNumber();
//	pGetOK->NumOfPartyMembers = pSock->m_pIMParty->GetMemberCount(FALSE);
//
//	int offset = MSG_SIZE(MSG_FI_PARTY_GET_PARTY_INFO_OK);
//	pSock->m_pIMParty->m_ListPartyMember.lock();
//	mt_list<IMPartyMember>::iterator itr = pSock->m_pIMParty->m_ListPartyMember.begin();
//	while(itr != pSock->m_pIMParty->m_ListPartyMember.end())
//	{
//		PARTY_MEMBER_INFO *pPartyMember = (PARTY_MEMBER_INFO*)(pGetOKBuf + offset);
//		pPartyMember->CharacterUniqueNumber = itr->CharacterUniqueNumber;
//		strncpy(pPartyMember->CurrentMapName, ms_pIMIOCP->GetCharacterByArrayIndex(itr->ClientIndex)->MapName, SIZE_MAX_MAP_NAME);
//
//		offset += sizeof(PARTY_MEMBER_INFO);
//		itr++;
//	}
//	pSock->m_pIMParty->m_ListPartyMember.unlock();
//#ifdef _DEBUG
//	assert(offset
//		== (MSG_SIZE(MSG_FI_PARTY_GET_PARTY_INFO_OK) + sizeof(PARTY_MEMBER_INFO)*pSock->m_pIMParty->GetMemberCount(FALSE))
//	);
//#endif
//	SendAddData(pGetOKBuf, offset);
//
//	return RES_RETURN_TRUE;
//}
//*/

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_FI_PARTY_AUTO_CREATE(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		ŔÎÇÇ´ĎĆĽ - ŔÚµż Ćí´ë 
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_FI_PARTY_AUTO_CREATE(const char* pPacket, int nLength, int &nBytesUsed) {

	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_PARTY_AUTO_CREATE, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	int							nRecvTypeSize;
	MSG_FI_PARTY_AUTO_CREATE	* pRecvAutoCreateOK = NULL;
	nRecvTypeSize = sizeof(MSG_FI_PARTY_AUTO_CREATE);
	if(nLength - nBytesUsed < nRecvTypeSize) {
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_FI_PARTY_AUTO_CREATE, ERR_PROTOCOL_INVALID_FIELD_DATA);
		return RES_BREAK;
	}
	pRecvAutoCreateOK = (MSG_FI_PARTY_AUTO_CREATE*)(pPacket+nBytesUsed);	
	nBytesUsed += nRecvTypeSize;

	nRecvTypeSize = pRecvAutoCreateOK->PartyMemberListCount*sizeof(UID32_t);
	if(nLength-nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_FI_PARTY_AUTO_CREATE, ERR_PROTOCOL_INVALID_FIELD_DATA);
		return RES_BREAK;
	}
	UID32_t *pPartyMemberUID = (UID32_t*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMIOCPSocket * pMasterIMSoc = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pRecvAutoCreateOK->MasterUID);
	if (NULL == pMasterIMSoc) {
		SendErrorMessage(T_FI_PARTY_AUTO_CREATE, ERR_COMMON_NO_SUCH_CHARACTER, pRecvAutoCreateOK->MasterUID);
		return RES_BREAK;
	}

	SPARTY_INFO PartyInfo;
	MEMSET_ZERO(&PartyInfo, sizeof(SPARTY_INFO));
	PartyInfo.PartyInfluence = pMasterIMSoc->m_character.InfluenceType;
	PartyInfo.MinLevel	= 1;
	PartyInfo.MaxLevel  = CHARACTER_LEVEL_110;

	// ĆÄĆĽ »ýĽş
	CIMParty *ptmParty = new CIMParty(ms_pIMIOCP, pMasterIMSoc->m_character.CharacterUniqueNumber, timeGetTime(), ms_pIMIOCP->m_UID32GenPartyID.GetNext(), &PartyInfo);	// 2008-06-02 by dhjin, EP3 Ćí´ë ĽöÁ¤ - Ćí´ë Á¤ş¸
	ptmParty->LockIMParty();
	IMPartyMember tmMasterMember(pMasterIMSoc->m_character.CharacterName, pMasterIMSoc->m_character.CharacterUniqueNumber, pMasterIMSoc, ptmParty);
	ptmParty->PushBackMember(tmMasterMember);
	
	pMasterIMSoc->SetIMParty(ptmParty);
	// ĆÄĆĽµéŔÇ listżˇ Ăß°ˇ
	ms_pIMIOCP->InsertParty(ptmParty);

	// 2008-02-28 by dhjin,	ľĆ·ąłŞ ĹëÇŐ - LastPartyIDżˇ °ŞŔ» ŔúŔĺÇŃ´Ů.
	pMasterIMSoc->m_character.LastPartyID	= ptmParty->m_PartyID;
	
	// Make msg
	INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_AUTO_CREATE_OK, T_FI_PARTY_AUTO_CREATE_OK, pMsgPartyCreateOK, pMsgPartyCreateOKBuf);
	pMsgPartyCreateOK->FieldPartyInfo.nTotalPartyMember = pRecvAutoCreateOK->PartyMemberListCount + 1;
	pMsgPartyCreateOK->FieldPartyInfo.lowestMemberLevel = 1;
	pMsgPartyCreateOK->MasterUID						= pRecvAutoCreateOK->MasterUID;
	pMsgPartyCreateOK->PartyID							= ptmParty->m_PartyID;

	// ¸Éąö Ăß°ˇ
	UID32_t * pRecvPartyMemberUID = (UID32_t*)( ((BYTE*)pRecvAutoCreateOK) + sizeof(MSG_FI_PARTY_AUTO_CREATE));
	int PartyMemberListCount = 0;
	UID32_t * PartyMemberUID = (UID32_t*)(pMsgPartyCreateOKBuf+MSG_SIZE(MSG_FI_PARTY_AUTO_CREATE_OK));
	for(int i=0; i < pRecvAutoCreateOK->PartyMemberListCount; i++) {
		CIMIOCPSocket * pMemberIMSoc = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pRecvPartyMemberUID[i]);
		if(NULL != pMemberIMSoc) {
			// party ÇŇ´ç
			pMemberIMSoc->SetIMParty(ptmParty);
			// partyżř »đŔÔ
			IMPartyMember tmMember(pMemberIMSoc->m_character.CharacterName, pMemberIMSoc->m_character.CharacterUniqueNumber, pMemberIMSoc, ptmParty);
			ptmParty->PushBackMember(tmMember);
			// 2008-02-28 by dhjin,	ľĆ·ąłŞ ĹëÇŐ - LastPartyIDżˇ °ŞŔ» ŔúŔĺÇŃ´Ů.
			pMemberIMSoc->m_character.LastPartyID	= ptmParty->m_PartyID;
			// ¸ŢĽĽÁö ĆÄĆĽżř Á¤ş¸ 
			PartyMemberUID[PartyMemberListCount] = pMemberIMSoc->m_character.CharacterUniqueNumber;
			PartyMemberListCount++;
		}
	}

	ptmParty->UnlockIMParty();

	pMsgPartyCreateOK->PartyMemberListCount	= PartyMemberListCount;
	pMasterIMSoc->SendMsgToCurrentFieldServerSocket(pMsgPartyCreateOKBuf, MSG_SIZE(MSG_FI_PARTY_AUTO_CREATE_OK) + PartyMemberListCount * sizeof(UID32_t));

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2010-03-18 by cmkwon, ¸ó˝şĹÍşŻ˝Ĺ ±¸Çö - 
/// \author		cmkwon
/// \date		2010-03-18 ~ 2010-03-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_FI_PARTY_UPDATE_ITEM_TRANSFORMER_OK(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_PARTY_UPDATE_ITEM_TRANSFORMER_OK, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_FI_PARTY_UPDATE_ITEM_TRANSFORMER_OK,
		MSG_FI_PARTY_UPDATE_ITEM_TRANSFORMER_OK, pMsg);
	
	// »çżëŔÚ Č®ŔÎ
	CIMIOCPSocket *pIISoc = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pMsg->CharacUID);
	if (pIISoc == NULL || FALSE == pIISoc->IsUsing())
	{
		SendErrorMessage(T_FI_PARTY_UPDATE_ITEM_TRANSFORMER_OK, ERR_COMMON_NO_SUCH_CHARACTER, pMsg->CharacUID);
		return RES_BREAK;
	}
	
	// ĆÄĆĽ Č®ŔÎ
	CIMParty *pParty = pIISoc->GetIMParty();
	if (pParty == NULL)
	{
		SendErrorMessage(T_FI_PARTY_UPDATE_ITEM_TRANSFORMER_OK, ERR_PROTOCOL_NO_SUCH_PARTY, pMsg->CharacUID);
		return RES_BREAK;
	}
	pParty->LockIMParty();
	{
		INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_UPDATE_ITEM_TRANSFORMER_OK, T_IC_PARTY_UPDATE_ITEM_TRANSFORMER_OK, pSMsg, SendBuf);
		*pSMsg		= *pMsg;
		pParty->SendMsgToMembers(SendBuf, MSG_SIZE(MSG_IC_PARTY_UPDATE_ITEM_TRANSFORMER_OK), pMsg->CharacUID);
	}
	pParty->UnlockIMParty();
	
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_FI_CHARACTER_DELETE_CHARACTER(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_CHARACTER_DELETE_CHARACTER, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE_SERVER(pPacket, nLength, nBytesUsed, T_FI_CHARACTER_DELETE_CHARACTER,
									MSG_FI_CHARACTER_DELETE_CHARACTER, msgDelCharacter);

	///////////////////////////////////////////////////////////////////////////////
	// »čÁ¦µČ Äł¸ŻĹÍ Ăł¸®

	///////////////////////////////////////////////////////////////////////////////
	// ż©´ÜĂł¸®
	if(IS_VALID_UNIQUE_NUMBER(msgDelCharacter->GuildUniqueNumber))
	{
		mt_auto_lock aMG(&ms_pIMIOCP->m_MapGuild);
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(msgDelCharacter->GuildUniqueNumber);
		if (ptmGuild
			&& ptmGuild->m_GuildUniqueNumber == msgDelCharacter->GuildUniqueNumber)
		{
			ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildLeaveMember, this, NULL, NULL,
				msgDelCharacter->CharacterUniqueNumber, msgDelCharacter->GuildUniqueNumber);
		}		
	}	

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_FI_CHARACTER_CHANGE_LEVEL(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_CHARACTER_CHANGE_LEVEL, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE_SERVER(pPacket, nLength, nBytesUsed, T_FI_CHARACTER_CHANGE_LEVEL,
									MSG_FI_CHARACTER_CHANGE_LEVEL, msgChangeLevel);

	CIMIOCPSocket *pSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(msgChangeLevel->CharacterUniqueNumber);
	if (pSocket == NULL)
	{
		return RES_BREAK;
	}

	pSocket->m_character.Level = msgChangeLevel->Level;

	// 2008-05-20 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ×
	if(IS_VALID_UNIQUE_NUMBER(pSocket->m_character.GuildUniqueNumber))
	{
		mt_auto_lock aMG(&ms_pIMIOCP->m_MapGuild);
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(pSocket->m_character.GuildUniqueNumber);
		if (ptmGuild)
		{
			ptmGuild->SetMemberLevel(pSocket->m_character.CharacterUniqueNumber, msgChangeLevel->Level);
			INIT_MSG_WITH_BUFFER(MSG_IC_GUILD_MEMBER_LEVEL_UP, T_IC_GUILD_MEMBER_LEVEL_UP, pSMsg, pSendBuf);
			pSMsg->CharacterUID		= pSocket->m_character.CharacterUniqueNumber;
			pSMsg->Level			= msgChangeLevel->Level;
			ptmGuild->SendMsgToGuildMembers(pSendBuf, MSG_SIZE(MSG_IC_GUILD_MEMBER_LEVEL_UP));
		}
	}

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_FI_CHARACTER_UPDATE_MAP_CHANNEL(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_CHARACTER_UPDATE_MAP_CHANNEL, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE_SERVER(pPacket, nLength, nBytesUsed, T_FI_CHARACTER_UPDATE_MAP_CHANNEL,
									MSG_FI_CHARACTER_UPDATE_MAP_CHANNEL, msgUpdateMap);

	CIMIOCPSocket *pSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(msgUpdateMap->CharacterUniqueNumber);
	if (pSocket == NULL || FALSE == pSocket->IsValidCharacter())
	{
		return RES_BREAK;
	}

	CIMParty *ptmParty = pSocket->GetIMParty();

	// ĆÄĆĽżřŔĚ ŔÖ´Ů¸é żöÇÁ Á¤ş¸ ŔüĽŰ
	if (ptmParty)
	{
		ptmParty->LockIMParty();
		{
			INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_UPDATE_MEMBER_INFO_MAPNAME, T_IC_PARTY_UPDATE_MEMBER_INFO_MAPNAME, pUpdatePartyMap, pUpdatePartyMapBuf);
			pUpdatePartyMap->CharacterUniqueNumber = msgUpdateMap->CharacterUniqueNumber;
			pUpdatePartyMap->MapChannelIndex = msgUpdateMap->MapChannel;
			ptmParty->SendMsgToMembers(pUpdatePartyMapBuf, MSG_SIZE(MSG_IC_PARTY_UPDATE_MEMBER_INFO_MAPNAME), msgUpdateMap->CharacterUniqueNumber);
		}
		ptmParty->UnlockIMParty();		
	}

	return RES_RETURN_TRUE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_FI_CHARACTER_CHANGE_INFLUENCE_TYPE(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		
/// \author		cmkwon
/// \date		2005-12-03 ~ 2005-12-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_FI_CHARACTER_CHANGE_INFLUENCE_TYPE(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_CHARACTER_CHANGE_INFLUENCE_TYPE, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE_SERVER(pPacket, nLength, nBytesUsed, T_FI_CHARACTER_CHANGE_INFLUENCE_TYPE,
									MSG_FI_CHARACTER_CHANGE_INFLUENCE_TYPE, pRMsg);

	CIMIOCPSocket *pSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pRMsg->CharacterUID);
	if (pSocket == NULL
		|| FALSE == pSocket->IsUsing())
	{
		return RES_BREAK;
	}
	
	pSocket->ChangedCharacterInfluenceType(pRMsg->InfluenceType0);
	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_FI_UPDATE_SUBLEADER(const char* pPacket, int nLength, int &nBytesUsed)

/// \brief		
/// \author		dhjin
/// \date		2007-02-14 ~ 2007-02-14
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_FI_UPDATE_SUBLEADER(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_UPDATE_SUBLEADER, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE_SERVER(pPacket, nLength, nBytesUsed, T_FI_UPDATE_SUBLEADER,
									MSG_FI_UPDATE_SUBLEADER, pRMsg);

	if(COMPARE_INFLUENCE(pRMsg->InfluenceType, INFLUENCE_TYPE_VCN))
	{
		if(1 == pRMsg->SubLeaderNum)
		{
			ms_pIMIOCP->m_VCNSub1LeaderCharacterUID = pRMsg->CharacterUID;
		}
		else if(2 == pRMsg->SubLeaderNum)
		{
			ms_pIMIOCP->m_VCNSub2LeaderCharacterUID = pRMsg->CharacterUID;
		}
		else 
		{
			return RES_BREAK;
		}
	}
	else if(COMPARE_INFLUENCE(pRMsg->InfluenceType, INFLUENCE_TYPE_ANI))
	{
		if(1 == pRMsg->SubLeaderNum)
		{
			ms_pIMIOCP->m_ANISub1LeaderCharacterUID = pRMsg->CharacterUID;
		}
		else if(2 == pRMsg->SubLeaderNum)
		{
			ms_pIMIOCP->m_ANISub2LeaderCharacterUID = pRMsg->CharacterUID;
		}
		else 
		{
			return RES_BREAK;
		}
	}

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_FI_GUILD_NOTIFY_START_WAR(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_GUILD_NOTIFY_START_WAR, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE_SERVER(pPacket, nLength, nBytesUsed, T_FI_GUILD_NOTIFY_START_WAR,
									MSG_FI_GUILD_NOTIFY_START_WAR, msgStartGuildWar);

	int nGuildWarTimerUniqueNumber = 0;
	ms_pIMIOCP->m_MapGuild.lock();
	{
		///////////////////////////////////////////////////////////////////////////////
		// ÇŇ´çÇĎ°í 1Ŕ» Áő°ˇ ˝ĂĹ˛´Ů
		nGuildWarTimerUniqueNumber = CGuild::ms_dwGuildWarUniqueNumber++;

		CGuild *pDefenderGuild = ms_pIMIOCP->m_MapGuild.findLock(msgStartGuildWar->DefenderGuildUID);
		if (pDefenderGuild == NULL) 
		{
			ms_pIMIOCP->m_MapGuild.unlock();			
			return RES_BREAK;
		}
		pDefenderGuild->StartWar(msgStartGuildWar->ChallengerGuildUID, msgStartGuildWar->WarMapChannel, nGuildWarTimerUniqueNumber);

		CGuild *pChallengerGuild = ms_pIMIOCP->m_MapGuild.findLock(msgStartGuildWar->ChallengerGuildUID);
		if (pChallengerGuild == NULL) 
		{
			ms_pIMIOCP->m_MapGuild.unlock();			
			return RES_BREAK;
		}
		pChallengerGuild->StartWar(msgStartGuildWar->DefenderGuildUID, msgStartGuildWar->WarMapChannel, nGuildWarTimerUniqueNumber);
		
		INIT_MSG_WITH_BUFFER(MSG_IC_COUNTDOWN_START, T_IC_COUNTDOWN_START, pSMsg, pSendBuf);
		pSMsg->byCountdownType	= COUNTDOWN_TYPE_GUILD_WAR;
		pSMsg->nWaitingSecond	= SECOND_COUNTDOWN_TIME_GAP;
		
		// to defender guild
		pDefenderGuild->SendMsgToGuildMembers(pSendBuf, MSG_SIZE(MSG_IC_COUNTDOWN_START), INVALID_UNIQUE_NUMBER, pSMsg);

		// to challenger guild
		pChallengerGuild->SendMsgToGuildMembers(pSendBuf, MSG_SIZE(MSG_IC_COUNTDOWN_START), INVALID_UNIQUE_NUMBER, pSMsg);
	}
	ms_pIMIOCP->m_MapGuild.unlock();

	///////////////////////////////////////////////////////////////////////////////
	// ż©´ÜŔü Ĺ¸ŔĚ¸Ó ĽłÁ¤(20şĐ)
	// 2006-09-01 by cmkwon, ÁřÇŕ˝Ă°Ł ĽöÁ¤ÇÔ(20şĐ --> 60şĐ)
	ms_pIMIOCP->m_pIMTickManager->InsertTickEvent(IM_TICK_EVENT_START_GUILD_WAR, 60*60
		, msgStartGuildWar->DefenderGuildUID, nGuildWarTimerUniqueNumber);


// 2004-10-20 by cmkwon
//	INIT_MSG_WITH_BUFFER(MSG_IC_GUILD_WAR_READY, T_IC_GUILD_WAR_READY, pMsgReady, pMsgReadyBuf);
//
//	// to defender guild
//	pMsgReady->PeerGuildUID = msgStartGuildWar->ChallengerGuildUID;
//	pMsgReady->WaitingTime = 5;
//	pDefenderGuild->SendMsgToGuildMembers(pMsgReadyBuf, MSG_SIZE(MSG_IC_GUILD_WAR_READY));
//
//	// to challenger guild
//	pMsgReady->PeerGuildUID = msgStartGuildWar->DefenderGuildUID;
//	pMsgReady->WaitingTime = 5;
//	pChallengerGuild->SendMsgToGuildMembers(pMsgReadyBuf, MSG_SIZE(MSG_IC_GUILD_WAR_READY));
//
//	ms_pIMIOCP->m_pIMTickManager->InsertTickEvent(IM_TICK_EVENT_START_GUILD_WAR, 5, msgStartGuildWar->DefenderGuildUID, msgStartGuildWar->ChallengerGuildUID);

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_FI_GUILD_DELETE_GUILD(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_GUILD_DELETE_GUILD, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE_SERVER(pPacket, nLength, nBytesUsed, T_FI_GUILD_DELETE_GUILD,
									MSG_FI_GUILD_DELETE_GUILD, msgDeleteGuild);

	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildDeleteGuild, NULL, NULL, NULL, msgDeleteGuild->DeleteGuildUID);
	return RES_RETURN_TRUE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_FI_GUILD_ADD_GUILD_FAME(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		
/// \author		cmkwon
/// \date		2005-12-27 ~ 2005-12-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_FI_GUILD_ADD_GUILD_FAME(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_GUILD_ADD_GUILD_FAME, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE_SERVER(pPacket, nLength, nBytesUsed, T_FI_GUILD_ADD_GUILD_FAME,
									MSG_FI_GUILD_ADD_GUILD_FAME, pRMsg);

	mt_auto_lock mtG(&ms_pIMIOCP->m_MapGuild);
	CGuild *pGuild = ms_pIMIOCP->m_MapGuild.findLock(pRMsg->guildUID);
	if (NULL == pGuild) 
	{// ż©´Ü Á¤ş¸°ˇ ľř´Ů
		return RES_BREAK;
	}
	pGuild->AddGuildFame(pRMsg->addValues);

	// DBżˇ Update
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildAddGuildFame, this, NULL
		, (void*)pGuild->m_GuildUniqueNumber, pRMsg->addValues, pRMsg->addValues);

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_FI_GUILD_OUTPOST(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ŔüÁř±âÁö °ü·Ă
/// \author		dhjin
/// \date		2008-05-21 ~ 2008-05-21
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_FI_GUILD_OUTPOST(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_GUILD_OUTPOST, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE_SERVER(pPacket, nLength, nBytesUsed, T_FI_GUILD_OUTPOST,
									MSG_FI_GUILD_OUTPOST, pRMsg);

	mt_auto_lock mtG(&ms_pIMIOCP->m_MapGuild);
	CGuild *pGuild = ms_pIMIOCP->m_MapGuild.findLock(pRMsg->GuildUID);
	if (NULL == pGuild) 
	{// ż©´Ü Á¤ş¸°ˇ ľř´Ů
		return RES_BREAK;
	}	

	pGuild->m_OutPostCityMapIndex = pRMsg->MapIndex;

	return RES_RETURN_TRUE;
}


// 2004-12-09 by cmkwon, ż©´ÜŔüŔÇ Áľ·á´Â IMServerżˇĽ­¸¸ ąß»ýÇŃ´Ů. ±×·ŻąÇ·Î ľË¸˛Ŕş IMServer -> Field·Î ąß»ýÇŃ´Ů
//ProcessResult CIMIOCPSocket::Process_FI_GUILD_NOTIFY_END_WAR(const char* pPacket, int nLength, int &nBytesUsed)
//{
//	DECLARE_MESSAGE_AND_CHECK_SIZE_SERVER(pPacket, nLength, nBytesUsed, T_FI_GUILD_NOTIFY_END_WAR,
//									MSG_FI_GUILD_NOTIFY_END_WAR, msgEndGuildWar);
//
//	char szChatBuf[SIZE_MAX_CHAT_MESSAGE];
//
//	ms_pIMIOCP->m_MapGuild.lock();
//	{
//		INIT_MSG_WITH_BUFFER(MSG_IC_GUILD_END_WAR, T_IC_GUILD_END_WAR, pMsgEndWar, pMsgEndWarBuf);
//
//		CGuild *pWinnerGuild = ms_pIMIOCP->m_MapGuild.findLock(msgEndGuildWar->WinnerGuildUID);
//		if (pWinnerGuild != NULL)
//		{
//			pWinnerGuild->m_GuildState = GUILD_STATE_NORMAL;
//		}
//
//		CGuild *pLoserGuild = ms_pIMIOCP->m_MapGuild.findLock(msgEndGuildWar->LoserGuildUID);
//		if (pLoserGuild != NULL)
//		{
//			pLoserGuild->m_GuildState = GUILD_STATE_NORMAL;
//		}
//
//		if (pLoserGuild == NULL && pWinnerGuild == NULL)
//		{
//			ms_pIMIOCP->m_MapGuild.unlock();
//			// do nothing
//			return RES_BREAK;
//		}
//		else if (pLoserGuild == NULL && pWinnerGuild != NULL)
//		{
//			sprintf(szChatBuf, "%s ż©´ÜŔĚ ż©´ÜŔüżˇĽ­ ˝Â¸®ÇĎż´˝Ŕ´Ď´Ů!", pWinnerGuild->m_GuildName);
//			ms_pIMIOCP->SendChatAll("żîżµŔÚ", szChatBuf);
//			
//			strncpy(pMsgEndWar->PeerGuildName, "łŞ°ˇąö¸° ", SIZE_MAX_GUILD_NAME);
//			pMsgEndWar->WarEndType = BATTLE_END_WIN;
//			pWinnerGuild->SendMsgToGuildMembers(pMsgEndWarBuf, MSG_SIZE(MSG_IC_GUILD_END_WAR));
//			
//			ms_pIMIOCP->m_MapGuild.unlock();
//			return RES_BREAK;
//		}
//		else if (pLoserGuild != NULL && pWinnerGuild == NULL)
//		{
//			sprintf(szChatBuf, "%s ż©´ÜŔĚ ż©´ÜŔüżˇĽ­ ˝Â¸®ÇĎż´˝Ŕ´Ď´Ů!", pLoserGuild->m_GuildName);
//			ms_pIMIOCP->SendChatAll("żîżµŔÚ", szChatBuf);
//
//			strncpy(pMsgEndWar->PeerGuildName, "łŞ°ˇąö¸° ", SIZE_MAX_GUILD_NAME);
//			pMsgEndWar->WarEndType = BATTLE_END_WIN;
//			pLoserGuild->SendMsgToGuildMembers(pMsgEndWarBuf, MSG_SIZE(MSG_IC_GUILD_END_WAR));
//
//			ms_pIMIOCP->m_MapGuild.unlock();
//			return RES_BREAK;
//		}
//
//		// to winner guild
//		strncpy(pMsgEndWar->PeerGuildName, pLoserGuild->m_GuildName, SIZE_MAX_GUILD_NAME);
//		if (msgEndGuildWar->WarEndType == BATTLE_END_WIN || msgEndGuildWar->WarEndType == BATTLE_END_DEFEAT)
//		{
//			pMsgEndWar->WarEndType = BATTLE_END_WIN;
//		}
//		else
//		{
//			pMsgEndWar->WarEndType = msgEndGuildWar->WarEndType;
//		}
//		pWinnerGuild->SendMsgToGuildMembers(pMsgEndWarBuf, MSG_SIZE(MSG_IC_GUILD_END_WAR));
//
//		// to loser guild
//		strncpy(pMsgEndWar->PeerGuildName, pWinnerGuild->m_GuildName, SIZE_MAX_GUILD_NAME);
//		if (msgEndGuildWar->WarEndType == BATTLE_END_WIN || msgEndGuildWar->WarEndType == BATTLE_END_DEFEAT)
//		{
//			pMsgEndWar->WarEndType = BATTLE_END_DEFEAT;
//		}
//		else
//		{
//			pMsgEndWar->WarEndType = msgEndGuildWar->WarEndType;
//		}
//		pLoserGuild->SendMsgToGuildMembers(pMsgEndWarBuf, MSG_SIZE(MSG_IC_GUILD_END_WAR));
//
//		// ż©´ÜŔü °á°ú ŔüĂĽ °řÁö
//		if (msgEndGuildWar->WarEndType == BATTLE_END_WIN || msgEndGuildWar->WarEndType == BATTLE_END_DEFEAT)
//		{
//			sprintf(szChatBuf, "%s ż©´ÜŔĚ %s ż©´Ü°úŔÇ ż©´ÜŔüżˇĽ­ ˝Â¸®ÇĎż´˝Ŕ´Ď´Ů!",
//						pWinnerGuild->m_GuildName,
//						pLoserGuild->m_GuildName);
//		}
//		else if (msgEndGuildWar->WarEndType == BATTLE_END_TIE)
//		{
//			sprintf(szChatBuf, "%s ż©´Ü°ú %s ż©´ÜŔĚ ą«˝ÂşÎ·Î ż©´ÜŔüŔ» ¸¶ĂĆ˝Ŕ´Ď´Ů!",
//						pWinnerGuild->m_GuildName,
//						pLoserGuild->m_GuildName);
//		}
//		else if (msgEndGuildWar->WarEndType == BATTLE_END_END)
//		{
//			sprintf(szChatBuf, "%s ż©´Ü°ú %s ż©´ÜŔĚ ż©´ÜŔüŔ» ¸¶ĂĆ˝Ŕ´Ď´Ů!",
//						pWinnerGuild->m_GuildName,
//						pLoserGuild->m_GuildName);
//		}
//	}// lock_end
//	ms_pIMIOCP->m_MapGuild.unlock();
//	
//	ms_pIMIOCP->SendChatAll("żîżµŔÚ", szChatBuf);
//
//	return RES_RETURN_TRUE;
//}


ProcessResult CIMIOCPSocket::Process_FI_CASH_USING_GUILD(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_CASH_USING_GUILD, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE_SERVER(pPacket, nLength, nBytesUsed, T_FI_CASH_USING_GUILD,
									MSG_FI_CASH_USING_GUILD, pRUsingGuildCard);

	mt_auto_lock mtA(&ms_pIMIOCP->m_MapGuild);
	CGuild *pGuild = ms_pIMIOCP->m_MapGuild.findLock(pRUsingGuildCard->guildUID);
	if (pGuild == NULL) 
	{			
		return RES_BREAK;
	}

	// 2008-08-12 by dhjin, ąö±×·Î ĽöÁ¤ 
	pGuild->m_nGuildMemberCapacity += pRUsingGuildCard->nIncreaseMemberCapacity;
	pGuild->m_nGuildMemberCapacity = min(CAtumSJ::GetMaxGuildSize(pGuild->m_nGuildMemberCapacity, IsExistPremiumCard()), SIZE_MAX_GUILD_CAPACITY);	// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ż©´Üżř Áő°ˇ Äł˝¬ ľĆŔĚĹŰ

	///////////////////////////////////////////////////////////////////////////////
	// DBżˇ Update
	// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ż©´Üżř Áő°ˇ Äł˝¬ ľĆŔĚĹŰ
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildUpdateMemberCapacity, this, NULL, (UID32_t*)pRUsingGuildCard->CashPrice, pGuild->m_GuildUniqueNumber, pGuild->m_nGuildMemberCapacity);

	///////////////////////////////////////////////////////////////////////////////
	// ±ćµĺżřżˇ°Ô ¸đµÎ ŔüĽŰ
	INIT_MSG_WITH_BUFFER(MSG_IC_GUILD_CHANGE_MEMBER_CAPACITY, T_IC_GUILD_CHANGE_MEMBER_CAPACITY, pSCapacity, SendBuf);
	pSCapacity->guildUID			= pGuild->m_GuildUniqueNumber;
	pSCapacity->nMemberCapacity		= pGuild->m_nGuildMemberCapacity;
	pSCapacity->IncreaseCapacity	= pRUsingGuildCard->nIncreaseMemberCapacity;		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ż©´Üżř Áő°ˇ Äł˝¬ ľĆŔĚĹŰ
	pGuild->SendMsgToGuildMembers(SendBuf, MSG_SIZE(MSG_IC_GUILD_CHANGE_MEMBER_CAPACITY));
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_FI_CASH_USING_CHANGE_CHARACTERNAME(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_CASH_USING_CHANGE_CHARACTERNAME, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE_SERVER(pPacket, nLength, nBytesUsed, T_FI_CASH_USING_CHANGE_CHARACTERNAME,
									MSG_FI_CASH_USING_CHANGE_CHARACTERNAME, pRChangeCharName);

	CIMIOCPSocket *pIISock = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pRChangeCharName->charUID);
	if(NULL == pIISock
		|| FALSE == pIISock->IsUsing())
	{
		return RES_BREAK;
	}

	///////////////////////////////////////////////////////////////////////////////
	// Guild żˇĽ­ ĽöÁ¤
	if (IS_VALID_UNIQUE_NUMBER(pIISock->m_uidGuildUniqueNumber))
	{
		mt_auto_lock mtG(&ms_pIMIOCP->m_MapGuild);
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(pIISock->m_uidGuildUniqueNumber);
		if(ptmGuild)
		{
			CGuildMember *ptmGMember;
			if(ptmGuild->GetGuildMember(pIISock->m_character.CharacterUniqueNumber, ptmGMember))
			{
				STRNCPY_MEMSET(ptmGMember->m_CharacterName, pRChangeCharName->szChangedCharacterName, SIZE_MAX_CHARACTER_NAME);
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	// IMIOCP°ˇ °ü¸®ÇĎ´Â µĄŔĚĹ¸ Ăł¸®
	mt_auto_lock mtA(&ms_pIMIOCP->m_mCharacterName);
	char tmCharacterName[SIZE_MAX_CHARACTER_NAME];
	ms_pIMIOCP->m_mCharacterName.deleteNoLock(GGetLowerCase(tmCharacterName, pIISock->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME));

	STRNCPY_MEMSET(pIISock->m_character.CharacterName, pRChangeCharName->szChangedCharacterName, SIZE_MAX_CHARACTER_NAME);
	ms_pIMIOCP->m_mCharacterName.insertNoLock(GGetLowerCase(tmCharacterName, pIISock->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME), pIISock);
	mtA.auto_unlock_cancel();

	return RES_RETURN_TRUE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_FI_CASH_PREMIUM_CARD_INFO(const char* pPacket, int nLength, int &nBytesUsed)	
/// \brief		
/// \author		dhjin
/// \date		2006-09-14 ~ 2006-09-14
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_FI_CASH_PREMIUM_CARD_INFO(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_CASH_PREMIUM_CARD_INFO, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

   	DECLARE_MESSAGE_AND_CHECK_SIZE_SERVER(pPacket, nLength, nBytesUsed, T_FI_CASH_PREMIUM_CARD_INFO,
								MSG_FI_CASH_PREMIUM_CARD_INFO, pRCashPremiumInfo);

	CIMIOCPSocket *pIISock = ms_pIMIOCP->GetIMIOCPSocketByAccountUID(pRCashPremiumInfo->AccountUID);
	if(NULL == pIISock
		|| FALSE == pIISock->IsUsing())
	{
		return RES_BREAK;
	}
	
	pIISock->m_MembershipInfo.nCardItemNum1			= pRCashPremiumInfo->nCardItemNum1;

	//////////////////////////////////////////////////////////////////////////
	// 2008-06-20 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ¸¸·á ˝Ă°Ł ÇĘżä
	pIISock->m_MembershipInfo.atumTimeExpireTime1	= pRCashPremiumInfo->atumTimeExpireTime1;	
	
	mt_auto_lock mtG(&ms_pIMIOCP->m_MapGuild);
	CGuild *pGuild = ms_pIMIOCP->m_MapGuild.findLock(pIISock->m_character.GuildUniqueNumber);
	if (NULL == pGuild) 
	{// ż©´Ü Á¤ş¸°ˇ ľř´Ů
		return RES_BREAK;
	}	

	if(pIISock->m_character.CharacterUniqueNumber == pGuild->m_CommanderUniqueNumber)
	{
		if(FALSE == pRCashPremiumInfo->nCardItemNum1)
		{
			pGuild->SetGuildMemberShip(FALSE);
		}
		else
		{
			pGuild->SetGuildMemberShip(TRUE, &pRCashPremiumInfo->atumTimeExpireTime1);
		}
		INIT_MSG_WITH_BUFFER(MSG_IC_GUILD_CHANGE_MEMBERSHIP, T_IC_GUILD_CHANGE_MEMBERSHIP, msg, SendBuf);
		msg->MemberShip					= pGuild->m_bMemberShip;
		msg->MemberShipExpireTime		= pGuild->m_MemberShipExpireTime;
		pGuild->SendMsgToGuildMembers(SendBuf, MSG_SIZE(MSG_IC_GUILD_CHANGE_MEMBERSHIP));				
	}

	return RES_RETURN_TRUE;
}


/***********************************************************************************************************
**
**	ĆÄĆĽ Ĺ»Ĺđ Ăł¸®.
**
**	Create Info :	??.??.??.
**
**	Update Info :	ľĆ·ąľĆ ŔĚµżżˇ ´ëÇŃ żążÜĂł¸® ŔÎŔÚ Ăß°ˇ.	'a_bMoveToArena'	2010.09.03. by hsLee.
**
************************************************************************************************************/
BOOL CIMIOCPSocket::ProcessPartyLeaveNoLock( CIMParty *i_pIMParty , const bool a_bMoveToArena /*= false*/ )
{	
	UID32_t tmOldUID = i_pIMParty->GetMasterCharacterUniqueNumber();
	i_pIMParty->LeaveMember(m_character.CharacterUniqueNumber);	
	if (0 == i_pIMParty->GetMemberCount())
	{// ŔŻŔú°ˇ ľř°ĹłŞ OnlineŔŻŔú°ˇ ľřŔ˝
		
// 		// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - ŔÓ˝Ăżë ·Î±×
// 		g_pIMGlobal->WriteSystemLogEX(TRUE, "  [TEMP] 090804  CIMIOCPSocket::ProcessPartyLeaveNoLock# %s CurIsOn(%d) FormationTy(%d) \r\n"
// 			, GetCharacterString(GetCharacter(), string()), i_pIMParty->m_SPartyInfo.IsAppliedFormationSkill, i_pIMParty->m_SPartyInfo.FormationType);
		///////////////////////////////////////////////////////////////////////////////
		// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - Ćí´ë Ĺ»Ĺđ˝Ă, 
		if(i_pIMParty->m_SPartyInfo.IsAppliedFormationSkill)
		{
			i_pIMParty->m_SPartyInfo.IsAppliedFormationSkill		= FALSE;
			this->SendFieldServPartyChangeFormationSkill(i_pIMParty->GetMasterCharacterUniqueNumber(), FALSE);
		}

		// ĆÄĆĽ ÇŘ»ęÁ¤ş¸¸¦ ŔüĽŰ
		if(this->IsUsing())
		{
			INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_DISMEMBER_OK, T_IC_PARTY_DISMEMBER_OK, pICDismemberOK, pICDismemberOKBuf);
			pICDismemberOK->PartyID = i_pIMParty->m_PartyID;
			SendAddData(pICDismemberOKBuf, MSG_SIZE(MSG_IC_PARTY_DISMEMBER_OK));
		}

		// Field Serverżˇ°Ô ş¸ł»±â
		INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_DISMEMBER_OK, T_FI_PARTY_DISMEMBER_OK, pFIDismemberOK, pFIDismemberOKBuf);
		pFIDismemberOK->PartyID = i_pIMParty->m_PartyID;
		ms_pIMIOCP->SendMessageToFieldServer(pFIDismemberOKBuf, MSG_SIZE(MSG_FI_PARTY_DISMEMBER_OK));

		return FALSE;
	}
	
	if (tmOldUID != i_pIMParty->GetMasterCharacterUniqueNumber())
	{
		SendMsgPartyTransferMasterInfoNoLock(i_pIMParty, tmOldUID, i_pIMParty->GetMasterCharacterUniqueNumber());
	}
	
// 	// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - ŔÓ˝Ăżë ·Î±×
// 	g_pIMGlobal->WriteSystemLogEX(TRUE, "  [TEMP] 090804  CIMIOCPSocket::ProcessPartyLeaveNoLock# %s CurIsOn(%d) FormationTy(%d) \r\n"
// 		, GetCharacterString(GetCharacter(), string()), i_pIMParty->m_SPartyInfo.IsAppliedFormationSkill, i_pIMParty->m_SPartyInfo.FormationType);

	///////////////////////////////////////////////////////////////////////////////
	// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - Ćí´ë Ĺ»Ĺđ˝Ă, 
	if(i_pIMParty->m_SPartyInfo.IsAppliedFormationSkill)
	{
		i_pIMParty->m_SPartyInfo.IsAppliedFormationSkill		= FALSE;
		this->SendFieldServPartyChangeFormationSkill(i_pIMParty->GetMasterCharacterUniqueNumber(), FALSE);
	}

	// 2009-09-09 ~ 2010-01-27 by dhjin, ŔÎÇÇ´ĎĆĽ - °­Á¦ Áľ·á ŔŻŔú ĆÄĆĽżˇĽ­ Ĺ»ĹđĂł¸®!, ąŘ°ú °°ŔĚ ĽöÁ¤
	// 2009-09-09 ~ 2010-01-27 by dhjin, ŔÎÇÇ´ĎĆĽ - ŔÚ±â ŔÚ˝Ĺ ŔüĽŰ¸¸ IsUsing ĂĽĹ© ÇĎµµ·Ď şŻ°ć, ´Ů¸Ą ŔŻŔúµéżˇ°Ő ŔüĽŰÇŘĽ­ ĆÄĆĽ łŞ°Ł »ç˝ÇŔ» ľË·ÁÁÖŔÚ
//	if(this->IsUsing())
//	{
//		// Clientżˇ°Ô ş¸ł»±â
//		INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_LEAVE_OK, T_IC_PARTY_LEAVE_OK, pICLeaveOK, pICLeaveOKBuf);
//		pICLeaveOK->PartyID					= i_pIMParty->m_PartyID;
//		pICLeaveOK->CharacterUniqueNumber	= m_character.CharacterUniqueNumber;
//
//		if(this->IsUsing()) { 
//			// 2009-09-09 ~ 2010-01-27 by dhjin, ŔÎÇÇ´ĎĆĽ - ŔÚ±â ŔÚ˝Ĺ ŔüĽŰ¸¸ IsUsing ĂĽĹ© ÇĎµµ·Ď şŻ°ć, ´Ů¸Ą ŔŻŔúµéżˇ°Ő ŔüĽŰÇŘĽ­ ĆÄĆĽ łŞ°Ł »ç˝ÇŔ» ľË·ÁÁÖŔÚ
//			///////////////////////////////////////////////////////////////////////////////
//			// ŔÚ±â ŔÚ˝Ĺżˇ°Ô ŔüĽŰÇŃ´Ů
//			this->SendAddData(pICLeaveOKBuf, MSG_SIZE(MSG_IC_PARTY_LEAVE_OK));
//		}
//
//		///////////////////////////////////////////////////////////////////////////////
//		// ł˛Ŕş ĆÄĆĽżřµéżˇ°Ô ŔüĽŰ
//		i_pIMParty->SendMsgToMembers(pICLeaveOKBuf, MSG_SIZE(MSG_IC_PARTY_LEAVE_OK));
//	}
	// Clientżˇ°Ô ş¸ł»±â
	INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_LEAVE_OK, T_IC_PARTY_LEAVE_OK, pICLeaveOK, pICLeaveOKBuf);
	pICLeaveOK->PartyID					= i_pIMParty->m_PartyID;
	pICLeaveOK->CharacterUniqueNumber	= m_character.CharacterUniqueNumber;
	pICLeaveOK->bMoveToArena			= a_bMoveToArena;
	///////////////////////////////////////////////////////////////////////////////
	// ł˛Ŕş ĆÄĆĽżřµéżˇ°Ô ŔüĽŰ
	i_pIMParty->SendMsgToMembers(pICLeaveOKBuf, MSG_SIZE(MSG_IC_PARTY_LEAVE_OK));

	if(this->IsUsing())	{
		// ŔÚ±â ŔÚ˝Ĺżˇ°Ô ŔüĽŰÇŃ´Ů
		this->SendAddData(pICLeaveOKBuf, MSG_SIZE(MSG_IC_PARTY_LEAVE_OK));
		// Field Serverżˇ°Ô ş¸ł»±â
		INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_LEAVE_OK, T_FI_PARTY_LEAVE_OK, pFILeaveOK, pFILeaveOKBuf);
		pFILeaveOK->PartyID								= i_pIMParty->m_PartyID;
		pFILeaveOK->CharacterUniqueNumber				= m_character.CharacterUniqueNumber;
		pFILeaveOK->FieldPartyInfo.nTotalPartyMember	= i_pIMParty->GetMemberCount(FALSE);
		pFILeaveOK->FieldPartyInfo.lowestMemberLevel	= i_pIMParty->GetLowestMemberLevel();
		this->SendMsgToCurrentFieldServerSocket(pFILeaveOKBuf, MSG_SIZE(MSG_FI_PARTY_LEAVE_OK));
	}
	else {
		// Field Serverżˇ°Ô ş¸ł»±â
		INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_DISCONNECT_LEAVE_OK, T_FI_PARTY_DISCONNECT_LEAVE_OK, pFILeaveOK, pFILeaveOKBuf);
		pFILeaveOK->PartyID								= i_pIMParty->m_PartyID;
		pFILeaveOK->CharacterUniqueNumber				= m_character.CharacterUniqueNumber;
		pFILeaveOK->FieldPartyInfo.nTotalPartyMember	= i_pIMParty->GetMemberCount(FALSE);
		pFILeaveOK->FieldPartyInfo.lowestMemberLevel	= i_pIMParty->GetLowestMemberLevel();
		this->SendMsgToCurrentFieldServerSocket(pFILeaveOKBuf, MSG_SIZE(MSG_FI_PARTY_DISCONNECT_LEAVE_OK));
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CIMIOCPSocket::ProcessPartyLeaveArenaEndNoLock(CIMParty *i_pIMParty)
/// \brief		ľĆ·ąłŞ Ľ­ąöżˇĽ­ °ć±â Áľ·áČÄ ĆÄĆĽ °ü·Ă Ăł¸®
/// \author		dhjin
/// \date		2008-02-28 ~ 2008-02-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CIMIOCPSocket::ProcessPartyLeaveArenaEndNoLock(CIMParty *i_pIMParty)
{
	UID32_t tmOldUID = i_pIMParty->GetMasterCharacterUniqueNumber();
	i_pIMParty->LeaveMember(m_character.CharacterUniqueNumber);	

	if (0 == i_pIMParty->GetMemberCount())
	{// ŔŻŔú°ˇ ľř°ĹłŞ OnlineŔŻŔú°ˇ ľřŔ˝

		// Field Serverżˇ°Ô ş¸ł»±â
		INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_DISMEMBER_OK, T_FI_PARTY_DISMEMBER_OK, pFIDismemberOK, pFIDismemberOKBuf);
		pFIDismemberOK->PartyID = i_pIMParty->m_PartyID;
		ms_pIMIOCP->SendMessageToFieldServer(pFIDismemberOKBuf, MSG_SIZE(MSG_FI_PARTY_DISMEMBER_OK));

		return FALSE;
	}
	
	if (tmOldUID != i_pIMParty->GetMasterCharacterUniqueNumber())
	{
		SendMsgPartyTransferMasterInfoNoLock(i_pIMParty, tmOldUID, i_pIMParty->GetMasterCharacterUniqueNumber());
	}

	if(this->IsUsing())
	{
		// Clientżˇ°Ô ş¸ł»±â
		INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_LEAVE_OK, T_IC_PARTY_LEAVE_OK, pICLeaveOK, pICLeaveOKBuf);
		pICLeaveOK->PartyID					= i_pIMParty->m_PartyID;
		pICLeaveOK->CharacterUniqueNumber	= m_character.CharacterUniqueNumber;

		///////////////////////////////////////////////////////////////////////////////
		// ł˛Ŕş ĆÄĆĽżřµéżˇ°Ô ŔüĽŰ
		i_pIMParty->SendMsgToMembers(pICLeaveOKBuf, MSG_SIZE(MSG_IC_PARTY_LEAVE_OK));
	}

	// Field Serverżˇ°Ô ş¸ł»±â
	INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_LEAVE_OK, T_FI_PARTY_LEAVE_OK, pFILeaveOK, pFILeaveOKBuf);
	pFILeaveOK->PartyID								= i_pIMParty->m_PartyID;
	pFILeaveOK->CharacterUniqueNumber				= m_character.CharacterUniqueNumber;
	pFILeaveOK->FieldPartyInfo.nTotalPartyMember	= i_pIMParty->GetMemberCount(FALSE);
	pFILeaveOK->FieldPartyInfo.lowestMemberLevel	= i_pIMParty->GetLowestMemberLevel();
	this->SendMsgToCurrentFieldServerSocket(pFILeaveOKBuf, MSG_SIZE(MSG_FI_PARTY_LEAVE_OK));

	return TRUE;
}

BOOL CIMIOCPSocket::ProcessPartyGameEndNoLock(CIMParty *i_pIMParty)
{
	UID32_t tmOldUID = i_pIMParty->GetMasterCharacterUniqueNumber();
	i_pIMParty->SetOfflineMemberW(m_character.CharacterUniqueNumber);	
	if (0 == i_pIMParty->GetMemberCount())
	{// ŔŻŔú°ˇ ľř°ĹłŞ OnlineŔŻŔú°ˇ ľřŔ˝
		
		// ĆÄĆĽ ÇŘ»ęÁ¤ş¸¸¦ ŔüĽŰ
		if(this->IsUsing())
		{
			INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_DISMEMBER_OK, T_IC_PARTY_DISMEMBER_OK, pICDismemberOK, pICDismemberOKBuf);
			pICDismemberOK->PartyID = i_pIMParty->m_PartyID;
			SendAddData(pICDismemberOKBuf, MSG_SIZE(MSG_IC_PARTY_DISMEMBER_OK));
		}

		// Field Serverżˇ°Ô ş¸ł»±â
		INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_DISMEMBER_OK, T_FI_PARTY_DISMEMBER_OK, pFIDismemberOK, pFIDismemberOKBuf);
		pFIDismemberOK->PartyID = i_pIMParty->m_PartyID;
		ms_pIMIOCP->SendMessageToFieldServer(pFIDismemberOKBuf, MSG_SIZE(MSG_FI_PARTY_DISMEMBER_OK));

		return FALSE;
	}
	
	if (tmOldUID != i_pIMParty->GetMasterCharacterUniqueNumber())
	{
		SendMsgPartyTransferMasterInfoNoLock(i_pIMParty, tmOldUID, i_pIMParty->GetMasterCharacterUniqueNumber());
	}

// 	// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - ŔÓ˝Ăżë ·Î±×
// 	g_pIMGlobal->WriteSystemLogEX(TRUE, "  [TEMP] 090804  CIMIOCPSocket::ProcessPartyGameEndNoLock# %s CurIsOn(%d) FormationTy(%d) \r\n"
// 		, GetCharacterString(GetCharacter(), string()), i_pIMParty->m_SPartyInfo.IsAppliedFormationSkill, i_pIMParty->m_SPartyInfo.FormationType);

	///////////////////////////////////////////////////////////////////////////////
	// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - Ćí´ë Ĺ»Ĺđ˝Ă, 
	if(i_pIMParty->m_SPartyInfo.IsAppliedFormationSkill)
	{
		i_pIMParty->m_SPartyInfo.IsAppliedFormationSkill		= FALSE;
		this->SendFieldServPartyChangeFormationSkill(i_pIMParty->GetMasterCharacterUniqueNumber(), FALSE);
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CIMIOCPSocket::ProcessPartyFromMainServerToArenaServerNoLock(CIMParty *i_pIMParty)
/// \brief		ľĆ·ąłŞ ĹëÇŐ - ş» Ľ­ąöżˇĽ­ ľĆ·ąłŞ Ľ­ąö·Î ŔĚµż ˝Ă ĆÄĆĽ Ăł¸® »çÇ×
/// \author		dhjin
/// \date		2008-02-28 ~ 2008-02-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CIMIOCPSocket::ProcessPartyFromMainServerToArenaServerNoLock(CIMParty *i_pIMParty)
{

	// ş»Ľ· -> ľĆ·ąłŞĽ· ŔĚµż ˝Ă ş»Ľ· ĆÄĆĽżˇĽ­´Â Ĺ»ĹđĂł¸®·Î ĽöÁ¤.	2010.09.03. by hsLee.
	return ProcessPartyLeaveNoLock ( i_pIMParty , true );

	/*
	UID32_t tmOldUID = i_pIMParty->GetMasterCharacterUniqueNumber();
	i_pIMParty->SetOfflineMemberW( m_character.CharacterUniqueNumber );
	if (0 == i_pIMParty->GetMemberCount())
	{// ŔŻŔú°ˇ ľř°ĹłŞ OnlineŔŻŔú°ˇ ľřŔ˝
		
		// Field Serverżˇ°Ô ş¸ł»±â
		INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_DISMEMBER_OK, T_FI_PARTY_DISMEMBER_OK, pFIDismemberOK, pFIDismemberOKBuf);
		pFIDismemberOK->PartyID = i_pIMParty->m_PartyID;
		ms_pIMIOCP->SendMessageToFieldServer(pFIDismemberOKBuf, MSG_SIZE(MSG_FI_PARTY_DISMEMBER_OK));

		return FALSE;
	}
	
	if (tmOldUID != i_pIMParty->GetMasterCharacterUniqueNumber())
	{
		SendMsgPartyTransferMasterInfoNoLock(i_pIMParty, tmOldUID, i_pIMParty->GetMasterCharacterUniqueNumber());
	}

	return TRUE;
	*/
}

BOOL CIMIOCPSocket::SendMsgPartyTransferMasterInfoNoLock(CIMParty *i_pIMParty, UINT OldMaster, UINT NewMaster)
{
	// send to Field Servers
	INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_TRANSFER_MASTER_OK, T_FI_PARTY_TRANSFER_MASTER_OK, pFITransMasterOK, pFITransMasterOKBuf);
	pFITransMasterOK->PartyID							= i_pIMParty->m_PartyID;
	pFITransMasterOK->OldMasterCharacterUniqueNumber	= OldMaster;
	pFITransMasterOK->NewMasterCharacterUniqueNumber	= NewMaster;
	ms_pIMIOCP->SendMessageToFieldServer(pFITransMasterOKBuf, MSG_SIZE(MSG_FI_PARTY_TRANSFER_MASTER_OK));

	// send to Party Members
	INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_TRANSFER_MASTER_OK, T_IC_PARTY_TRANSFER_MASTER_OK, pICTransMasterOK, pICTransMasterOKBuf);
	pICTransMasterOK->PartyID							= i_pIMParty->m_PartyID;
	pICTransMasterOK->OldMasterCharacterUniqueNumber	= OldMaster;
	pICTransMasterOK->NewMasterCharacterUniqueNumber	= NewMaster;
	i_pIMParty->SendMsgToMembers(pICTransMasterOKBuf, MSG_SIZE(MSG_IC_PARTY_TRANSFER_MASTER_OK));
	return TRUE;
}

void CIMIOCPSocket::ProcessPartyDismember(PartyID_t i_partyID)
{
	ms_pIMIOCP->m_MapParties.lock();
	{
		CIMParty *pTmpParty = ms_pIMIOCP->m_MapParties.findLock(i_partyID);
		if(NULL == pTmpParty)
		{
			ms_pIMIOCP->m_MapParties.unlock();
			return;
		}
		pTmpParty->LockIMParty();
		{
			// send to Clients
			INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_DISMEMBER_OK, T_IC_PARTY_DISMEMBER_OK, pICDisMemberOK, pICDisMemberOKBuf);
			pICDisMemberOK->PartyID = i_partyID;
			pTmpParty->SendMsgToMembers(pICDisMemberOKBuf, MSG_SIZE(MSG_IC_PARTY_DISMEMBER_OK), INVALID_UNIQUE_NUMBER, TRUE);

			// send to Field Server
			INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_DISMEMBER_OK, T_FI_PARTY_DISMEMBER_OK, pFIDismemberOK, pFIDismemberOKBuf);
			pFIDismemberOK->PartyID = i_partyID;
			ms_pIMIOCP->SendMessageToFieldServer(pFIDismemberOKBuf, MSG_SIZE(MSG_FI_PARTY_DISMEMBER_OK));
		}
		pTmpParty->UnlockIMParty();
	}	
	// ĆÄĆĽ Á¦°Ĺ
	ms_pIMIOCP->DeleteParty(i_partyID);
	ms_pIMIOCP->m_MapParties.unlock();	
}

ProcessResult CIMIOCPSocket::Process_IC_PARTY_LEAVE(const char* pPacket, int nLength, int &nBytesUsed)
{
	int						nRecvTypeSize = 0;
	MSG_IC_PARTY_LEAVE		*pMsgLeave = NULL;

	nRecvTypeSize = sizeof(MSG_IC_PARTY_LEAVE);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_PARTY_LEAVE, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12022);
		return RES_RETURN_FALSE;
	}
	pMsgLeave = (MSG_IC_PARTY_LEAVE*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMParty *ptmParty = this->GetIMParty();
	if (ptmParty == NULL)
	{
		// error: °ˇŔÔÇŃ ĆÄĆĽ°ˇ ľřŔ˝
		SendErrorMessage(T_IC_PARTY_LEAVE, ERR_PROTOCOL_NOT_HAVE_PARTY);
		return RES_BREAK;
	}
	ptmParty->LockIMParty();
	if (ptmParty->m_PartyID != pMsgLeave->PartyID)
	{
		SendErrorMessage(T_IC_PARTY_LEAVE, ERR_PROTOCOL_PARTY_ID_NOT_MATCHED, pMsgLeave->PartyID, ptmParty->m_PartyID);
		ptmParty->UnlockIMParty();
		return RES_BREAK;
	}

	BOOL bReted = ProcessPartyLeaveNoLock(ptmParty);
	ptmParty->UnlockIMParty();

	if(FALSE == bReted)
	{// ĆÄĆĽ¸ĘżˇĽ­ Á¦°ĹÇŘľßÇŃ´Ů

		ProcessPartyDismember(pMsgLeave->PartyID);
	}

	///////////////////////////////////////////////////////////////////////////////
	// Äł¸ŻĹÍŔÇ ĆÄĆĽ Á¤ş¸ »čÁ¦
	this->SetIMParty(NULL);

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_PARTY_TRANSFER_MASTER(const char* pPacket, int nLength, int &nBytesUsed)
{
	int								nRecvTypeSize = 0;
	MSG_IC_PARTY_TRANSFER_MASTER	*pMsgTranMaster= NULL;

	nRecvTypeSize = sizeof(MSG_IC_PARTY_TRANSFER_MASTER);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_PARTY_TRANSFER_MASTER, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201D);
		return RES_RETURN_FALSE;
	}
	pMsgTranMaster = (MSG_IC_PARTY_TRANSFER_MASTER*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	if (m_character.CharacterUniqueNumber != pMsgTranMaster->OldMasterCharacterUniqueNumber)
	{// ŔÚ˝ĹŔÇ CharacterUniqueNumberżÍ ´Ů¸Ł´Ů

		SendErrorMessage(T_IC_PARTY_TRANSFER_MASTER, ERR_PROTOCOL_PARTY_MASTER_NOT_MATCHED);
		return RES_BREAK;
	}

	if(pMsgTranMaster->NewMasterCharacterUniqueNumber == m_character.CharacterUniqueNumber)
	{// 
		
		SendErrorMessage(T_IC_PARTY_TRANSFER_MASTER, ERR_COMMON_NOT_ALLOWED_TO_MYSELF);
		return RES_BREAK;
	}
	
	CIMIOCPSocket *pIISoc = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pMsgTranMaster->NewMasterCharacterUniqueNumber);
	if(NULL == pIISoc
		|| FALSE == pIISoc->IsValidCharacter())
	{// 2007-10-24 by cmkwon, »ő·Îżî ĆÄĆĽŔĺ ŔŻČż ÇŃÁö ĂĽĹ© -
		SendErrorMessage(T_IC_PARTY_TRANSFER_MASTER, ERR_PROTOCOL_NO_SUCH_PARTY_MEMBER, 0, 30);
		return RES_BREAK;
	}
	if(FALSE == m_character.MapChannelIndex.IsSameMapChannelIndex(pIISoc->m_character.MapChannelIndex))
	{// 2007-10-24 by cmkwon, »ő·Îżî ĆÄĆĽŔĺ ŔŻČż ÇŃÁö ĂĽĹ© - °°Ŕş ¸Ężˇ ŔÖ´ÂÁö ĂĽĹ©
		SendErrorMessage(T_IC_PARTY_TRANSFER_MASTER, ERR_PROTOCOL_MAP_CHANNEL_NOT_MATCHED, 0, 40);
		return RES_BREAK;
	}


	CIMParty *ptmParty = this->GetIMParty();
	if (ptmParty == NULL || ptmParty->m_PartyID != pMsgTranMaster->PartyID)
	{// error

		SendErrorMessage(T_IC_PARTY_TRANSFER_MASTER, ERR_PROTOCOL_NO_SUCH_PARTY);
		return RES_BREAK;
	}
	ptmParty->LockIMParty();
	{
		if (ptmParty->GetMasterCharacterUniqueNumber() != pMsgTranMaster->OldMasterCharacterUniqueNumber)
		{// error: ŔÚ˝ĹŔĚ ĆÄĆĽŔĺŔĚ ľĆ´ĎąÇ·Î ±ÇÇŃŔĚ ľřŔ˝

			ptmParty->UnlockIMParty();
			SendErrorMessage(T_IC_PARTY_TRANSFER_MASTER, ERR_PROTOCOL_PARTY_PERMISSION_DENIED);
			return RES_BREAK;
		}

		if(FALSE == ptmParty->TransferMaster(pMsgTranMaster->NewMasterCharacterUniqueNumber))
		{// error

			ptmParty->UnlockIMParty();
			SendErrorMessage(T_IC_PARTY_TRANSFER_MASTER, ERR_PROTOCOL_NO_SUCH_PARTY_MEMBER);
			return RES_BREAK;
		}
		
		SendMsgPartyTransferMasterInfoNoLock(ptmParty, pMsgTranMaster->OldMasterCharacterUniqueNumber, pMsgTranMaster->NewMasterCharacterUniqueNumber);
	}
	ptmParty->UnlockIMParty();

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_PARTY_BAN_MEMBER(const char* pPacket, int nLength, int &nBytesUsed)
{
	int						nRecvTypeSize = 0;
	MSG_IC_PARTY_BAN_MEMBER	*pMsgBan = NULL;

	nRecvTypeSize = sizeof(MSG_IC_PARTY_BAN_MEMBER);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_PARTY_BAN_MEMBER, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12020);
		return RES_RETURN_FALSE;
	}
	pMsgBan = (MSG_IC_PARTY_BAN_MEMBER*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	if (pMsgBan->CharacterUniqueNumber == m_character.CharacterUniqueNumber)
	{
		SendErrorMessage(T_IC_PARTY_BAN_MEMBER, ERR_COMMON_NOT_ALLOWED_TO_MYSELF);
		return RES_BREAK;
	}

	CIMParty *ptmParty = this->GetIMParty();
	if (ptmParty == NULL)
	{
		// error
		SendErrorMessage(T_IC_PARTY_BAN_MEMBER, ERR_PROTOCOL_NOT_HAVE_PARTY);
		return RES_BREAK;
	}

	ptmParty->LockIMParty();
	{
		if (ptmParty->m_PartyID != pMsgBan->PartyID)
		{
			// error
			ptmParty->UnlockIMParty();
			SendErrorMessage(T_IC_PARTY_BAN_MEMBER, ERR_PROTOCOL_PARTY_ID_NOT_MATCHED);
			return RES_BREAK;
		}

		if (ptmParty->GetMasterCharacterUniqueNumber() != m_character.CharacterUniqueNumber)
		{
			// error: ŔÚ˝ĹŔĚ ĆÄĆĽŔĺŔĚ ľĆ´ĎąÇ·Î ±ÇÇŃŔĚ ľřŔ˝
			ptmParty->UnlockIMParty();
			SendErrorMessage(T_IC_PARTY_BAN_MEMBER, ERR_PROTOCOL_PARTY_PERMISSION_DENIED);
			return RES_BREAK;
		}

		// ĂŁľĆş¸±â
		IMPartyMember *pMember = ptmParty->GetMemberPtrByCharacterUniqueNumber(pMsgBan->CharacterUniqueNumber);
		if (NULL == pMember)
		{
			// error
			ptmParty->UnlockIMParty();
			SendErrorMessage(T_IC_PARTY_BAN_MEMBER, ERR_PROTOCOL_NO_SUCH_PARTY_MEMBER);
			return RES_BREAK;
		}

		// °ÔŔÓŔ» ÇĎÁö ľĘ´Â ĆÄĆĽżřŔÎ °ćżě - ±×łÉ ĆÄĆĽĽ­ Á¦°ĹÇÔ
		if (FALSE == pMember->IsOnlineMember() || FALSE == pMember->PM_pIMIOCPSocket->IsUsing())
		{
			// ĂßąćÇŇ ľĆ±× Áöżě±â
			if (FALSE == ptmParty->LeaveMember(pMsgBan->CharacterUniqueNumber))
			{
				ptmParty->UnlockIMParty();
				SendErrorMessage(T_IC_PARTY_BAN_MEMBER, ERR_COMMON_NO_SUCH_CHARACTER, pMsgBan->CharacterUniqueNumber, 1);
				return RES_BREAK;
			}

// 			// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - ŔÓ˝Ăżë ·Î±×
// 			g_pIMGlobal->WriteSystemLogEX(TRUE, "  [TEMP] 090804  CIMIOCPSocket::Process_IC_PARTY_BAN_MEMBER# 1  %s CurIsOn(%d) FormationTy(%d) \r\n"
// 				, GetCharacterString(GetCharacter(), string()), ptmParty->m_SPartyInfo.IsAppliedFormationSkill, ptmParty->m_SPartyInfo.FormationType);

			///////////////////////////////////////////////////////////////////////////////
			// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - Ćí´ëżř Ăßąć˝Ă, żŔÇÁ¶óŔÎ Ćí´ëżř Ăł¸®
			if(ptmParty->m_SPartyInfo.IsAppliedFormationSkill)
			{
				ptmParty->m_SPartyInfo.IsAppliedFormationSkill	= FALSE;
				this->SendFieldServPartyChangeFormationSkill(m_character.CharacterUniqueNumber, FALSE);
			}

			// send to SERVER
			INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_BAN_MEMBER_OK, T_FI_PARTY_BAN_MEMBER_OK, pFIBanMemberOK, pFIBanMemberOKBuf);
			pFIBanMemberOK->PartyID					= pMsgBan->PartyID;
			pFIBanMemberOK->CharacterUniqueNumber	= pMsgBan->CharacterUniqueNumber;
			pFIBanMemberOK->FieldPartyInfo.nTotalPartyMember = ptmParty->GetMemberCount(FALSE);	// ÁÖŔÇ: m_ListPartyMemberżˇĽ­ »čÁ¦ ŔĚČÄż©ľß ÇÔ!
			pFIBanMemberOK->FieldPartyInfo.lowestMemberLevel = ptmParty->GetLowestMemberLevel();	// ÁÖŔÇ: m_ListPartyMemberżˇĽ­ »čÁ¦ ŔĚČÄż©ľß ÇÔ!
			ms_pIMIOCP->SendMessageToFieldServer(pFIBanMemberOKBuf, MSG_SIZE(MSG_FI_PARTY_BAN_MEMBER_OK));

			// send to CLIENTS
			INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_BAN_MEMBER_OK, T_IC_PARTY_BAN_MEMBER_OK, pICBanMemberOK, pICBanMemberOKBuf);
			pICBanMemberOK->PartyID					= pMsgBan->PartyID;
			pICBanMemberOK->CharacterUniqueNumber	= pMsgBan->CharacterUniqueNumber;
			ptmParty->SendMsgToMembers(pICBanMemberOKBuf, MSG_SIZE(MSG_IC_PARTY_BAN_MEMBER_OK));
		}
		else
		{
			// Ăßąć ´çÇŇ ľĆ±× ĂŁ±â
			CIMIOCPSocket *pSocket = pMember->PM_pIMIOCPSocket;	// Ăßąć ´çÇŇ ĆÄĆĽżř
			if (pSocket == NULL || !pSocket->IsUsing() || pSocket->m_character.CharacterUniqueNumber != pMember->PM_CharacterUniqueNumber)
			{
				ptmParty->UnlockIMParty();
				SendErrorMessage(T_IC_PARTY_BAN_MEMBER, ERR_COMMON_NO_SUCH_CHARACTER, pMsgBan->CharacterUniqueNumber, 2);
				return RES_BREAK;
			}

			// ĂßąćÇŇ ľĆ±× Áöżě±â
			if (FALSE == ptmParty->LeaveMember(pMsgBan->CharacterUniqueNumber))
			{
				ptmParty->UnlockIMParty();
				SendErrorMessage(T_IC_PARTY_BAN_MEMBER, ERR_COMMON_NO_SUCH_CHARACTER, pMsgBan->CharacterUniqueNumber, 1);
				return RES_BREAK;
			}

// 			// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - ŔÓ˝Ăżë ·Î±×
// 			g_pIMGlobal->WriteSystemLogEX(TRUE, "  [TEMP] 090804  CIMIOCPSocket::Process_IC_PARTY_BAN_MEMBER# 2  %s CurIsOn(%d) FormationTy(%d) \r\n"
// 				, GetCharacterString(GetCharacter(), string()), ptmParty->m_SPartyInfo.IsAppliedFormationSkill, ptmParty->m_SPartyInfo.FormationType);

			///////////////////////////////////////////////////////////////////////////////
			// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - Ćí´ëżř Ăßąć˝Ă, żÂ¶óŔÎ Ćí´ëżř Ăł¸®
			if(ptmParty->m_SPartyInfo.IsAppliedFormationSkill)
			{
				ptmParty->m_SPartyInfo.IsAppliedFormationSkill	= FALSE;
				this->SendFieldServPartyChangeFormationSkill(m_character.CharacterUniqueNumber, FALSE);
			}

			// send to SERVER
			INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_BAN_MEMBER_OK, T_FI_PARTY_BAN_MEMBER_OK, pFIBanMemberOK, pFIBanMemberOKBuf);
			pFIBanMemberOK->PartyID					= pMsgBan->PartyID;
			pFIBanMemberOK->CharacterUniqueNumber	= pMsgBan->CharacterUniqueNumber;
			pFIBanMemberOK->FieldPartyInfo.nTotalPartyMember = ptmParty->GetMemberCount(FALSE);		// ÁÖŔÇ: m_ListPartyMemberżˇĽ­ »čÁ¦ ŔĚČÄż©ľß ÇÔ!
			pFIBanMemberOK->FieldPartyInfo.lowestMemberLevel = ptmParty->GetLowestMemberLevel();		// ÁÖŔÇ: m_ListPartyMemberżˇĽ­ »čÁ¦ ŔĚČÄż©ľß ÇÔ!
			pSocket->SendMsgToCurrentFieldServerSocket(pFIBanMemberOKBuf, MSG_SIZE(MSG_FI_PARTY_BAN_MEMBER_OK));

			// send to CLIENTS
			INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_BAN_MEMBER_OK, T_IC_PARTY_BAN_MEMBER_OK, pICBanMemberOK, pICBanMemberOKBuf);
			pICBanMemberOK->PartyID					= pMsgBan->PartyID;
			pICBanMemberOK->CharacterUniqueNumber	= pMsgBan->CharacterUniqueNumber;
			ptmParty->SendMsgToMembers(pICBanMemberOKBuf, MSG_SIZE(MSG_IC_PARTY_BAN_MEMBER_OK));

			// Ŕ§żˇĽ­ Ăßąć ´çÇŃ ŔÚ ÁöżüŔ¸ąÇ·Î µű·Î ş¸łż
			pSocket->SendAddData(pICBanMemberOKBuf, MSG_SIZE(MSG_IC_PARTY_BAN_MEMBER_OK));

			// Ăßąć ´çÇŃ ľĆ±×żˇĽ­ ĆÄĆĽ Á¤ş¸ Á¦°Ĺ
			pSocket->SetIMParty(NULL);
		}
	}// LockIMParty_end
	ptmParty->UnlockIMParty();

	return RES_RETURN_TRUE;
}

// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - »çżëÇĎÁö ľĘ´Â ÇÁ·ÎĹäÄÝŔÓ
// ProcessResult CIMIOCPSocket::Process_IC_PARTY_DISMEMBER(const char* pPacket, int nLength, int &nBytesUsed)
// {
// 	int						nRecvTypeSize = 0;
// 	MSG_IC_PARTY_DISMEMBER	*pMsgDismember = NULL;
// 
// 	nRecvTypeSize = sizeof(MSG_IC_PARTY_DISMEMBER);
// 	if(nLength - nBytesUsed < nRecvTypeSize)
// 	{
// 		SendErrorMessage(T_IC_PARTY_DISMEMBER, ERR_PROTOCOL_INVALID_FIELD_DATA);
// 		Close(0x12021);
// 		return RES_RETURN_FALSE;
// 	}
// 	pMsgDismember = (MSG_IC_PARTY_DISMEMBER*)(pPacket + nBytesUsed);
// 	nBytesUsed += nRecvTypeSize;
// 
// 	CIMParty *ptmParty = this->GetIMParty();
// 	if (ptmParty == NULL)
// 	{
// 		// error
// 		SendErrorMessage(T_IC_PARTY_DISMEMBER, ERR_PROTOCOL_NOT_HAVE_PARTY);
// 		return RES_BREAK;
// 	}
// 
// 	ptmParty->LockIMParty();
// 	{
// 		if (ptmParty->m_PartyID != pMsgDismember->PartyID)
// 		{// error
// 
// 			ptmParty->UnlockIMParty();
// 			SendErrorMessage(T_IC_PARTY_DISMEMBER, ERR_PROTOCOL_PARTY_ID_NOT_MATCHED);
// 			return RES_BREAK;
// 		}
// 
// 		if (ptmParty->GetMasterCharacterUniqueNumber() != m_character.CharacterUniqueNumber)
// 		{// error: ŔÚ˝ĹŔĚ ĆÄĆĽŔĺŔĚ ľĆ´ĎąÇ·Î ±ÇÇŃŔĚ ľřŔ˝
// 			
// 			ptmParty->UnlockIMParty();
// 			SendErrorMessage(T_IC_PARTY_DISMEMBER, ERR_PROTOCOL_PARTY_PERMISSION_DENIED);
// 			return RES_BREAK;
// 		}
// 	}
// 	PartyID_t partyID = ptmParty->m_PartyID;
// 	ptmParty->UnlockIMParty();
// 
// 	// ĆÄĆĽ ÇŘ»ę
// 	ProcessPartyDismember(partyID);
// 
// 	///////////////////////////////////////////////////////////////////////////////
// 	// Äł¸ŻĹÍŔÇ ĆÄĆĽ Á¤ş¸ »čÁ¦
// 	this->SetIMParty(NULL);
// 	return RES_RETURN_TRUE;
// }

ProcessResult CIMIOCPSocket::Process_IC_PARTY_CHANGE_FLIGHT_FORMATION(const char* pPacket, int nLength, int &nBytesUsed)
{
	int										nRecvTypeSize = 0;
	MSG_IC_PARTY_CHANGE_FLIGHT_FORMATION	*pMsgChangeForm = NULL;

	nRecvTypeSize = sizeof(MSG_IC_PARTY_CHANGE_FLIGHT_FORMATION);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_PARTY_CHANGE_FLIGHT_FORMATION, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12021);
		return RES_RETURN_FALSE;
	}
	pMsgChangeForm = (MSG_IC_PARTY_CHANGE_FLIGHT_FORMATION*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMParty *ptmParty = this->GetIMParty();

	if (ptmParty == NULL)
	{
		SendErrorMessage(T_IC_PARTY_CHANGE_FLIGHT_FORMATION, ERR_PROTOCOL_NO_SUCH_PARTY);
		return RES_BREAK;
	}

	ptmParty->LockIMParty();
	{
		if (ptmParty->m_PartyID != pMsgChangeForm->PartyID)
		{
			SendErrorMessage(T_IC_PARTY_CHANGE_FLIGHT_FORMATION, ERR_PROTOCOL_PARTY_ID_NOT_MATCHED, LODWORD(ptmParty->m_PartyID), HIDWORD(pMsgChangeForm->PartyID));
			ptmParty->UnlockIMParty();
			return RES_BREAK;
		}

		if (ptmParty->GetMasterCharacterUniqueNumber() != m_character.CharacterUniqueNumber)
		{			
			SendErrorMessage(T_IC_PARTY_CHANGE_FLIGHT_FORMATION, ERR_PROTOCOL_PARTY_PERMISSION_DENIED,
				ptmParty->GetMasterCharacterUniqueNumber(), m_character.CharacterUniqueNumber);
			ptmParty->UnlockIMParty();
			return RES_BREAK;
		}

// 		// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - ŔÓ˝Ăżë ·Î±×
// 		g_pIMGlobal->WriteSystemLogEX(TRUE, "  [TEMP] 090804  CIMIOCPSocket::Process_IC_PARTY_CHANGE_FLIGHT_FORMATION#  %s CurIsOn(%d) FormationTy(%d<-%d) \r\n"
// 			, GetCharacterString(GetCharacter(), string()), ptmParty->m_SPartyInfo.IsAppliedFormationSkill, pMsgChangeForm->Formation, ptmParty->m_SPartyInfo.FormationType);

		///////////////////////////////////////////////////////////////////////////////
		// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - Ćí´ë ´ëÇü şŻ°ć˝Ă, ÇöŔç ´ëÇüŔ» ŔúŔĺÇŃ´Ů.
		ptmParty->m_SPartyInfo.FormationType	= pMsgChangeForm->Formation;
		if(ptmParty->m_SPartyInfo.IsAppliedFormationSkill)
		{
			ptmParty->m_SPartyInfo.IsAppliedFormationSkill	= FALSE;
			this->SendFieldServPartyChangeFormationSkill(m_character.CharacterUniqueNumber, FALSE);
		}

		INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_CHANGE_FLIGHT_FORMATION_OK, T_IC_PARTY_CHANGE_FLIGHT_FORMATION_OK, pChangeFormOK, pChangeFormOKBuf);
		pChangeFormOK->PartyID = pMsgChangeForm->PartyID;
		pChangeFormOK->Formation = pMsgChangeForm->Formation;
		ptmParty->SendMsgToMembers(pChangeFormOKBuf, MSG_SIZE(MSG_IC_PARTY_CHANGE_FLIGHT_FORMATION_OK));
	}
	ptmParty->UnlockIMParty();


// 2004-08-09 by cmkwon
//	// send to SERVER
//	INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_CHANGE_FLIGHT_FORMATION_OK, T_FI_PARTY_CHANGE_FLIGHT_FORMATION_OK, pFIFormationOK, pFIFormationOKBuf);
//	pFIFormationOK->PartyID		= pMsgChangeForm->PartyID;
//	pFIFormationOK->Formation	= pMsgChangeForm->Formation;
//	m_pIMParty->SendMsgToFieldServers(pFIFormationOKBuf, MSG_SIZE(MSG_FI_PARTY_CHANGE_FLIGHT_FORMATION_OK));

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_PARTY_GET_FLIGHT_POSITION(const char* pPacket, int nLength, int &nBytesUsed)
{
	int									nRecvTypeSize = 0;
	MSG_IC_PARTY_GET_FLIGHT_POSITION	*pMsgGetPosition = NULL;

	nRecvTypeSize = sizeof(MSG_IC_PARTY_GET_FLIGHT_POSITION);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_PARTY_GET_FLIGHT_POSITION, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12021);
		return RES_RETURN_FALSE;
	}
	pMsgGetPosition = (MSG_IC_PARTY_GET_FLIGHT_POSITION*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	if (m_character.CharacterUniqueNumber != pMsgGetPosition->CharacterUniqueNumber)
	{
		SendErrorMessage(T_IC_PARTY_GET_FLIGHT_POSITION, ERR_PROTOCOL_INVALID_CHARACTER_UNIQUENUMBER, m_character.CharacterUniqueNumber, pMsgGetPosition->CharacterUniqueNumber);
		return RES_BREAK;
	}

	CIMParty *ptmParty = this->GetIMParty();
	if (ptmParty == NULL)
	{
		SendErrorMessage(T_IC_PARTY_GET_FLIGHT_POSITION, ERR_PROTOCOL_NO_SUCH_PARTY);
		return RES_BREAK;
	}

	ptmParty->LockIMParty();
	// ĆÄĆĽŔĺżˇ°Ô ş¸ł»±â
	CIMIOCPSocket *pPartyMasterSock = ptmParty->GetMasterIMIOCPSocket();
	if (NULL == pPartyMasterSock 
		|| FALSE == pPartyMasterSock->IsUsing())
	{		
		SendErrorMessage(T_IC_PARTY_GET_FLIGHT_POSITION, ERR_PROTOCOL_PARTY_MASTER_NOT_MATCHED,
						pPartyMasterSock->m_character.CharacterUniqueNumber, ptmParty->GetMasterCharacterUniqueNumber());
		ptmParty->UnlockIMParty();
		return RES_BREAK;
	}
	ptmParty->UnlockIMParty();

	// ŔüĽŰ
	INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_GET_FLIGHT_POSITION, T_IC_PARTY_GET_FLIGHT_POSITION, pSend, pSendBuf);
	pSend->CharacterUniqueNumber = pMsgGetPosition->CharacterUniqueNumber;
	pPartyMasterSock->SendAddData(pSendBuf, MSG_SIZE(MSG_IC_PARTY_GET_FLIGHT_POSITION));

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_PARTY_CHANGE_FLIGHT_POSITION(const char* pPacket, int nLength, int &nBytesUsed)
{
	int									nRecvTypeSize = 0;
	MSG_IC_PARTY_CHANGE_FLIGHT_POSITION	*pMsgChangePos = NULL;

	nRecvTypeSize = sizeof(MSG_IC_PARTY_CHANGE_FLIGHT_POSITION);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_PARTY_CHANGE_FLIGHT_POSITION, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12021);
		return RES_RETURN_FALSE;
	}
	pMsgChangePos = (MSG_IC_PARTY_CHANGE_FLIGHT_POSITION*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMParty *ptmParty = this->GetIMParty();
	if (ptmParty == NULL)
	{
		SendErrorMessage(T_IC_PARTY_CHANGE_FLIGHT_POSITION, ERR_PROTOCOL_NO_SUCH_PARTY);
		return RES_BREAK;
	}

	ptmParty->LockIMParty();
	{
		if (ptmParty->GetMasterCharacterUniqueNumber() != m_character.CharacterUniqueNumber)
		{
			SendErrorMessage(T_IC_PARTY_CHANGE_FLIGHT_POSITION, ERR_PROTOCOL_PARTY_PERMISSION_DENIED, ptmParty->GetMasterCharacterUniqueNumber(), m_character.CharacterUniqueNumber);
			ptmParty->UnlockIMParty();
			return RES_BREAK;
		}

		// ĆÄĆĽżř ĂŁ±â
		IMPartyMember *pMember = ptmParty->GetMemberPtrByCharacterUniqueNumber(pMsgChangePos->CharacterUniqueNumber);		
		if (NULL == pMember
			|| FALSE == pMember->IsOnlineMember()
			|| FALSE == pMember->PM_pIMIOCPSocket->IsUsing()
			|| pMember->PM_pIMIOCPSocket->m_character.CharacterUniqueNumber != pMember->PM_CharacterUniqueNumber)
		{
			ptmParty->UnlockIMParty();
			SendErrorMessage(T_IC_PARTY_CHANGE_FLIGHT_POSITION, ERR_PROTOCOL_INVALID_CHARACTER_UNIQUENUMBER,
							pMsgChangePos->CharacterUniqueNumber, pMember->PM_CharacterUniqueNumber);
			return RES_BREAK;
		}

		// ŔüĽŰ
		INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_CHANGE_FLIGHT_POSITION, T_IC_PARTY_CHANGE_FLIGHT_POSITION, pSend, tmSendBuf);
		pSend->CharacterUniqueNumber = pMsgChangePos->CharacterUniqueNumber;
		pSend->FlightPosition = pMsgChangePos->FlightPosition;
		ptmParty->SendMsgToMembers(tmSendBuf, MSG_SIZE(MSG_IC_PARTY_CHANGE_FLIGHT_POSITION));
	}
	ptmParty->UnlockIMParty();

// 2004-08-09 by cmkwon
//	INIT_MSG(MSG_FI_PARTY_CHANGE_FLIGHT_POSITION, T_FI_PARTY_CHANGE_FLIGHT_POSITION, pFISend, tmSendBuf);
//	pFISend->PartyID				= m_pIMParty->m_PartyID;
//	pFISend->CharacterUniqueNumber	= pMsgChangePos->CharacterUniqueNumber;
//	pFISend->FlightPosition			= pMsgChangePos->FlightPosition;
//	m_pIMParty->SendMsgToFieldServers(tmSendBuf, MSG_SIZE(MSG_FI_PARTY_CHANGE_FLIGHT_POSITION));

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_PARTY_CANCEL_FLIGHT_POSITION(const char* pPacket, int nLength, int &nBytesUsed)
{
	int									nRecvTypeSize = 0;
	MSG_IC_PARTY_CANCEL_FLIGHT_POSITION	*pMsgCancelPos = NULL;

	nRecvTypeSize = sizeof(MSG_IC_PARTY_CANCEL_FLIGHT_POSITION);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_PARTY_CANCEL_FLIGHT_POSITION, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12021);
		return RES_RETURN_FALSE;
	}
	pMsgCancelPos = (MSG_IC_PARTY_CANCEL_FLIGHT_POSITION*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	if (pMsgCancelPos->CharacterUniqueNumber != m_character.CharacterUniqueNumber)
	{
		SendErrorMessage(T_IC_PARTY_CANCEL_FLIGHT_POSITION, ERR_PROTOCOL_INVALID_CHARACTER_UNIQUENUMBER,
						pMsgCancelPos->CharacterUniqueNumber, m_character.CharacterUniqueNumber);
		return RES_BREAK;
	}

	CIMParty *ptmParty = this->GetIMParty();
	if (ptmParty == NULL)
	{
		SendErrorMessage(T_IC_PARTY_CANCEL_FLIGHT_POSITION, ERR_PROTOCOL_NO_SUCH_PARTY);
		return RES_BREAK;
	}

	ptmParty->LockIMParty();

	// ĆÄĆĽŔĺżˇ°Ô ş¸ł»±â
	CIMIOCPSocket *pPartyMasterSock = ptmParty->GetMasterIMIOCPSocket();
	
	// 2011-02-22 by shcho&hsSon, Ćí´ëąöÇÁ ÇŘÁ¦ ľČµÇ´Â ąö±× ĽöÁ¤
	CIMIOCPSocket *pPartyUserSock = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pMsgCancelPos->CharacterUniqueNumber);
	// end 2011-02-22 by shcho&hsSon, Ćí´ëąöÇÁ ÇŘÁ¦ ľČµÇ´Â ąö±× ĽöÁ¤

	if (pPartyMasterSock == NULL
		|| FALSE == pPartyMasterSock->IsUsing())
	{
		SendErrorMessage(T_IC_PARTY_CANCEL_FLIGHT_POSITION, ERR_PROTOCOL_PARTY_MASTER_NOT_MATCHED,
						ptmParty->GetMasterCharacterUniqueNumber());
		ptmParty->UnlockIMParty();
		return RES_BREAK;
	}
	ptmParty->UnlockIMParty();

	// 2011-02-22 by shcho&hsSon, Ćí´ëąöÇÁ ÇŘÁ¦ ľČµÇ´Â ąö±× ĽöÁ¤
    // ¸¶˝şĹÍ ŔüĽŰ
	INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_CANCEL_FLIGHT_POSITION, T_IC_PARTY_CANCEL_FLIGHT_POSITION, pCancel, tmSendBuf);
	pCancel->Formation_On_Off		= FORMATION_SKILL_NULL;
	pCancel->CharacterUniqueNumber = pMsgCancelPos->CharacterUniqueNumber;
	pPartyMasterSock->SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_PARTY_CANCEL_FLIGHT_POSITION));

	//29-08-2020 by Inetpub - masangs are nobs
	if (pPartyUserSock == NULL || FALSE == pPartyUserSock->IsUsing()) {
		return RES_RETURN_TRUE;
	}
	else {
		//end
		// ÇŘ´ç ŔŻŔúżˇ°Ôµµ ŔüĽŰ
		INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_CANCEL_FLIGHT_POSITION, T_IC_PARTY_CANCEL_FLIGHT_POSITION, pPartyUserCancel, tmSendBuf1);
		pPartyUserCancel->Formation_On_Off = FORMATION_SKILL_OFF;
		pPartyUserCancel->CharacterUniqueNumber = pMsgCancelPos->CharacterUniqueNumber;
		pPartyUserSock->SendAddData(tmSendBuf1, MSG_SIZE(MSG_IC_PARTY_CANCEL_FLIGHT_POSITION));
		// END 2011-02-22 by shcho&hsSon, Ćí´ëąöÇÁ ÇŘÁ¦ ľČµÇ´Â ąö±× ĽöÁ¤
	}
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_PARTY_ALL_FLIGHT_POSITION(const char* pPacket, int nLength, int &nBytesUsed)
{
	int									nRecvTypeSize = 0;
	MSG_IC_PARTY_ALL_FLIGHT_POSITION	*pRecvMsg = NULL;

	nRecvTypeSize = sizeof(MSG_IC_PARTY_ALL_FLIGHT_POSITION);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_PARTY_ALL_FLIGHT_POSITION, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12021);
		return RES_RETURN_FALSE;
	}
	pRecvMsg = (MSG_IC_PARTY_ALL_FLIGHT_POSITION*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	///////////////////////////////////////////////////////////////////////////////
	// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - ´ëÇü Á¤·Ä Á¤ş¸ ŔüĽŰ˝Ă żŔÇÁ¶óŔÎŔŻŔúĂĽĹ©
	BOOL bIsAllPartyMemberOnline = TRUE;
	for(int i=0; i < SIZE_MAX_PARTY_MEMBER-1; i++)
	{
		if(pRecvMsg->AllPartyMemberCharacterUniqueNumber[i])
		{
			CIMIOCPSocket *pIISoc = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pRecvMsg->AllPartyMemberCharacterUniqueNumber[i]);
			if(NULL == pIISoc
				|| FALSE == pIISoc->IsValidCharacter())
			{
				bIsAllPartyMemberOnline		= FALSE;
			}
		}
	}

	CIMParty *ptmParty = this->GetIMParty();
	if (ptmParty == NULL)
	{
		SendErrorMessage(T_IC_PARTY_ALL_FLIGHT_POSITION, ERR_PROTOCOL_NO_SUCH_PARTY);
		return RES_BREAK;
	}

// 	// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - ŔÓ˝Ăżë ·Î±×
// 	g_pIMGlobal->WriteSystemLogEX(TRUE, "[TEMP] 090804  Process_IC_PARTY_ALL_FLIGHT_POSITION# %s IsOn(%d) FormationTy(%d) AllOnline(%d) (%d %d %d %d %d)\r\n"
// 		, GetCharacterString(GetCharacter(), string()), ptmParty->m_SPartyInfo.IsAppliedFormationSkill, ptmParty->m_SPartyInfo.FormationType, bIsAllPartyMemberOnline
// 		, pRecvMsg->AllPartyMemberCharacterUniqueNumber[0], pRecvMsg->AllPartyMemberCharacterUniqueNumber[1], pRecvMsg->AllPartyMemberCharacterUniqueNumber[2]
// 		, pRecvMsg->AllPartyMemberCharacterUniqueNumber[3], pRecvMsg->AllPartyMemberCharacterUniqueNumber[4]);

	ptmParty->LockIMParty();
	{
		if (ptmParty->GetMasterCharacterUniqueNumber() != m_character.CharacterUniqueNumber)
		{
			SendErrorMessage(T_IC_PARTY_ALL_FLIGHT_POSITION, ERR_PROTOCOL_PARTY_PERMISSION_DENIED
				, m_character.CharacterUniqueNumber, ptmParty->GetMasterCharacterUniqueNumber());
			ptmParty->UnlockIMParty();
			return RES_BREAK;
		}

		// ĆÄĆĽŔĺ ĂĽĹ©
		CIMIOCPSocket *pPartyMasterSock = ptmParty->GetMasterIMIOCPSocket();
		if (pPartyMasterSock == NULL
			|| !pPartyMasterSock->IsUsing())
		{
			SendErrorMessage(T_IC_PARTY_ALL_FLIGHT_POSITION, ERR_PROTOCOL_PARTY_MASTER_NOT_MATCHED,
							ptmParty->GetMasterCharacterUniqueNumber());
			ptmParty->UnlockIMParty();
			return RES_BREAK;
		}

		///////////////////////////////////////////////////////////////////////////////
		// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - 
		if(FALSE == bIsAllPartyMemberOnline
			|| 0 == pRecvMsg->AllPartyMemberCharacterUniqueNumber[SIZE_MAX_PARTY_MEMBER-1-1]
			|| FLIGHT_FORM_NONE == ptmParty->m_SPartyInfo.FormationType)
		{
			// Ćí´ë ´ëÇü ˝şĹłŔ» ˛ô´Â Ăł¸®¸¦ ÇŘľßÇÔ.
			if(ptmParty->m_SPartyInfo.IsAppliedFormationSkill)
			{
				ptmParty->m_SPartyInfo.IsAppliedFormationSkill	= FALSE;
				this->SendFieldServPartyChangeFormationSkill(m_character.CharacterUniqueNumber, FALSE);
			}
		}
		else
		{
			// Ćí´ë ´ëÇü ˝şĹłŔ» Ĺ°´Â Ăł¸®¸¦ ÇŘľßÇÔ.
			if(FALSE == ptmParty->m_SPartyInfo.IsAppliedFormationSkill)
			{
				ptmParty->m_SPartyInfo.IsAppliedFormationSkill	= TRUE;
				this->SendFieldServPartyChangeFormationSkill(m_character.CharacterUniqueNumber, TRUE, ptmParty->m_SPartyInfo.FormationType);
			}
		}
		
		// ĆÄĆĽŔĺŔ¸·Î şÎĹÍ ŔüĽŰąŢŔş Ćí´ë Position Á¤ş¸¸¦ Ćí´ëŔĺŔ» Á¦żÜÇŃ Ćí´ëżř ¸đµÎżˇ°Ô ŔüĽŰÇŃ´Ů
		INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_ALL_FLIGHT_POSITION, T_IC_PARTY_ALL_FLIGHT_POSITION, pSend, tmSendBuf);
		*pSend = *pRecvMsg;
		ptmParty->SendMsgToMembers(tmSendBuf, MSG_SIZE(MSG_IC_PARTY_ALL_FLIGHT_POSITION), m_character.CharacterUniqueNumber);
	}
	ptmParty->UnlockIMParty();

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_PARTY_REQUEST_PARTYINFO_FROM_A_TO_M(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		ľĆ·ąłŞ ĹëÇŐ - ľĆ·ąłŞ Áľ·á ČÄ ¸ŢŔÎ Ľ­ąöżˇ żÔŔ» ¶§ ±âÁ¸ ĆÄĆĽ °Ë»ç
/// \author		dhjin
/// \date		2008-02-28 ~ 2008-02-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_PARTY_REQUEST_PARTYINFO_FROM_A_TO_M(const char* pPacket, int nLength, int &nBytesUsed)
{
	if (this->m_character.LastPartyID != 0)
	{
#ifdef _DEBUG
		this->SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0005);
#endif
		// ĆÄĆĽ °Ë»ö
		CIMParty *pParty = ms_pIMIOCP->FindParty(this->m_character.LastPartyID);
		if (pParty == NULL)
		{
			// ĆÄĆĽ°ˇ ŔĚąĚ ÇŘ»ęµČ °ćżě
			this->SendErrorMessage(T_FI_CONNECT_NOTIFY_GAMESTART, ERR_PROTOCOL_NO_SUCH_PARTY, this->m_character.LastPartyID);

			this->SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0006);

			// Update DB
			QPARAM_SAVELASTPARTYID *pQSavePartyID = new QPARAM_SAVELASTPARTYID;
			pQSavePartyID->CharacterUniqueNumber = this->m_character.CharacterUniqueNumber;
			pQSavePartyID->LastPartyID = 0;
			ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_SaveLastPartyID, this, pQSavePartyID);

			// Update CHARACTER
			this->m_character.LastPartyID = 0;
			return RES_RETURN_TRUE;
		}
		
		pParty->LockIMParty();
		{
			IMPartyMember *pMember = pParty->GetMemberPtrByCharacterUniqueNumber(this->m_character.CharacterUniqueNumber);
			if (pMember == NULL)
			{
				// ĆÄĆĽżřŔĚ ľř´Â °ćżě
				pParty->UnlockIMParty();
				return RES_RETURN_TRUE;
			}

			// »çżëŔÚżˇ°Ô ĆÄĆĽ Á¤ş¸ ÇŇ´ç
			this->SetIMParty(pParty);

			// validate party member
			pMember->SetOnlineMember(this);

			// ŔÚ±â ŔÚ˝Ĺżˇ°Ô ş¸łż
			INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_PUT_LAST_PARTY_INFO, T_IC_PARTY_PUT_LAST_PARTY_INFO, pLastPartyInfo, pLastPartyInfoBuf);
			pLastPartyInfo->PartyID = pParty->m_PartyID;
			this->SendAddData(pLastPartyInfoBuf, MSG_SIZE(MSG_IC_PARTY_PUT_LAST_PARTY_INFO));

			this->SendString128(STRING_128_USER_NOTICE, STRMSG_S_I2NOTIFY_0007);
		}
		pParty->UnlockIMParty();
	}	

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_PARTY_LEAVE_FROM_M_TO_A(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		ľĆ·ąłŞ ĹëÇŐ - ľĆ·ąłŞ ˝ĂŔŰŔ» Ŕ§ÇŘ ¸ŢŔÎĽ­ąöżˇĽ­ ĆÄĆĽ °ü·Ă Ăł¸®	
/// \author		dhjin
/// \date		2008-02-28 ~ 2008-02-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_PARTY_LEAVE_FROM_M_TO_A(const char* pPacket, int nLength, int &nBytesUsed)
{
	CIMParty *ptmParty = this->GetIMParty();
	if (ptmParty)
	{
		ptmParty->LockIMParty();
		PartyID_t partyID = ptmParty->m_PartyID;
		BOOL bReted = ProcessPartyFromMainServerToArenaServerNoLock(ptmParty);
		ptmParty->UnlockIMParty();

		if(FALSE == bReted)
		{// ĆÄĆĽ¸ĘżˇĽ­ Á¦°ĹÇŘľßÇŃ´Ů
			
			ProcessPartyDismember(partyID);			
		}
		
		///////////////////////////////////////////////////////////////////////////////
		// Äł¸ŻĹÍŔÇ ĆÄĆĽ Á¤ş¸ »čÁ¦
		this->SetIMParty(NULL);

		//ş»Ľ· ĆÄĆĽ´Â Ĺ»Ĺđ Ăł¸®·Î ĽöÁ¤µÇľî DBŔúŔĺÇĎÁö ľĘŔ˝. 2010.09.03. by hsLee.
		/*
		QPARAM_SAVELASTPARTYID *pQSavePartyID	= new QPARAM_SAVELASTPARTYID;
		pQSavePartyID->CharacterUniqueNumber	= m_character.CharacterUniqueNumber;
		pQSavePartyID->LastPartyID				= partyID;
		ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_SaveLastPartyID, this, pQSavePartyID);
		*/
	}

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_PARTY_LEAVE_FROM_A_TO_M(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		ľĆ·ąłŞ Ľ­ąöżˇĽ­ ¸ŢŔÎĽ­ąö·Î şą±ÍÇŇ¶§ ľĆ·ąłŞ Ľ­ąöżˇĽ­ ĆÄĆĽ Ĺ»Ĺđ Ăł¸® Ĺ¬¶óŔĚľđĆ®żˇ°Ô ŔüĽŰÇĎÁö ľĘ´Â´Ů.
/// \author		dhjin
/// \date		2008-02-28 ~ 2008-02-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_PARTY_LEAVE_FROM_A_TO_M(const char* pPacket, int nLength, int &nBytesUsed)
{
	CIMParty *ptmParty = this->GetIMParty();
	if (ptmParty == NULL)
	{
		return RES_BREAK;
	}

	// 2008-10-28 by dhjin, ĆÄĆĽ°ü·Ă lockşŻ°ć
	ptmParty->LockIMParty();
	PartyID_t partyID = ptmParty->m_PartyID;
	BOOL bReted = ProcessPartyLeaveArenaEndNoLock(ptmParty);
	ptmParty->UnlockIMParty();

	if(FALSE == bReted)
	{// ĆÄĆĽ¸ĘżˇĽ­ Á¦°ĹÇŘľßÇŃ´Ů
		// 2008-10-28 by dhjin, ĆÄĆĽ°ü·Ă lockşŻ°ć - ľĆ·ˇżÍ °°ŔĚ şŻ°ć
		// ProcessPartyDismember(ptmParty->m_PartyID);
		ProcessPartyDismember(partyID);
	}

	///////////////////////////////////////////////////////////////////////////////
	// Äł¸ŻĹÍŔÇ ĆÄĆĽ Á¤ş¸ »čÁ¦
	this->SetIMParty(NULL);	

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_PARTY_LIST_INFO(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ćí´ë ĽöÁ¤ - Ćí´ë Á¤ş¸ ¸®˝şĆ® żäĂ»	
/// \author		dhjin
/// \date		2008-06-02 ~ 2008-06-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_PARTY_LIST_INFO(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_PARTY_LIST_INFO,
									MSG_IC_PARTY_LIST_INFO, pMsg);

	this->SendPartyListInfo(pMsg->StartNum);
	
	return RES_RETURN_TRUE;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_PARTY_JOIN_FREE(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ćí´ë ĽöÁ¤ - Ćí´ë ŔÚŔŻ Âüż©
/// \author		dhjin
/// \date		2008-06-03 ~ 2008-06-03
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_PARTY_JOIN_FREE(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_PARTY_JOIN_FREE,
									MSG_IC_PARTY_JOIN_FREE, pMsg);
	
	CIMParty *pParty = ms_pIMIOCP->FindParty(pMsg->PartyNum);
	if (pParty == NULL)
	{
		SendErrorMessage(T_IC_PARTY_JOIN_FREE, ERR_PROTOCOL_NO_SUCH_PARTY, pMsg->PartyNum);
		return RES_BREAK;
	}
	
	pParty->LockIMParty();
	
	//////////////////////////////////////////////////////////////////////////
	// 2008-06-03 by dhjin, ĆÄĆĽ ŔÔŔĺ Ŕü ĂĽĹ© »çÇ×µé
	if(NULL != this->m_pIMParty)
	{
		SendErrorMessage(T_IC_PARTY_JOIN_FREE, ERR_PROTOCOL_PARTY_MEMBER_ALREADY_EXISTS, pMsg->PartyNum);
		pParty->UnlockIMParty();
		return RES_BREAK;
	}

	// 2008-11-06 by cmkwon, Ćí´ë ŔÚŔŻ Âüż© °ü·Ă ąö±× ĽöÁ¤(E0016695) - 
	if(PARTY_JOIN_TYPE_FREE != pParty->m_SPartyInfo.PartyJoinType)
	{
		SendErrorMessage(T_IC_PARTY_JOIN_FREE, ERR_PROTOCOL_NO_SUCH_PARTY, pMsg->PartyNum);
		pParty->UnlockIMParty();
		return RES_BREAK;
	}

	if(pParty->m_SPartyInfo.PartyLock)
	{
		if(strcmp(pMsg->PartyPW, pParty->m_SPartyInfo.PartyPW))
		{
			SendErrorMessage(T_IC_PARTY_JOIN_FREE, ERR_PROTOCOL_REQ_PW_NOT_MATCHED, pMsg->PartyNum);
			pParty->UnlockIMParty();
			return RES_BREAK;
		}
	}
	if(SIZE_MAX_PARTY_MEMBER <= pParty->GetMemberCount(FALSE) )
	{
		SendErrorMessage(T_IC_PARTY_JOIN_FREE, ERR_PROTOCOL_ALREADY_MAX_PARTY_MEMBER, pMsg->PartyNum);
		pParty->UnlockIMParty();
		return RES_BREAK;
	}
	if(this->m_character.InfluenceType != pParty->m_SPartyInfo.PartyInfluence)
	{
		SendErrorMessage(T_IC_PARTY_JOIN_FREE, ERR_PROTOCOL_REQ_INFLUENCE_NOT_MATCHED, pMsg->PartyNum);
		pParty->UnlockIMParty();
		return RES_BREAK;
	}
	if(this->m_character.Level < pParty->m_SPartyInfo.MinLevel)
	{
		SendErrorMessage(T_IC_PARTY_JOIN_FREE, ERR_PROTOCOL_REQ_MINLEVEL_NOT_MATCHED, pMsg->PartyNum);
		pParty->UnlockIMParty();
		return RES_BREAK;
	}
	if(this->m_character.Level > pParty->m_SPartyInfo.MaxLevel)
	{
		SendErrorMessage(T_IC_PARTY_JOIN_FREE, ERR_PROTOCOL_REQ_MAXLEVEL_NOT_MATCHED, pMsg->PartyNum);
		pParty->UnlockIMParty();
		return RES_BREAK;
	}

	CIMIOCPSocket *pMasterSocket = pParty->GetMasterIMIOCPSocket();
	if(NULL == pMasterSocket || FALSE == pMasterSocket->IsValidCharacter())
	{// 2008-12-16 by dhjin, EP3 °ĹşÎ¸ń·Ď - °ĹşÎ¸ń·Ď°ú °ü·ĂÇĎż© Ăß°ˇ ş¸ľČ ÄÚµĺ
		SendErrorMessage(T_IC_PARTY_JOIN_FREE, ERR_PROTOCOL_PARTY_MASTER_NOT_MATCHED);
		pParty->UnlockIMParty();
		return RES_BREAK;
	}
	if(this->IsCharacterNameFromRejectList(pMasterSocket->m_character.CharacterName)
		|| pMasterSocket->IsCharacterNameFromRejectList(this->m_character.CharacterName))
	{// 2008-07-03 by dhjin, EP3 °ĹşÎ¸ń·Ď -
		SendErrorMessage(T_IC_PARTY_JOIN_FREE, ERR_FRIEND_REGISTERED_PEER_REJECTLIST);	
		pParty->UnlockIMParty();
		return RES_BREAK;
	}

	// party ÇŇ´ç
	this->SetIMParty(pParty);

	// partyżř »đŔÔ
	IMPartyMember tmMember(this->m_character.CharacterName, this->m_character.CharacterUniqueNumber, this, pParty);
	pParty->PushBackMember(tmMember);

	// 2008-02-28 by dhjin,	ľĆ·ąłŞ ĹëÇŐ - LastPartyIDżˇ °ŞŔ» ŔúŔĺÇŃ´Ů.
	this->m_character.LastPartyID	= pMsg->PartyNum;

	// send to SERVER
	INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_ACCEPT_INVITE_OK, T_FI_PARTY_ACCEPT_INVITE_OK, pInviteOKFI, pInviteOKFIBuf);
	pInviteOKFI->PartyID				= pMsg->PartyNum;
	pInviteOKFI->CharacterUniqueNumber	= this->m_character.CharacterUniqueNumber;
	pInviteOKFI->FieldPartyInfo.nTotalPartyMember = pParty->GetMemberCount(FALSE);		// ÁÖŔÇ: m_ListPartyMemberżˇ »đŔÔ ŔĚČÄż©ľß ÇÔ!
	pInviteOKFI->FieldPartyInfo.lowestMemberLevel = pParty->GetLowestMemberLevel();		// ÁÖŔÇ: m_ListPartyMemberżˇ »đŔÔ ŔĚČÄż©ľß ÇÔ!
	this->SendMsgToCurrentFieldServerSocket(pInviteOKFIBuf, MSG_SIZE(MSG_FI_PARTY_ACCEPT_INVITE_OK));

	// clientµéżˇ°Ô ŔüĽŰÇĎ±â
	INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_ACCEPT_INVITE_OK, T_IC_PARTY_ACCEPT_INVITE_OK, pInviteOKIC, pInviteOKICBuf);
	pInviteOKIC->PartyID			= pMsg->PartyNum;
	pInviteOKIC->IMPartyMemberInfo	= this->m_character;
	pParty->SendMsgToMembers(pInviteOKICBuf, MSG_SIZE(MSG_IC_PARTY_ACCEPT_INVITE_OK));

	INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_INFO, T_IC_PARTY_INFO, pSendMsg, pSendBuf);
	pSendMsg->PartyInfo = pParty->m_SPartyInfo;
	this->SendAddData(pSendBuf, MSG_SIZE(MSG_IC_PARTY_INFO));
	pParty->UnlockIMParty();	

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_PARTY_CHANGE_INFO(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ćí´ë ĽöÁ¤ - Ćí´ë ŔÚŔŻ Âüż©
/// \author		dhjin
/// \date		2008-06-04 ~ 2008-06-04
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_PARTY_CHANGE_INFO(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_PARTY_CHANGE_INFO,
									MSG_IC_PARTY_CHANGE_INFO, pMsg);
	
	CIMParty *ptmParty = this->GetIMParty();
	if (ptmParty == NULL)
	{
		SendErrorMessage(T_IC_PARTY_CHANGE_INFO, ERR_PROTOCOL_NO_SUCH_PARTY);
		return RES_BREAK;
	}

	// 2008-06-02 by dhjin, EP3 Ćí´ë ĽöÁ¤ - Ćí´ë Á¤ş¸
	// 2009-12-29 by cmkwon, Äł¸ŻĹÍ ĂÖ´ë ·ąş§ »óÇâ(110Ŕ¸·Î) - ľĆ·ˇżÍ °°ŔĚ ĽöÁ¤
	//if(pMsg->PartyInfo.MinLevel > pMsg->PartyInfo.MaxLevel
	//	|| pMsg->PartyInfo.MinLevel <= 0
	//	|| pMsg->PartyInfo.MaxLevel > 100
	//	)
	///////////////////////////////////////////////////////////////////////////////
	// 2009-12-29 by cmkwon, Äł¸ŻĹÍ ĂÖ´ë ·ąş§ »óÇâ(110Ŕ¸·Î) - 
	if(pMsg->PartyInfo.MinLevel > pMsg->PartyInfo.MaxLevel
		|| FALSE == IS_IN_RANGE(1, pMsg->PartyInfo.MinLevel, CHARACTER_LEVEL_110)
		|| FALSE == IS_IN_RANGE(1, pMsg->PartyInfo.MaxLevel, CHARACTER_LEVEL_110)
		)
	{
		SendErrorMessage(T_IC_PARTY_CHANGE_INFO, ERR_PROTOCOL_REQ_LEVEL_NOT_MATCHED, pMsg->PartyInfo.MinLevel, pMsg->PartyInfo.MaxLevel);
		return RES_BREAK;
	}

	ptmParty->LockIMParty();

	if (ptmParty->GetMasterCharacterUniqueNumber() != m_character.CharacterUniqueNumber)
	{
		SendErrorMessage(T_IC_PARTY_CHANGE_INFO, ERR_PROTOCOL_PARTY_PERMISSION_DENIED
			, m_character.CharacterUniqueNumber, ptmParty->GetMasterCharacterUniqueNumber());
		ptmParty->UnlockIMParty();
		return RES_BREAK;
	}

	if(ptmParty->m_SPartyInfo.ExpDistributeType != pMsg->PartyInfo.ExpDistributeType)
	{
		INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_CHANGE_EXP_DISTRIBUTE_TYPE, T_FI_PARTY_CHANGE_EXP_DISTRIBUTE_TYPE, pSendMsg, pSendBuf);
		pSendMsg->PartyID			= ptmParty->m_PartyID;
		pSendMsg->ExpDistributeType	= pMsg->PartyInfo.ExpDistributeType;
		this->SendMsgToCurrentFieldServerSocket(pSendBuf, MSG_SIZE(MSG_FI_PARTY_CHANGE_EXP_DISTRIBUTE_TYPE));
	}
	if(ptmParty->m_SPartyInfo.ItemDistributeType != pMsg->PartyInfo.ItemDistributeType)
	{
		INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_CHANGE_ITEM_DISTRIBUTE_TYPE, T_FI_PARTY_CHANGE_ITEM_DISTRIBUTE_TYPE, pSendMsg, pSendBuf);
		pSendMsg->PartyID				= ptmParty->m_PartyID;
		pSendMsg->ItemDistributeType	= pMsg->PartyInfo.ItemDistributeType;
		this->SendMsgToCurrentFieldServerSocket(pSendBuf, MSG_SIZE(MSG_FI_PARTY_CHANGE_ITEM_DISTRIBUTE_TYPE));
	}

	ptmParty->SetPartyInfo(&(pMsg->PartyInfo));
	this->SendAddMessageType(T_IC_PARTY_CHANGE_INFO_OK);
	INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_INFO, T_IC_PARTY_INFO, pSendMsg, pSendBuf);
	pSendMsg->PartyInfo = pMsg->PartyInfo;
	ptmParty->SendMsgToMembers(pSendBuf, MSG_SIZE(MSG_IC_PARTY_INFO));
	ptmParty->UnlockIMParty();
	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_PARTY_RECOMMENDATION_MEMBER(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ćí´ë ĽöÁ¤ - ĂßĂµ ÄÉ¸ŻĹÍ żäĂ»
/// \author		dhjin
/// \date		2008-06-04 ~ 2008-06-04
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_PARTY_RECOMMENDATION_MEMBER(const char* pPacket, int nLength, int &nBytesUsed)
{
	CIMParty *ptmParty = this->GetIMParty();
	if (ptmParty == NULL)
	{
		SendErrorMessage(T_IC_PARTY_RECOMMENDATION_MEMBER, ERR_PROTOCOL_NO_SUCH_PARTY);
		return RES_BREAK;
	}

	ptmParty->LockIMParty();

	if (ptmParty->GetMasterCharacterUniqueNumber() != m_character.CharacterUniqueNumber)
	{
		SendErrorMessage(T_IC_PARTY_RECOMMENDATION_MEMBER, ERR_PROTOCOL_PARTY_PERMISSION_DENIED
			, m_character.CharacterUniqueNumber, ptmParty->GetMasterCharacterUniqueNumber());
		ptmParty->UnlockIMParty();
		return RES_BREAK;
	}

	ptmParty->SendRecommendationMember(this);

	ptmParty->UnlockIMParty();	
	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_PARTY_GET_AUTO_PARTY_INFO(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		ŔÎÇÇ´ĎĆĽ - ŔÚµż Ćí´ë 
/// \author		dhjin
/// \date		2009-09-09 ~ 2010
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_PARTY_GET_AUTO_PARTY_INFO(const char* pPacket, int nLength, int &nBytesUsed) {
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_PARTY_GET_AUTO_PARTY_INFO,
		MSG_IC_PARTY_GET_AUTO_PARTY_INFO, pMsg);

	CIMParty *ptmParty = this->GetIMParty();
	if (ptmParty == NULL
		|| ptmParty->m_PartyID != pMsg->PartyID) {
		SendErrorMessage(T_IC_PARTY_GET_AUTO_PARTY_INFO, ERR_PROTOCOL_NO_SUCH_PARTY);
		return RES_BREAK;
	}
	
	ptmParty->LockIMParty();

	// ĆÄĆĽ ą× ĆÄĆĽżř Á¤ş¸ ŔüĽŰ
	INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_GET_AUTO_PARTY_INFO_OK, T_IC_PARTY_GET_AUTO_PARTY_INFO_OK, pSendMsg, pSendBuf);
	pSendMsg->PartyInfo = ptmParty->m_SPartyInfo;
	pSendMsg->PartyID				= ptmParty->m_PartyID;
	pSendMsg->MasterUniqueNumber	= ptmParty->GetMasterCharacterUniqueNumber();
	ptmParty->MakeMsg_IC_PARTY_PUT_ALL_MEMBER(&pSendMsg->nNumOfPartyMembers, (IM_PARTY_MEMBER_INFO*)(pSendBuf+MSG_SIZE(MSG_IC_PARTY_GET_AUTO_PARTY_INFO_OK)), m_character.CharacterUniqueNumber);
	ptmParty->UnlockIMParty();	// unlock
	
	// µĄŔĚĹÍ ŔüĽŰ
	SendAddData(pSendBuf, MSG_SIZE(MSG_IC_PARTY_GET_AUTO_PARTY_INFO_OK) + pSendMsg->nNumOfPartyMembers*sizeof(IM_PARTY_MEMBER_INFO));

	return RES_RETURN_TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// Guild
ProcessResult CIMIOCPSocket::Process_IC_GUILD_CREATE(const char* pPacket, int nLength, int &nBytesUsed)
{
	int						nRecvTypeSize = 0;
	MSG_IC_GUILD_CREATE		*pMsgCreate = NULL;

	nRecvTypeSize = sizeof(MSG_IC_GUILD_CREATE);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_GUILD_CREATE, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12023);
		return RES_RETURN_FALSE;
	}
	pMsgCreate = (MSG_IC_GUILD_CREATE*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	// 2009-01-23 by cmkwon, ĹÍĹ° i,I ą®ŔÚ ą®Á¦ Ăł¸® - ż©´Ü»ýĽş˝Ă ż©´Ü¸í Ăł¸®
	LowercaseOnly_i_forTurkish(pMsgCreate->GuildName, SIZE_MAX_GUILD_NAME);

	if(COMPARE_INFLUENCE(m_character.InfluenceType, INFLUENCE_TYPE_NORMAL))
	{// 2005-12-06 by cmkwon, ŔĎąÝĽĽ·ÂŔş ż©´Ü »ýĽş şŇ°ˇ, ĽĽ·ÂĽ±ĹĂ ČÄ ż©´Ü »ýĽş °ˇ´É
		SendErrorMessage(T_IC_GUILD_CREATE, ERR_REQ_CHOOSE_INFLUENCE_TYPE);
		return RES_BREAK;
	}

	// ż©´ÜŔĺŔÇ ·ąş§ Č®ŔÎ
	if (FALSE == GCheckLimitLevel(CHECK_TYPE_GUILD_CREATE, m_character.Level))
	{
		SendErrorMessage(T_IC_GUILD_CREATE, ERR_PROTOCOL_REQ_LEVEL_NOT_MATCHED);
		return RES_BREAK;
	}

	if(FALSE == CAtumSJ::IsValidGuildName(pMsgCreate->GuildName, g_pIMGlobal->GetLanguageType()))
	{
		SendErrorMessage(T_IC_GUILD_CREATE, ERR_PROTOCOL_INVLID_GUILDNAME, 1, 0, pMsgCreate->GuildName);
		return RES_BREAK;
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2009-01-21 by cmkwon, ŔŻČżÇŃ ż©´Ü¸í ĂĽĹ©˝Ăżˇ żĺĽł, ±ÝÁöľîµµ ĂĽĹ© Ăß°ˇ - 
	if (FALSE == g_pIMGlobal->CheckProhibitedName(pMsgCreate->GuildName))
	{
		SendErrorMessage(T_IC_GUILD_CREATE, ERR_PROTOCOL_INVLID_GUILDNAME, 2, 0, pMsgCreate->GuildName);
		return RES_BREAK;
	}


	// 2008-06-04 by dhjin, EP3 - ż©´Ü ĽöÁ¤ - ĆÄĆĽ ÇĘżäľřŔĚ »ýĽş°ˇ´É ąŘ°ú °°ŔĚ ĽöÁ¤
	CAtumIMDBManager::MSG_DB_GUILD_CREATE *pDBCreate = new CAtumIMDBManager::MSG_DB_GUILD_CREATE;
	MEMSET_ZERO(pDBCreate, sizeof(CAtumIMDBManager::MSG_DB_GUILD_CREATE));
	CIMParty *ptmParty = this->GetIMParty();
	if (NULL == ptmParty)
	{
		pDBCreate->arrGuildRank[0]			= GUILD_RANK_COMMANDER;	
		pDBCreate->arrMemberCharacterUID[0]	= this->m_character.CharacterUniqueNumber;
		pDBCreate->nMemberCounts			= 1;
		STRNCPY_MEMSET(pDBCreate->szGuildName, pMsgCreate->GuildName, SIZE_MAX_GUILD_NAME);
		pDBCreate->uidCommanderCharacter	= this->m_character.CharacterUniqueNumber;
	}
	else
	{
		ptmParty->LockIMParty();
		{
			if (ptmParty->GetMasterCharacterUniqueNumber() != m_character.CharacterUniqueNumber)
			{
				// error: ĆÄĆĽŔĺ şŇŔĎÄˇ, ĆÄĆĽŔĺ¸¸ŔĚ ±ćµĺ »ýĽş °ˇ´É
				ptmParty->UnlockIMParty();
				SendErrorMessage(T_IC_GUILD_CREATE, ERR_PROTOCOL_PARTY_MASTER_NOT_MATCHED);
				SAFE_DELETE(pDBCreate);
				return RES_BREAK;
			}

			// ĂĘ±â ±ćµĺ »ýĽş »ç ĆÄĆĽżřŔş ĂÖĽŇÇŃ 1¸í ŔĚ»óŔĚľîľß ÇÔ
			if (!COMPARE_RACE(m_character.Race, RACE_OPERATION)
				&& ptmParty->GetMemberCount() < 1)
			{
				ptmParty->UnlockIMParty();
				SendErrorMessage(T_IC_GUILD_CREATE, ERR_PROTOCOL_NOT_ENOUGH_PARTY_MEMBER);
				SAFE_DELETE(pDBCreate);
				return RES_BREAK;
			}

			int nRetedErrNum = ptmParty->MakeMsg_DB_GUILD_CREATE(&pDBCreate->nMemberCounts, pDBCreate->arrMemberCharacterUID);
			switch(nRetedErrNum)
			{
			case ERR_NO_ERROR:
				{
					STRNCPY_MEMSET(pDBCreate->szGuildName, pMsgCreate->GuildName, SIZE_MAX_GUILD_NAME);
					pDBCreate->uidCommanderCharacter = ptmParty->GetMasterCharacterUniqueNumber();
				}
				break;
			case ERR_PROTOCOL_NO_SUCH_PARTY_MEMBER:
				{
					ptmParty->UnlockIMParty();
					SendErrorMessage(T_IC_GUILD_CREATE, ERR_PROTOCOL_NO_SUCH_PARTY_MEMBER, 0, 0, STRMSG_S_I2NOTIFY_0010);
					SAFE_DELETE(pDBCreate);
					return RES_BREAK;
				}			
			case ERR_PROTOCOL_GUILD_MEMBER_ALREADY_EXISTS:			
				{
					ptmParty->UnlockIMParty();
					SendErrorMessage(T_IC_GUILD_CREATE, ERR_PROTOCOL_GUILD_MEMBER_ALREADY_EXISTS, 0, 0, STRMSG_S_I2NOTIFY_0011);
					SAFE_DELETE(pDBCreate);
					return RES_BREAK;
				}
				break;
			default:
				{
					ptmParty->UnlockIMParty();
					SendErrorMessage(T_IC_GUILD_CREATE, ERR_COMMON_UNKNOWN_ERROR);
					SAFE_DELETE(pDBCreate);
					return RES_BREAK;
				}
			}
		}
		ptmParty->UnlockIMParty();
	}

	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildCreate, this, pDBCreate);	// DB·Î ŔÎÁő ŔýÂ÷¸¦ łŃ±ä´Ů.
	return RES_RETURN_TRUE;
}

// check: T_IC_GUILD_GET_GUILD_INFO·Î ĹëÇŐ, ÇĘżäÇĎ¸é »ě¸˛, 20040520, kelovon
//ProcessResult CIMIOCPSocket::Process_IC_GUILD_GET_MEMBER(const char* pPacket, int nLength, int &nBytesUsed)
//{
//
//	assert(0);
//
//	return RES_RETURN_TRUE;
//}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_GET_GUILD_INFO(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_GET_GUILD_INFO,
									MSG_IC_GUILD_GET_GUILD_INFO, pMsgGetGuildInfo);

	if (FALSE == IS_VALID_UNIQUE_NUMBER(this->m_uidGuildUniqueNumber)
		|| this->m_uidGuildUniqueNumber != pMsgGetGuildInfo->GuildUniqueNumber)
	{
		SendErrorMessage(T_IC_GUILD_GET_GUILD_INFO, ERR_PROTOCOL_NO_SUCH_GUILD
			, this->m_uidGuildUniqueNumber, pMsgGetGuildInfo->GuildUniqueNumber);
		return RES_BREAK;
	}

	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(this->m_uidGuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			SendErrorMessage(T_IC_GUILD_GET_GUILD_INFO, ERR_PROTOCOL_NO_SUCH_GUILD, pMsgGetGuildInfo->GuildUniqueNumber, this->m_uidGuildUniqueNumber);
			ms_pIMIOCP->m_MapGuild.unlock();
			return RES_BREAK;
		}
		
		ptmGuild->Send_MSG_IC_GUILD_GET_GUILD_INFO_OK(this);		
	}
	ms_pIMIOCP->m_MapGuild.unlock();

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_REQUEST_INVITE(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_REQUEST_INVITE,
									MSG_IC_GUILD_REQUEST_INVITE, pMsgInvite);

	// 2009-01-23 by cmkwon, ĹÍĹ° i,I ą®ŔÚ ą®Á¦ Ăł¸® - T_IC_GUILD_REQUEST_INVITE
	LowercaseOnly_i_forTurkish(pMsgInvite->InviteeCharacterName, SIZE_MAX_CHARACTER_NAME);

	if (FALSE == IS_VALID_UNIQUE_NUMBER(this->m_uidGuildUniqueNumber))
	{
		SendErrorMessage(T_IC_GUILD_REQUEST_INVITE, ERR_PROTOCOL_NO_SUCH_GUILD);
		return RES_BREAK;
	}

	CIMIOCPSocket *pInviteeSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(pMsgInvite->InviteeCharacterName);
	if (NULL == pInviteeSocket
		|| FALSE == pInviteeSocket->IsValidCharacter())
	{
		SendErrorMessage(T_IC_GUILD_REQUEST_INVITE, ERR_COMMON_NO_SUCH_CHARACTER);
		return RES_BREAK;
	}
	
	CIMIOCPSocket *pCharacterSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(m_character.CharacterUniqueNumber);
	if (pCharacterSocket == NULL || FALSE == pCharacterSocket->IsUsing())
	{
		SendErrorMessage(T_IC_GUILD_REQUEST_INVITE, ERR_COMMON_NO_SUCH_CHARACTER, m_character.CharacterUniqueNumber);
		return RES_BREAK;
	}

	if (IS_VALID_UNIQUE_NUMBER(pInviteeSocket->m_uidGuildUniqueNumber))
	{
		SendErrorMessage(T_IC_GUILD_REQUEST_INVITE, ERR_PROTOCOL_CANNOT_INVITE_USER);
		SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0012);
		return RES_BREAK;
	}

	if(FALSE == GCheckRaceAndInfluenceType(CHECK_TYPE_GUILD_JOIN, m_character.Race, m_character.InfluenceType,
		pInviteeSocket->GetCharacter()->Race, pInviteeSocket->GetCharacter()->InfluenceType))
	{// 2005-12-07 by cmkwon, ŔĎąÝĽĽ·ÂŔş ż©´Ü ĂĘ´ë şŇ°ˇ, ´Ů¸Ą ĽĽ·Â°ŁŔş ĂĘ´ë şŇ°ˇ, ĽĽ·ÂĽ±ĹĂ ČÄ ż©´Ü ĂĘ´ë °ˇ´É
		SendErrorMessage(T_IC_GUILD_REQUEST_INVITE, ERR_PROTOCOL_CANNOT_INVITE_USER);
		return RES_BREAK;
	}

	if (strnicmp(pMsgInvite->InviteeCharacterName, m_character.CharacterName, SIZE_MAX_CHARACTER_NAME) == 0)
	{
		SendErrorMessage(T_IC_GUILD_REQUEST_INVITE, ERR_PROTOCOL_CANNOT_INVITE_USER);
		SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0013);
		return RES_BREAK;
	}

	INIT_MSG_WITH_BUFFER(MSG_IC_GUILD_REQUEST_INVITE_QUESTION, T_IC_GUILD_REQUEST_INVITE_QUESTION, msgInviteQuestion, SendBuf);

	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_REQUEST_INVITE, ERR_COMMON_NO_SUCH_CHARACTER);
			return RES_BREAK;
		}
		
		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× -
		CGuildMember *pMember = NULL;
		if(!ptmGuild->GetGuildMember(this->m_character.CharacterUniqueNumber, pMember))
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_REQUEST_INVITE, ERR_PROTOCOL_NO_SUCH_GUILD_MEMBER);
			return RES_BREAK;
		}

		if (FALSE == ptmGuild->IsGuildCommander(m_character.CharacterUniqueNumber)
			&& GUILD_RANK_SUBCOMMANDER != pMember->m_Rank)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_REQUEST_INVITE, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
			return RES_BREAK;
		}

		if (ptmGuild->m_GuildState != GUILD_STATE_NORMAL)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0014);
			return RES_BREAK;
		}

		// 2006-09-19 by dhjin, ż©´Ü ŔÎżř Äł˝¬ ľĆŔĚĹŰ »čÁ¦
//		if (ptmGuild->m_nGuildMemberCapacity <= ptmGuild->m_ListGuildMember.size()
//			|| CAtumSJ::GetMaxGuildSize(pCharacterSocket->IsExistPremiumCard()) <= ptmGuild->m_ListGuildMember.size())
		if (CAtumSJ::GetMaxGuildSize(ptmGuild->m_nGuildMemberCapacity, pCharacterSocket->IsExistPremiumCard()) <= ptmGuild->m_ListGuildMember.size())		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ż©´Üżř Áő°ˇ Äł˝¬ ľĆŔĚĹŰ
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0015);
			return RES_BREAK;
		}

		// msg setting
		STRNCPY_MEMSET(msgInviteQuestion->GuildName, ptmGuild->m_GuildName, SIZE_MAX_GUILD_NAME);
		msgInviteQuestion->GuildUniqueNumber = ptmGuild->m_GuildUniqueNumber;
	}
	ms_pIMIOCP->m_MapGuild.unlock();
	
	if(this->IsUsing() && pInviteeSocket->IsUsing())
	{
		STRNCPY_MEMSET(msgInviteQuestion->GuildCommanderCharacterName, m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);

		pInviteeSocket->SendAddData(SendBuf, MSG_SIZE(MSG_IC_GUILD_REQUEST_INVITE_QUESTION));
	}
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_ACCEPT_INVITE(const char* pPacket, int nLength, int &nBytesUsed)
{
	int							nRecvTypeSize = 0;
	MSG_IC_GUILD_ACCEPT_INVITE	*pMsgAccept = NULL;

	nRecvTypeSize = sizeof(MSG_IC_GUILD_ACCEPT_INVITE);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_GUILD_ACCEPT_INVITE, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x12026);
		return RES_RETURN_FALSE;
	}
	pMsgAccept = new MSG_IC_GUILD_ACCEPT_INVITE;
	*pMsgAccept = *(MSG_IC_GUILD_ACCEPT_INVITE*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(pMsgAccept->GuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_ACCEPT_INVITE, ERR_PROTOCOL_NOT_HAVE_GUILD);
			SAFE_DELETE(pMsgAccept);
			return RES_BREAK;
		}

		CGuildMember *pMember = NULL;
		BOOL ret = ptmGuild->GetGuildMember(m_character.CharacterUniqueNumber, pMember);
		if (ret)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_ACCEPT_INVITE, ERR_PROTOCOL_GUILD_MEMBER_ALREADY_EXISTS);
			SAFE_DELETE(pMsgAccept);
			return RES_BREAK;
		}

		if (ptmGuild->m_GuildState != GUILD_STATE_NORMAL)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0016);
			SAFE_DELETE(pMsgAccept);
			return RES_BREAK;
		}

		///////////////////////////////////////////////////////////////////////////////
		// 2007-01-31 by cmkwon, ż©´Üżř ĂÖ´ë Ä«żîĆ® ĂĽĹ©
		// 2008-07-18 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ąŘ°ú °°ŔĚ ĽöÁ¤ 
//		CGuildMember *ptmGMember = NULL;
//		ptmGuild->GetGuildMember(ptmGuild->m_CommanderUniqueNumber, ptmGMember);
//		if(NULL == ptmGMember
//			|| NULL == ptmGMember->m_pIMIOCPSocket
//			|| FALSE == ptmGMember->m_pIMIOCPSocket->IsValidCharacter())
//		{
//			ms_pIMIOCP->m_MapGuild.unlock();
//			SendErrorMessage(T_IC_GUILD_REQUEST_INVITE, ERR_COMMON_NO_SUCH_CHARACTER);
//			SAFE_DELETE(pMsgAccept);
//			return RES_BREAK;
//		}
//		if (CAtumSJ::GetMaxGuildSize(ptmGuild->m_nGuildMemberCapacity, ptmGMember->m_pIMIOCPSocket->IsExistPremiumCard()) <= ptmGuild->m_ListGuildMember.size())		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ż©´Üżř Áő°ˇ Äł˝¬ ľĆŔĚĹŰ	
//		{
//			ms_pIMIOCP->m_MapGuild.unlock();
//			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0015);
//			SAFE_DELETE(pMsgAccept);
//			return RES_BREAK;
//		}
		CGuildMember *ptmGMember = NULL;
		ptmGuild->GetGuildMember(ptmGuild->m_CommanderUniqueNumber, ptmGMember);
		if(NULL == ptmGMember)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_ACCEPT_INVITE, ERR_COMMON_NO_SUCH_CHARACTER);
			SAFE_DELETE(pMsgAccept);
			return RES_BREAK;
		}
		if (CAtumSJ::GetMaxGuildSize(ptmGuild->m_nGuildMemberCapacity, ptmGuild->m_bMemberShip) <= ptmGuild->m_ListGuildMember.size())		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ż©´Üżř Áő°ˇ Äł˝¬ ľĆŔĚĹŰ	
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0015);
			SAFE_DELETE(pMsgAccept);
			return RES_BREAK;
		}
	}
	ms_pIMIOCP->m_MapGuild.unlock();	

	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildAddMember, this, pMsgAccept);

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_REJECT_INVITE(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_REJECT_INVITE,
									MSG_IC_GUILD_REJECT_INVITE, pMsgReject);

	UID32_t uidGuildMaster = INVALID_GUILD_UID;
	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(pMsgReject->GuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_REJECT_INVITE, ERR_PROTOCOL_NOT_HAVE_GUILD);
			return RES_BREAK;
		}

		uidGuildMaster = ptmGuild->m_CommanderUniqueNumber;
	}
	ms_pIMIOCP->m_MapGuild.unlock();

	CIMIOCPSocket *pMasterSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(uidGuildMaster);
	if (NULL == pMasterSocket || FALSE == pMasterSocket->IsUsing())
	{
		SendErrorMessage(T_IC_GUILD_REJECT_INVITE, ERR_COMMON_NO_SUCH_CHARACTER);
		return RES_BREAK;
	}

	INIT_MSG_WITH_BUFFER(MSG_IC_GUILD_REJECT_INVITE_OK, T_IC_GUILD_REJECT_INVITE_OK, msgRejectOK, SendBuf);
	STRNCPY_MEMSET(msgRejectOK->CharacterName, m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);

	pMasterSocket->SendAddData(SendBuf, MSG_SIZE(MSG_IC_GUILD_REJECT_INVITE_OK));

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_LEAVE(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_LEAVE,
									MSG_IC_GUILD_LEAVE, pMsgLeave);

	if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
	{
		SendErrorMessage(T_IC_GUILD_LEAVE, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}

	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_LEAVE, ERR_PROTOCOL_NOT_HAVE_GUILD);
			return RES_BREAK;
		}

		if (m_character.CharacterUniqueNumber == ptmGuild->m_CommanderUniqueNumber)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_LEAVE, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0017);
			return RES_BREAK;
		}

		if (ptmGuild->m_GuildState == GUILD_STATE_IN_GUILD_WAR)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0018);
			return RES_BREAK;
		}

		if (pMsgLeave->GuildUniqueNumber != ptmGuild->m_GuildUniqueNumber)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_LEAVE, ERR_PROTOCOL_GUILD_NOT_MATCHED);
			return RES_BREAK;
		}

		CGuildMember *pRetMember = NULL;
		BOOL bRet = ptmGuild->GetGuildMember(m_character.CharacterUniqueNumber, pRetMember);
		if (!bRet)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_LEAVE, ERR_PROTOCOL_NO_SUCH_GUILD_MEMBER);
			return RES_BREAK;
		}
	}
	ms_pIMIOCP->m_MapGuild.unlock();

	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildLeaveMember, this, NULL, NULL,
						m_character.CharacterUniqueNumber, this->m_uidGuildUniqueNumber);

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_BAN_MEMBER(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_BAN_MEMBER,
									MSG_IC_GUILD_BAN_MEMBER, pMsgBan);

	if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
	{
		SendErrorMessage(T_IC_GUILD_BAN_MEMBER, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}

	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_BAN_MEMBER, ERR_PROTOCOL_NOT_HAVE_GUILD);
			return RES_BREAK;
		}

		CGuildMember *pMySelfMember = NULL;
		if(!ptmGuild->GetGuildMember(this->m_character.CharacterUniqueNumber, pMySelfMember))
		{// 2008-06-17 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× -
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_BAN_MEMBER, ERR_PROTOCOL_NO_SUCH_GUILD_MEMBER);
			return RES_BREAK;
		}

		if (m_character.CharacterUniqueNumber != ptmGuild->m_CommanderUniqueNumber
			&& GUILD_RANK_SUBCOMMANDER != pMySelfMember->m_Rank			// 2008-06-17 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× -
			)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_BAN_MEMBER, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
			return RES_BREAK;
		}

		if (ptmGuild->m_GuildState == GUILD_STATE_IN_GUILD_WAR)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0019);
			return RES_BREAK;
		}

		if (pMsgBan->MemberUniqueNumber == ptmGuild->m_CommanderUniqueNumber)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_BAN_MEMBER, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
			SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0020);
			return RES_BREAK;
		}

		CGuildMember *pRetMember = NULL;
		BOOL bRet = ptmGuild->GetGuildMember(pMsgBan->MemberUniqueNumber, pRetMember);
		if (!bRet)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_BAN_MEMBER, ERR_PROTOCOL_NO_SUCH_GUILD_MEMBER);
			return RES_BREAK;
		}
	}
	ms_pIMIOCP->m_MapGuild.unlock();

	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildBanMember, this, NULL, NULL, pMsgBan->MemberUniqueNumber, m_uidGuildUniqueNumber);
	return RES_RETURN_TRUE;
}

// 2007-11-12 by dhjin, Process_IC_GUILD_DISMEMBER ÇÔĽö¸íŔş şŻ°ćľřŔĚ T_FI_GUILD_DISMEMBER·Î şŻ°ć
ProcessResult CIMIOCPSocket::Process_IC_GUILD_DISMEMBER(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_FI_GUILD_DISMEMBER,
									MSG_FI_GUILD_DISMEMBER, pMsgDismember);

	if (FALSE == IS_VALID_UNIQUE_NUMBER(pMsgDismember->GuildUniqueNumber))
	{
		SendErrorMessage(T_FC_GUILD_DISMEMBER, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}

	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(pMsgDismember->GuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_FC_GUILD_DISMEMBER, ERR_PROTOCOL_NOT_HAVE_GUILD);
			return RES_BREAK;
		}

		if (ptmGuild->m_GuildState == GUILD_STATE_IN_GUILD_WAR)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0021);
			return RES_BREAK;
		}
// 2007-11-12 by dhjin,	FieldĽ­ąöżˇĽ­ ĂĽĹ©
//		if (m_character.CharacterUniqueNumber != ptmGuild->m_CommanderUniqueNumber)
//		{
//			ms_pIMIOCP->m_MapGuild.unlock();
//			SendErrorMessage(T_IC_GUILD_DISMEMBER, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
//			return RES_BREAK;
//		}
//
//		if (pMsgDismember->GuildUniqueNumber != ptmGuild->m_GuildUniqueNumber)
//		{
//			ms_pIMIOCP->m_MapGuild.unlock();
//			SendErrorMessage(T_IC_GUILD_DISMEMBER, ERR_PROTOCOL_NO_SUCH_GUILD);
//			return RES_BREAK;
//		}

	}
	ms_pIMIOCP->m_MapGuild.unlock();	

	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildDismember, this, NULL, NULL, pMsgDismember->GuildUniqueNumber);

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_CANCEL_DISMEMBER(const char *pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_CANCEL_DISMEMBER,
									MSG_IC_GUILD_CANCEL_DISMEMBER, pMsgCancelDismember);

	if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
	{
		SendErrorMessage(T_IC_GUILD_CANCEL_DISMEMBER, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}

	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_CANCEL_DISMEMBER, ERR_PROTOCOL_NOT_HAVE_GUILD);
			return RES_BREAK;
		}

		if (m_character.CharacterUniqueNumber != ptmGuild->m_CommanderUniqueNumber)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_CANCEL_DISMEMBER, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
			return RES_BREAK;
		}

		if (pMsgCancelDismember->GuildUniqueNumber != ptmGuild->m_GuildUniqueNumber)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_CANCEL_DISMEMBER, ERR_PROTOCOL_NO_SUCH_GUILD);
			return RES_BREAK;
		}

		if (ptmGuild->m_GuildState != GUILD_STATE_DISMEMBER_READY)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_CANCEL_DISMEMBER, ERR_PROTOCOL_NO_SUCH_GUILD);
			return RES_BREAK;
		}
	}
	ms_pIMIOCP->m_MapGuild.unlock();

	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildCancelDismember, this, NULL, NULL, pMsgCancelDismember->GuildUniqueNumber);

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_GET_DISMEMBER_DATE(const char *pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_GET_DISMEMBER_DATE,
									MSG_IC_GUILD_GET_DISMEMBER_DATE, pMsgGetDate);

	if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
	{
		SendErrorMessage(T_IC_GUILD_GET_DISMEMBER_DATE, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}
	
	INIT_MSG_WITH_BUFFER(MSG_IC_GUILD_GET_DISMEMBER_DATE_OK, T_IC_GUILD_GET_DISMEMBER_DATE_OK, msgGetDateOK, msgGetDateOKBuf);
	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_GET_DISMEMBER_DATE, ERR_PROTOCOL_NOT_HAVE_GUILD);
			return RES_BREAK;
		}

		if (pMsgGetDate->GuildUniqueNumber != ptmGuild->m_GuildUniqueNumber)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_GET_DISMEMBER_DATE, ERR_PROTOCOL_NO_SUCH_GUILD);
			return RES_BREAK;
		}

		msgGetDateOK->DismemberDateTime = ptmGuild->m_DismemberDate;
	}
	ms_pIMIOCP->m_MapGuild.unlock();	

	SendAddData(msgGetDateOKBuf, MSG_SIZE(MSG_IC_GUILD_GET_DISMEMBER_DATE_OK));

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_CHANGE_GUILD_NAME(const char *pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_CHANGE_GUILD_NAME,
									MSG_IC_GUILD_CHANGE_GUILD_NAME, pMsgChangeGuildName);

	// 2009-01-23 by cmkwon, ĹÍĹ° i,I ą®ŔÚ ą®Á¦ Ăł¸® - ż©´Ü¸í şŻ°ć Ä«µĺ »çżë˝Ă ż©´Ü¸í Ăł¸®
	LowercaseOnly_i_forTurkish(pMsgChangeGuildName->NewGuildName, SIZE_MAX_GUILD_NAME);

	if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
	{
		SendErrorMessage(T_IC_GUILD_CHANGE_GUILD_NAME, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}

	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_CHANGE_GUILD_NAME, ERR_PROTOCOL_NOT_HAVE_GUILD);
			return RES_BREAK;
		}
	
		if (m_character.CharacterUniqueNumber != ptmGuild->m_CommanderUniqueNumber)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_CHANGE_GUILD_NAME, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
			return RES_BREAK;
		}

		if (strnicmp(ptmGuild->m_GuildName, pMsgChangeGuildName->NewGuildName, SIZE_MAX_GUILD_NAME) == 0)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0022);
			return RES_BREAK;
		}

		if (ptmGuild->m_GuildState != GUILD_STATE_NORMAL)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0023);
			return RES_BREAK;
		}
	}
	ms_pIMIOCP->m_MapGuild.unlock();

	QPARAM_CHANGE_GUILD_NAME *pQParamChangeGuildName = new QPARAM_CHANGE_GUILD_NAME;
	pQParamChangeGuildName->GuildUniqueNumber = m_uidGuildUniqueNumber;
	STRNCPY_MEMSET(pQParamChangeGuildName->NewGuildName, pMsgChangeGuildName->NewGuildName, SIZE_MAX_GUILD_NAME);

	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildChangeGuildName, this, pQParamChangeGuildName, NULL, 0, m_uidGuildUniqueNumber);

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_GET_GUILD_MARK(const char *pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_GET_GUILD_MARK,
									MSG_IC_GUILD_GET_GUILD_MARK, pMsgGetGuildMark);

	if(FALSE == IS_VALID_UNIQUE_NUMBER(pMsgGetGuildMark->GuildUniqueNumber))
	{
		SendErrorMessage(T_IC_GUILD_GET_GUILD_MARK, ERR_PROTOCOL_NOT_HAVE_GUILD);		
		return RES_BREAK;
	}

	INIT_MSG_WITH_BUFFER(MSG_IC_GUILD_GET_GUILD_MARK_OK, T_IC_GUILD_GET_GUILD_MARK_OK, msgGetMarkOK, msgGetMarkOKBuf);
	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(pMsgGetGuildMark->GuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
// 2005-12-02 by cmkwon, ÇĘżä ľřŔ˝
//			SendErrorMessage(T_IC_GUILD_GET_GUILD_MARK, ERR_PROTOCOL_NOT_HAVE_GUILD);
			return RES_BREAK;
		}
		if (ptmGuild->m_GuildMark.nGuildMarkSize <= 0
			|| ptmGuild->m_GuildMark.nGuildMarkSize > 1300 // 2007-08-02 by cmkwon, ąöĆŰ »çŔĚÁî´Â SIZE_MAX_GUILD_MARK_IMAGEŔĚÁö¸¸ ĽŇÄĎ ĂÖ´ë»çŔĚÁî(SIZE_MAX_PACKET)ş¸´Ů ŔŰľĆľß ÇĎąÇ·Î ŔÓŔÇ·Î 1300Bytes·Î ĂĽĹ©ÇÔ
			|| ptmGuild->m_GuildMark.nGuildMarkVersion == 0)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			return RES_BREAK;
		}
		
		if(GUILD_MARK_STATE_NORMAL != ptmGuild->m_GuildMark.byGuildMarkState)
		{// 2007-08-02 by cmkwon, ż©´Ü ¸¶Ĺ© ˝É»ç ˝Ă˝şĹŰ ±¸Çö - ż©´Ü¸¶Ĺ©»óĹÂ ĂĽĹ©
			ms_pIMIOCP->m_MapGuild.unlock();
			return RES_BREAK;
		}

		msgGetMarkOK->GuildUniqueNumber	= ptmGuild->m_GuildUniqueNumber;
		msgGetMarkOK->GuildMarkVersion	= ptmGuild->m_GuildMark.nGuildMarkVersion;
		msgGetMarkOK->SizeOfGuildMark	= ptmGuild->m_GuildMark.nGuildMarkSize;
		memcpy(msgGetMarkOKBuf+MSG_SIZE(MSG_IC_GUILD_GET_GUILD_MARK_OK), ptmGuild->m_GuildMark.GuildMarkImage, ptmGuild->m_GuildMark.nGuildMarkSize);
	}
	ms_pIMIOCP->m_MapGuild.unlock();

	SendAddData(msgGetMarkOKBuf, MSG_SIZE(MSG_IC_GUILD_GET_GUILD_MARK_OK)+msgGetMarkOK->SizeOfGuildMark);
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_SET_GUILD_MARK(const char *pPacket, int nLength, int &nBytesUsed)
{
	int							nRecvTypeSize = 0;
	MSG_IC_GUILD_SET_GUILD_MARK	*msgSetGuildMark = NULL;

	nRecvTypeSize = sizeof(MSG_IC_GUILD_SET_GUILD_MARK)
					+ ((MSG_IC_GUILD_SET_GUILD_MARK*)(pPacket + nBytesUsed))->SizeOfGuildMark;
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_GUILD_SET_GUILD_MARK, ERR_PROTOCOL_INVALID_FIELD_DATA);
		return RES_RETURN_FALSE;
	}
	msgSetGuildMark = (MSG_IC_GUILD_SET_GUILD_MARK*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize + msgSetGuildMark->SizeOfGuildMark;

	if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
	{
		SendErrorMessage(T_IC_GUILD_SET_GUILD_MARK, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}

	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_SET_GUILD_MARK, ERR_PROTOCOL_NOT_HAVE_GUILD);
			return RES_BREAK;
		}

		if (ptmGuild->m_GuildState != GUILD_STATE_NORMAL)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0024);
			return RES_BREAK;
		}

		if(FALSE == IS_IN_RANGE(0, msgSetGuildMark->SizeOfGuildMark, 1300))
		{// 2007-08-02 by cmkwon, ąöĆŰ »çŔĚÁî´Â SIZE_MAX_GUILD_MARK_IMAGEŔĚÁö¸¸ ĽŇÄĎ ĂÖ´ë»çŔĚÁî(SIZE_MAX_PACKET)ş¸´Ů ŔŰľĆľß ÇĎąÇ·Î ŔÓŔÇ·Î 1300Bytes·Î ĂĽĹ©ÇÔ
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_SET_GUILD_MARK, ERR_PROTOCOL_INVLID_GuildMarkSize);
			return RES_BREAK;
		}

		if (m_character.CharacterUniqueNumber != ptmGuild->m_CommanderUniqueNumber)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_SET_GUILD_MARK, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
			return RES_BREAK;
		}

		// 2007-08-02 by cmkwon, ż©´Ü ¸¶Ĺ© ˝É»ç ˝Ă˝şĹŰ ±¸Çö - ż©±âżˇĽ­ ÇŇ´çÇĎÁö ľĘ°í QP_GuildSetGuildMark()żˇĽ­ ÇŇ´çÇĎ´Â °ÍŔ¸·Î şŻ°ćÇŃ´Ů
		//memcpy(ptmGuild->m_GuildMark.GuildMarkImage, ((char*)msgSetGuildMark)+sizeof(MSG_IC_GUILD_SET_GUILD_MARK), msgSetGuildMark->SizeOfGuildMark);
	}
	ms_pIMIOCP->m_MapGuild.unlock();
	
	///////////////////////////////////////////////////////////////////////////////
	// 2007-08-02 by cmkwon, ż©´Ü ¸¶Ĺ© ˝É»ç ˝Ă˝şĹŰ ±¸Çö - ż©±âżˇĽ­ ÇŇ´çÇĎÁö ľĘ°í QP_GuildSetGuildMark()żˇĽ­ ÇŇ´çÇĎ´Â °ÍŔ¸·Î şŻ°ćÇŃ´Ů
	GUILD_MARK *pGuildMark = new GUILD_MARK;
	pGuildMark->nGuildMarkSize		= msgSetGuildMark->SizeOfGuildMark;
	memcpy(pGuildMark->GuildMarkImage, ((char*)msgSetGuildMark)+sizeof(MSG_IC_GUILD_SET_GUILD_MARK), msgSetGuildMark->SizeOfGuildMark);
	pGuildMark->byGuildMarkState	= GUILD_MARK_STATE_NORMAL;
	if(0 >= msgSetGuildMark->SizeOfGuildMark)
	{
		pGuildMark->byGuildMarkState	= GUILD_MARK_STATE_NONE;
	}
	else if(ms_pIMIOCP->IsJudgeGuildMark())
	{
		pGuildMark->byGuildMarkState	= GUILD_MARK_STATE_WAITING_PERMISSION;
	}
	
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildSetGuildMark, this, pGuildMark, NULL, m_uidGuildUniqueNumber);
// 2007-08-02 by cmkwon, ż©´Ü ¸¶Ĺ© ˝É»ç ˝Ă˝şĹŰ ±¸Çö - ±âş»°Ş Çăżë
//	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildSetGuildMark, this, NULL, NULL,
//						m_uidGuildUniqueNumber, msgSetGuildMark->SizeOfGuildMark);

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_SET_RANK(const char *pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_SET_RANK,
									MSG_IC_GUILD_SET_RANK, pMsgSetRank);

	if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
	{
		SendErrorMessage(T_IC_GUILD_SET_RANK, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}
	
	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_SET_RANK, ERR_PROTOCOL_NOT_HAVE_GUILD);
			return RES_BREAK;
		}

		CGuildMember *pMySelfMember = NULL;
		if(!ptmGuild->GetGuildMember(this->m_character.CharacterUniqueNumber, pMySelfMember))
		{// 2008-06-17 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× -
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_SET_RANK, ERR_PROTOCOL_NO_SUCH_GUILD_MEMBER);
			return RES_BREAK;
		}

		if (m_character.CharacterUniqueNumber != ptmGuild->m_CommanderUniqueNumber
			&& GUILD_RANK_SUBCOMMANDER != pMySelfMember->m_Rank			// 2008-06-17 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× -
			)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_SET_RANK, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
			return RES_BREAK;
		}

		if(GUILD_RANK_SUBCOMMANDER == pMySelfMember->m_Rank
			&& (GUILD_RANK_COMMANDER == pMsgSetRank->GuildRank
				|| 	GUILD_RANK_SUBCOMMANDER == pMsgSetRank->GuildRank)
			)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_SET_RANK, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
			return RES_BREAK;
		}

		if (ptmGuild->m_GuildState != GUILD_STATE_NORMAL)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0025);
			return RES_BREAK;
		}

		CGuildMember *pMember = NULL;
		BOOL ret = ptmGuild->GetGuildMember(pMsgSetRank->MemberUniqueNumber, pMember);
		if (!ret)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendErrorMessage(T_IC_GUILD_SET_RANK, ERR_PROTOCOL_NO_SUCH_GUILD_MEMBER);
			return RES_BREAK;
		}

		if (!ptmGuild->CheckRank(pMsgSetRank->MemberUniqueNumber, pMsgSetRank->GuildRank)
			&& GUILD_RANK_SUBCOMMANDER != pMsgSetRank->GuildRank		// 2008-06-17 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× -
			)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0026);
			//////////////////////////////////////////////////////////////////////////
			// 2008-06-17 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× - Ĺ¬¶óŔĚľđĆ® żäĂ»Ŕ¸·Î Á¤ş¸ ş¸ł»ÁŘ´Ů.
			INIT_MSG_WITH_BUFFER(MSG_IC_GUILD_SET_RANK_OK, T_IC_GUILD_SET_RANK_OK, msgRank, msgRankBuf);
			msgRank->MemberUniqueNumber	= pMsgSetRank->MemberUniqueNumber;
			msgRank->GuildRank			= pMsgSetRank->GuildRank;
			this->SendAddData(msgRankBuf, MSG_SIZE(MSG_IC_GUILD_SET_RANK_OK));
			return RES_BREAK;
		}

		if (pMsgSetRank->GuildRank == GUILD_RANK_COMMANDER)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0027);
			return RES_BREAK;
		}

		if (pMember->m_Rank == GUILD_RANK_COMMANDER)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
			SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0028);
			return RES_BREAK;
		}

		if(GUILD_RANK_SUBCOMMANDER == pMsgSetRank->GuildRank)
		{// 2008-06-17 by dhjin, EP3 - ż©´Ü ĽöÁ¤ »çÇ× -
			CGuildMember *pSubCommander = NULL;
			if(ptmGuild->GetGuildMemberByRank(GUILD_RANK_SUBCOMMANDER, pSubCommander))
			{
				pSubCommander->m_Rank	= GUILD_RANK_PRIVATE_NULL;
				ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildSetRank, this, NULL
					, (void*)GUILD_RANK_PRIVATE_NULL, pSubCommander->m_CharacterUniqueNumber, m_uidGuildUniqueNumber);
			}
		}
		
		pMember->m_Rank		= pMsgSetRank->GuildRank;
	}
	ms_pIMIOCP->m_MapGuild.unlock();

	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildSetRank, this, NULL
		, (void*)pMsgSetRank->GuildRank, pMsgSetRank->MemberUniqueNumber, m_uidGuildUniqueNumber);

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_GET_OTHER_GUILD_INFO(const char *pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_GET_OTHER_GUILD_INFO,
									MSG_IC_GUILD_GET_OTHER_GUILD_INFO, msgGetOtherGuildInfo);

	ms_pIMIOCP->m_MapGuild.lock();
	{
		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(msgGetOtherGuildInfo->GuildUniqueNumber);
		if(NULL == ptmGuild)
		{
			ms_pIMIOCP->m_MapGuild.unlock();
// 2005-12-02 by cmkwon, ÇĘżäľřŔ˝
//			SendErrorMessage(T_IC_GUILD_GET_OTHER_GUILD_INFO, ERR_PROTOCOL_NO_SUCH_GUILD, msgGetOtherGuildInfo->GuildUniqueNumber);
			return RES_BREAK;
		}

		INIT_MSG_WITH_BUFFER(MSG_IC_GUILD_GET_OTHER_GUILD_INFO_OK, T_IC_GUILD_GET_OTHER_GUILD_INFO_OK,
							msgGetOK, msgGetOKBuf);
		msgGetOK->OtherGuildMemberInfo = *ptmGuild;
		SendAddData(msgGetOKBuf, MSG_SIZE(MSG_IC_GUILD_GET_OTHER_GUILD_INFO_OK));
	}
	ms_pIMIOCP->m_MapGuild.unlock();	

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_GUILD_SURRENDER_GUILD_WAR(const char *pPacket, int nLength, int &nBytesUsed)
{	
	ms_pIMIOCP->GuildWarEnd(BATTLE_END_SURRENDER, this->m_uidGuildUniqueNumber);
	return RES_RETURN_TRUE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_GUILD_NEW_COMMANDER(const char *pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´ÜŔĺ Ŕ§ŔÓ
/// \author		dhjin
/// \date		2008-05-21 ~ 2008-05-21
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_GUILD_NEW_COMMANDER(const char *pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_NEW_COMMANDER,
									MSG_IC_GUILD_NEW_COMMANDER, pRMsg);

	if(NULL == this
		|| !this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_GUILD_NEW_COMMANDER, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	mt_auto_lock mtG(&ms_pIMIOCP->m_MapGuild);
	CGuild *pGuild = ms_pIMIOCP->m_MapGuild.findLock(this->m_character.GuildUniqueNumber);
	if (NULL == pGuild) 
	{// ż©´Ü Á¤ş¸°ˇ ľř´Ů
		SendErrorMessage(T_IC_GUILD_NEW_COMMANDER, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}	

	if(this->m_character.CharacterUniqueNumber != pGuild->m_CommanderUniqueNumber)
	{
		SendErrorMessage(T_IC_GUILD_NEW_COMMANDER, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
		return RES_BREAK;
	}
	
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildUpdateCommander, this, NULL, (void*)pRMsg->NewCommanderUID);

	return RES_RETURN_TRUE;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_GUILD_NOTICE_WRITE(const char *pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´Ü °řÁö
/// \author		dhjin
/// \date		2008-05-21 ~ 2008-05-21
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_GUILD_NOTICE_WRITE(const char *pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_NOTICE_WRITE,
									MSG_IC_GUILD_NOTICE_WRITE, pRMsg);
	
	if(NULL == this
		|| !this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_GUILD_NOTICE_WRITE, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}
	
	mt_auto_lock mtG(&ms_pIMIOCP->m_MapGuild);
	CGuild *pGuild = ms_pIMIOCP->m_MapGuild.findLock(this->m_character.GuildUniqueNumber);
	if (NULL == pGuild) 
	{// ż©´Ü Á¤ş¸°ˇ ľř´Ů
		SendErrorMessage(T_IC_GUILD_NOTICE_WRITE, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}
	
	CGuildMember *pMember = NULL;
	if(!pGuild->GetGuildMember(this->m_character.CharacterUniqueNumber, pMember))
	{
		SendErrorMessage(T_IC_GUILD_NOTICE_WRITE, ERR_PROTOCOL_NO_SUCH_GUILD_MEMBER);
		return RES_BREAK;
	}

	if(this->m_character.CharacterUniqueNumber != pGuild->m_CommanderUniqueNumber
		&& GUILD_RANK_SUBCOMMANDER != pMember->m_Rank)
	{
		SendErrorMessage(T_IC_GUILD_NOTICE_WRITE, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
		return RES_BREAK;
	}	

	QPARAM_GUILD_NOTICE *pQParam = new QPARAM_GUILD_NOTICE;
	STRNCPY_MEMSET(pQParam->Notice, pRMsg->Notice, SIZE_MAX_NOTICE);

	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildNotice, this, pQParam);

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_GUILD_GET_APPLICANT(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ÁöżřŔÚ °ü¸®
/// \author		dhjin
/// \date		2008-05-27 ~ 2008-05-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_GUILD_GET_APPLICANT(const char* pPacket, int nLength, int &nBytesUsed)
{
	if(NULL == this
		|| !this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_GUILD_GET_APPLICANT, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}
	
	mt_auto_lock mtG(&ms_pIMIOCP->m_MapGuild);
	CGuild *pGuild = ms_pIMIOCP->m_MapGuild.findLock(this->m_character.GuildUniqueNumber);
	if (NULL == pGuild) 
	{// ż©´Ü Á¤ş¸°ˇ ľř´Ů
		SendErrorMessage(T_IC_GUILD_GET_APPLICANT, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}

	CGuildMember *pMember = NULL;
	if(!pGuild->GetGuildMember(this->m_character.CharacterUniqueNumber, pMember))
	{
		SendErrorMessage(T_IC_GUILD_GET_APPLICANT, ERR_PROTOCOL_NO_SUCH_GUILD_MEMBER);
		return RES_BREAK;
	}

	if(this->m_character.CharacterUniqueNumber != pGuild->m_CommanderUniqueNumber
		&& GUILD_RANK_SUBCOMMANDER != pMember->m_Rank)
	{
		SendErrorMessage(T_IC_GUILD_GET_APPLICANT, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
		return RES_BREAK;
	}	
	
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildGetApplicant, this, NULL);
	
	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_GUILD_GET_INTRODUCTION(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´Ü ĽŇ°ł
/// \author		dhjin
/// \date		2008-05-27 ~ 2008-05-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_GUILD_GET_INTRODUCTION(const char* pPacket, int nLength, int &nBytesUsed)
{
	if(NULL == this
		|| !this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_GUILD_GET_INTRODUCTION, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}
	
	mt_auto_lock mtG(&ms_pIMIOCP->m_MapGuild);
	CGuild *pGuild = ms_pIMIOCP->m_MapGuild.findLock(this->m_character.GuildUniqueNumber);
	if (NULL == pGuild) 
	{// ż©´Ü Á¤ş¸°ˇ ľř´Ů
		SendErrorMessage(T_IC_GUILD_GET_INTRODUCTION, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}
	
	CGuildMember *pMember = NULL;
	if(!pGuild->GetGuildMember(this->m_character.CharacterUniqueNumber, pMember))
	{
		SendErrorMessage(T_IC_GUILD_GET_INTRODUCTION, ERR_PROTOCOL_NO_SUCH_GUILD_MEMBER);
		return RES_BREAK;
	}

	if(this->m_character.CharacterUniqueNumber != pGuild->m_CommanderUniqueNumber
		&& GUILD_RANK_SUBCOMMANDER != pMember->m_Rank)
	{
		SendErrorMessage(T_IC_GUILD_GET_INTRODUCTION, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
		return RES_BREAK;
	}	
	
	pGuild->CheckOldIntroductionContent();

	INIT_MSG_WITH_BUFFER(MSG_IC_GUILD_GET_INTRODUCTION_OK, T_IC_GUILD_GET_INTRODUCTION_OK, pMsg, SendBuf);
	STRNCPY_MEMSET(pMsg->GuildIntroduction, pGuild->GetGuildIntroduction(), SIZE_MAX_NOTICE);
	this->SendAddData(SendBuf, MSG_SIZE(MSG_IC_GUILD_GET_INTRODUCTION_OK));

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_GUILD_GET_SELF_INTRODUCTION(const char *pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´Ü ÁöżřŔÚ ĽŇ°łĽ­ 
/// \author		dhjin
/// \date		2008-05-27 ~ 2008-05-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_GUILD_GET_SELF_INTRODUCTION(const char *pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_GET_SELF_INTRODUCTION,
									MSG_IC_GUILD_GET_SELF_INTRODUCTION, pRMsg);

	if(NULL == this
		|| !this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_GUILD_GET_SELF_INTRODUCTION, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}
	
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GetSelfIntroduction, this, NULL, (void*)pRMsg->CharacterUID);

	return RES_RETURN_TRUE;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult Process_IC_GUILD_SEARCH_INTRODUCTION(const char *pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´Ü ĽŇ°ł °Ë»ö
/// \author		dhjin
/// \date		2008-05-27 ~ 2008-05-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_GUILD_SEARCH_INTRODUCTION(const char *pPacket, int nLength, int &nBytesUsed)
{
	if(NULL == this
		|| !this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_GUILD_SEARCH_INTRODUCTION, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}
	
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildSearchIntroduction, this, NULL);

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_GUILD_UPDATE_INTRODUCTION(const char *pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´Ü ĽŇ°ł ŔŰĽş 
/// \author		dhjin
/// \date		2008-05-28 ~ 2008-05-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_GUILD_UPDATE_INTRODUCTION(const char *pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_UPDATE_INTRODUCTION,
									MSG_IC_GUILD_UPDATE_INTRODUCTION, pRMsg);

	if(NULL == this
		|| !this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_GUILD_UPDATE_INTRODUCTION, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}
	
	mt_auto_lock mtG(&ms_pIMIOCP->m_MapGuild);
	CGuild *pGuild = ms_pIMIOCP->m_MapGuild.findLock(this->m_character.GuildUniqueNumber);
	if (NULL == pGuild) 
	{// ż©´Ü Á¤ş¸°ˇ ľř´Ů
		SendErrorMessage(T_IC_GUILD_UPDATE_INTRODUCTION, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}
	
	CGuildMember *pMember = NULL;
	if(!pGuild->GetGuildMember(this->m_character.CharacterUniqueNumber, pMember))
	{
		SendErrorMessage(T_IC_GUILD_UPDATE_INTRODUCTION, ERR_PROTOCOL_NO_SUCH_GUILD_MEMBER);
		return RES_BREAK;
	}

	if(this->m_character.CharacterUniqueNumber != pGuild->m_CommanderUniqueNumber
		&& GUILD_RANK_SUBCOMMANDER != pMember->m_Rank)
	{
		SendErrorMessage(T_IC_GUILD_UPDATE_INTRODUCTION, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
		return RES_BREAK;
	}	

	SGUILD_INTRODUCTION	NewGuldIntroduction;
	MEMSET_ZERO(&NewGuldIntroduction, sizeof(SGUILD_INTRODUCTION));
	STRNCPY_MEMSET(NewGuldIntroduction.IntroductionContent, pRMsg->GuildIntroduction, SIZE_MAX_NOTICE);
	NewGuldIntroduction.RegDate.SetCurrentDateTime();
	
	pGuild->SetGuildIntroduction(&NewGuldIntroduction);
	
	QPARAM_GUILD_UPDATE_INTRODUCTION *pQParam = new QPARAM_GUILD_UPDATE_INTRODUCTION;
	STRNCPY_MEMSET(pQParam->GuildIntroduction, pRMsg->GuildIntroduction, SIZE_MAX_NOTICE);
	pQParam->WriteDate	= NewGuldIntroduction.RegDate;
	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildUpdateIntroduction, this, pQParam);

	return RES_RETURN_TRUE;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_GUILD_DELETE_INTRODUCTION(const char *pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ż©´Ü ĽŇ°ł Áöżě±â
/// \author		dhjin
/// \date		2008-05-28 ~ 2008-05-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_GUILD_DELETE_INTRODUCTION(const char *pPacket, int nLength, int &nBytesUsed)
{
	if(NULL == this
		|| !this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_GUILD_DELETE_INTRODUCTION, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}
		
	mt_auto_lock mtG(&ms_pIMIOCP->m_MapGuild);
	CGuild *pGuild = ms_pIMIOCP->m_MapGuild.findLock(this->m_character.GuildUniqueNumber);
	if (NULL == pGuild) 
	{// ż©´Ü Á¤ş¸°ˇ ľř´Ů
		SendErrorMessage(T_IC_GUILD_DELETE_INTRODUCTION, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}
	
	CGuildMember *pMember = NULL;
	if(!pGuild->GetGuildMember(this->m_character.CharacterUniqueNumber, pMember))
	{
		SendErrorMessage(T_IC_GUILD_DELETE_INTRODUCTION, ERR_PROTOCOL_NO_SUCH_GUILD_MEMBER);
		return RES_BREAK;
	}

	if(this->m_character.CharacterUniqueNumber != pGuild->m_CommanderUniqueNumber
		&& GUILD_RANK_SUBCOMMANDER != pMember->m_Rank)
	{
		SendErrorMessage(T_IC_GUILD_DELETE_INTRODUCTION, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
		return RES_BREAK;
	}	

	pGuild->InitGuildIntroduction();

	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildDeleteIntroduction, NULL, NULL, (void*)pGuild->m_GuildUniqueNumber);

	this->SendAddMessageType(T_IC_GUILD_DELETE_INTRODUCTION_OK);
	return RES_RETURN_TRUE;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_GUILD_UPDATE_SELFINTRODUCTION(const char *pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ŔÚ±â ĽŇ°ł ŔŰĽş 
/// \author		dhjin
/// \date		2008-05-28 ~ 2008-05-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_GUILD_UPDATE_SELFINTRODUCTION(const char *pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_UPDATE_SELFINTRODUCTION,
									MSG_IC_GUILD_UPDATE_SELFINTRODUCTION, pRMsg);

	if(NULL == this
		|| !this->IsValidCharacter()
		|| IS_VALID_UNIQUE_NUMBER(this->m_character.GuildUniqueNumber)
		|| COMPARE_INFLUENCE(m_character.InfluenceType, INFLUENCE_TYPE_NORMAL)
		)
	{
		SendErrorMessage(T_IC_GUILD_UPDATE_SELFINTRODUCTION, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}
	
	QPARAM_GUILD_UPDATE_SELFINTRODUCTION *pQParam = new QPARAM_GUILD_UPDATE_SELFINTRODUCTION;
	pQParam->TargetGuildUID		= pRMsg->TargetGuildUID;
	STRNCPY_MEMSET(pQParam->SelfIntroduction, pRMsg->SelfIntroduction, SIZE_MAX_NOTICE);

	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildUpdateSelfIntroduction, this, pQParam);

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_GUILD_DELETE_SELFINTRODUCTION(const char *pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ŔÚ±â ĽŇ°ł Áöżě±â  
/// \author		dhjin
/// \date		2008-05-28 ~ 2008-05-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_GUILD_DELETE_SELFINTRODUCTION(const char *pPacket, int nLength, int &nBytesUsed)
{
	if(NULL == this
		|| !this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_GUILD_DELETE_SELFINTRODUCTION, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildDeleteSelfIntroduction, this, NULL);

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_GUILD_APPLICANT_INVITE(const char *pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ÁöżřŔÚ °ü¸®żˇĽ­ ż©´Üżř ĂĘ´ë
/// \author		dhjin
/// \date		2008-06-12 ~ 2008-06-12
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_GUILD_APPLICANT_INVITE(const char *pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_APPLICANT_INVITE,
									MSG_IC_GUILD_APPLICANT_INVITE, pRMsg);
	
	mt_auto_lock mtG(&ms_pIMIOCP->m_MapGuild);

	CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(this->m_character.GuildUniqueNumber);
	if(NULL == ptmGuild)
	{
		SendErrorMessage(T_IC_GUILD_APPLICANT_INVITE, ERR_PROTOCOL_NOT_HAVE_GUILD);
		return RES_BREAK;
	}

	CGuildMember *pMember = NULL;
	if(!ptmGuild->GetGuildMember(this->m_character.CharacterUniqueNumber, pMember))
	{
		SendErrorMessage(T_IC_GUILD_APPLICANT_INVITE, ERR_PROTOCOL_NO_SUCH_GUILD_MEMBER);
		return RES_BREAK;
	}

	if(this->m_character.CharacterUniqueNumber != ptmGuild->m_CommanderUniqueNumber
		&& GUILD_RANK_SUBCOMMANDER != pMember->m_Rank)
	{
		SendErrorMessage(T_IC_GUILD_APPLICANT_INVITE, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
		return RES_BREAK;
	}

	if (ptmGuild->m_GuildState != GUILD_STATE_NORMAL)
	{
		SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0016);
		return RES_BREAK;
	}

	CGuildMember *pInviteMember = NULL;
	if(ptmGuild->GetGuildMember(pRMsg->CharacterUID, pInviteMember))
	{
		SendErrorMessage(T_IC_GUILD_APPLICANT_INVITE, ERR_PROTOCOL_GUILD_MEMBER_ALREADY_EXISTS);
		return RES_BREAK;
	}
	
	if (CAtumSJ::GetMaxGuildSize(ptmGuild->m_nGuildMemberCapacity, this->IsExistPremiumCard()) <= ptmGuild->m_ListGuildMember.size())		// 2008-05-28 by dhjin, EP3 ż©´Ü ĽöÁ¤ »çÇ× - ż©´Üżř Áő°ˇ Äł˝¬ ľĆŔĚĹŰ
	{
		SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0015);
		return RES_BREAK;
	}

	CIMIOCPSocket * pIISock = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pRMsg->CharacterUID);
	if(NULL != pIISock)
	{
		MSG_IC_GUILD_ACCEPT_INVITE	*pMsgAccept = new MSG_IC_GUILD_ACCEPT_INVITE;
		pMsgAccept->GuildUniqueNumber		= ptmGuild->m_GuildUniqueNumber;
		ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildAddMember, pIISock, pMsgAccept);
	}
	else
	{
		MSG_IC_GUILD_APPLICANT_INVITE	* pMsgAccept = new MSG_IC_GUILD_APPLICANT_INVITE;
		*pMsgAccept	= *pRMsg;
		ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildAddOffMember, this, pMsgAccept);		
	}
	
	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_GUILD_APPLICANT_REJECT_INVITE(const char *pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 - ż©´Ü ĽöÁ¤ »çÇ× - ÁöżřŔÚ °ü¸®żˇĽ­ ż©´Üżř ĂĘ´ë °ĹşÎ 
/// \author		dhjin
/// \date		2008-06-12 ~ 2008-06-12
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_GUILD_APPLICANT_REJECT_INVITE(const char *pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_GUILD_APPLICANT_REJECT_INVITE,
									MSG_IC_GUILD_APPLICANT_REJECT_INVITE, pRMsg);

	CIMIOCPSocket * pIISock = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pRMsg->CharacterUID);
	if(NULL != pIISock)
	{
		pIISock->SendString128(STRING_128_USER_ERR, STRMSG_080613_0001, pRMsg->GuildName);
		ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildDeleteSelfIntroduction, pIISock, NULL);
	}
	else
	{
		char NotifyMsgString[SIZE_MAX_NOTIFY_MSG_STRING];
		sprintf(NotifyMsgString, STRMSG_080613_0001, pRMsg->GuildName);
		SNOTIFY_MSG *pQParam = new SNOTIFY_MSG;
		MEMSET_ZERO(pQParam, sizeof(SNOTIFY_MSG));
		pQParam->CharacterUID	= pRMsg->CharacterUID;
		pQParam->NotifyMsgType	= NOTIFY_MSG_TYPE_TEXT;
		STRNCPY_MEMSET(pQParam->NotifyMsgString, NotifyMsgString,SIZE_MAX_NOTIFY_MSG_STRING);
		ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_InsertNotifyMsg, NULL, pQParam);
		ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildDeleteSelfIntroductionOffUser, NULL, NULL, (void*)pRMsg->CharacterUID);		
	}
	
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_FI_ADMIN_GET_CHARACTER_INFO_OK(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_ADMIN_GET_CHARACTER_INFO_OK, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	int									nRecvTypeSize = 0;
	MSG_FI_ADMIN_GET_CHARACTER_INFO_OK	*pMsgGetOK = NULL;

	nRecvTypeSize = sizeof(MSG_FI_ADMIN_GET_CHARACTER_INFO_OK);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_FI_ADMIN_GET_CHARACTER_INFO_OK, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201C);
		return RES_RETURN_FALSE;
	}
	pMsgGetOK = (MSG_FI_ADMIN_GET_CHARACTER_INFO_OK*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMIOCPSocket *pAdminSock = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pMsgGetOK->AdminCharacterUniqueNumber);
	if (NULL == pAdminSock || FALSE == pAdminSock->IsUsing())
	{
		// abnormal error, do nothing
		return RES_BREAK;
	}

	if (pMsgGetOK->Type == ADMIN_GET_CHARAC_TYPE_SEND)
	{
		pAdminSock->SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0050,
									pMsgGetOK->CharacterName,
									pMsgGetOK->AccountName,
									pMsgGetOK->MapChannelIndex.MapIndex,
									pMsgGetOK->MapChannelIndex.ChannelIndex,
									pMsgGetOK->Level);
	}
	else if (pMsgGetOK->Type == ADMIN_GET_CHARAC_TYPE_MOVETO)
	{
		INIT_MSG_WITH_BUFFER(MSG_FI_ADMIN_MOVETO_CHARACTER, T_FI_ADMIN_MOVETO_CHARACTER, pMsgAdminMoveTo, pMsgAdminMoveToBuf);
		pMsgAdminMoveTo->AdminCharacterUniqueNumber = pMsgGetOK->AdminCharacterUniqueNumber;
		pMsgAdminMoveTo->MapChannelIndex = pMsgGetOK->MapChannelIndex;
		pMsgAdminMoveTo->Position = pMsgGetOK->Position;

		if (FALSE == pAdminSock->SendMsgToCurrentFieldServerSocket(pMsgAdminMoveToBuf, MSG_SIZE(MSG_FI_ADMIN_MOVETO_CHARACTER)))
		{
			// current field server socketŔĚ NULLŔĚ¸é ą«˝Ă.
			char	szSystemLog[1024];
			sprintf(szSystemLog, "  Fatal Error: CurrentFieldServerSocket is NULL. at %s\r\n",
				GetProtocolTypeString(T_FI_ADMIN_GET_CHARACTER_INFO_OK));
			DBGOUT(szSystemLog);
			g_pGlobal->WriteSystemLog(szSystemLog);

			return RES_BREAK;
		}
	}
	else
	{
		// do nothing
	}


	return RES_RETURN_TRUE;
}

BOOL CIMIOCPSocket::SendGuildInfoToFieldServer(UID32_t i_nGuildUniqueNumber, const char *i_szGuildName)
{
	if (FALSE == this->IsUsing())
	{
		return FALSE;
	}

	INIT_MSG_WITH_BUFFER(MSG_FI_CHARACTER_UPDATE_GUILD_INFO, T_FI_CHARACTER_UPDATE_GUILD_INFO, msgUpdateGuild, msgUpdateGuildBuf);
	msgUpdateGuild->CharacterUniqueNumber	= m_character.CharacterUniqueNumber;
	msgUpdateGuild->GuildUniqueNumber		= i_nGuildUniqueNumber;
	if (NULL == i_szGuildName)
	{
		MEMSET_ZERO(msgUpdateGuild->GuildName, SIZE_MAX_GUILD_NAME);
	}
	else
	{
		STRNCPY_MEMSET(msgUpdateGuild->GuildName, i_szGuildName, SIZE_MAX_GUILD_NAME);
	}

	this->SendMsgToCurrentFieldServerSocket(msgUpdateGuildBuf, MSG_SIZE(MSG_FI_CHARACTER_UPDATE_GUILD_INFO));
	return TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_ADMIN_CALL_CHARACTER(const char* pPacket, int nLength, int &nBytesUsed)
{
	int							nRecvTypeSize = 0;
	MSG_IC_ADMIN_CALL_CHARACTER	*pMsgCallCharac = NULL;

	nRecvTypeSize = sizeof(MSG_IC_ADMIN_CALL_CHARACTER);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_ADMIN_CALL_CHARACTER, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201C);
		return RES_RETURN_FALSE;
	}
	pMsgCallCharac = (MSG_IC_ADMIN_CALL_CHARACTER*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	if ( !COMPARE_RACE(m_character.Race, RACE_OPERATION|RACE_GAMEMASTER) )
	{
		// żîżµŔÚżë, ŔĎąÝ »çżëŔÚ´Â ą«˝Ă
		return RES_BREAK;
	}

	// ŔÚ±â ŔÚ˝ĹŔĚ¸é ą«˝Ă
	if ( strnicmp(m_character.CharacterName, pMsgCallCharac->CharacterName, SIZE_MAX_CHARACTER_NAME) == 0 )
	{
		SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0051);
		return RES_BREAK;
	}

	CIMIOCPSocket *pCharacSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(pMsgCallCharac->CharacterName);
	if (pCharacSocket == NULL)
	{
		SendString128(STRING_128_ADMIN_CMD, "%s NOT PLAYING", pMsgCallCharac->CharacterName);
	}
	else
	{
		INIT_MSG_WITH_BUFFER(MSG_FI_ADMIN_CALL_CHARACTER, T_FI_ADMIN_CALL_CHARACTER, pMsgFICall, pMsgFICallBuf);
		pMsgFICall->CharacterUniqueNumber = pCharacSocket->m_character.CharacterUniqueNumber;
		pMsgFICall->MapChannelIndex = pMsgCallCharac->MapChannelIndex;
		pMsgFICall->Position = pMsgCallCharac->Position;

		if (FALSE == pCharacSocket->SendMsgToCurrentFieldServerSocket(pMsgFICallBuf, MSG_SIZE(MSG_FI_ADMIN_CALL_CHARACTER)))
		{
			// current field server socketŔĚ NULLŔĚ¸é ą«˝Ă.
			char	szSystemLog[1024];
			sprintf(szSystemLog, "  Fatal Error: CurrentFieldServerSocket is NULL, %s at %s\r\n",
				GetCharacterString(&pCharacSocket->m_character, string()), GetProtocolTypeString(T_IC_ADMIN_CALL_CHARACTER));
			DBGOUT(szSystemLog);
			g_pGlobal->WriteSystemLog(szSystemLog);

			return RES_BREAK;
		}
	}

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_ADMIN_CALL_GUILD(const char* pPacket, int nLength, int &nBytesUsed)
{
	int						nRecvTypeSize = 0;
	MSG_IC_ADMIN_CALL_GUILD	*pMsgCallGuild = NULL;

	nRecvTypeSize = sizeof(MSG_IC_ADMIN_CALL_GUILD);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_ADMIN_CALL_GUILD, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201C);
		return RES_RETURN_FALSE;
	}
	pMsgCallGuild = (MSG_IC_ADMIN_CALL_GUILD*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	// format: "/ż©´ÜżŔ±â [ż©´Ü¸í]" - ÇŘ´ç Äł¸ŻĹÍ¸¦ şŇ·ŻżČ
	if ( !COMPARE_RACE(m_character.Race, RACE_OPERATION|RACE_GAMEMASTER) )
	{
		// żîżµŔÚżë, ŔĎąÝ »çżëŔÚ´Â ą«˝Ă
		return RES_BREAK;
	}

	ms_pIMIOCP->WarpGulid(pMsgCallGuild->GuildName, pMsgCallGuild->MapChannelIndex, A2DX(pMsgCallGuild->Position));
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_ADMIN_GET_SERVER_STAT(const char* pPacket, int nLength, int &nBytesUsed)
{
	if (!COMPARE_RACE(m_character.Race, RACE_OPERATION|RACE_GAMEMASTER))
	{
		return RES_BREAK;
	}

	INIT_MSG_WITH_BUFFER(MSG_IC_ADMIN_GET_SERVER_STAT_OK, T_IC_ADMIN_GET_SERVER_STAT_OK, pStatOK, pStatOKBuf);
	pStatOK->TotalUserCount = ms_pIMIOCP->m_MapChactarterUniqueNumber.size();

	SendAddData(pStatOKBuf, MSG_SIZE(MSG_IC_ADMIN_GET_SERVER_STAT_OK));

	return RES_RETURN_TRUE;
}


ProcessResult CIMIOCPSocket::Process_IC_COUNTDOWN_DONE(const char* pPacket, int nLength, int &nBytesUsed)
{
	int			nRecvTypeSize = 0;
	MSG_IC_COUNTDOWN_DONE	*pRecvMsg = NULL;

	nRecvTypeSize = sizeof(MSG_IC_COUNTDOWN_DONE);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_COUNTDOWN_DONE, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201C);
		return RES_RETURN_FALSE;
	}
	pRecvMsg = (MSG_IC_COUNTDOWN_DONE*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	///////////////////////////////////////////////////////////////////////////////
	// ˝Ă°ŁŔĚ °ć°ú µÇľú´ÂÁö ĂĽĹ©(ÇâČÄ Ăß°ˇÇŘľßÇÔ)
	// ERR_COUNTDOWN_NOT_ENOUGH_TIME
	if (FALSE == m_countdownChecker.IsEndedCountDown(pRecvMsg->byCountdownType, timeGetTime()))
	{
		SendErrorMessage(T_IC_COUNTDOWN_DONE, ERR_COUNTDOWN_NOT_ENOUGH_TIME, pRecvMsg->byCountdownType);
		
		INIT_MSG_WITH_BUFFER(MSG_IC_COUNTDOWN_START, T_IC_COUNTDOWN_START, pSMsg, pSendBuf);
		pSMsg->byCountdownType	= pRecvMsg->byCountdownType;
		pSMsg->nWaitingSecond	= SECOND_COUNTDOWN_TIME_GAP/2;
		this->SendAddData(pSendBuf, MSG_SIZE(MSG_IC_COUNTDOWN_START));
		return RES_BREAK;
	}

	switch(pRecvMsg->byCountdownType)
	{
	case COUNTDOWN_TYPE_GUILD_WAR:
		{
			if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
			{
				SendErrorMessage(T_IC_COUNTDOWN_DONE, ERR_PROTOCOL_NOT_HAVE_GUILD);
				return RES_BREAK;
			}

			///////////////////////////////////////////////////////////////////////////////
			// Ĺ¬¶óŔĚľđĆ®·Î ŔüĽŰ
			INIT_MSG_WITH_BUFFER(MSG_IC_GUILD_START_WAR, T_IC_GUILD_START_WAR, pMsgStart, pMsgStartBuf);
			ms_pIMIOCP->m_MapGuild.lock();
			{
				CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
				if(NULL == ptmGuild)
				{
					ms_pIMIOCP->m_MapGuild.unlock();
					SendErrorMessage(T_IC_COUNTDOWN_DONE, ERR_PROTOCOL_NOT_HAVE_GUILD);
					return RES_BREAK;
				}

				pMsgStart->PeerGuildUID = ptmGuild->m_uidWarPeerGuildUID;
			}
			ms_pIMIOCP->m_MapGuild.unlock();			

			this->SendAddData(pMsgStartBuf, MSG_SIZE(MSG_IC_GUILD_START_WAR));
		}
		break;
	default:
		{
			SendErrorMessage(T_IC_COUNTDOWN_DONE, ERR_COUNTDOWN_INVALID_COUNTDOWN_TYPE, pRecvMsg->byCountdownType);
			return RES_BREAK;
		}
	}

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_VOIP_SET(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 VOIP - ĽłÁ¤
/// \author		dhjin
/// \date		2008-06-17 ~ 2008-06-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_VOIP_SET(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_VOIP_SET,
									MSG_IC_VOIP_SET, pRMsg);

	if(!this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_VOIP_SET, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	this->SendVoipType(pRMsg->VoipType);

	m_VoipType	= pRMsg->VoipType;

	return RES_RETURN_TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 2008-06-16 by dhjin, EP3 Ă¤ĆĂąć -
///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_CREATE(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ă¤ĆĂąć - Ă¤ĆĂąć »ýĽş
/// \author		dhjin
/// \date		2008-06-16 ~ 2008-06-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_CREATE(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_CHATROOM_CREATE,
									MSG_IC_CHATROOM_CREATE, pRMsg);
	
	if(!this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHATROOM_CREATE, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	Err_t errCode =  ms_pIMIOCP->m_ChatRoomManager.CreateChatRoom(this, pRMsg);
	if(ERR_NO_ERROR != errCode)
	{
		SendErrorMessage(T_IC_CHATROOM_CREATE, errCode);
		return RES_BREAK;
	}

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_LIST_INFO(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ¸ń·Ď °ˇÁ®żŔ±â		
/// \author		dhjin
/// \date		2008-06-16 ~ 2008-06-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_LIST_INFO(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_CHATROOM_LIST_INFO,
									MSG_IC_CHATROOM_LIST_INFO, pRMsg);
	
	if(!this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHATROOM_LIST_INFO, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}
	
	this->SendChatRoomListInfo(pRMsg->StartNum);

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_REQUEST_INVITE(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ĂĘ´ë żäĂ» 	
/// \author		dhjin
/// \date		2008-06-16 ~ 2008-06-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_REQUEST_INVITE(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_CHATROOM_REQUEST_INVITE,
									MSG_IC_CHATROOM_REQUEST_INVITE, pMsgInvite);

	if(!this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHATROOM_REQUEST_INVITE, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	CIMIOCPSocket *pInviteeSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(pMsgInvite->InviteeCharacterName);
	if (NULL == pInviteeSocket
		|| FALSE == pInviteeSocket->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHATROOM_REQUEST_INVITE, ERR_COMMON_NO_SUCH_CHARACTER);
		return RES_BREAK;
	}
	
	if(this->m_character.InfluenceType != pInviteeSocket->m_character.InfluenceType)
	{
		SendErrorMessage(T_IC_CHATROOM_REQUEST_INVITE, ERR_PROTOCOL_CANNOT_INVITE_USER);
		return RES_BREAK;
	}

	if (strnicmp(pMsgInvite->InviteeCharacterName, this->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME) == 0)
	{
		SendErrorMessage(T_IC_CHATROOM_REQUEST_INVITE, ERR_PROTOCOL_CANNOT_INVITE_USER);
		SendString128(STRING_128_USER_ERR, STRMSG_S_I2NOTIFY_0013);
		return RES_BREAK;
	}

	if(pInviteeSocket->m_bChatRoomUseing)
	{
		SendErrorMessage(T_IC_CHATROOM_REQUEST_INVITE, ERR_CHATROOM_ALREADY_USING);
		return RES_BREAK;
	}

	INIT_MSG_WITH_BUFFER(MSG_IC_CHATROOM_REQUEST_INVITE_QUESTION, T_IC_CHATROOM_REQUEST_INVITE_QUESTION, msgInviteQuestion, SendBuf);

	mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));

	if(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.empty())
	{
		SendErrorMessage(T_IC_CHATROOM_REQUEST_INVITE, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
	{
		if(pMsgInvite->ChatRoomNum == (*itr)->GetChatRoomNum())
		{
			if(this->m_character.CharacterUniqueNumber != (*itr)->GetChatRoomMasterCharacterUID())
			{
				SendErrorMessage(T_IC_CHATROOM_REQUEST_INVITE, ERR_CHATROOM_PERMISSION_DENIED);
				return RES_BREAK;
			}
			
			if((*itr)->GetChatRoomMemberCount() >= (*itr)->GetChatRoomMaxMemberCount())
			{
				SendErrorMessage(T_IC_CHATROOM_REQUEST_INVITE, ERR_CHATROOM_FULL_MEMBER);
				return RES_BREAK;
			}

			if(this->IsUsing() && pInviteeSocket->IsUsing())
			{
				msgInviteQuestion->ChatRoomNum	= pMsgInvite->ChatRoomNum;
				STRNCPY_MEMSET(msgInviteQuestion->ChatRoomMasterName, m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);
				pInviteeSocket->SendAddData(SendBuf, MSG_SIZE(MSG_IC_CHATROOM_REQUEST_INVITE_QUESTION));
				return RES_RETURN_TRUE;
			}		
		}
	}

	SendErrorMessage(T_IC_CHATROOM_REQUEST_INVITE, ERR_CHATROOM_NO_SUCH);
	return RES_BREAK;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_JOIN(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ă¤ĆĂąć - Ă¤ĆĂąć Âüż©	
/// \author		dhjin
/// \date		2008-06-16 ~ 2008-06-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
typedef struct {
	string ip;
	BYTE InflType;
	USHORT UnitKind;
	BYTE Level;
	char Name[SIZE_MAX_CHARACTER_NAME];
	char Acc[SIZE_MAX_ACCOUNT_NAME];
	MAP_CHANNEL_INDEX MapChanIdx;
} TempStoreType;
bool sortByIPAlpha(const TempStoreType& a, const TempStoreType& b)
{
	return a.ip < b.ip;
}
ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_JOIN(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_CHATROOM_JOIN,
									MSG_IC_CHATROOM_JOIN, pMsgJoin);

	if(!this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHATROOM_JOIN, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	if(this->m_bChatRoomUseing)
	{
		SendErrorMessage(T_IC_CHATROOM_JOIN, ERR_CHATROOM_ALREADY_USING);
		return RES_BREAK;
	}

	mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));

	if(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.empty())
	{
		SendErrorMessage(T_IC_CHATROOM_JOIN, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
	{
		if(pMsgJoin->ChatRoomNum == (*itr)->GetChatRoomNum())
		{	
			if((*itr)->GetChatRoomLock())
			{
				if(strcmp(pMsgJoin->ChatRoomPW ,(*itr)->GetChatRoomPW()))
				{
					SendErrorMessage(T_IC_CHATROOM_JOIN, ERR_NOT_MATCH_PW);
					return RES_BREAK;
				}
			}

			if((*itr)->GetChatRoomMemberCount() >= (*itr)->GetChatRoomMaxMemberCount())
			{
				SendErrorMessage(T_IC_CHATROOM_JOIN, ERR_CHATROOM_FULL_MEMBER);
				return RES_BREAK;
			}
			INIT_MSG_WITH_BUFFER(MSG_IC_CHATROOM_ACCEPT_INVITE_OK, T_IC_CHATROOM_ACCEPT_INVITE_OK, msg, SendBuf);
			STRNCPY_MEMSET(msg->CharacterName, this->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);
			msg->CharacterUID	= this->m_character.CharacterUniqueNumber;
			(*itr)->SendMessagetoChatRoomMember(SendBuf, MSG_SIZE(MSG_IC_CHATROOM_ACCEPT_INVITE_OK));				
			(*itr)->InsertNewMember(this);
			this->m_bChatRoomUseing = TRUE;
			this->m_nChatRoomNum	= pMsgJoin->ChatRoomNum;
			
			(*itr)->SendChatRoomMemberInfo(this);

			return RES_RETURN_TRUE;
		}
	}

	SendErrorMessage(T_IC_CHATROOM_JOIN, ERR_CHATROOM_NO_SUCH);
	return RES_BREAK;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_ACCEPT_INVITE(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ĂĘ´ë Ľö¶ô	
/// \author		dhjin
/// \date		2008-06-16 ~ 2008-06-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_ACCEPT_INVITE(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_CHATROOM_ACCEPT_INVITE,
									MSG_IC_CHATROOM_ACCEPT_INVITE, pMsgInvite);

	if(!this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHATROOM_ACCEPT_INVITE, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	if(this->m_bChatRoomUseing)
	{
		SendErrorMessage(T_IC_CHATROOM_ACCEPT_INVITE, ERR_CHATROOM_ALREADY_USING);
		return RES_BREAK;
	}

	mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));

	if(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.empty())
	{
		SendErrorMessage(T_IC_CHATROOM_ACCEPT_INVITE, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
	{
		if(pMsgInvite->ChatRoomNum == (*itr)->GetChatRoomNum())
		{
			if((*itr)->GetChatRoomMemberCount() >= (*itr)->GetChatRoomMaxMemberCount())
			{
				SendErrorMessage(T_IC_CHATROOM_ACCEPT_INVITE, ERR_CHATROOM_FULL_MEMBER);
				return RES_BREAK;
			}
			INIT_MSG_WITH_BUFFER(MSG_IC_CHATROOM_ACCEPT_INVITE_OK, T_IC_CHATROOM_ACCEPT_INVITE_OK, msg, SendBuf);
			STRNCPY_MEMSET(msg->CharacterName, this->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);
			msg->CharacterUID	= this->m_character.CharacterUniqueNumber;
			(*itr)->SendMessagetoChatRoomMember(SendBuf, MSG_SIZE(MSG_IC_CHATROOM_ACCEPT_INVITE_OK));				
			(*itr)->InsertNewMember(this);
			this->m_bChatRoomUseing = TRUE;
			this->m_nChatRoomNum	= pMsgInvite->ChatRoomNum;
			
			(*itr)->SendChatRoomMemberInfo(this);
			
			return RES_RETURN_TRUE;
		}
	}

	SendErrorMessage(T_IC_CHATROOM_ACCEPT_INVITE, ERR_CHATROOM_NO_SUCH);
	return RES_BREAK;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_REJECT_INVITE(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ĂĘ´ë °ĹŔý	
/// \author		dhjin
/// \date		2008-06-16 ~ 2008-06-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_REJECT_INVITE(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_CHATROOM_REJECT_INVITE,
									MSG_IC_CHATROOM_REJECT_INVITE, pMsg);

	if(!this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHATROOM_REJECT_INVITE, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));

	if(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.empty())
	{
		return RES_BREAK;
	}

	mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
	{
		if(pMsg->ChatRoomNum == (*itr)->GetChatRoomNum())
		{
			CIMIOCPSocket * pMasterIISoc = (*itr)->GetMasterIISoc();
			if(NULL != pMasterIISoc)
			{
				pMasterIISoc->SendAddMessageType(T_IC_CHATROOM_REJECT_INVITE_OK);
			}
			
			return RES_RETURN_TRUE;
		}
	}

	SendErrorMessage(T_IC_CHATROOM_ACCEPT_INVITE, ERR_CHATROOM_NO_SUCH);
	return RES_BREAK;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_LEAVE(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ă¤ĆĂąć - Ă¤ĆĂąć łŞ°ˇ±â	
/// \author		dhjin
/// \date		2008-06-16 ~ 2008-06-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_LEAVE(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_CHATROOM_LEAVE,
									MSG_IC_CHATROOM_LEAVE, pMsg);

	if(!this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHATROOM_LEAVE, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	if(!this->m_bChatRoomUseing
		|| this->m_nChatRoomNum != pMsg->ChatRoomNum)
	{
		SendErrorMessage(T_IC_CHATROOM_LEAVE, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));

	if(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.empty())
	{
		SendErrorMessage(T_IC_CHATROOM_LEAVE, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}
	
	mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
	{
		if(pMsg->ChatRoomNum == (*itr)->GetChatRoomNum())
		{
			if(this->m_character.CharacterUniqueNumber == (*itr)->GetChatRoomMasterCharacterUID()
				&& 1 < (*itr)->GetChatRoomMemberCount())
			{
				INIT_MSG_WITH_BUFFER(MSG_IC_CHATROOM_CHANGE_MASTER_OK, T_IC_CHATROOM_CHANGE_MASTER_OK, msg, SendBuf);
				STRNCPY_MEMSET(msg->CharacterName, (*itr)->ChangeChatRoomMaster(), SIZE_MAX_CHARACTER_NAME);
				if(NULL == msg->CharacterName)
				{
					SendErrorMessage(T_IC_CHATROOM_LEAVE, ERR_CHATROOM_NO_SUCH_NEW_MASTER);
					return RES_BREAK;
				}
				msg->NewCharacterUID = (*itr)->GetChatRoomMasterCharacterUID();
				(*itr)->SendMessagetoChatRoomMember(SendBuf, MSG_SIZE(MSG_IC_CHATROOM_CHANGE_MASTER_OK));
			}
			
			INIT_MSG_WITH_BUFFER(MSG_IC_CHATROOM_LEAVE_OK, T_IC_CHATROOM_LEAVE_OK, msg, SendBuf);
			STRNCPY_MEMSET(msg->LeaveCharacterName, pMsg->LeaveCharacterName, SIZE_MAX_CHARACTER_NAME);
			msg->CharacterUID = this->m_character.CharacterUniqueNumber;
			(*itr)->SendMessagetoChatRoomMember(SendBuf, MSG_SIZE(MSG_IC_CHATROOM_LEAVE_OK));

			if((*itr)->LeaveMemberAndCheckEmpty(this->m_character.CharacterUniqueNumber))
			{
				ms_pIMIOCP->m_ChatRoomManager.SetReserveChatNum((*itr)->GetChatRoomNum());
				SAFE_DELETE(*itr);
				itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.erase(itr);
			}
					
			this->m_bChatRoomUseing = FALSE;
			this->m_nChatRoomNum	= 0;		// 2008-10-22 by cmkwon, Ă¤ĆĂąć ąö±× ĽöÁ¤ - 

			return RES_RETURN_TRUE;
		}
	}

	SendErrorMessage(T_IC_CHATROOM_LEAVE, ERR_CHATROOM_NO_SUCH);
	return RES_BREAK;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_BAN(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ă¤ĆĂąć - Ă¤ĆĂąć Ăßąć	
/// \author		dhjin
/// \date		2008-06-16 ~ 2008-06-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_BAN(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_CHATROOM_BAN,
									MSG_IC_CHATROOM_BAN, pMsg);

	if(!this->IsValidCharacter()
		|| this->m_character.CharacterUniqueNumber == pMsg->BanCharacterUID)
	{
		SendErrorMessage(T_IC_CHATROOM_BAN, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	if(!this->m_bChatRoomUseing
		|| this->m_nChatRoomNum != pMsg->ChatRoomNum)
	{
		SendErrorMessage(T_IC_CHATROOM_BAN, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));

	if(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.empty())
	{
		SendErrorMessage(T_IC_CHATROOM_BAN, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
	{
		if(pMsg->ChatRoomNum == (*itr)->GetChatRoomNum())
		{
			if(this->m_character.CharacterUniqueNumber != (*itr)->GetChatRoomMasterCharacterUID())
			{
				SendErrorMessage(T_IC_CHATROOM_BAN, ERR_CHATROOM_PERMISSION_DENIED);
				return RES_BREAK;
			}

			INIT_MSG_WITH_BUFFER(MSG_IC_CHATROOM_BAN_OK, T_IC_CHATROOM_BAN_OK, msg, SendBuf);
			STRNCPY_MEMSET(msg->BanCharacterName, pMsg->BanCharacterName, SIZE_MAX_CHARACTER_NAME);
			msg->BanCharacterUID = pMsg->BanCharacterUID;
			(*itr)->SendMessagetoChatRoomMember(SendBuf, MSG_SIZE(MSG_IC_CHATROOM_BAN_OK));				
			if((*itr)->LeaveMemberAndCheckEmpty(pMsg->BanCharacterUID))
			{
				ms_pIMIOCP->m_ChatRoomManager.SetReserveChatNum((*itr)->GetChatRoomNum());
				SAFE_DELETE(*itr);
				itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.erase(itr);
			}

			CIMIOCPSocket * pBanFISoc = ms_pIMIOCP->GetIMIOCPSocketByCharacterUID(pMsg->BanCharacterUID);
			if(NULL != pBanFISoc)
			{
				pBanFISoc->m_bChatRoomUseing	= FALSE;
				pBanFISoc->m_nChatRoomNum		= 0;		// 2008-10-22 by cmkwon, Ă¤ĆĂąć ąö±× ĽöÁ¤ - 
			}
			
			return RES_RETURN_TRUE;
		}
	}

	SendErrorMessage(T_IC_CHATROOM_BAN, ERR_CHATROOM_NO_SUCH);
	return RES_BREAK;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_CHANGE_NAME(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ŔĚ¸§ şŻ°ć	
/// \author		dhjin
/// \date		2008-06-16 ~ 2008-06-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_CHANGE_NAME(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_CHATROOM_CHANGE_NAME,
									MSG_IC_CHATROOM_CHANGE_NAME, pMsg);

	if(!this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHATROOM_CHANGE_NAME, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	if(!this->m_bChatRoomUseing
		|| this->m_nChatRoomNum != pMsg->ChatRoomNum)
	{
		SendErrorMessage(T_IC_CHATROOM_CHANGE_NAME, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));

	if(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.empty())
	{
		SendErrorMessage(T_IC_CHATROOM_CHANGE_NAME, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
	{
		if(pMsg->ChatRoomNum == (*itr)->GetChatRoomNum())
		{
			if(this->m_character.CharacterUniqueNumber != (*itr)->GetChatRoomMasterCharacterUID())
			{
				SendErrorMessage(T_IC_CHATROOM_CHANGE_NAME, ERR_CHATROOM_PERMISSION_DENIED);
				return RES_BREAK;
			}
			
			(*itr)->UpdateChatRoomName(pMsg->ChatRoomName);
			INIT_MSG_WITH_BUFFER(MSG_IC_CHATROOM_CHANGE_NAME_OK, T_IC_CHATROOM_CHANGE_NAME_OK, msg, SendBuf);
			STRNCPY_MEMSET(msg->ChatRoomName, pMsg->ChatRoomName, SIZE_MAX_CHATROOM_NAME);
			(*itr)->SendMessagetoChatRoomMember(SendBuf, MSG_SIZE(MSG_IC_CHATROOM_CHANGE_NAME_OK));				
			
			return RES_RETURN_TRUE;
		}
	}

	SendErrorMessage(T_IC_CHATROOM_CHANGE_NAME, ERR_CHATROOM_NO_SUCH);
	return RES_BREAK;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_CHANGE_MASTER(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ąćŔĺ şŻ°ć	
/// \author		dhjin
/// \date		2008-06-16 ~ 2008-06-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_CHANGE_MASTER(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_CHATROOM_CHANGE_MASTER,
									MSG_IC_CHATROOM_CHANGE_MASTER, pMsg);

	if(!this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHATROOM_CHANGE_MASTER, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	if(!this->m_bChatRoomUseing
		|| this->m_nChatRoomNum != pMsg->ChatRoomNum)
	{
		SendErrorMessage(T_IC_CHATROOM_CHANGE_MASTER, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));

	if(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.empty())
	{
		SendErrorMessage(T_IC_CHATROOM_CHANGE_MASTER, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}
 
	mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
	{
		if(pMsg->ChatRoomNum == (*itr)->GetChatRoomNum())
		{
			if(this->m_character.CharacterUniqueNumber != (*itr)->GetChatRoomMasterCharacterUID())
			{
				SendErrorMessage(T_IC_CHATROOM_CHANGE_MASTER, ERR_CHATROOM_PERMISSION_DENIED);
				return RES_BREAK;
			}
			
			INIT_MSG_WITH_BUFFER(MSG_IC_CHATROOM_CHANGE_MASTER_OK, T_IC_CHATROOM_CHANGE_MASTER_OK, msg, SendBuf);
			STRNCPY_MEMSET(msg->CharacterName, (*itr)->ChangeChatRoomMaster(pMsg->NewCharacterUID), SIZE_MAX_CHARACTER_NAME);
			if(NULL == msg->CharacterName)
			{
				SendErrorMessage(T_IC_CHATROOM_CHANGE_MASTER, ERR_CHATROOM_NO_SUCH_NEW_MASTER);
				return RES_BREAK;
			}
			msg->NewCharacterUID = pMsg->NewCharacterUID;
			(*itr)->SendMessagetoChatRoomMember(SendBuf, MSG_SIZE(MSG_IC_CHATROOM_CHANGE_MASTER_OK));				
			
			return RES_RETURN_TRUE;
		}
	}

	SendErrorMessage(T_IC_CHATROOM_CHANGE_MASTER, ERR_CHATROOM_NO_SUCH);
	return RES_BREAK;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_CHANGE_LOCK_PW(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ă¤ĆĂąć - Ă¤ĆĂąć şńąĐąřČŁ şŻ°ć	
/// \author		dhjin
/// \date		2008-06-16 ~ 2008-06-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_CHANGE_LOCK_PW(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_CHATROOM_CHANGE_LOCK_PW,
									MSG_IC_CHATROOM_CHANGE_LOCK_PW, pMsg);

	if(!this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHATROOM_CHANGE_LOCK_PW, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	if(!this->m_bChatRoomUseing
		|| this->m_nChatRoomNum != pMsg->ChatRoomNum)
	{
		SendErrorMessage(T_IC_CHATROOM_CHANGE_LOCK_PW, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));

	if(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.empty())
	{
		SendErrorMessage(T_IC_CHATROOM_CHANGE_LOCK_PW, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
	{
		if(pMsg->ChatRoomNum == (*itr)->GetChatRoomNum())
		{
			if(this->m_character.CharacterUniqueNumber != (*itr)->GetChatRoomMasterCharacterUID())
			{
				SendErrorMessage(T_IC_CHATROOM_CHANGE_LOCK_PW, ERR_CHATROOM_PERMISSION_DENIED);
				return RES_BREAK;
			}
			
			(*itr)->UpdateChatRoomLock(pMsg->ChatRoomLock, pMsg->ChatRoomPW);
			INIT_MSG_WITH_BUFFER(MSG_IC_CHATROOM_CHANGE_LOCK_PW_OK, T_IC_CHATROOM_CHANGE_LOCK_PW_OK, msg, SendBuf);
			msg->ChatRoomLock		= pMsg->ChatRoomLock;
			(*itr)->SendMessagetoChatRoomMember(SendBuf, MSG_SIZE(MSG_IC_CHATROOM_CHANGE_LOCK_PW_OK));				
			
			return RES_RETURN_TRUE;
		}
	}

	SendErrorMessage(T_IC_CHATROOM_CHANGE_LOCK_PW, ERR_CHATROOM_NO_SUCH);
	return RES_BREAK;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_CHANGE_MAX_MEMBER(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ŔÎżřĽö şŻ°ć	
/// \author		dhjin
/// \date		2008-06-16 ~ 2008-06-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_CHANGE_MAX_MEMBER(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_CHATROOM_CHANGE_MAX_MEMBER,
									MSG_IC_CHATROOM_CHANGE_MAX_MEMBER, pMsg);

	if(!this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHATROOM_CHANGE_MAX_MEMBER, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	if(!this->m_bChatRoomUseing
		|| this->m_nChatRoomNum != pMsg->ChatRoomNum)
	{
		SendErrorMessage(T_IC_CHATROOM_CHANGE_MAX_MEMBER, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));

	if(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.empty())
	{
		SendErrorMessage(T_IC_CHATROOM_CHANGE_MAX_MEMBER, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
	{
		if(pMsg->ChatRoomNum == (*itr)->GetChatRoomNum())
		{
			if(this->m_character.CharacterUniqueNumber != (*itr)->GetChatRoomMasterCharacterUID())
			{
				SendErrorMessage(T_IC_CHATROOM_CHANGE_MAX_MEMBER, ERR_CHATROOM_PERMISSION_DENIED);
				return RES_BREAK;
			}
			
			(*itr)->UpdateChatRoomMaxMemberCount(pMsg->ChatRoomMaxMember);
			INIT_MSG_WITH_BUFFER(MSG_IC_CHATROOM_CHANGE_MAX_MEMBER_OK, T_IC_CHATROOM_CHANGE_MAX_MEMBER_OK, msg, SendBuf);
			msg->ChatRoomMaxMember		= pMsg->ChatRoomMaxMember;
			(*itr)->SendMessagetoChatRoomMember(SendBuf, MSG_SIZE(MSG_IC_CHATROOM_CHANGE_MAX_MEMBER_OK));				
			
			return RES_RETURN_TRUE;
		}
	}

	SendErrorMessage(T_IC_CHATROOM_CHANGE_MAX_MEMBER, ERR_CHATROOM_NO_SUCH);
	return RES_BREAK;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_OTHER_MEMBER_INFO(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		EP3 Ă¤ĆĂąć - Ă¤ĆĂąć ¸Éąö Á¤ş¸ ŔüĽŰ
/// \author		dhjin
/// \date		2008-06-25 ~ 2008-06-25
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_IC_CHATROOM_OTHER_MEMBER_INFO(const char* pPacket, int nLength, int &nBytesUsed)
{
	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_IC_CHATROOM_OTHER_MEMBER_INFO,
									MSG_IC_CHATROOM_MEMBER_INFO, pMsg);

	if(!this->IsValidCharacter())
	{
		SendErrorMessage(T_IC_CHATROOM_OTHER_MEMBER_INFO, ERR_INVALID_CHARACTER);
		return RES_BREAK;
	}

	mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));

	if(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.empty())
	{
		SendErrorMessage(T_IC_CHATROOM_OTHER_MEMBER_INFO, ERR_CHATROOM_NO_SUCH);
		return RES_BREAK;
	}

	mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
	{
		if(pMsg->ChatRoomNum == (*itr)->GetChatRoomNum())
		{
			(*itr)->SendOtherChatRoomMemberInfo(this);			
			return RES_RETURN_TRUE;
		}
	}

	SendErrorMessage(T_IC_CHATROOM_MEMBER_INFO, ERR_CHATROOM_NO_SUCH);
	return RES_BREAK;
}

// VoIP °ü·Ă
ProcessResult CIMIOCPSocket::Process_IC_VOIP_GET_ACCOUNTUNIQUENUMBER(const char* pPacket, int nLength, int &nBytesUsed)
{
	int							nRecvTypeSize = 0;
	MSG_IC_VOIP_GET_ACCOUNTUNIQUENUMBER	*pRecvMsg = NULL;

	nRecvTypeSize = sizeof(MSG_IC_VOIP_GET_ACCOUNTUNIQUENUMBER);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_VOIP_GET_ACCOUNTUNIQUENUMBER, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201C);
		return RES_RETURN_FALSE;
	}
	pRecvMsg = (MSG_IC_VOIP_GET_ACCOUNTUNIQUENUMBER*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMIOCPSocket *pCalledSock = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(pRecvMsg->szCalledCharacterName);
	if (NULL == pCalledSock)
	{
		INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_ERROR, T_IC_VOIP_ERROR, pSendErr, tmSendBuf);
		pSendErr->nErrorNum					= VOIPERR_1to1_NO_SUCH_CHARACTER;
		pSendErr->nSend2AccountUniqueNumber	= this->m_character.AccountUniqueNumber;
		SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_VOIP_ERROR));
		return RES_BREAK;
	}

	INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_GET_ACCOUNTUNIQUENUMBER_OK, T_IC_VOIP_GET_ACCOUNTUNIQUENUMBER_OK, pSendMsg, tmSendBuf);
	pSendMsg->nCalledAccountUniqueNumber = pCalledSock->m_character.AccountUniqueNumber;
	SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_VOIP_GET_ACCOUNTUNIQUENUMBER_OK));
	
	return RES_RETURN_TRUE;	
}

ProcessResult CIMIOCPSocket::Process_IC_VOIP_1to1_DIAL(const char* pPacket, int nLength, int &nBytesUsed)
{
	int							nRecvTypeSize = 0;
	MSG_IC_VOIP_1to1_DIAL		*pRecvMsg = NULL;

	nRecvTypeSize = sizeof(MSG_IC_VOIP_1to1_DIAL);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_VOIP_1to1_DIAL, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201C);
		return RES_RETURN_FALSE;
	}
	pRecvMsg = (MSG_IC_VOIP_1to1_DIAL*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMIOCPSocket *pCalledSock = ms_pIMIOCP->GetIMIOCPSocketByAccountUID(pRecvMsg->nCalledAccountUniqueNumber);
	if (NULL == pCalledSock)
	{
		INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_ERROR, T_IC_VOIP_ERROR, pSendErr, tmSendBuf);
		pSendErr->nErrorNum					= VOIPERR_1to1_NO_SUCH_ACCOUNTUNIQUENUMBER;
		pSendErr->nSend2AccountUniqueNumber	= this->m_character.AccountUniqueNumber;
		SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_VOIP_ERROR));

		return RES_BREAK;
	}

// 2008-05-15 by dhjin, EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć
//	if (FALSE == COMPARE_CHATTYPE_BIT(pCalledSock->m_enableChatFlag8, CHAT_TYPE_PTOP))
	if (FALSE == COMPARE_CHATTYPE_BIT(pCalledSock->m_enableChatFlag16, CHAT_TYPE_PTOP))
	{
		INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_ERROR, T_IC_VOIP_ERROR, pSendErr, tmSendBuf);
		pSendErr->nErrorNum					= VOIPERR_1to1_CANNOT_CALL;
		pSendErr->nSend2AccountUniqueNumber	= this->m_character.AccountUniqueNumber;
		SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_VOIP_ERROR));		
		return RES_BREAK;
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2005-12-07 by cmkwon, ĽĽ·ÂĂĽĹ©
	if(FALSE == GCheckRaceAndInfluenceType(CHECK_TYPE_VOIP_1to1, m_character.Race, m_character.InfluenceType,
		pCalledSock->GetCharacter()->Race, pCalledSock->GetCharacter()->InfluenceType))
	{
		SendErrorMessage(T_IC_VOIP_1to1_DIAL, ERR_PROTOCOL_REQ_INFLUENCE_NOT_MATCHED);	
		return RES_BREAK;
	}

	if(pCalledSock->IsCharacterNameFromRejectList(this->m_character.CharacterName))
	{// »ó´ëąć °ĹşÎ¸®˝şĆ®żˇ µî·ĎµÇľî ŔÖŔ˝

		SendErrorMessage(T_IC_VOIP_1to1_DIAL, ERR_FRIEND_REGISTERED_PEER_REJECTLIST);	
		return RES_BREAK;
	}

	INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_1to1_DIAL, T_IC_VOIP_1to1_DIAL, pSendMsg, tmSendBuf);
	*pSendMsg = *pRecvMsg;
	pCalledSock->SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_VOIP_1to1_DIAL));

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_VOIP_1to1_DIAL_OK(const char* pPacket, int nLength, int &nBytesUsed)
{
	int							nRecvTypeSize = 0;
	MSG_IC_VOIP_1to1_DIAL_OK	*pRecvMsg = NULL;

	nRecvTypeSize = sizeof(MSG_IC_VOIP_1to1_DIAL_OK);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_VOIP_1to1_DIAL_OK, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201C);
		return RES_RETURN_FALSE;
	}
	pRecvMsg = (MSG_IC_VOIP_1to1_DIAL_OK*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	CIMIOCPSocket *pCallerSock = ms_pIMIOCP->GetIMIOCPSocketByAccountUID(pRecvMsg->nCallerAccountUniqueNumber);
	if (NULL == pCallerSock)
	{
		INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_ERROR, T_IC_VOIP_ERROR, pSendErr, tmSendBuf);
		pSendErr->nErrorNum					= VOIPERR_1to1_NO_SUCH_ACCOUNTUNIQUENUMBER;
		pSendErr->nSend2AccountUniqueNumber	= this->m_character.AccountUniqueNumber;
		SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_VOIP_ERROR));
		return RES_BREAK;
	}

	INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_1to1_DIAL_OK, T_IC_VOIP_1to1_DIAL_OK, pSendMsg, tmSendBuf);
	*pSendMsg = *pRecvMsg;
	pCallerSock->SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_VOIP_1to1_DIAL_OK));
	
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_VOIP_1to1_END(const char* pPacket, int nLength, int &nBytesUsed)
{
	int							nRecvTypeSize = 0;
	MSG_IC_VOIP_1to1_END			*pRecvMsg = NULL;
	
	nRecvTypeSize = sizeof(MSG_IC_VOIP_1to1_END);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_VOIP_ERROR, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201C);
		return RES_RETURN_FALSE;
	}
	pRecvMsg = (MSG_IC_VOIP_1to1_END*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;
	
	CIMIOCPSocket *pSend2Sock = ms_pIMIOCP->GetIMIOCPSocketByAccountUID(pRecvMsg->nSend2AccountUniqueNumber);
	if (NULL == pSend2Sock)
	{
		// Error¸¦ Client·Î ŔüĽŰÇŇ ÇĘżä ľřŔ˝
		return RES_BREAK;
	}
	
	INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_1to1_END, T_IC_VOIP_1to1_END, pSendMsg, tmSendBuf);
	*pSendMsg = *pRecvMsg;
	pSend2Sock->SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_VOIP_1to1_END));
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_VOIP_NtoN_CREATEROOM_REQUEST(const char* pPacket, int nLength, int &nBytesUsed)
{
	int							nRecvTypeSize = 0;
	MSG_IC_VOIP_NtoN_CREATEROOM_REQUEST		*pRecvMsg = NULL;
	
	nRecvTypeSize = sizeof(MSG_IC_VOIP_NtoN_CREATEROOM_REQUEST);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_VOIP_NtoN_CREATEROOM_REQUEST, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201C);
		return RES_RETURN_FALSE;
	}
	pRecvMsg = (MSG_IC_VOIP_NtoN_CREATEROOM_REQUEST*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	///////////////////////////////////////////////////////////////////////////////
	// 1. ±ćµĺĹëČ­
	// 2. Ćí´ëĹëČ­
	if(FALSE == pRecvMsg->byIsPartyCall)
	{
		if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
		{
			SendErrorMessage(T_IC_VOIP_NtoN_CREATEROOM_REQUEST, ERR_PROTOCOL_NOT_HAVE_GUILD, pRecvMsg->byIsPartyCall);
			return RES_BREAK;
		}

		ms_pIMIOCP->m_MapGuild.lock();
		{
			CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
			if(NULL == ptmGuild)
			{
				ms_pIMIOCP->m_MapGuild.unlock();
				SendErrorMessage(T_IC_VOIP_NtoN_CREATEROOM_REQUEST, ERR_PROTOCOL_NOT_HAVE_GUILD);
				return RES_BREAK;
			}
			
			if (m_character.CharacterUniqueNumber != ptmGuild->m_CommanderUniqueNumber)
			{
				ms_pIMIOCP->m_MapGuild.unlock();				
				SendErrorMessage(T_IC_VOIP_NtoN_CREATEROOM_REQUEST, ERR_PROTOCOL_GUILD_PERMISSION_DENIED, pRecvMsg->byIsPartyCall);
				return RES_BREAK;
			}
		}
		ms_pIMIOCP->m_MapGuild.unlock();
	}
	else
	{
		CIMParty *ptmParty = this->GetIMParty();
		if (ptmParty == NULL)
		{
			// error
			SendErrorMessage(T_IC_VOIP_NtoN_CREATEROOM_REQUEST, ERR_PROTOCOL_NOT_HAVE_PARTY, pRecvMsg->byIsPartyCall);
			return RES_BREAK;
		}

		ptmParty->LockIMParty();
		if (ptmParty->GetMasterCharacterUniqueNumber() != m_character.CharacterUniqueNumber)
		{
			// error: ŔÚ˝ĹŔĚ ĆÄĆĽŔĺŔĚ ľĆ´ĎąÇ·Î ±ÇÇŃŔĚ ľřŔ˝
			ptmParty->UnlockIMParty();
			SendErrorMessage(T_IC_VOIP_NtoN_CREATEROOM_REQUEST, ERR_PROTOCOL_PARTY_PERMISSION_DENIED, pRecvMsg->byIsPartyCall);
			return RES_BREAK;
		}
		ptmParty->UnlockIMParty();
	}

	INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_NtoN_CREATEROOM_REQUEST_OK, T_IC_VOIP_NtoN_CREATEROOM_REQUEST_OK, pSendMsg, pBuffer);
	pSendMsg->byIsPartyCall = pRecvMsg->byIsPartyCall;
	this->SendAddData(pBuffer, MSG_SIZE(MSG_IC_VOIP_NtoN_CREATEROOM_REQUEST_OK));
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_VOIP_NtoN_INVITE_ALL(const char* pPacket, int nLength, int &nBytesUsed)
{
	int							nRecvTypeSize = 0;
	MSG_IC_VOIP_NtoN_INVITE_ALL		*pRecvMsg = NULL;
	
	nRecvTypeSize = sizeof(MSG_IC_VOIP_NtoN_INVITE_ALL);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_VOIP_NtoN_INVITE_ALL, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201C);
		return RES_RETURN_FALSE;
	}
	pRecvMsg = (MSG_IC_VOIP_NtoN_INVITE_ALL*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	///////////////////////////////////////////////////////////////////////////////
	// 1. ±ćµĺĹëČ­
	// 2. Ćí´ëĹëČ­
	if(FALSE == pRecvMsg->byIsPartyCall)
	{
		if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
		{
			SendErrorMessage(T_IC_VOIP_NtoN_INVITE_ALL, ERR_PROTOCOL_NOT_HAVE_GUILD, pRecvMsg->byIsPartyCall);
			return RES_BREAK;
		}

		ms_pIMIOCP->m_MapGuild.lock();
		{
			CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
			if(NULL == ptmGuild)
			{
				ms_pIMIOCP->m_MapGuild.unlock();
				SendErrorMessage(T_IC_VOIP_NtoN_INVITE_ALL, ERR_PROTOCOL_NOT_HAVE_GUILD);
				return RES_BREAK;
			}
			
			if (m_character.CharacterUniqueNumber != ptmGuild->m_CommanderUniqueNumber)
			{
				ms_pIMIOCP->m_MapGuild.unlock();				
				SendErrorMessage(T_IC_VOIP_NtoN_INVITE_ALL, ERR_PROTOCOL_GUILD_PERMISSION_DENIED, pRecvMsg->byIsPartyCall);
				return RES_BREAK;
			}
			
			INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_NtoN_INVITE_YOU, T_IC_VOIP_NtoN_INVITE_YOU, pSMsg, pBuffer);
			pSMsg->byIsPartyCall = pRecvMsg->byIsPartyCall;
			ptmGuild->SendMsgToGuildMembers(pBuffer, MSG_SIZE(MSG_IC_VOIP_NtoN_INVITE_YOU), this->m_character.CharacterUniqueNumber);
		}
		ms_pIMIOCP->m_MapGuild.unlock();
	}
	else
	{
		CIMParty *ptmParty = this->GetIMParty();
		if (ptmParty == NULL)
		{
			// error
			SendErrorMessage(T_IC_VOIP_NtoN_INVITE_ALL, ERR_PROTOCOL_NOT_HAVE_PARTY, pRecvMsg->byIsPartyCall);
			return RES_BREAK;
		}
		
		ptmParty->LockIMParty();
		{
			if (ptmParty->GetMasterCharacterUniqueNumber() != m_character.CharacterUniqueNumber)
			{
				// error: ŔÚ˝ĹŔĚ ĆÄĆĽŔĺŔĚ ľĆ´ĎąÇ·Î ±ÇÇŃŔĚ ľřŔ˝
				ptmParty->UnlockIMParty();
				SendErrorMessage(T_IC_VOIP_NtoN_INVITE_ALL, ERR_PROTOCOL_PARTY_PERMISSION_DENIED, pRecvMsg->byIsPartyCall);
				return RES_BREAK;
			}
			
			INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_NtoN_INVITE_YOU, T_IC_VOIP_NtoN_INVITE_YOU, pSMsg, pBuffer);
			pSMsg->byIsPartyCall = pRecvMsg->byIsPartyCall;
			ptmParty->SendMsgToMembers(pBuffer, MSG_SIZE(MSG_IC_VOIP_NtoN_INVITE_YOU), this->m_character.CharacterUniqueNumber);
		}
		ptmParty->UnlockIMParty();
	}

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_VOIP_NtoN_INVITE_USER(const char* pPacket, int nLength, int &nBytesUsed)
{
	int							nRecvTypeSize = 0;
	MSG_IC_VOIP_NtoN_INVITE_USER		*pRecvMsg = NULL;
	
	nRecvTypeSize = sizeof(MSG_IC_VOIP_NtoN_INVITE_USER);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_VOIP_NtoN_INVITE_USER, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201C);
		return RES_RETURN_FALSE;
	}
	pRecvMsg = (MSG_IC_VOIP_NtoN_INVITE_USER*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;


	CIMIOCPSocket *pPeerSocket = NULL;
	///////////////////////////////////////////////////////////////////////////////
	// 1. ±ćµĺĹëČ­
	// 2. Ćí´ëĹëČ­
	if(FALSE == pRecvMsg->byIsPartyCall)
	{
		if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
		{
			SendErrorMessage(T_IC_VOIP_NtoN_INVITE_USER, ERR_PROTOCOL_NOT_HAVE_GUILD, pRecvMsg->byIsPartyCall);
			return RES_BREAK;
		}

		ms_pIMIOCP->m_MapGuild.lock();
		{
			CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
			if(NULL == ptmGuild)
			{
				ms_pIMIOCP->m_MapGuild.unlock();
				SendErrorMessage(T_IC_VOIP_NtoN_INVITE_USER, ERR_PROTOCOL_NOT_HAVE_GUILD);
				return RES_BREAK;
			}
			
			if (m_character.CharacterUniqueNumber != ptmGuild->m_CommanderUniqueNumber)
			{
				ms_pIMIOCP->m_MapGuild.unlock();
				SendErrorMessage(T_IC_VOIP_NtoN_INVITE_USER, ERR_PROTOCOL_GUILD_PERMISSION_DENIED, pRecvMsg->byIsPartyCall);
				return RES_BREAK;
			}
			
			CGuildMember *pMember = NULL;
			BOOL bReted = ptmGuild->GetGuildMember(pRecvMsg->nCharacterUniqueNumber, pMember);
			if (!bReted)
			{
				ms_pIMIOCP->m_MapGuild.unlock();
				SendErrorMessage(T_IC_GUILD_LEAVE, ERR_PROTOCOL_NO_SUCH_GUILD_MEMBER);
				return RES_BREAK;
			}

			if(FALSE == pMember->m_bIsOnline
				|| NULL == pMember->m_pIMIOCPSocket)
			{
				ms_pIMIOCP->m_MapGuild.unlock();
				return RES_BREAK;
			}
			pPeerSocket = pMember->m_pIMIOCPSocket;
		}
		ms_pIMIOCP->m_MapGuild.unlock();

	}
	else
	{
		CIMParty *ptmParty = this->GetIMParty();
		if (ptmParty == NULL)
		{
			// error
			SendErrorMessage(T_IC_VOIP_NtoN_INVITE_USER, ERR_PROTOCOL_NOT_HAVE_PARTY, pRecvMsg->byIsPartyCall);
			return RES_BREAK;
		}
		
		ptmParty->LockIMParty();
		{
			if (ptmParty->GetMasterCharacterUniqueNumber() != m_character.CharacterUniqueNumber)
			{
				// error: ŔÚ˝ĹŔĚ ĆÄĆĽŔĺŔĚ ľĆ´ĎąÇ·Î ±ÇÇŃŔĚ ľřŔ˝
				ptmParty->UnlockIMParty();
				SendErrorMessage(T_IC_VOIP_NtoN_INVITE_USER, ERR_PROTOCOL_PARTY_PERMISSION_DENIED, pRecvMsg->byIsPartyCall);
				return RES_BREAK;
			}
			
			IMPartyMember *pIMMember = ptmParty->GetMemberPtrByCharacterUniqueNumber(pRecvMsg->nCharacterUniqueNumber);
			if(NULL == pIMMember)
			{
				// error: »ó´ëąćŔĚ Partyżˇ Á¸ŔçÇĎÁö ľĘŔ˝, żˇ·Ż ŔüĽŰÇĎÁö ľĘŔ˝
				ptmParty->UnlockIMParty();
				return RES_BREAK;
			}
			pPeerSocket = pIMMember->PM_pIMIOCPSocket;
		}
		ptmParty->UnlockIMParty();

		if(NULL == pPeerSocket || FALSE == pPeerSocket->IsUsing())
		{
			return RES_BREAK;
		}
	}

	if(pPeerSocket->IsUsing())
	{
		INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_NtoN_INVITE_YOU, T_IC_VOIP_NtoN_INVITE_YOU, pSMsg, pSBuffer);
		pSMsg->byIsPartyCall = pRecvMsg->byIsPartyCall;
		pPeerSocket->SendAddData(pSBuffer, MSG_SIZE(MSG_IC_VOIP_NtoN_INVITE_YOU));
	}
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IC_VOIP_NtoN_GOOUT_ALL(const char* pPacket, int nLength, int &nBytesUsed)
{
	int							nRecvTypeSize = 0;
	MSG_IC_VOIP_NtoN_GOOUT_ALL		*pRecvMsg = NULL;
	
	nRecvTypeSize = sizeof(MSG_IC_VOIP_NtoN_GOOUT_ALL);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_VOIP_NtoN_GOOUT_ALL, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201C);
		return RES_RETURN_FALSE;
	}
	pRecvMsg = (MSG_IC_VOIP_NtoN_GOOUT_ALL*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;
	
	///////////////////////////////////////////////////////////////////////////////
	// 1. ±ćµĺĹëČ­
	// 2. Ćí´ëĹëČ­
	if(FALSE == pRecvMsg->byIsPartyCall)
	{
		if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
		{
			SendErrorMessage(T_IC_VOIP_NtoN_GOOUT_ALL, ERR_PROTOCOL_NOT_HAVE_GUILD, pRecvMsg->byIsPartyCall);
			return RES_BREAK;
		}

		ms_pIMIOCP->m_MapGuild.lock();
		{
			CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
			if(NULL == ptmGuild)
			{
				ms_pIMIOCP->m_MapGuild.unlock();
				SendErrorMessage(T_IC_VOIP_NtoN_GOOUT_ALL, ERR_PROTOCOL_NOT_HAVE_GUILD);
				return RES_BREAK;
			}
			
			if (m_character.CharacterUniqueNumber != ptmGuild->m_CommanderUniqueNumber)
			{
				ms_pIMIOCP->m_MapGuild.unlock();				
				SendErrorMessage(T_IC_VOIP_NtoN_GOOUT_ALL, ERR_PROTOCOL_GUILD_PERMISSION_DENIED, pRecvMsg->byIsPartyCall);
				return RES_BREAK;
			}
			
			INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_NtoN_GOOUT_ALL, T_IC_VOIP_NtoN_GOOUT_ALL, pSMsg, pBuffer);
			pSMsg->byIsPartyCall = pRecvMsg->byIsPartyCall;
			ptmGuild->SendMsgToGuildMembers(pBuffer, MSG_SIZE(MSG_IC_VOIP_NtoN_GOOUT_ALL), this->m_character.CharacterUniqueNumber);
		}
		ms_pIMIOCP->m_MapGuild.unlock();		
	}
	else
	{
		CIMParty *ptmParty = this->GetIMParty();
		if (ptmParty == NULL)
		{
			// error
			SendErrorMessage(T_IC_VOIP_NtoN_GOOUT_ALL, ERR_PROTOCOL_NOT_HAVE_PARTY, pRecvMsg->byIsPartyCall);
			return RES_BREAK;
		}
		
		ptmParty->LockIMParty();
		{
			if (ptmParty->GetMasterCharacterUniqueNumber() != m_character.CharacterUniqueNumber)
			{
				// error: ŔÚ˝ĹŔĚ ĆÄĆĽŔĺŔĚ ľĆ´ĎąÇ·Î ±ÇÇŃŔĚ ľřŔ˝
				ptmParty->UnlockIMParty();
				SendErrorMessage(T_IC_VOIP_NtoN_GOOUT_ALL, ERR_PROTOCOL_PARTY_PERMISSION_DENIED, pRecvMsg->byIsPartyCall);
				return RES_BREAK;
			}
			
			INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_NtoN_GOOUT_ALL, T_IC_VOIP_NtoN_GOOUT_ALL, pSMsg, pBuffer);
			pSMsg->byIsPartyCall = pRecvMsg->byIsPartyCall;
			ptmParty->SendMsgToMembers(pBuffer, MSG_SIZE(MSG_IC_VOIP_NtoN_GOOUT_ALL), this->m_character.CharacterUniqueNumber);
		}
		ptmParty->UnlockIMParty();
	}

	return RES_RETURN_TRUE;
}


ProcessResult CIMIOCPSocket::Process_IC_VOIP_ERROR(const char* pPacket, int nLength, int &nBytesUsed)
{
	int							nRecvTypeSize = 0;
	MSG_IC_VOIP_ERROR			*pRecvMsg = NULL;
	
	nRecvTypeSize = sizeof(MSG_IC_VOIP_ERROR);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_IC_VOIP_ERROR, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201C);
		return RES_RETURN_FALSE;
	}
	pRecvMsg = (MSG_IC_VOIP_ERROR*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;
	
	CIMIOCPSocket *pSend2Sock = ms_pIMIOCP->GetIMIOCPSocketByAccountUID(pRecvMsg->nSend2AccountUniqueNumber);
	if (NULL == pSend2Sock)
	{
		// Error¸¦ Client·Î ŔüĽŰÇŇ ÇĘżä ľřŔ˝
		return RES_BREAK;
	}
	
	INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_ERROR, T_IC_VOIP_ERROR, pSendMsg, tmSendBuf);
	*pSendMsg = *pRecvMsg;
	pSend2Sock->SendAddData(tmSendBuf, MSG_SIZE(MSG_IC_VOIP_ERROR));
	return RES_RETURN_TRUE;
}


ProcessResult CIMIOCPSocket::Process_IM_PRINT_DEBUG_MSG(const char* pPacket, int nLength, int &nBytesUsed)
{
	int						nRecvTypeSize	= 0;
	MSG_FM_PRINT_DEBUG_MSG	*pRecvPrintDbg = NULL;

	nRecvTypeSize = sizeof(MSG_FM_PRINT_DEBUG_MSG);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		// Protocl Error Ăł¸®
		// - Client·Î şÎĹÍ ąŢŔş Data Size°ˇ Field Typeżˇ µű¸Ą Data Sizeş¸´Ů ŔŰ´Ů
		// Error Code : ERR_PROTOCOL_INVALID_FIELD_DATA
		SendErrorMessage(T_IM_PRINT_DEBUG_MSG, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x14056);
		return RES_RETURN_FALSE;
	}
	pRecvPrintDbg = (MSG_FM_PRINT_DEBUG_MSG*)(pPacket+nBytesUsed);
	nBytesUsed += nRecvTypeSize;

	// ÇĘµĺ Ľ­ąöµéżˇ°Ô ŔüĽŰ
	MessageType_t msgType = T_FI_ADMIN_PRINT_DEBUG_MSG;
	ms_pIMIOCP->SendMessageToFieldServer((BYTE*)&msgType, SIZE_FIELD_TYPE_HEADER);
// 2004-10-21 by cmkwon, Ŕ§ŔÇ ÇÔĽö·Î ´ëĂĽ
//	ms_pIMIOCP->m_MapFieldServer.lock();
//	mt_map<SERVER_ID, FieldServerInfo*>::iterator itrServer = ms_pIMIOCP->m_MapFieldServer.begin();
//	while(itrServer != ms_pIMIOCP->m_MapFieldServer.end())
//	{
//		FieldServerInfo *pServerInfo = itrServer->second;
//		MessageType_t pMsgType = T_FI_ADMIN_PRINT_DEBUG_MSG;
//		pServerInfo->pSocket->SendAddData((BYTE*)&pMsgType, SIZE_FIELD_TYPE_HEADER);
//		itrServer++;
//	}
//	ms_pIMIOCP->m_MapFieldServer.unlock();

	DBGOUT("=== START - DEBUG MESSAGE REQUESTED BY MONITOR SERVER ========\n");
	DBGOUT("  Int Param1   : %d \n", pRecvPrintDbg->IntParam1);
	DBGOUT("  Int Param2   : %d \n", pRecvPrintDbg->IntParam2);
	DBGOUT("  Float Param1 : %d \n", (int)pRecvPrintDbg->FloatParam1);
	DBGOUT("  Float Param2 : %d \n", (int)pRecvPrintDbg->FloatParam2);
	DBGOUT("  String Param : \"%s\" \n", pRecvPrintDbg->StringParam);

	//char msg[200000];
	char *msg = NULL;
// 2008-08-29 by cmkwon, ŔüĽŰ ÇĘżä ľřŔ˝
//	PRINT_DEBUG_MESSAGE(msg);


	DBGOUT("=== END --- DEBUG MESSAGE REQUESTED BY MONITOR SERVER ========\n");

	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_IM_GET_TOTAL_USER_COUNTS(const char* pPacket, int nLength, int &nBytesUsed)
{
	INIT_MSG_WITH_BUFFER(MSG_IM_GET_TOTAL_USER_COUNTS_ACK, T_IM_GET_TOTAL_USER_COUNTS_ACK, pSeAck, SendBuf);
	pSeAck->AccumulatedTotalUserCounts	= ms_pIMIOCP->m_uiIMAccumulatedTotalUserCounts;
	pSeAck->CurrentTotalUserCounts		= ms_pIMIOCP->m_MapAccountUniqueNumber.size();
	pSeAck->MaxTotalUserCounts			= ms_pIMIOCP->m_uiIMMaxTotalUserCounts;
	
	ms_pIMIOCP->SendMessageToMonitor(SendBuf, MSG_SIZE(MSG_IM_GET_TOTAL_USER_COUNTS_ACK));
	return RES_RETURN_TRUE;
}

ProcessResult CIMIOCPSocket::Process_FI_GET_FIELD_USER_COUNTS_ACK(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_GET_FIELD_USER_COUNTS_ACK, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	int									nRecvTypeSize = 0;
	MSG_FI_GET_FIELD_USER_COUNTS_ACK	*pMsgGetCountsAck = NULL;

	nRecvTypeSize = sizeof(MSG_FI_GET_FIELD_USER_COUNTS_ACK);
	if(nLength - nBytesUsed < nRecvTypeSize)
	{
		SendErrorMessage(T_FI_GET_FIELD_USER_COUNTS_ACK, ERR_PROTOCOL_INVALID_FIELD_DATA);
		Close(0x1201D);
		return RES_RETURN_FALSE;
	}
	pMsgGetCountsAck = (MSG_FI_GET_FIELD_USER_COUNTS_ACK*)(pPacket + nBytesUsed);
	nBytesUsed += nRecvTypeSize;

//	FieldServerInfo *pFSInfo = ms_pIMIOCP->m_MapFieldServer.findLock(pMsgGetCountsAck->FieldServerID);
//	if (pFSInfo == NULL)
//	{
//		DBGOUT("[Error]ERR_PROTOCOL_NO_SUCH_FIELD_SERVER: %s\n", pMsgGetCountsAck->FieldServerID.GetString(string()));
//		SendErrorMessage(T_FI_GET_FIELD_USER_COUNTS_ACK, ERR_PROTOCOL_NO_SUCH_FIELD_SERVER, pMsgGetCountsAck->FieldServerID.port, 0, pMsgGetCountsAck->FieldServerID.IP);
//		return RES_BREAK;
//	}
//	pFSInfo->AccumulatedFieldUserCounts		= pMsgGetCountsAck->AccumulatedFieldUserCounts;
//	pFSInfo->CurrentFieldUserCounts			= pMsgGetCountsAck->CurrentFieldUserCounts;
//	pFSInfo->MaxFieldUserCounts				= pMsgGetCountsAck->MaxFieldUserCounts;
//	pFSInfo->AccumulatedFieldMonsterCounts	= pMsgGetCountsAck->AccumulatedFieldMonsterCounts;
//	pFSInfo->CurrentFieldMonsterCounts		= pMsgGetCountsAck->CurrentFieldMonsterCounts;
//
//	BYTE	SendBuf[SIZE_MAX_PACKET];
//	MSG_FIELDSERVER_USERCOUNTS *pFUserCounts = NULL;
//
//	INIT_MSG(MSG_IM_GET_TOTAL_USER_COUNTS_ACK, T_IM_GET_TOTAL_USER_COUNTS_ACK, pSendTotalCounts, SendBuf);
//	memset(pSendTotalCounts, 0x00, sizeof(MSG_IM_GET_TOTAL_USER_COUNTS_ACK));
//	pFUserCounts = (MSG_FIELDSERVER_USERCOUNTS*)(SendBuf + MSG_SIZE(MSG_IM_GET_TOTAL_USER_COUNTS_ACK));
//
//	ms_pIMIOCP->m_MapFieldServer.lock();
//	mt_map<SERVER_ID, FieldServerInfo*>::iterator  itr(ms_pIMIOCP->m_MapFieldServer.begin());
//	while(itr != ms_pIMIOCP->m_MapFieldServer.end())
//	{
//		strncpy(pFUserCounts->FieldIP, itr->second->serverID.IP, SIZE_MAX_IPADDRESS);
//		pFUserCounts->FieldListenPort = itr->second->serverID.port;
//		pFUserCounts->AccumulatedFieldUserCounts = itr->second->AccumulatedFieldUserCounts;
//		pFUserCounts->CurrentFieldUserCounts = itr->second->CurrentFieldUserCounts;
//		pFUserCounts->MaxFieldUserCounts = itr->second->MaxFieldUserCounts;
//		pFUserCounts->AccumulatedFieldMonsterCounts = itr->second->AccumulatedFieldMonsterCounts;
//		pFUserCounts->CurrentFieldMonsterCounts = itr->second->CurrentFieldMonsterCounts;
//
//		pSendTotalCounts->AccumulatedTotalUserCounts += pFUserCounts->AccumulatedFieldUserCounts;
//		pSendTotalCounts->CurrentTotalUserCounts += pFUserCounts->CurrentFieldUserCounts;
//		pSendTotalCounts->MaxTotalUserCounts += pFUserCounts->MaxFieldUserCounts;
//		pSendTotalCounts->AccumulatedTotalMonsterCounts += pFUserCounts->AccumulatedFieldMonsterCounts;
//		pSendTotalCounts->CurrentTotalMonsterCounts += pFUserCounts->CurrentFieldMonsterCounts;
//
//		pSendTotalCounts->FieldServerCounts++;
//		pFUserCounts++;
//		itr++;
//	}
//	ms_pIMIOCP->m_MapFieldServer.unlock();
//
//	ms_pIMIOCP->SendIMToMonitor(SendBuf, MSG_SIZE(MSG_IM_GET_TOTAL_USER_COUNTS_ACK) + sizeof(MSG_FIELDSERVER_USERCOUNTS) * pSendTotalCounts->FieldServerCounts);
	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			ProcessResult CIMIOCPSocket::Process_FI_INFO_DECLARATION_MSWAR_SET_OK(const char* pPacket, int nLength, int &nBytesUsed)
/// \brief		Ľ±Ŕü Ć÷°í - Ľ±ŔüĆ÷°í ˝Ă°Ł ą× Ć÷±â ĽłÁ¤ F->I
/// \author		dhjin
/// \date		2009-01-20 ~ 2009-01-20
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
ProcessResult CIMIOCPSocket::Process_FI_INFO_DECLARATION_MSWAR_SET_OK(const char* pPacket, int nLength, int &nBytesUsed)
{
	//10-08-2015 by inetpub - filtering BIN<=>BIN allowed ip
	if (FALSE == g_pIMGlobal->CheckAllowedToolIP(this->GetPeerIP()))
	{
		char szTempLog[1024];
		sprintf(szTempLog, "[PacketFilter] Hack by sending BIN<=>BIN packet detected! Hacker was trying to use :: Process_FI_INFO_DECLARATION_MSWAR_SET_OK, Hacker IP :: [%s]\r\n", this->GetPeerIP());
		g_pIMGlobal->WriteSystemLog(szTempLog);
		return RES_BREAK;
	}
	//end of 10-08-2015 by inetpub

	DECLARE_MESSAGE_AND_CHECK_SIZE(pPacket, nLength, nBytesUsed, T_FI_INFO_DECLARATION_MSWAR_SET_OK,
									MSG_FI_INFO_DECLARATION_MSWAR_SET_OK, pMsg);
	
	INIT_MSG_WITH_BUFFER(MSG_IC_INFO_DECLARATION_MSWAR_SET_OK, T_IC_INFO_DECLARATION_MSWAR_SET_OK, pSendMsg, SendBuf);
	pSendMsg->Influence			= pMsg->Influence;
	pSendMsg->MSWarStartTime	= pMsg->MSWarStartTime;
	pSendMsg->GiveUp			= pMsg->GiveUp;
	pSendMsg->SelectCount		= pMsg->SelectCount;
	this->ms_pIMIOCP->SendMessageToAllClients(SendBuf, MSG_SIZE(MSG_IC_INFO_DECLARATION_MSWAR_SET_OK));

	return RES_RETURN_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CIMIOCPSocket::GameStartRoutineIM(MSG_FI_CONNECT_NOTIFY_GAMESTART *i_FINotifyGameStart, CIMIOCPSocket *i_pF2IISock)
/// \brief		
/// \author		cmkwon
/// \date		2006-05-09 ~ 2006-05-09
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CIMIOCPSocket::GameStartRoutineIM(MSG_FI_CONNECT_NOTIFY_GAMESTART *i_FINotifyGameStart, CIMIOCPSocket *i_pF2IISock)
{
	if(this->GetCharacter()->CharacterUniqueNumber != i_FINotifyGameStart->CharacterUniqueNumber)
	{// 2006-05-09 by cmkwon
		return;
	}

	if(COMPARE_RACE(this->m_character.Race, RACE_OPERATION|RACE_GAMEMASTER))
	{// 2006-05-08 by cmkwon,

		///////////////////////////////////////////////////////////////////////////////
		// 2006-11-14 by cmkwon, ±âş»ŔűŔ¸·Î /±Ó¸» Ŕ» şńČ°ĽşČ­ ÇŃ´Ů
// 2008-05-15 by dhjin, EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć
//		CLEAR_CHATTYPE_BIT(m_enableChatFlag8, CHAT_TYPE_PTOP);
		CLEAR_CHATTYPE_BIT(m_enableChatFlag16, CHAT_TYPE_PTOP);

		///////////////////////////////////////////////////////////////////////////////
		// 2006-05-08 by cmkwon
		ms_pIMIOCP->AddToAdminCharacter(this->GetCharacter()->CharacterUniqueNumber);

		///////////////////////////////////////////////////////////////////////////////
		// 2007-11-19 by cmkwon, ÁřÁ¤˝Ă˝şĹŰ ľ÷µĄŔĚĆ® - 
		if(ms_pIMIOCP->IsOnCallGMSystem())
		{
			this->SendString128(STRING_128_ADMIN_CMD, STRMSG_071120_0004
				, ms_pIMIOCP->m_atCallGMStartTime.GetDateTimeString(STRNBUF(SIZE_MAX_SQL_DATETIME_STRING))
				, ms_pIMIOCP->m_atCallGMEndTime.GetDateTimeString(STRNBUF(SIZE_MAX_SQL_DATETIME_STRING)));
		}
		
		///////////////////////////////////////////////////////////////////////////////
		// 2006-05-08 by cmkwon
		int nCallGMCnt = ms_pIMIOCP->m_mtvectSCallGMList.size();
		if(0 < nCallGMCnt)
		{
			INIT_MSG_WITH_BUFFER(MSG_IC_ADMIN_CALLGM_INFO_OK, T_IC_ADMIN_CALLGM_INFO_OK, pCallGMInfo, SendBuf);
			pCallGMInfo->nCallGMTotalCount	= nCallGMCnt;
			this->SendAddData(SendBuf, MSG_SIZE(MSG_IC_ADMIN_CALLGM_INFO_OK));
		}
	}

	this->m_bGameEndRoutineFlag			= TRUE;
	this->m_bFirstTimeLoadFriendList	= TRUE;		// 2009-03-05 by cmkwon, ĹëÇŐ ľĆ·ąłŞ ŔĚżë˝Ă ÄŁ±¸ ¸®˝şĆ® ş¸łĘ˝ş °ćÇčÄˇ °ü·Ă ąö±× ĽöÁ¤ - 

	///////////////////////////////////////////////////////////////////////////
	// MAP_CHANNEL_INDEX ÇŇ´ç	
	this->m_character.MapChannelIndex	= i_FINotifyGameStart->MapChannelIndex;

	///////////////////////////////////////////////////////////////////////////
	// ±ćµĺ Á¤ş¸ ·Îµů	
	if(IS_VALID_UNIQUE_NUMBER(this->m_character.GuildUniqueNumber))
	{
		ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildLoadGuild, this, NULL, NULL, i_FINotifyGameStart->CharacterUniqueNumber);
// 2007-07-30 by cmkwon, SCAdminToolżˇĽ­ ż©´Ü¸í şŻ°ć ±â´É Ăß°ˇ - ż©´Ü Á¤ş¸ ż©´Üżř Á˘ĽÓ˝Ă¸¶´Ů ·ÎµůÇĎµµ·Ď Ŕ§żÍ °°ŔĚ ĽöÁ¤
//		///////////////////////////////////////////////////////////////////////////////
//		// 2007-02-06 by cmkwon, ±âÁ¸żˇ ÇĎłŞŔÇ DBżˇ ż©·Ż°łŔÇ Ľ­ąö°ˇ Á˘ĽÓÇĎ´Â ˝Ă˝şĹŰŔş ĽĽ·ÂŔüŔ¸·Î şŇ°ˇ´ÉÇĎąÇ·Î ľĆ·ˇżÍ °°ŔĚ ż©´ÜÁ¤ş¸´Â ÇŃąř¸¸ ·ÎµůÇĎµµ·Ď ĽöÁ¤ÇŃ´Ů
//		mt_auto_lock mtGA(&ms_pIMIOCP->m_MapGuild);
//
//		CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_character.GuildUniqueNumber);
//		if(NULL == ptmGuild)
//		{// 2007-02-06 by cmkwon, ·ÎµůµČ ż©´ÜÁ¤ş¸°ˇ ľř´Ů
//			ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_GuildLoadGuild, this, NULL, NULL, i_FINotifyGameStart->CharacterUniqueNumber);
//		}
//		else
//		{// 2007-02-06 by cmkwon, ż©´ÜÁ¤ş¸°ˇ ŔĚąĚ ·Îµů µÇľîŔÖ´Ů
//			CGuildMember *pGMember = NULL;
//			ptmGuild->GetGuildMember(m_character.CharacterUniqueNumber, pGMember);
//			if(pGMember)
//			{
//				ptmGuild->SetOnlineGuildMember(this);
//
//				this->SetGuildUniqueNumber(m_character.GuildUniqueNumber);
//
//				INIT_MSG_WITH_BUFFER(MSG_FI_GUILD_RANK, T_FI_GUILD_RANK_OK, pGRank, SendBuf);
//				pGRank->charUID		= m_character.CharacterUniqueNumber;
//				pGRank->GuildRank	= pGMember->m_Rank;
//				this->SendMsgToCurrentFieldServerSocket(SendBuf, MSG_SIZE(MSG_IC_GUILD_SET_RANK_OK));
//
//				INIT_MSG(MSG_IC_GUILD_LOADING_GUILD_DONE, T_IC_GUILD_LOADING_GUILD_DONE, pGLoading, SendBuf);
//				pGLoading->GuildUniqueNumber = m_character.GuildUniqueNumber;
//				this->SendAddData(SendBuf, MSG_SIZE(MSG_IC_GUILD_LOADING_GUILD_DONE));
//			}
//		}
	}	

	///////////////////////////////////////////////////////////////////////////
	// ŔĚŔü ĆÄĆĽ °Ë»ç
	///////////////////////////////////////////////////////////////////////////
	if (this->m_character.LastPartyID != 0)
	{
#ifdef _DEBUG
		this->SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0005);
#endif
		// ĆÄĆĽ °Ë»ö
		CIMParty *pParty = ms_pIMIOCP->FindParty(this->m_character.LastPartyID);
		if (pParty == NULL)
		{
			// ĆÄĆĽ°ˇ ŔĚąĚ ÇŘ»ęµČ °ćżě
			this->SendErrorMessage(T_FI_CONNECT_NOTIFY_GAMESTART, ERR_PROTOCOL_NO_SUCH_PARTY, this->m_character.LastPartyID);

			this->SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0006);

			// Update DB
			QPARAM_SAVELASTPARTYID *pQSavePartyID = new QPARAM_SAVELASTPARTYID;
			pQSavePartyID->CharacterUniqueNumber = this->m_character.CharacterUniqueNumber;
			pQSavePartyID->LastPartyID = 0;
			ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_SaveLastPartyID, this, pQSavePartyID);

			// Update CHARACTER
			this->m_character.LastPartyID = 0;
			return;
		}
		
		pParty->LockIMParty();
		{
			IMPartyMember *pMember = pParty->GetMemberPtrByCharacterUniqueNumber(i_FINotifyGameStart->CharacterUniqueNumber);
			if (pMember == NULL)
			{
				// ĆÄĆĽżřŔĚ ľř´Â °ćżě
				pParty->UnlockIMParty();
				i_pF2IISock->SendErrorMessage(T_FI_CONNECT_NOTIFY_GAMESTART, ERR_PROTOCOL_NO_SUCH_PARTY_MEMBER, i_FINotifyGameStart->CharacterUniqueNumber);
				return;
			}

			// »çżëŔÚżˇ°Ô ĆÄĆĽ Á¤ş¸ ÇŇ´ç
			this->SetIMParty(pParty);

			// validate party member
			pMember->SetOnlineMember(this);

			// ŔÚ±â ŔÚ˝Ĺżˇ°Ô ş¸łż
			INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_PUT_LAST_PARTY_INFO, T_IC_PARTY_PUT_LAST_PARTY_INFO, pLastPartyInfo, pLastPartyInfoBuf);
			pLastPartyInfo->PartyID = pParty->m_PartyID;
			this->SendAddData(pLastPartyInfoBuf, MSG_SIZE(MSG_IC_PARTY_PUT_LAST_PARTY_INFO));

			this->SendString128(STRING_128_USER_NOTICE, STRMSG_S_I2NOTIFY_0007);

			// 2008-06-10 by dhjin, EP3 Ćí´ë ĽöÁ¤ - Ćí´ë Á¤ş¸ ŔüĽŰ
			INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_INFO, T_IC_PARTY_INFO, pSendMsg, pSendBuf);
			pSendMsg->PartyInfo = pParty->m_SPartyInfo;
			this->SendAddData(pSendBuf, MSG_SIZE(MSG_IC_PARTY_INFO));
		}
		pParty->UnlockIMParty();
	}
}

///////////////////////////////////////////////////////////////////////////////
// return ÇŇ ¶§ unlock ĽöÇŕÇĎ±â! 20031103, kelovon
///////////////////////////////////////////////////////////////////////////////
void CIMIOCPSocket::GameEndRoutineIM(void)
{
	this->m_mtCritSecForGameEndRoutine.Enter();
	{
		if (FALSE == m_bGameEndRoutineFlag
			|| 0 == m_character.ClientIndex
			|| 0 == m_character.CharacterUniqueNumber
			|| 0 == m_character.AccountUniqueNumber
			|| ST_INVALID_TYPE == m_PeerSocketType)
		{
			DBGOUT("  ClientIndex(%d)'s CIMIOCPSocket::GameEndRoutineIM()... ALREADY PROCESSED..\r\n", this->GetClientArrayIndex());

			this->m_mtCritSecForGameEndRoutine.Leave();
			return;
		}
		m_bGameEndRoutineFlag = FALSE;
	}
	this->m_mtCritSecForGameEndRoutine.Leave();
		
#ifdef _DEBUG
	DBGOUT("  %s's CIMIOCPSocket::GameEndRoutineIM()...\r\n", GetCharacterString(&m_character, string()));
#endif

	//////////////////////////////////////////////////////////////////////////
	// 2008-06-18 by dhjin, EP3 Ă¤ĆĂąć -
	if(this->m_bChatRoomUseing)
	{
		mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));
		mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
		for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
		{
			if(this->m_nChatRoomNum == (*itr)->GetChatRoomNum())
			{
				if(this->m_character.CharacterUniqueNumber == (*itr)->GetChatRoomMasterCharacterUID()
					&& 1 < (*itr)->GetChatRoomMemberCount())
				{
					INIT_MSG_WITH_BUFFER(MSG_IC_CHATROOM_CHANGE_MASTER_OK, T_IC_CHATROOM_CHANGE_MASTER_OK, msg, SendBuf);
					STRNCPY_MEMSET(msg->CharacterName, (*itr)->ChangeChatRoomMaster(), SIZE_MAX_CHARACTER_NAME);
					if(NULL == msg->CharacterName)
					{
						break;
					}
					msg->NewCharacterUID = (*itr)->GetChatRoomMasterCharacterUID();
					(*itr)->SendMessagetoChatRoomMember(SendBuf, MSG_SIZE(MSG_IC_CHATROOM_CHANGE_MASTER_OK));
				}

				INIT_MSG_WITH_BUFFER(MSG_IC_CHATROOM_LEAVE_OK, T_IC_CHATROOM_LEAVE_OK, msg, SendBuf);
				STRNCPY_MEMSET(msg->LeaveCharacterName, this->m_character.CharacterName, SIZE_MAX_CHARACTER_NAME);
				msg->CharacterUID = this->m_character.CharacterUniqueNumber;
				(*itr)->SendMessagetoChatRoomMember(SendBuf, MSG_SIZE(MSG_IC_CHATROOM_LEAVE_OK));

				if((*itr)->LeaveMemberAndCheckEmpty(this->m_character.CharacterUniqueNumber))
				{
					ms_pIMIOCP->m_ChatRoomManager.SetReserveChatNum((*itr)->GetChatRoomNum());
					SAFE_DELETE(*itr);
					itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.erase(itr);
				}
				break;
			}
		}

		///////////////////////////////////////////////////////////////////////////////
		// 2008-10-22 by cmkwon, Ă¤ĆĂąć ąö±× ĽöÁ¤ - 
		this->m_bChatRoomUseing		= FALSE;
		this->m_nChatRoomNum		= 0;
	}

	CIMParty *ptmParty = this->GetIMParty();
	if (ptmParty)
	{
		ptmParty->LockIMParty();
		PartyID_t partyID = ptmParty->m_PartyID;

		//////////////////////////////////////////////////////////////////////////
		// 2008-02-28 by dhjin, ľĆ·ąłŞ ĹëÇŐ - ľĆ·ąłŞ Ľ­ąöżˇĽ­ Áľ·á´Â ąŮ·Î »čÁ¦·Î Ăł¸®ÇŘľßÇŃ´Ů.~!!!
		if(ms_pIMIOCP->m_bArenaServerCheck)
		{
			BOOL bReted = ProcessPartyLeaveNoLock(ptmParty);
			ptmParty->UnlockIMParty();

			if(FALSE == bReted)
			{// ĆÄĆĽ¸ĘżˇĽ­ Á¦°ĹÇŘľßÇŃ´Ů

				ProcessPartyDismember(partyID);
			}
		}
		else
		{
			BOOL bReted = ProcessPartyGameEndNoLock(ptmParty);
			ptmParty->UnlockIMParty();

			if(FALSE == bReted)
			{// ĆÄĆĽ¸ĘżˇĽ­ Á¦°ĹÇŘľßÇŃ´Ů
				
				ProcessPartyDismember(partyID);			
			}
			else
			{
				// Update DB
				QPARAM_SAVELASTPARTYID *pQSavePartyID	= new QPARAM_SAVELASTPARTYID;
				pQSavePartyID->CharacterUniqueNumber	= m_character.CharacterUniqueNumber;
				pQSavePartyID->LastPartyID				= partyID;
				ms_pIMIOCP->m_pAtumIMDBManager->MakeAndEnqueueQuery(QT_SaveLastPartyID, this, pQSavePartyID);
			}
		}
		///////////////////////////////////////////////////////////////////////////////
		// Äł¸ŻĹÍŔÇ ĆÄĆĽ Á¤ş¸ »čÁ¦
		this->SetIMParty(NULL);
	
	}
	
	if (IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
	{
		ms_pIMIOCP->m_MapGuild.lock();
		{
			///////////////////////////////////////////////////////////////////////////////
			// ż©´ÜŔü ĂĽĹ©(żŔÇÁ¶óŔÎ ĽłÁ¤ Ŕüżˇ ĂĽĹ©ÇŘľßÇÔ)
			ms_pIMIOCP->GuildWarCheck(m_uidGuildUniqueNumber, m_character.MapChannelIndex);

			CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
			if(ptmGuild)
			{
				// żŔÇÁ¶óŔÎŔ¸·Î ĽłÁ¤
				ptmGuild->SetOfflineGuildMember(m_character.CharacterUniqueNumber);
				
				ptmGuild->SetMemberVoipType(m_character.CharacterUniqueNumber, CHECK_TYPE_VOIP_NONE);				// 2008-07-16 by dhjin, EP3 - Voip Á¤ş¸
				INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_SET_OK, T_IC_VOIP_SET_OK, pSMsg, pBuffer);
				pSMsg->CharacterUID	= m_character.CharacterUniqueNumber;
				pSMsg->VoipType		= CHECK_TYPE_VOIP_NONE;
				ptmGuild->SendMsgToGuildMembers(pBuffer, MSG_SIZE(MSG_IC_VOIP_SET_OK));

				int nOnlineMemberCount = ptmGuild->GetOnlineMemberCount(m_character.CharacterUniqueNumber);
				if (nOnlineMemberCount == 0)
				{
					ms_pIMIOCP->m_MapGuild.deleteLock(ptmGuild->m_GuildUniqueNumber);
					SAFE_DELETE(ptmGuild);
				}
				
			}
		}
		ms_pIMIOCP->m_MapGuild.unlock();
		this->SetGuildUniqueNumber(INVALID_GUILD_UID);
	}

	///////////////////////////////////////////////////////////////////////////////
	// 2006-07-19 by cmkwon, ÄŁ±¸ ¸®˝şĆ® Á¤¸®
	if(false == m_mtvectorFriendList.empty())
	{
		mt_auto_lock mtA(&m_mtvectorFriendList);
		mtvectorFRIENDINFO::iterator itr(m_mtvectorFriendList.begin());
		for(; itr != m_mtvectorFriendList.end(); itr++)
		{
			if(itr->byIsOnline)
			{
				CIMIOCPSocket *pIISockFriend = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(itr->szCharacterName);
				if(pIISockFriend
					&& pIISockFriend->IsValidCharacter()
					&& pIISockFriend->DecreaseOnlineEachOtherFriendCnts(m_character.CharacterName))
				{
					///////////////////////////////////////////////////////////////////////////////
					// 2006-07-18 by cmkwon, Ľ­·Î µî·ĎÇŃ żÂ¶óŔÎ ÄŁ±¸ Ä«żîĆ® FieldServer·Î ŔüĽŰ
					pIISockFriend->SendOnlineEachOtherFriendCntsToFieldServer();
				}
			}
		}
		m_mtvectorFriendList.clear();
		m_nEachOtherFriendCnts	= 0;		// 2006-07-19 by cmkwon
	}
	m_mtvectorRejectList.lock();
	m_mtvectorRejectList.clear();
	m_mtvectorRejectList.unlock();

	// °˘Áľ mapżˇĽ­ »čÁ¦
	if (COMPARE_RACE(m_character.Race, RACE_OPERATION|RACE_GAMEMASTER))
	{
		ms_pIMIOCP->DeleteFromAdminCharacter(m_character.CharacterUniqueNumber);
	}
	ms_pIMIOCP->m_mAccountName.deleteLock(m_character.AccountName);
	ms_pIMIOCP->m_MapAccountUniqueNumber.deleteLock(m_character.AccountUniqueNumber);
	char tmCharacterName[SIZE_MAX_CHARACTER_NAME];
	ms_pIMIOCP->m_mCharacterName.deleteLock(GGetLowerCase(tmCharacterName, m_character.CharacterName, SIZE_MAX_CHARACTER_NAME));
	ms_pIMIOCP->m_MapChactarterUniqueNumber.deleteLock(m_character.CharacterUniqueNumber);

	memset(&m_character, 0x00, sizeof(CHARACTER));

	this->SetIMParty(NULL);
	this->SetGuildUniqueNumber(INVALID_GUILD_UID);
	this->SetCurrentFieldServerSocket(NULL);
	// 2008-05-15 by dhjin, EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć
//	m_enableChatFlag8		= (BitFlag8_t)0xFF;
	m_enableChatFlag16		= (BitFlag16_t)0xFFFF;
	m_PeerSocketType		= ST_INVALID_TYPE;
	m_bSendMessgeTypeDBG	= FALSE;
	m_bGameEndRoutineFlag	= FALSE;

	m_VoipType				= CHECK_TYPE_VOIP_NONE;		// 2008-10-21 by cmkwon, ąö±× ĽöÁ¤
	return;
}


void CIMIOCPSocket::CharacterDeadRoutine(UID32_t i_uidAttackerGuildUID)
{
	if (IS_VALID_UNIQUE_NUMBER(this->m_uidGuildUniqueNumber)
		&& IS_VALID_UNIQUE_NUMBER(i_uidAttackerGuildUID))
	{
		ms_pIMIOCP->GuildWarCheck(m_uidGuildUniqueNumber, m_character.MapChannelIndex, i_uidAttackerGuildUID);
	}

	///////////////////////////////////////////////////////////////////////////////
	//
	SET_BODYCON_BIT(m_character.BodyCondition, BODYCON_DEAD_MASK);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CIMIOCPSocket::SendMsgToCurrentFieldServerSocket(BYTE *i_pData, int i_nLength)
/// \brief		
/// \author		cmkwon
/// \date		2004-10-21 ~ 2004-10-21
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CIMIOCPSocket::SendMsgToCurrentFieldServerSocket(BYTE *i_pData, int i_nLength)
{
	CIMIOCPSocket *ptmIISocket = this->m_pCurrentFieldServerSocket;
	if(NULL == ptmIISocket || FALSE == ptmIISocket->IsUsing())
	{
		return FALSE;
	}

	return ptmIISocket->SendAddData(i_pData, i_nLength);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CIMIOCPSocket::SendOnlineEachOtherFriendCntsToFieldServer(void)
/// \brief		
/// \author		cmkwon
/// \date		2006-07-19 ~ 2006-07-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CIMIOCPSocket::SendOnlineEachOtherFriendCntsToFieldServer(void)
{	
	///////////////////////////////////////////////////////////////////////////////
	// 2006-07-18 by cmkwon, Ľ­·Î µî·ĎÇŃ żÂ¶óŔÎ ÄŁ±¸ Ä«żîĆ® FieldServer·Î ŔüĽŰ
	INIT_MSG_WITH_BUFFER(MSG_FI_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT_OK, T_FI_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT_OK, pSFriendCnts, SendBuf);
	pSFriendCnts->CharacterUniqueNumber	= this->GetCharacter()->CharacterUniqueNumber;
	pSFriendCnts->OnlineFriendCnts		= this->m_nEachOtherFriendCnts;
	ms_pIMIOCP->SendMessageToFieldServer(SendBuf, MSG_SIZE(MSG_FI_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT_OK));

	//////////////////////////////////////////////////////////////////////////
	// 2008-07-11 by dhjin, EP3 ÄŁ±¸¸ń·Ď -
	INIT_MSG_WITH_BUFFER(MSG_IC_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT, T_IC_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT, pSICFriendCnts, ICSendBuf);
	pSICFriendCnts->OnlineFriendCnts		= this->m_nEachOtherFriendCnts;
	this->SendAddData(ICSendBuf, MSG_SIZE(MSG_IC_CHAT_ONLINE_EACHOTHER_FRIEND_COUNT));
}

void CIMIOCPSocket::PRINT_DEBUG_MESSAGE(char*& msg)
{
	char szTime[128];
	time_t ltime;
	struct tm *today = NULL;
	time(&ltime);
	today = localtime(&ltime);
	strftime(szTime, 128, "[%m-%d %H:%M:%S]", today );

	PRINT_DEBUG_MESSAGE_LOCK.lock();
	///////////////////////////////////////////////////////////////////////////
	// START - DEBUG MESSAGE
	///////////////////////////////////////////////////////////////////////////

	// ŔÚ±â ŔÚ˝ĹŔÇ Á¤ş¸
	SendString128(STRING_128_ADMIN_CMD, "IM - AUN(%d) CUN(%d) CI(%d) MAP(%s)",
		m_character.AccountUniqueNumber, m_character.CharacterUniqueNumber,
		m_character.ClientIndex, GET_MAP_STRING(m_character.MapChannelIndex));

// check: DBGOUTżˇ ą®Á¦ ąß»ýÇÔ, Č®ŔÎ ÇĘżäÇÔ, 20041101, kelovon
//	DBGOUT2(DO2_ALL, "\n[Party Info] - %s\n", szTime);
//
//	ms_pIMIOCP->m_MapParties.lock();
//	mt_map<PartyID_t, CIMParty*>::iterator itr = ms_pIMIOCP->m_MapParties.begin();
//	while(itr != ms_pIMIOCP->m_MapParties.end())
//	{
//		CIMParty *pParty = itr->second;
//		DBGOUT2(DO2_ALL, "PARTY Master: %10s(%d, %d) - key: %d, MemberCounts: %d/%d\n"
//			, ms_pIMIOCP->GetCharacterByArrayIndex(pParty->GetMasterClientIndex())->CharacterName
//			, pParty->GetMasterCharacterUniqueNumber()
//			, pParty->GetMasterClientIndex()
//			, HIDWORD(pParty->m_PartyID)
//			, pParty->GetMemberCount(TRUE)
//			, pParty->GetMemberCount(FALSE)
//		);	// check: DBGOUTżˇ ą®Á¦ ąß»ýÇÔ, Č®ŔÎ ÇĘżäÇÔ, 20041101, kelovon
//
//		pParty->m_ListPartyMember.lock();
//		mt_list<IMPartyMember>::iterator itr2 = pParty->m_ListPartyMember.begin();
//		while (itr2 != pParty->m_ListPartyMember.end())
//		{			
//			if (FALSE == itr2->IsValidMember())
//			{
//				DBGOUT2(DO2_ALL, "      Member: %10s(%6d, (N/A))\n"
//					, itr2->PM_CharacterName
//					, itr2->PM_CharacterUniqueNumber
//				);
//			}
//			else
//			{
//				DBGOUT2(DO2_ALL, "      Member: %10s(%6d, %4d)\n"
//					, itr2->PM_CharacterName
//					, itr2->PM_CharacterUniqueNumber
//					, itr2->PM_pIMIOCPSocket->GetClientArrayIndex()
//				);
//			}
//
//			itr2++;
//		}
//		pParty->m_ListPartyMember.unlock();
//
//		itr++;
//	}
//	ms_pIMIOCP->m_MapParties.unlock();

	///////////////////////////////////////////////////////////////////////////
	// ÁÖŔÇ: Ľ­şń˝ş µą¸± ¶§´Â °čÁ¤ ÂďÁö ¸»ŔÚ, 20031103, kelovon
	///////////////////////////////////////////////////////////////////////////
	PRINT_DEBUG_MESSAGE_LOCK.unlock();
	return;
	///////////////////////////////////////////////////////////////////////////
	// ÁÖŔÇ: Ľ­şń˝ş µą¸± ¶§´Â °čÁ¤ ÂďÁö ¸»ŔÚ, 20031103, kelovon
	///////////////////////////////////////////////////////////////////////////

	DBGOUT2(DO2_ALL, "[Character Info] - %s\n", szTime);
	ms_pIMIOCP->m_mCharacterName.lock();
	mt_map<string, CIMIOCPSocket* >::iterator itr3 = ms_pIMIOCP->m_mCharacterName.begin();
	while(itr3 != ms_pIMIOCP->m_mCharacterName.end())
	{
		UINT master = 0;
		CIMIOCPSocket *pSocke = itr3->second;
		if (pSocke 
			&& pSocke->IsUsing() 
			&& pSocke->GetIMParty() != NULL)
		{
			master = pSocke->m_pIMParty->GetMasterCharacterUniqueNumber();
		}

		DBGOUT2(DO2_ALL, "%s at %s: PartyMaster(%d)\n", itr3->first.c_str(), GET_MAP_STRING(pSocke->m_character.MapChannelIndex), master);
		itr3++;
	}
	ms_pIMIOCP->m_mCharacterName.unlock();

	DBGOUT2(DO2_ALL, "\n");
	///////////////////////////////////////////////////////////////////////////
	// END - DEBUG MESSAGE
	///////////////////////////////////////////////////////////////////////////
	PRINT_DEBUG_MESSAGE_LOCK.unlock();
}

BOOL CIMIOCPSocket::FilterChattingString(char *chatString)
{
	// check: ±¸ÇöÇŘľß ÇÔ

	return TRUE;
}

BOOL CIMIOCPSocket::CheckCommandExecutability(USHORT i_nAllowedRace, BOOL i_bAllowAdminTool /* = FALSE */)
{
	BOOL bRet = COMPARE_RACE(m_character.Race, i_nAllowedRace)
				|| (i_bAllowAdminTool && m_PeerSocketType == ST_ADMIN_TOOL);

	return bRet;
}
bool bCompareAccUidAsc(std::pair<UID32_t, string> lhs, std::pair<UID32_t, string> rhs) { return (lhs.first < rhs.first); }
vector<string> CIMIOCPSocket::FindDuplicateIP(vector<string> words)
{
	vector<string>duplicate;

	sort(words.begin(), words.end());

	for (int i = 1; i < words.size(); i++)
		if (words[i - 1] == words[i])
			duplicate.push_back(words[i]);

	return duplicate;

}
///////////////////////////////////////////////////////////////////////////////
/// \fn			BOOL CIMIOCPSocket::HandleAdminCommands(char *cmd)
/// \brief		
/// \author		cmkwon
/// \date		2005-06-07 ~ 2005-06-07
/// \warning	
///
/// \param		
/// \return		TRUE	==> Ăł¸®°ˇ żĎ·áµČ ¸í·Éľî·Î ´őŔĚ»óŔÇ Ăł¸®´Â ľř´Ů.
///				FALSE	==> Ăł¸®°ˇ żĎ·á µÇÁö ľĘŔş ¸í·Éľî·Î Field Server·Î ŔüĽŰµÇľîĽ­ Ăß°ˇ·Î Ăł¸®µĘ
///////////////////////////////////////////////////////////////////////////////
inline string getCurrentDateTime(string s) {
	time_t now = time(0);
	struct tm  tstruct;
	char  buf[80];
	tstruct = *localtime(&now);
	if (s == "now")
		strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	else if (s == "date")
		strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);

	return string(buf);
};
inline void Logger(string logMsg) {
#ifdef ARENA
	FILE* pFile = fopen("ArenaIM_CommandLog.txt", "a");
#else
	FILE* pFile = fopen("IM_CommandLog.txt", "a");

#endif
	fprintf(pFile, "%s\n", logMsg);
	fclose(pFile);
	/*string filePath = "/log/IM_CommandLog_" + getCurrentDateTime("date") + ".txt";
	string now = getCurrentDateTime("now");
	ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
	ofs << now << '\t' << logMsg << '\n';
	ofs.close();*/
}
BOOL CIMIOCPSocket::HandleAdminCommands(char *cmd)
{
	char *token;
	char seps[] = " ";
	char buffer[SIZE_MAX_CHAT_MESSAGE];
//12-01-2020 by Inetpub - all commands log
	char szTemp[1024];
	sprintf(szTemp, "%s :: %s(%s) used command[ %s ]", getCurrentDateTime("now").c_str(), this->m_character.CharacterName, this->m_character.AccountName, cmd);
	string sLog = string(szTemp);
	Logger(szTemp);
	memset(szTemp, 0x00, sizeof(szTemp));
//end of 12-01-2020 by Inetpub
	STRNCPY_MEMSET(buffer, cmd, SIZE_MAX_CHAT_MESSAGE);
	token = strtok(buffer, seps);
	// 2011-11-18 by shcho, Ľ­ąö´Ůżî ÇÁ¸®ĆäľîĽ­ąö´Ůżî Á¦°Ĺ Ăł¸® - Ăß°ˇ ¸í·Éą® »čÁ¦
	if( CheckAdminCommand(token) )
		return FALSE;



	if ( stricmp(token, STRCMD_CS_COMMAND_DEBUGPRINTDBG) == 0
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_DEBUGPRINTDBG_1))
	{
		if ( !CheckCommandExecutability(RACE_OPERATION) )
		{// 2006-04-13 by cmkwon, °ü¸®ŔÚ¸¸ »çżë °ˇ´É
			return FALSE;
		}

// 2008-02-14 by cmkwon, FieldServer ·Î ŔüĽŰ µÇµµ·Ď FALSE ¸¦ ¸®ĹĎÇŃ´Ů.
// 		MessageType_t msgType = T_FI_ADMIN_PRINT_DEBUG_MSG;
// 		ms_pIMIOCP->SendMessageToFieldServer((BYTE*)&msgType, SIZE_FIELD_TYPE_HEADER);
// 
// #ifdef _DEBUG
// 		char *dpm = NULL;
// 		PRINT_DEBUG_MESSAGE(dpm);
// #endif
// 		return TRUE;
		return FALSE;	// 2008-02-14 by cmkwon, FieldServer ·Î ŔüĽŰ µÇµµ·Ď FALSE ¸¦ ¸®ĹĎÇŃ´Ů.
	}
	else if ( stricmp(token, STRCMD_CS_COMMAND_DEBUGSETPARAMI) == 0)
	{
		if ( !CheckCommandExecutability(RACE_OPERATION, TRUE) )
		{
			// żîżµŔÚżë, ŔĎąÝ »çżëŔÚ´Â ą«˝Ă
			return FALSE;
		}

		return TRUE;
	}
#ifdef _INET_TRADE_INTERNATIONAL
	else if (stricmp(token, "/nationtrade") == 0
		|| 0 == stricmp(token, "/nationtrade"))

	{
		if (!CheckCommandExecutability(RACE_OPERATION | RACE_GAMEMASTER, TRUE))
			return FALSE;
		token = strtok(NULL, seps);
		if (token == NULL)
		{
			if (ms_pIMIOCP->m_bINETTradeChatCmdState)
				SendString128(STRING_128_ADMIN_CMD, "The international trade chat is \\genabled\\g");
			else
				SendString128(STRING_128_ADMIN_CMD, "The international trade chat is \\rdisabled\\r");

			SendString128(STRING_128_ADMIN_CMD, "format: /nationtrade [ | on | off] - Enable or disable international trade chat (\\e[$]\\e), status with no param" );
			return TRUE;
		}
		if (stricmp(token, "on") == 0)
		{
			ms_pIMIOCP->m_bINETTradeChatCmdState = TRUE;
			SendString128(STRING_128_ADMIN_CMD, "International trade chat is now \\genabled\\g.");
			ms_pIMIOCP->SendString128ToAllClients(STRING_128_USER_NOTICE, "\\yThe international trade chat has been \\genabled\\y.");

		}
		else if (stricmp(token, "off") == 0)
		{
			ms_pIMIOCP->m_bINETTradeChatCmdState = FALSE;
			SendString128(STRING_128_ADMIN_CMD, "International trade chat is now \\rdisabled\\r.");
			ms_pIMIOCP->SendString128ToAllClients(STRING_128_USER_NOTICE, "\\yThe international trade chat has been \\rdisabled\\y.");
		}
		else
			SendString128(STRING_128_ADMIN_CMD, "format: /nationtrade [ | on | off] - Enable or disable international trade chat (\\e[$]\\e), status with no param");

		return TRUE;
	}
#endif
//#ifdef COMMAND_MULTI_CHAT_SILVER
	// 2015-08-18 by silver cmd for multi chat
	else if (stricmp(token, "/multichat") == 0
		|| 0 == stricmp(token, "/multichat"))

	{
		if (!CheckCommandExecutability(RACE_OPERATION | RACE_GAMEMASTER, TRUE))
			return FALSE;
		token = strtok(NULL, seps);
		if (token == NULL)
		{
			if (ms_pIMIOCP->m_bMultiChatStatues)
				SendString128(STRING_128_ADMIN_CMD, "The inter chat is \\genabled\\g");
			else
				SendString128(STRING_128_ADMIN_CMD, "The inter chat is \\rdisabled\\r");

			SendString128(STRING_128_ADMIN_CMD, "format: /multichat [ | on | off] - Enable or disable multi chat, status with no param" );
			return TRUE;
		}
		if (stricmp(token, "on") == 0)
		{
			ms_pIMIOCP->m_bMultiChatStatues = TRUE;
			SendString128(STRING_128_ADMIN_CMD, "Multi chat is now \\genabled\\g.");
			ms_pIMIOCP->SendString128ToAllClients(STRING_128_USER_NOTICE, "\\yThe multi nation chat has been \\genabled\\y.");

		}
		else if (stricmp(token, "off") == 0)
		{
			ms_pIMIOCP->m_bMultiChatStatues = FALSE;
			SendString128(STRING_128_ADMIN_CMD, "Multi chat is now \\rdisabled\\r.");
			ms_pIMIOCP->SendString128ToAllClients(STRING_128_USER_NOTICE, "\\yThe multi nation chat has been \\rdisabled\\y.");
		}
		else
			SendString128(STRING_128_ADMIN_CMD, "format: /multichat [ | on | off] - Enable or disable multi chat, status with no param");
		return TRUE;
	}
	//end 
//#endif // COMMAND_MULTI_CHAT_SILVER
	else if (stricmp(token, "/whos") == 0) {
		if (!CheckCommandExecutability(RACE_OPERATION | RACE_GAMEMASTER | RACE_MONITOR, TRUE)) {
			return FALSE;
		}
		int nCount = 100;

		token = strtok(NULL, seps);
		if (token != NULL) {
			nCount = atoi(token);
		}

		ms_pIMIOCP->m_MapChactarterUniqueNumber.lock();
		if (CheckCommandExecutability(RACE_OPERATION, TRUE)) {
			SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0029,
						  ms_pIMIOCP->m_MapChactarterUniqueNumber.size(), nCount);
		}
		vector<TempStoreType> tempstored;
		tempstored.clear();
		mt_map<UID32_t, CIMIOCPSocket*>::iterator itr3 = ms_pIMIOCP->m_MapChactarterUniqueNumber.begin();
		while (itr3 != ms_pIMIOCP->m_MapChactarterUniqueNumber.end()) {

			CIMIOCPSocket* pSocket = itr3->second;	// 2015-02-03 by silver
			CHARACTER* pCharacter = &pSocket->m_character;	// 2015-02-03 by silver	
			TempStoreType in;
			string tmp(pSocket->GetPeerIP());
			//start adding to vect
			in.ip = tmp;
			in.InflType = pCharacter->InfluenceType;
			in.UnitKind = pCharacter->UnitKind;
			in.Level = pCharacter->Level;
			strcpy(in.Name, pCharacter->CharacterName);
			strcpy(in.Acc, pCharacter->AccountName);
			in.MapChanIdx = pCharacter->MapChannelIndex;
			tempstored.push_back(in);
			memset(in.Name, 0x00, SIZE_MAX_CHARACTER_NAME);
			memset(in.Acc, 0x00, SIZE_MAX_ACCOUNT_NAME);
			//end add
			itr3++;
			if (--nCount <= 0) { break; }
		}
		//read print
		SendString128(STRING_128_ADMIN_CMD, "\\yPrinting list sorted by IP\\y");
		sort(tempstored.begin(), tempstored.end(), sortByIPAlpha);
		for (int i = 0; i != tempstored.size(); i++) {
			string sInfluence = "\\yUnknown\\y";
			if (tempstored[i].InflType == INFLUENCE_TYPE_NORMAL)
				sInfluence = "\\wFreeSKA\\w";
			else if (tempstored[i].InflType == INFLUENCE_TYPE_VCN)
				sInfluence = "\\eBCU\\e";
			else if (tempstored[i].InflType == INFLUENCE_TYPE_ANI)
				sInfluence = "\\cANI\\c";
			else if (tempstored[i].InflType == INFLUENCE_TYPE_GM)
				sInfluence = "\\mSTAFF\\m";

			SendString128(STRING_128_ADMIN_CMD, "\\w[%s] [\\y%s\\w] Lv.%d %s (%s) at [%s] %s \\a%s", sInfluence.c_str(), CAtumSJ::GetGLOGUnitKindString(tempstored[i].UnitKind), tempstored[i].Level, tempstored[i].Name, tempstored[i].Acc, CAtumSJ::GetMapName(tempstored[i].MapChanIdx.MapIndex), GET_MAP_STRING(tempstored[i].MapChanIdx), tempstored[i].ip.c_str());
		}
		tempstored.clear();
		ms_pIMIOCP->m_MapChactarterUniqueNumber.unlock();
		return TRUE;
	}
	else if ( stricmp(token, STRCMD_CS_COMMAND_WHO) == 0
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_WHO_1))
	{
		if (!CheckCommandExecutability(RACE_OPERATION|RACE_GAMEMASTER|RACE_MONITOR, TRUE))
		{// 2006-04-13 by cmkwon, °ü¸®ŔÚ/żîżµŔÚ/SCMonitor/SCAdminTool¸¸ »çżë °ˇ´É
			return FALSE;
		}

		// format: "/´©±¸ŔÖÁö [# of users]" - ÇöŔç Ľ­ąöżˇ ŔÖ´Â »çżëŔÚ(¸Ę ą«°ü)¸¦ ¸đµÎ Ăâ·Â

		int nCount = 100;

		token = strtok(NULL, seps);
		if (token != NULL)
		{
			nCount = atoi(token);
		}

		ms_pIMIOCP->m_MapChactarterUniqueNumber.lock();
		if(CheckCommandExecutability(RACE_OPERATION))
		{// 2007-11-08 by cmkwon, GM ¸í·Éľî Á¤¸® - °ü¸®ŔÚ¸¸ ŔüĂĽ ŔŻŔúĽö¸¦ ŔüĽŰÇŃ´Ů.
			SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0029,
				ms_pIMIOCP->m_MapChactarterUniqueNumber.size(), nCount);
		}

		vector<string> tempstoreip;
		tempstoreip.clear();

		mt_map<UID32_t, CIMIOCPSocket*>::iterator itr3 = ms_pIMIOCP->m_MapChactarterUniqueNumber.begin();
		while(itr3 != ms_pIMIOCP->m_MapChactarterUniqueNumber.end())
		{


//			UINT master = 0;
//			if (itr3->second->m_pIMParty != NULL)
//			{
//				master = itr3->second->m_pIMParty->GetMasterCharacterUniqueNumber();
//			}

			//CHARACTER *pCharacter = &itr3->second->m_character;
			CIMIOCPSocket *pSocket = itr3->second;	// 2015-02-03 by silver
			CHARACTER *pCharacter = &pSocket->m_character;	// 2015-02-03 by silver
			
			string tmp(pSocket->GetPeerIP());
			tempstoreip.push_back(tmp);
			if(pCharacter->CharacterUniqueNumber!= 441){
			// START 2015-06-08 by silver
			if (pCharacter->InfluenceType == INFLUENCE_TYPE_NORMAL)
				SendString128(STRING_128_ADMIN_CMD, "\\w[SKA]\\w[\\y%s\\y] Lv.%d \\w%s\\w (%s) at [%s] %s \\a%s", CAtumSJ::GetGLOGUnitKindString(pCharacter->UnitKind),pCharacter->Level, pCharacter->CharacterName, pCharacter->AccountName, CAtumSJ::GetMapName(pCharacter->MapChannelIndex.MapIndex), GET_MAP_STRING(pCharacter->MapChannelIndex), pSocket->GetPeerIP());
			else if (pCharacter->InfluenceType == INFLUENCE_TYPE_VCN)
				SendString128(STRING_128_ADMIN_CMD, "\\e[BCU]\\e[\\y%s\\y] Lv.%d \\e%s\\e (%s) at [%s] %s \\a%s", CAtumSJ::GetGLOGUnitKindString(pCharacter->UnitKind), pCharacter->Level, pCharacter->CharacterName, pCharacter->AccountName, CAtumSJ::GetMapName(pCharacter->MapChannelIndex.MapIndex), GET_MAP_STRING(pCharacter->MapChannelIndex), pSocket->GetPeerIP());
			else if (pCharacter->InfluenceType == INFLUENCE_TYPE_ANI)
				SendString128(STRING_128_ADMIN_CMD, "\\c[ANI]\\c[\\y%s\\y] Lv.%d \\c%s\\c (%s) at [%s] %s \\a%s", CAtumSJ::GetGLOGUnitKindString(pCharacter->UnitKind), pCharacter->Level, pCharacter->CharacterName, pCharacter->AccountName, CAtumSJ::GetMapName(pCharacter->MapChannelIndex.MapIndex), GET_MAP_STRING(pCharacter->MapChannelIndex), pSocket->GetPeerIP());
			else if (pCharacter->InfluenceType == 6)
				SendString128(STRING_128_ADMIN_CMD, "\\m[STAFF]\\m[\\y%s\\y] Lv.%d \\m%s\\m (%s) at [%s] %s", CAtumSJ::GetGLOGUnitKindString(pCharacter->UnitKind), pCharacter->Level, pCharacter->CharacterName, pCharacter->AccountName, CAtumSJ::GetMapName(pCharacter->MapChannelIndex.MapIndex), GET_MAP_STRING(pCharacter->MapChannelIndex));// , pSocket->GetPeerIP());
			else
				SendString128(STRING_128_ADMIN_CMD, "\\r[UNKNOW]\\r[\\y%s\\y] Lv.%d \\r%s\\r (%s) at [%s] %s \\a%s", CAtumSJ::GetGLOGUnitKindString(pCharacter->UnitKind), pCharacter->Level, pCharacter->CharacterName, pCharacter->AccountName, CAtumSJ::GetMapName(pCharacter->MapChannelIndex.MapIndex), GET_MAP_STRING(pCharacter->MapChannelIndex), pSocket->GetPeerIP());
			//END 2015-06-08 by silver
			}
			itr3++;
			if (--nCount <= 0) { break; }
		}

		//24-09-2018 by Inet - finding duplicates
		SendString128(STRING_128_ADMIN_CMD, "\\c------------------DUPLICATES--------------------\\c");
		SendString128(STRING_128_ADMIN_CMD, "\\y*NOTE* If IP is once then 2 accounts, if twice then 3 account and so on\\y");
		vector<string> duplicate = FindDuplicateIP(tempstoreip);
		int i = 0;
		for (i = 0; i < duplicate.size(); i++)
			SendString128(STRING_128_ADMIN_CMD, strdup(duplicate[i].c_str()));

		if (i == 0)
			SendString128(STRING_128_ADMIN_CMD, "\\cNo Duplicate IP's\\c");
		
		//end of inet
		
		ms_pIMIOCP->m_MapChactarterUniqueNumber.unlock();

		return TRUE;
	}
	else if (stricmp(token, "/reloadWhiteList" ) == 0 || 0 == stricmp(token, "/reloadWL"))
	{
		if (!CheckCommandExecutability(RACE_OPERATION, TRUE)) {
			return FALSE;
		}
		bool bWhitelist = ms_pIMIOCP->LoadWhiteList();
		if(!bWhitelist)
			return FALSE;

		char szTmp[1024]; *szTmp = '\0';
		SendString128(STRING_128_ADMIN_CMD, "\\e============[WHITELIST INFO]============\\e");
		for (auto& el : ms_pIMIOCP->vecWhiteListMultiAcc)
		{
			sprintf(szTmp,"\\wAccount UID:[\\y%d\\w] AccountName: [\\g%s\\w]",el.AccUID,el.AccountName);
			SendString128(STRING_128_ADMIN_CMD,szTmp);
		}

		SendString128(STRING_128_ADMIN_CMD, "\\gWhiteList reloaded!\\g");

		return TRUE;
	}
	else if (stricmp(token, "/addWhiteList") == 0 || 0 == stricmp(token, "/addWL"))
	{
		if (!CheckCommandExecutability(RACE_OPERATION, TRUE)) {
			return FALSE;
		}
	
		char* szAccName = strtok(NULL, seps);
		if (szAccName == NULL)
		{
			SendString128(STRING_128_ADMIN_CMD, "\\eInsert Account Name!: correct syntax \"/addWhiteList AccName\"\\e");
			return FALSE;
		}
		INT nRes = ms_pIMIOCP->AddWhiteList(szAccName);
		if (nRes == 1)
		{
			SendString128(STRING_128_ADMIN_CMD, "\\yFailed to insert this account to whitelist - already on list\\y");
			return FALSE;
		}
		if (nRes == 2)
		{
			SendString128(STRING_128_ADMIN_CMD, "\\yFailed to insert this account to whitelist - account does not exist in database!\\y");
			return FALSE;
		}
		if (nRes == 3)
		{
			SendString128(STRING_128_ADMIN_CMD, "\\yFailed to connect DB!\\y");
			return FALSE;
		}
		if (nRes == 4)
		{
			SendString128(STRING_128_ADMIN_CMD, "\\yFailed to execute add query - check server logs!\\y");
			return FALSE;
		}

		SendString128(STRING_128_ADMIN_CMD, "\\gAdded %s to whitelist!\\g", szAccName);

		return TRUE;
	}
	else if (stricmp(token, "/kickAllalts") == 0)
	{
		if (!CheckCommandExecutability(RACE_OPERATION, TRUE)) {
			return FALSE;
		}
		SendString128(STRING_128_ADMIN_CMD, "\\e============[ADMIN INFO]============\\e");
		vector<pair< UID32_t, string >> vectAccIp;
		vectAccIp.clear();
		vector<string> vectUidOccurences;
		vectUidOccurences.clear();
		SendString128(STRING_128_ADMIN_CMD, "Starting check procedure");
		auto itr = ms_pIMIOCP->m_MapChactarterUniqueNumber.begin();
		while (itr != ms_pIMIOCP->m_MapChactarterUniqueNumber.end())
		{
			auto* pSocket = itr->second;
			auto* pCharacter = &pSocket->m_character;
			string sIP(pSocket->GetPeerIP());

			vectAccIp.push_back(make_pair(pCharacter->AccountUniqueNumber, sIP));
			
			++itr;
		}
		
		SendString128(STRING_128_ADMIN_CMD, "Total users loaded/online: %d/%d", vectAccIp.size(),ms_pIMIOCP->m_MapChactarterUniqueNumber.size());
		
		sort(vectAccIp.begin(), vectAccIp.end(), bCompareAccUidAsc);
		SendString128(STRING_128_ADMIN_CMD, "Searching for duplicates...");
		auto itr2 = vectAccIp.begin();
		auto nNum = 0;

		ifstream fileInput;
		string line;

		while (itr2 != vectAccIp.end())
		{
			vectUidOccurences.push_back(itr2->second);
			auto it_Uid = vectUidOccurences.begin();
			auto nOccurrences = 0;
			while (it_Uid != vectUidOccurences.end()) {
				if (itr2->second == *it_Uid) {
					nOccurrences++;
					SendString128(STRING_128_ADMIN_CMD, "Current account: %s occured: %d times", &*it_Uid->c_str(), nOccurrences);
					if (nOccurrences > 1) {
						nNum++;
						SendString128(STRING_128_ADMIN_CMD, "\\y%d. Duplicate found:\\c %d",nNum, itr2->first);

						auto* pCharacSocket = ms_pIMIOCP->GetIMIOCPSocketByAccountUID(itr2->first);
						if (pCharacSocket != nullptr && pCharacSocket->IsValidCharacter()) {
						
							bool bWhitelist = ms_pIMIOCP->CheckIsWhiteListed(itr2->first);
							
							if (!bWhitelist) {
								SendString128(STRING_128_ADMIN_CMD, "\\yAccount : %s Character %s AccID: %d IP: %s KICKED !\\y",
									pCharacSocket->m_character.AccountName, pCharacSocket->m_character.CharacterName, pCharacSocket->m_character.AccountUniqueNumber, itr2->second.c_str());
								pCharacSocket->Close(0x1230000);
							}

						
						}
						else {
							SendString128(STRING_128_ADMIN_CMD, "   Invalid character connected, accuid: %d", itr2->first);
						}
					}
				}
				++it_Uid;
			}		
		
			++itr2;
		}
		SendString128(STRING_128_ADMIN_CMD, "\\e===============[END]================\\e");

		return TRUE;	
	}
	else if ( stricmp(token, STRCMD_CS_COMMAND_REGISTERADMIN) == 0
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_REGISTERADMIN_1))
	{
		if ( !CheckCommandExecutability(RACE_OPERATION) )
		{
			// żîżµŔÚżë, ŔĎąÝ »çżëŔÚ´Â ą«˝Ă
			return FALSE;
		}

		// format: "/żîżµµî·Ď" - żîżµŔÚżˇ°Ô ŔĎÁ¤ ŔĚşĄĆ® ąß»ý ˝Ă żŔ´Â ¸ŢĽĽÁö¸¦ ąŢµµ·Ď Ľ­ąöżˇ µî·ĎÇÔ
		BOOL ret = ms_pIMIOCP->AddToAdminCharacter(m_character.CharacterUniqueNumber);

		if (ret)
		{
			SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0030);
		}
		else
		{
			ms_pIMIOCP->DeleteFromAdminCharacter(m_character.CharacterUniqueNumber);
			SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0031);
		}

		return TRUE;
	}
	else if ( stricmp(token, STRCMD_CS_COMMAND_DEBUGPRINTMSGI_0) == 0 
		|| stricmp(token, STRCMD_CS_COMMAND_DEBUGPRINTMSGI_1) == 0 )
	{
		if ( !CheckCommandExecutability(RACE_OPERATION) )
		{// 2006-04-13 by cmkwon, °ü¸®ŔÚ¸¸ »çżë °ˇ´É
			return TRUE;
		}

		// format: "/messagei" - Ĺ¬¶óŔĚľđĆ®żÍ IM Ľ­ąö°ˇ ÁÖ°í ąŢ´Â ÇÁ·ÎĹäÄÝŔ» ¸đµÎ Ăâ·ÂÇÔ
		if (m_bSendMessgeTypeDBG)
		{
			m_bSendMessgeTypeDBG = FALSE;
		}
		else
		{
			m_bSendMessgeTypeDBG = TRUE;
		}

		return TRUE;
	}
	else if ( stricmp(token, STRCMD_CS_COMMAND_USERSINFOTOTAL) == 0
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_USERSINFOTOTAL_1))
	{
		if (!CheckCommandExecutability(RACE_OPERATION|RACE_MONITOR, TRUE))
		{// 2007-11-23 by cmkwon, GM ¸í·Éľî Á¤¸® - '/Ľ­ąö' żîżµŔÚ şŇ°ˇ// 2006-04-13 by cmkwon, °ü¸®ŔÚ/żîżµŔÚ/SCMonitor/SCAdminTool¸¸ »çżë °ˇ´É
			return FALSE;
		}

		// format: "/Ľ­ąö" - Ľ­ąöŔÇ Á¤ş¸¸¦ Ăâ·ÂÇÔ
		SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0032, ms_pIMIOCP->m_szLocalIPAddress);
		SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0033, g_pIMGlobal->GetServerGroupName(), ms_pIMIOCP->m_MapChactarterUniqueNumber.size());
		return FALSE;	// ÇĘµĺĽ­ąöżˇµµ ŔüĽŰÇŘľß ÇĎąÇ·Î FALSE¸¦ returnÇŃ´Ů.
	}
	else if ( stricmp(token, STRCMD_CS_COMMAND_SERVERDOWN) == 0
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_SERVERDOWN_1))
	{
		if (!CheckCommandExecutability(RACE_OPERATION))
		{// 2006-04-13 by cmkwon, °ü¸®ŔÚ¸¸ »çżë °ˇ´É
			return FALSE;
		}

		// format: "/Ľ­ąö´Ůżî [ŔÎÁőĽýŔÚ]" - Ľ­ąö¸¦ Áľ·á˝ĂĹ´
		token = strtok(NULL, seps);
		if (token == NULL)
		{
			SendString128(STRING_128_ADMIN_CMD, STRCMD_CS_COMMAND_SERVERDOWN_HELP);
			return TRUE;
		}

		USHORT nTmpNumber = atoi(token);
		if (ms_pIMIOCP->m_nServerDownVerifyNumber != nTmpNumber)
		{
			ms_pIMIOCP->m_nServerDownVerifyNumber = RANDI(100, 999);
			SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0034,
				ms_pIMIOCP->m_szLocalIPAddress,
				ms_pIMIOCP->m_nServerDownVerifyNumber);
		}
		else
		{
			// 2011-11-18 by shcho, Ľ­ąö´Ůżî ÇÁ¸®ĆäľîĽ­ąö´Ůżî Á¦°Ĺ Ăł¸® - ÇĘµĺĽ­ąö ¸í·É
			// PostMessage(g_pGlobalGameServer->GetMainWndHandle(), WM_CLOSE, 0, 0);
		}
		return TRUE;
	}
	else if ( strcmp(token, STRCMD_CS_COMMAND_LIST) == 0
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_LIST_1))
	{
		if (!CheckCommandExecutability(RACE_OPERATION|RACE_GAMEMASTER|RACE_DEMO))
		{
			// żîżµŔÚżë, ŔĎąÝ »çżëŔÚ´Â ą«˝Ă
			return FALSE;
		}

		// check: żä±¸ÇĎ¸é »ě¸®ŔÚ, kelovon
//		SendString128(STRING_128_ADMIN_CMD, "Ľ­ąö ±×·ě \'%s\'ŔÇ ŔüĂĽ Á˘ĽÓŔÚ: %d¸í", g_szServerGroupName, ms_pIMIOCP->m_MapChactarterUniqueNumber.size());

		return FALSE;		// ÇĘµĺĽ­ąöżˇµµ ŔüĽŰÇŘľß ÇĎąÇ·Î FALSE¸¦ returnÇŃ´Ů.
	}
	else if (stricmp(token, STRCMD_CS_COMMAND_WHOAREYOU) == 0
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_WHOAREYOU_1))
	{
		// format: "/´©±¸ [character name]"

		// ŔĎąÝ »çżëŔÚżë
		token = strtok(NULL, seps);
		if (FALSE == CheckCommandExecutability(RACE_OPERATION|RACE_GAMEMASTER|RACE_MONITOR, TRUE))
		{// 2006-05-09 by cmkwon, µĄ¸đ/ŔĎąÝ»çżëŔÚ Ăł¸®

			// 2009-02-04 by cmkwon, ĹÂ±ążˇ¸¸ "/WhoareYou" ¸í·Éľî ŔĎąÝ ŔŻŔú »çżë şŇ°ˇ - 
			// 2009-07-31 by cmkwon, (·Ż˝ĂľĆ żäĂ») ŔĎąÝ ŔŻŔú /´©±¸ ¸í·Éľî »çżë ¸·±â -
			if(LANGUAGE_TYPE_THAI == g_pIMGlobal->GetLanguageType()
				|| LANGUAGE_TYPE_RUSSIAN == g_pIMGlobal->GetLanguageType())
			{
				return TRUE;
			}

#if defined(_DEFINED_GAMEFORGE4D_)		// 2009-04-21 by cmkwon, ˝şĆç¸µ ĽöÁ¤ÇÔ, // 2009-04-20 by cmkwon, Gameforge4D "/´©±¸" ¸í·Éľî Á¦°ĹÇĎ±â - 
			return TRUE;				// 2009-04-20 by cmkwon, Gameforge4D "/´©±¸" ¸í·Éľî Á¦°ĹÇĎ±â - ŔĎąÝ ŔŻŔú´Â ąŮ·Î return Ăł¸®ÇŃ´Ů.
#endif

			if(NULL == token)
			{
				SendString128(STRING_128_USER_NOTICE, STRCMD_CS_COMMAND_WHOAREYOU_HELP);
				return TRUE;
			}

			CIMIOCPSocket *pCharacSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(token);

			if (NULL == pCharacSocket
				|| FALSE == pCharacSocket->IsValidCharacter())
			{
				SendString128(STRING_128_USER_NOTICE, STRMSG_S_I2NOTIFY_0035, token);
				return TRUE;
			}

			if(COMPARE_RACE(pCharacSocket->GetCharacter()->Race, RACE_OPERATION|RACE_GAMEMASTER))
			{
				SendString128(STRING_128_USER_NOTICE, STRMSG_S_I2NOTIFY_0035, token);
// 2006-11-14 by cmkwon, Ŕ§żÍ °°ŔĚ ±¸şĐ ÇŇ Ľö ľřµµ·Ď ĽöÁ¤ÇÔ
//				SendString128(STRING_128_USER_NOTICE, "Permission denied !!");
				return TRUE;
			}

			if (FALSE == COMPARE_INFLUENCE(m_character.InfluenceType, pCharacSocket->GetCharacter()->InfluenceType))
			{// 2006-11-13 by dhjin, ´Ů¸Ą ĽĽ·Â°Łżˇ´Â »çżë ±ÝÁö
				SendString128(STRING_128_USER_NOTICE, STRMSG_S_I2NOTIFY_0035, token);
				return TRUE;
			}


			if(FALSE == IS_VALID_UNIQUE_NUMBER(pCharacSocket->m_uidGuildUniqueNumber))
			{
				SendString128(STRING_128_USER_NOTICE, "\'%s\', \'%s\'"
					, token
					, CAtumSJ::GetMapName(pCharacSocket->m_character.MapChannelIndex.MapIndex));
			}
			else
			{
				SendString128(STRING_128_USER_NOTICE, "\'%s\'(%s), \'%s\'"
					, token
					, pCharacSocket->m_character.GuildName
					, CAtumSJ::GetMapName(pCharacSocket->m_character.MapChannelIndex.MapIndex));
			}

			return TRUE;
		}

		// żîżµŔÚżë
		if (token != NULL)
		{
			// »çżëŔÚ°ˇ ¸í˝ĂµČ °ćżě
			char *CharacName = token;

			CIMIOCPSocket *pCharacSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(CharacName);
			if (NULL == pCharacSocket
				|| FALSE == pCharacSocket->IsValidCharacter())
			{
				SendString128(STRING_128_ADMIN_CMD, "%s NOT PLAYING", CharacName);
				return TRUE;
			}
			
			//SendString128(STRING_128_ADMIN_CMD, "%s PLAYING", CharacName, pCharacSocket->m_character.MapName);
			INIT_MSG_WITH_BUFFER(MSG_FI_ADMIN_GET_CHARACTER_INFO, T_FI_ADMIN_GET_CHARACTER_INFO, pMsgAdminGetCharInfo, pMsgAdminGetCharInfoBuf);
			pMsgAdminGetCharInfo->Type = ADMIN_GET_CHARAC_TYPE_SEND;
			pMsgAdminGetCharInfo->AdminCharacterUniqueNumber = m_character.CharacterUniqueNumber;
			pMsgAdminGetCharInfo->CharacterUniqueNumber = pCharacSocket->m_character.CharacterUniqueNumber;

			if (FALSE == pCharacSocket->SendMsgToCurrentFieldServerSocket(pMsgAdminGetCharInfoBuf, MSG_SIZE(MSG_FI_ADMIN_GET_CHARACTER_INFO)))
			{
				// current field server socketŔĚ NULLŔĚ¸é ą«˝Ă.
				char	szSystemLog[1024];
				sprintf(szSystemLog, "  Fatal Error: CurrentFieldServerSocket is NULL, %s at %s\r\n",
					GetCharacterString(&pCharacSocket->m_character, string()), STRCMD_CS_COMMAND_WHOAREYOU);
				DBGOUT(szSystemLog);
				g_pGlobal->WriteSystemLog(szSystemLog);

				return TRUE;	// ÇĘµĺ Ľ­ąö·Î Ăß°ˇ ŔüĽŰ ÇĘżä ľřŔ˝
			}
		}
		else
		{
			// »çżëŔÚ°ˇ ¸í˝ĂµÇÁö ľĘŔş °ćżě
			int nCount = 10;
			ms_pIMIOCP->m_MapChactarterUniqueNumber.lock();
			SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0036,
				ms_pIMIOCP->m_MapChactarterUniqueNumber.size(), nCount);
			mt_map<UID32_t, CIMIOCPSocket*>::iterator itr3 = ms_pIMIOCP->m_MapChactarterUniqueNumber.begin();
			while(itr3 != ms_pIMIOCP->m_MapChactarterUniqueNumber.end())
			{
				CHARACTER *pCharacter = &itr3->second->m_character;
				SendString128(STRING_128_ADMIN_CMD, "%s(%s) at %s",
					pCharacter->CharacterName, pCharacter->AccountName,
					GET_MAP_STRING(pCharacter->MapChannelIndex));
				itr3++;
				if (--nCount <= 0) { break; }
			}
			ms_pIMIOCP->m_MapChactarterUniqueNumber.unlock();
		}

		return TRUE;
	}
	else if (stricmp(token, STRCMD_CS_COMMAND_USERSEND) == 0
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_USERSEND_1))
	{
		if ( !CheckCommandExecutability(RACE_OPERATION/*|RACE_GAMEMASTER*/, TRUE) )
		{
			// żîżµŔÚżë, ŔĎąÝ »çżëŔÚ´Â ą«˝Ă
			return FALSE;
		}

		// format: "/ş¸ł»±â [character name] [map name]" - Äł¸ŻĹÍ¸¦ ÇŘ´ç ¸ĘŔ¸·Î ş¸łż
		token = strtok(NULL, seps);
		if (token == NULL)
		{
			SendString128(STRING_128_ADMIN_CMD, STRCMD_CS_COMMAND_USERSEND_HELP);
			return TRUE;
		}
		char *CharacName = token;

		CIMIOCPSocket *pCharacSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(CharacName);

		if (NULL == pCharacSocket)
		{
			SendString128(STRING_128_ADMIN_CMD, "%s NOT PLAYING", CharacName);
			return TRUE;
		}
		
		INIT_MSG_WITH_BUFFER(MSG_FI_CHAT_REGION, T_FI_CHAT_REGION, msgChatRegion, msgChatRegionBuf);
		msgChatRegion->CharacterUniqueNumber = m_character.CharacterUniqueNumber;
		msgChatRegion->MessageLength = strlen(cmd) + 1;		// + ¸¶Áö¸· null Ć÷ÇÔ
		STRNCPY_MEMSET((char*)msgChatRegionBuf + MSG_SIZE(MSG_FI_CHAT_REGION), cmd, SIZE_MAX_CHAT_MESSAGE);

		if (FALSE == pCharacSocket->SendMsgToCurrentFieldServerSocket(msgChatRegionBuf, MSG_SIZE(MSG_FI_CHAT_REGION) + msgChatRegion->MessageLength))
		{
			// current field server socketŔĚ NULLŔĚ¸é ą«˝Ă.
			char	szSystemLog[1024];
			sprintf(szSystemLog, "  Fatal Error: CurrentFieldServerSocket is NULL, %s at %s\r\n",
				GetCharacterString(&pCharacSocket->m_character, string()), STRCMD_CS_COMMAND_USERSEND);
			DBGOUT(szSystemLog);
			g_pGlobal->WriteSystemLog(szSystemLog);
			return TRUE;	// ÇĘµĺ Ľ­ąö·Î Ăß°ˇ ŔüĽŰ ÇĘżä ľřŔ˝
		}
		return TRUE;
	}
	else if (stricmp(token, STRCMD_CS_COMMAND_GOUSER) == 0
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_GOUSER_1))
	{
		if ( !CheckCommandExecutability(RACE_OPERATION|RACE_GAMEMASTER|RACE_MONITOR) )
		{// 2008-09-09 by cmkwon, ¸đ´ĎĹÍ ±ÇÇŃ °čÁ¤żˇ »çżë ±ÇÇŃ Ăß°ˇ - STRCMD_CS_COMMAND_GOUSER, // 2006-04-13 by cmkwon, °ü¸®ŔÚ/żîżµŔÚ¸¸ »çżë °ˇ´É
			return FALSE;
		}

		// format: "/°ˇ±â [character name]" - ÇŘ´ç Äł¸ŻĹÍ°ˇ ŔÖ´Â °÷Ŕ¸·Î ŔĚµżÇÔ
		token = strtok(NULL, seps);
		if (token == NULL)
		{
			SendString128(STRING_128_ADMIN_CMD, STRCMD_CS_COMMAND_GOUSER_HELP);
			return TRUE;			
		}
		char *CharacName = token;

		CIMIOCPSocket *pCharacSocket = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(CharacName);
		if (NULL == pCharacSocket)
		{
			SendString128(STRING_128_ADMIN_CMD, "%s NOT PLAYING", CharacName);
			return TRUE;		// ÇĘµĺ Ľ­ąö·Î Ăß°ˇ ŔüĽŰ ÇĘżä ľřŔ˝
		}

		INIT_MSG_WITH_BUFFER(MSG_FI_ADMIN_GET_CHARACTER_INFO, T_FI_ADMIN_GET_CHARACTER_INFO, pMsgAdminGetCharInfo, pMsgAdminGetCharInfoBuf);
		pMsgAdminGetCharInfo->Type = ADMIN_GET_CHARAC_TYPE_MOVETO;
		pMsgAdminGetCharInfo->AdminCharacterUniqueNumber = m_character.CharacterUniqueNumber;
		pMsgAdminGetCharInfo->CharacterUniqueNumber = pCharacSocket->m_character.CharacterUniqueNumber;

		if (FALSE == pCharacSocket->SendMsgToCurrentFieldServerSocket(pMsgAdminGetCharInfoBuf, MSG_SIZE(MSG_FI_ADMIN_GET_CHARACTER_INFO)))
		{
			// current field server socketŔĚ NULLŔĚ¸é...
			char	szSystemLog[1024];
			sprintf(szSystemLog, "  Fatal Error: CurrentFieldServerSocket is NULL, %s at %s\r\n",
				GetCharacterString(&pCharacSocket->m_character, string()), STRCMD_CS_COMMAND_GOUSER);
			DBGOUT(szSystemLog);
			g_pGlobal->WriteSystemLog(szSystemLog);
			return TRUE;		// ÇĘµĺ Ľ­ąö·Î Ăß°ˇ ŔüĽŰ ÇĘżä ľřŔ˝
		}
		return TRUE;
	}
	else if (stricmp(token, STRCMD_CS_COMMAND_COMEON) == 0)
	{
		// format: "/żŔ±â [Äł¸ŻĹÍ¸í]" - ÇŘ´ç Äł¸ŻĹÍ¸¦ şŇ·ŻżČ
		// MSG_IC_ADMIN_CALL_CHARACTER ·Î Ăł¸®
		return FALSE;
	}
// 2006-07-25 by cmkwon, Ĺ¬¶óŔĚľđĆ®żˇĽ­ T_IC_ADMIN_CALL_GUILD ÇÁ·ÎĹäÄÝ·Î ş¸ł»Áŕľß ÇŃ´Ů.
// 	else if (stricmp(token, STRCMD_CS_COMMAND_GUILDCOMEON) == 0
// 		|| 0 == stricmp(token, STRCMD_CS_COMMAND_GUILDCOMEON_1))
// 	{
// 		if ( !CheckCommandExecutability(RACE_OPERATION|RACE_GAMEMASTER) )
// 		{
// 			// żîżµŔÚżë, ŔĎąÝ »çżëŔÚ´Â ą«˝Ă
// 			return FALSE;
// 		}
// 
// 		// format: "/ż©´ÜżŔ±â [ż©´Ü¸í]" - ÇŘ´ç ż©´ÜŔ» şŇ·ŻżČ
// 		token = strtok(NULL, seps);
// 		if (token == NULL)
// 		{
// 			SendString128(STRING_128_ADMIN_CMD, STRCMD_CS_COMMAND_GUILDSEND_HELP);
// 			return TRUE;
// 		}
// 
// 		return FALSE;
// 	}
	else if (stricmp(token, STRCMD_CS_COMMAND_GUILDSEND) == 0
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_GUILDSEND_1))
	{
		if ( !CheckCommandExecutability(RACE_OPERATION/*|RACE_GAMEMASTER*/, TRUE) )
		{
			// żîżµŔÚżë, ŔĎąÝ »çżëŔÚ´Â ą«˝Ă
			return FALSE;
		}

		// format: "/ż©´Üş¸ł»±â [ż©´Ü¸í] [map name]" - Äł¸ŻĹÍ¸¦ ÇŘ´ç ¸ĘŔ¸·Î ş¸łż
		token = strtok(NULL, seps);
		if (token == NULL)
		{
			SendString128(STRING_128_ADMIN_CMD, STRCMD_CS_COMMAND_GUILDSEND_HELP);
			return TRUE;
		}

		char *szGuildName = token;
		char *szMapName = strtok(NULL, seps);
		if (szMapName == NULL)
		{// ÇŘ´ç ¸ĘŔĚ ľřŔ˝
			SendString128(STRING_128_ADMIN_CMD, STRMSG_S_IMSERVER_050607_0001);
			return TRUE;
		}

		ms_pIMIOCP->m_MapGuild.lock();
		{			
			CGuild *pGuild = ms_pIMIOCP->GetGuildByName(szGuildName);

			if (pGuild != NULL)
			{
				char szCmdBuffer[SIZE_MAX_CHAT_MESSAGE];
				listGuildMember::iterator itr = pGuild->m_ListGuildMember.begin();
				while (itr != pGuild->m_ListGuildMember.end())
				{
					CGuildMember *pGuildMember = &(*itr);

					CIMIOCPSocket *pCharacSocket = pGuildMember->m_pIMIOCPSocket;
					if (pCharacSocket == NULL || !pCharacSocket->IsUsing())
					{
						itr++;
						continue;
					}

					sprintf(szCmdBuffer, "/%s %s %s", STRCMD_CS_COMMAND_USERSEND, pCharacSocket->m_character.CharacterName, szMapName);

					INIT_MSG_WITH_BUFFER(MSG_FI_CHAT_REGION, T_FI_CHAT_REGION, msgChatRegion, msgChatRegionBuf);
					msgChatRegion->CharacterUniqueNumber = m_character.CharacterUniqueNumber;
					msgChatRegion->MessageLength = strlen(szCmdBuffer) + 1;		// + ¸¶Áö¸· null Ć÷ÇÔ
					STRNCPY_MEMSET((char*)msgChatRegionBuf + MSG_SIZE(MSG_FI_CHAT_REGION), szCmdBuffer, SIZE_MAX_CHAT_MESSAGE);

					if (FALSE == pCharacSocket->SendMsgToCurrentFieldServerSocket(msgChatRegionBuf, MSG_SIZE(MSG_FI_CHAT_REGION) + msgChatRegion->MessageLength))
					{
						// current field server socketŔĚ NULLŔĚ¸é ą«˝Ă.
						char	szSystemLog[1024];
						sprintf(szSystemLog, "  Fatal Error: CurrentFieldServerSocket is NULL, %s at %s\r\n",
							GetCharacterString(&pCharacSocket->m_character, string()), STRCMD_CS_COMMAND_GUILDSEND);
						DBGOUT(szSystemLog);
						g_pGlobal->WriteSystemLog(szSystemLog);

						itr++;
						continue;
					}

					itr++;
				}// while_end
			}
			else
			{
				SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0038);
			}
		}// lock_end
		ms_pIMIOCP->m_MapGuild.unlock();			
		return TRUE;
	}
	else if (stricmp(token, STRCMD_CS_COMMAND_CHATPTOPFLAG) == 0
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_CHATPTOPFLAG_1))
	{
		// format: "/±Ó¸»" - ±Ó¸»Ŕ» ąŢÁö ľĘŔ˝, toggleµĘ
		// ¸đµÎ ÇăżëÇÔ

		// 2008-05-15 by dhjin, EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć
//		if (FALSE == COMPARE_CHATTYPE_BIT(m_enableChatFlag8, CHAT_TYPE_PTOP))
		if (FALSE == COMPARE_CHATTYPE_BIT(m_enableChatFlag16, CHAT_TYPE_PTOP))
		{
			// 2008-05-15 by dhjin, EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć
//			SET_CHATTYPE_BIT(m_enableChatFlag8, CHAT_TYPE_PTOP);
			SET_CHATTYPE_BIT(m_enableChatFlag16, CHAT_TYPE_PTOP);
			this->SendString128(STRING_128_USER_NOTICE, STRMSG_S_I2NOTIFY_0039);
		}
		else
		{
			// 2008-05-15 by dhjin, EP3 - Ă¤ĆĂ ˝Ă˝şĹŰ şŻ°ć
//			CLEAR_CHATTYPE_BIT(m_enableChatFlag8, CHAT_TYPE_PTOP);
			CLEAR_CHATTYPE_BIT(m_enableChatFlag16, CHAT_TYPE_PTOP);
			this->SendString128(STRING_128_USER_NOTICE, STRMSG_S_I2NOTIFY_0040);
		}
		this->SendToFieldServerChatBitFlag();
		return TRUE;
	}
	else if (stricmp(token, STRCMD_CS_COMMAND_GUILDINFO) == 0
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_GUILDINFO_1))
	{
		// format: "/±ćµĺ" - ±ćµĺ Á¤ş¸ Ăâ·Â
		if (!CheckCommandExecutability(RACE_OPERATION, TRUE))
		{// 2006-04-13 by cmkwon, °ü¸®ŔÚ¸¸ »çżë °ˇ´É
			return FALSE;
		}

		if (FALSE == IS_VALID_UNIQUE_NUMBER(m_uidGuildUniqueNumber))
		{
			SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0041);
		}
		else
		{
			ms_pIMIOCP->m_MapGuild.lock();
			{
				CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
				if(ptmGuild)
				{
					ptmGuild->PrintGuildInfo(this);
				}
				else
				{
					SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0042, m_uidGuildUniqueNumber);
				}
			}
			ms_pIMIOCP->m_MapGuild.unlock();
		}

		return TRUE;
	}
	else if ( stricmp(token, STRCMD_CS_COMMAND_WEATHERSET) == 0
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_WEATHERSET_1))
	{
		// format: "/łŻľľ [±âş»|¸ĽŔ˝|şń|´«|Čĺ¸˛|ľČ°ł] [ŔüĂĽ|¸ĘŔĚ¸§] [on|off]" - łŻľľ Á¶Á¤

		if ( !CheckCommandExecutability(RACE_OPERATION, TRUE) )
		{// 2006-04-13 by cmkwon, °ü¸®ŔÚ/SCAdminTool¸¸ »çżë °ˇ´É
			return FALSE;
		}

		INIT_MSG_WITH_BUFFER(MSG_FI_ADMIN_CHANGE_WEATHER, T_FI_ADMIN_CHANGE_WEATHER, pAdminChangerWeather, pAdminChangerWeatherBuf);
		memset(pAdminChangerWeather, sizeof(MSG_FI_ADMIN_CHANGE_WEATHER), 0x00);

		CIMIOCPSocket *pFieldServerSocket = NULL;
		BOOL bAllMaps = FALSE;					// ŔüĂĽ ¸Ę ĽłÁ¤ŔĚ¸é TRUE

		// łŻľľ Áľ·ů
		token = strtok(NULL, seps);
		if (token == NULL)
		{
			SendString128(STRING_128_ADMIN_CMD, STRCMD_CS_COMMAND_WEATHERSET_HELP);
			return TRUE;
		}
		
		if (stricmp(token, STRCMD_CS_COMMAND_WEATHERSET_P1NORMAL)==0)		// 2006-12-11 by cmkwon, ÇÔĽöşŻ°ć(strcmp->stricmp)
		{
			pAdminChangerWeather->MapWeather = WEATHER_DEFAULT;
		}
		else if (stricmp(token, STRCMD_CS_COMMAND_WEATHERSET_P1FINE)==0)	// 2006-12-11 by cmkwon, ÇÔĽöşŻ°ć(strcmp->stricmp)
		{
			pAdminChangerWeather->MapWeather = WEATHER_SUNNY;
		}
		else if (stricmp(token, STRCMD_CS_COMMAND_WEATHERSET_P1RAIN)==0)	// 2006-12-11 by cmkwon, ÇÔĽöşŻ°ć(strcmp->stricmp)
		{
			pAdminChangerWeather->MapWeather = WEATHER_RAINY;
		}
		else if (stricmp(token, STRCMD_CS_COMMAND_WEATHERSET_P1SNOW)==0)	// 2006-12-11 by cmkwon, ÇÔĽöşŻ°ć(strcmp->stricmp)
		{
			pAdminChangerWeather->MapWeather = WEATHER_SNOWY;
		}
		else if (stricmp(token, STRCMD_CS_COMMAND_WEATHERSET_P1CLOUDY)==0)	// 2006-12-11 by cmkwon, ÇÔĽöşŻ°ć(strcmp->stricmp)
		{
			pAdminChangerWeather->MapWeather = WEATHER_CLOUDY;
		}
		else if (stricmp(token, STRCMD_CS_COMMAND_WEATHERSET_P1FOG)==0)		// 2006-12-11 by cmkwon, ÇÔĽöşŻ°ć(strcmp->stricmp)
		{
			pAdminChangerWeather->MapWeather = WEATHER_FOGGY;
		}
		else
		{
			SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0043, token);
			return TRUE;		// ÇĘµĺ Ľ­ąö·Î Ăß°ˇ ŔüĽŰ ÇĘżä ľřŔ˝
		}

		// map ŔĚ¸§
		token = strtok(NULL, seps);
		if (token != NULL)
		{
			if(NULL == ms_pIMIOCP->m_pFieldServerSocket || FALSE == ms_pIMIOCP->m_pFieldServerSocket->IsUsing())
			{
				SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0044);
				return TRUE;		// ÇĘµĺ Ľ­ąö·Î Ăß°ˇ ŔüĽŰ ÇĘżä ľřŔ˝
			}

			if (stricmp(STRCMD_CS_COMMAND_WEATHERSET_P2ALL, token) == 0)	// 2006-12-11 by cmkwon, ÇÔĽöşŻ°ć(strcmp->stricmp)
			{
				bAllMaps = TRUE;
			}
			else
			{
				if( FALSE == ms_pIMIOCP->m_FieldServerInfo.IsExistMapIndex(atoi(token)) )
				{
					SendString128(STRING_128_ADMIN_CMD, STRMSG_S_I2NOTIFY_0045, token);
					return TRUE;		// ÇĘµĺ Ľ­ąö·Î Ăß°ˇ ŔüĽŰ ÇĘżä ľřŔ˝
				}

				pFieldServerSocket = ms_pIMIOCP->m_pFieldServerSocket;
			}
			pAdminChangerWeather->MapIndex = atoi(token);
		}
		else
		{
			pFieldServerSocket = m_pCurrentFieldServerSocket;
			pAdminChangerWeather->MapIndex = m_character.MapChannelIndex.MapIndex;
		}

		// on|off
		token = strtok(NULL, seps);
		if (token != NULL)
		{
			if (stricmp(token, STRCMD_CS_COMMAND_WEATHERSET_P3ON)==0)			// 2006-12-11 by cmkwon, ÇÔĽöşŻ°ć(strcmp->stricmp)
			{
				pAdminChangerWeather->OnOff = TRUE;
			}
			else if (stricmp(token, STRCMD_CS_COMMAND_WEATHERSET_P3OFF)==0)		// 2006-12-11 by cmkwon, ÇÔĽöşŻ°ć(strcmp->stricmp)
			{
				pAdminChangerWeather->OnOff = FALSE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			pAdminChangerWeather->OnOff = TRUE;
		}

		if (bAllMaps)
		{
			// ŔüĂĽ ¸ĘŔÇ łŻľľ¸¦ şŻ°ć
			pAdminChangerWeather->MapIndex		= MAP_INDEX_ALL;			// 2006-12-11 by cmkwon
			ms_pIMIOCP->SendMessageToFieldServer(pAdminChangerWeatherBuf, MSG_SIZE(MSG_FI_ADMIN_CHANGE_WEATHER));
		}
		else
		{
			// ĆŻÁ¤ ¸ĘŔÇ łŻľľ¸¦ şŻ°ć
#ifdef _DEBUG
			assert(pFieldServerSocket != NULL);
#endif
			if (pFieldServerSocket == NULL)
			{
				g_pGlobal->WriteSystemLogEX(TRUE, "FATAL ERROR: NULL pFieldServerSocket(%s)\r\n", cmd);
				return TRUE;		// ÇĘµĺ Ľ­ąö·Î Ăß°ˇ ŔüĽŰ ÇĘżä ľřŔ˝
			}

			pFieldServerSocket->SendAddData(pAdminChangerWeatherBuf, MSG_SIZE(MSG_FI_ADMIN_CHANGE_WEATHER));
		}

		return TRUE;		// ÇĘµĺ Ľ­ąö·Î Ăß°ˇ ŔüĽŰ ÇĘżä ľřŔ˝
	}
	else if (0 == stricmp(token, STRCMD_CS_COMMAND_CHATFORBID)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_CHATFORBID_1))
	{
		if (FALSE == CheckCommandExecutability(RACE_OPERATION|RACE_GAMEMASTER, TRUE))
		{// 2006-04-13 by cmkwon, °ü¸®ŔÚ/żîżµŔÚ/SCAdminTool¸¸ »çżë °ˇ´É
			return FALSE;
		}

		// format: "/Ă¤±Ý [character name] [˝Ă°Ł(şĐ)]" - Ă¤ĆĂ±ÝÁö ĽłÁ¤ÇĎ±â
		token = strtok(NULL, seps);
		if (NULL == token)
		{// CharacterName ŔĚ ľřŔ˝
			SendString128(STRING_128_ADMIN_CMD, STRCMD_CS_COMMAND_CHATFORBID_HELP);
			return TRUE;		// ÇĘµĺ Ľ­ąö·Î Ăß°ˇ ŔüĽŰ ÇĘżä ľřŔ˝
		}

		char szCharName[SIZE_MAX_CHARACTER_NAME];
		int  nMinutes = 1;
		STRNCPY_MEMSET(szCharName, token, SIZE_MAX_CHARACTER_NAME);

		token = strtok(NULL, seps);
		if(token)
		{
			nMinutes = max(nMinutes, atoi(token));
		}
		
		CIMIOCPSocket *pIISock = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(szCharName);
		if(pIISock
			&& pIISock->IsValidCharacter())
		{
			pIISock->SendString128(STRING_128_USER_NOTICE, STRMSG_S_I2NOTIFY_0046, nMinutes);
		}
		SendString128(STRING_128_USER_NOTICE, STRMSG_S_I2NOTIFY_0047, szCharName, nMinutes);
		ms_pIMIOCP->m_chatBlockManager.InsertChatBlockList(szCharName, nMinutes);
		return TRUE;
	}
	else if (0 == stricmp(token, STRCMD_CS_COMMAND_CHATFORBIDRELEASE)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_CHATFORBIDRELEASE_1))
	{
		if (FALSE == CheckCommandExecutability(RACE_OPERATION|RACE_GAMEMASTER, TRUE))
		{// 2006-04-13 by cmkwon, °ü¸®ŔÚ/żîżµŔÚ/SCAdminTool¸¸ »çżë °ˇ´É
			return FALSE;
		}

		// format: "/Ă¤±ÝÇŘÁ¦ [character name]" - Ă¤ĆĂ±ÝÁö ÇŘÁ¦ÇĎ±â
		token = strtok(NULL, seps);
		if (NULL == token)
		{// CharacterName ŔĚ ľřŔ˝
			SendString128(STRING_128_ADMIN_CMD, STRCMD_CS_COMMAND_CHATFORBIDRELEASE_HELP);
			return TRUE;
		}

		CIMIOCPSocket *pIISock = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(token);
		if(pIISock)
		{
			pIISock->SendString128(STRING_128_USER_NOTICE, STRMSG_S_I2NOTIFY_0048);
		}

		SendString128(STRING_128_USER_NOTICE, STRMSG_S_I2NOTIFY_0049, token);
		ms_pIMIOCP->m_chatBlockManager.DeleteChatBlockList(token);
		return TRUE;
	}
	else if (stricmp(token, STRCMD_CS_COMMAND_COMMANDLIST_0) == 0
		|| stricmp(token, STRCMD_CS_COMMAND_COMMANDLIST_1) == 0
		|| stricmp(token, STRCMD_CS_COMMAND_COMMANDLIST_2) == 0)
	{
		// format: "/?" - ¸í·Éľî ¸®˝şĆ® Ăâ·Â
		
		// 2007-11-08 by cmkwon, GM ¸í·Éľî Á¤¸® - ŔĎąÝŔŻŔú ¸í·Éľî ¸®˝şĆ® ŔüĽŰ
		int i; //inetpub 'i' declaration with 'int' type
		for (i = 0; i < ms_pIMIOCP->m_vectstringCommandListforUser.size(); i++)
		{
			SendString128(STRING_128_USER_NOTICE, "%s", ms_pIMIOCP->m_vectstringCommandListforUser[i].c_str());
		}
		if (FALSE == CheckCommandExecutability(RACE_OPERATION|RACE_GAMEMASTER, TRUE))
		{// 2007-11-08 by cmkwon, ŔĎąÝ ŔŻŔú´Â ż©±âżˇĽ­ ¸®ĹĎ
			return TRUE;
		}

		// 2007-11-08 by cmkwon, GM ¸í·Éľî Á¤¸® - żîżµŔÚ ¸í·Éľî ¸®˝şĆ® ŔüĽŰ
		SendString128(STRING_128_USER_NOTICE, "[%s] command list.", GetRaceString(RACE_GAMEMASTER));
		SendString128(STRING_128_USER_NOTICE, "\\a==============================\\a", GetRaceString(RACE_GAMEMASTER));
		for (i = 0; i < ms_pIMIOCP->m_vectstringGameMasterCommandList.size(); i++)
		{
			SendString128(STRING_128_USER_NOTICE, "\\a%s\\a", ms_pIMIOCP->m_vectstringGameMasterCommandList[i].c_str());
		}
		if(FALSE == CheckCommandExecutability(RACE_OPERATION, TRUE))
		{// 2007-11-08 by cmkwon, żîżµŔÚ´Â ż©±âżˇĽ­ ¸®ĹĎ
			return TRUE;
		}

		// 2007-11-08 by cmkwon, GM ¸í·Éľî Á¤¸® - °ü¸®ŔÚ ¸í·Éľî ¸®˝şĆ® ŔüĽŰ
		SendString128(STRING_128_USER_NOTICE, "[%s] command list.", GetRaceString(RACE_OPERATION));
		SendString128(STRING_128_USER_NOTICE, "\\a==============================\\a", GetRaceString(RACE_GAMEMASTER));
		for (i = 0; i < ms_pIMIOCP->m_vectstringCommandList.size(); i++)
		{
			SendString128(STRING_128_USER_NOTICE, "\\a%s\\a", ms_pIMIOCP->m_vectstringCommandList[i].c_str());
		}
		return TRUE;
	}
	else if(0 == stricmp(token, STRCMD_CS_COMMAND_CALLGM_0)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_CALLGM_1)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_CALLGM_2))
	{// 2006-05-08 by cmkwon

// 2007-11-06 by cmkwon, ÇŃ±ąµµ µµżěąĚ ˝Ă˝şĹŰ »çżë
//		if(LANGUAGE_TYPE_KOREAN == g_pIMGlobal->GetLanguageType())
//		{// 2006-10-17 by cmkwon, ÇŃ±Ű ąöÁŻżˇĽ­´Â ¸·Ŕ˝
//			return FALSE;
//		}

		// 2007-11-19 by cmkwon, ÁřÁ¤˝Ă˝şĹŰ ľ÷µĄŔĚĆ® - ˝Ă˝şĹŰ ÄŃÁř »óĹÂ ĂĽĹ©
		if(FALSE == ms_pIMIOCP->IsOnCallGMSystem())
		{
			this->SendString128(STRING_128_USER_NOTICE, STRMSG_071120_0001);
			return TRUE;
		}

		// "format: /µµżěąĚ [»ó´çł»żë] - GM°ú »ó´ăŔ» ˝ĹĂ»ÇŃ´Ů."
		token = strtok(NULL, "");
		if(NULL == token)
		{
			this->SendString128(STRING_128_USER_NOTICE, STRCMD_CS_COMMAND_CALLGM_HELP);
			return TRUE;
		}

		// 2007-11-19 by cmkwon, ÁřÁ¤˝Ă˝şĹŰ ľ÷µĄŔĚĆ® - CIMIOCP::CallGMInsertRequest() ÇÔĽöżˇ ĽĽ·Â ŔÎŔÚ Ăß°ˇ
		int nReqCnt = ms_pIMIOCP->CallGMInsertRequest(m_character.CharacterUniqueNumber, m_character.InfluenceType, m_character.CharacterName, token);
		
		if(0 >= nReqCnt)
		{
			this->SendString128(STRING_128_USER_NOTICE, "CallGM overflow !!");
			return TRUE;
		}

		// 2006-05-26 by cmkwon, temporay in english
		this->SendString128(STRING_128_USER_NOTICE, STRMSG_060526_0000);

		INIT_MSG_WITH_BUFFER(MSG_IC_ADMIN_CALLGM_INFO_OK, T_IC_ADMIN_CALLGM_INFO_OK, pCallGMInfo, SendBuf);
		pCallGMInfo->nCallGMTotalCount	= nReqCnt;
		SendAddData(SendBuf, MSG_SIZE(MSG_IC_ADMIN_CALLGM_INFO_OK));		
		return TRUE;
	}
	else if(0 == stricmp(token, STRCMD_CS_COMMAND_VIEWCALLGM_0)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_VIEWCALLGM_1)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_VIEWCALLGM_2))
	{// 2006-05-08 by cmkwon
		if(FALSE == CheckCommandExecutability(RACE_OPERATION|RACE_GAMEMASTER))
		{// 2006-05-08 by cmkwon, °ü¸®ŔÚ/żîżµŔÚ¸¸ »çżë °ˇ´É
			return TRUE;
		}

		// "format: /µµżěąĚş¸±â [|°łĽö(1~10)] - GM »ó´ă żäĂ» ¸®˝şĆ®¸¦ ÁöÁ¤ÇŃ °łĽö ¸¸Ĺ­ »ěĆěş»´Ů"
		int nReqCount = SIZE_MAX_CALLGM_BRING_COUNT;
		token = strtok(NULL, seps);
		if(token)
		{
			nReqCount = atoi(token);
			nReqCount = min(nReqCount, SIZE_MAX_CALLGM_BRING_COUNT);
			nReqCount = max(nReqCount, 1);
		}

		INIT_MSG_WITH_BUFFER(MSG_IC_ADMIN_CALLGM_VIEW_OK, T_IC_ADMIN_CALLGM_VIEW_OK, pViewOK, SendBuf);
		if(ms_pIMIOCP->CallGMMake_MSG_IC_ADMIN_CALLGM_VIEW_OK(pViewOK, nReqCount))
		{
			SendAddData(SendBuf, MSG_SIZE(MSG_IC_ADMIN_CALLGM_VIEW_OK) + sizeof(SCALLGM)*pViewOK->nCallGMCount);
		}
		return TRUE;
	}
	else if(0 == stricmp(token, STRCMD_CS_COMMAND_BRINGCALLGM_0)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_BRINGCALLGM_1)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_BRINGCALLGM_2))
	{// 2006-05-08 by cmkwon
		if(FALSE == CheckCommandExecutability(RACE_OPERATION|RACE_GAMEMASTER))
		{// 2006-05-08 by cmkwon, °ü¸®ŔÚ/żîżµŔÚ¸¸ »çżë °ˇ´É
			return TRUE;
		}

		// "format: /µµżěąĚ°ˇÁ®żŔ±â [|°łĽö(1~10)] - GM »ó´ă żäĂ» ¸®˝şĆ®¸¦ ÁöÁ¤ÇŃ °łĽö ¸¸Ĺ­ °ˇÁ®żÂ´Ů(Ľ­ąöżˇĽ­ »čÁ¦µĘ)"
		int nReqCount = SIZE_MAX_CALLGM_BRING_COUNT;
		token = strtok(NULL, seps);
		if(token)
		{
			nReqCount = atoi(token);
			nReqCount = min(nReqCount, SIZE_MAX_CALLGM_BRING_COUNT);
			nReqCount = max(nReqCount, 1);
		}

		INIT_MSG_WITH_BUFFER(MSG_IC_ADMIN_CALLGM_BRING_OK, T_IC_ADMIN_CALLGM_BRING_OK, pBringOK, SendBuf);
		if(ms_pIMIOCP->CallGMMake_MSG_IC_ADMIN_CALLGM_BRING_OK(pBringOK, nReqCount))
		{
			SendAddData(SendBuf, MSG_SIZE(MSG_IC_ADMIN_CALLGM_BRING_OK) + sizeof(SCALLGM)*pBringOK->nCallGMCount);
		}
		return TRUE;
	}
	else if(0 == stricmp(token, STRCMD_CS_COMMAND_PrepareShutdown_0)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_PrepareShutdown_1)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_PrepareShutdown_2))
	{// 2007-08-27 by cmkwon, Ľ­ąö´ŮżîÁŘşń ¸í·Éľî Ăß°ˇ(SCAdminToolżˇĽ­ SCMonitorŔÇ PrepareShutdownŔ» ÁřÇŕ ÇŇ Ľö ŔÖ°Ô)
		if (FALSE == CheckCommandExecutability(RACE_OPERATION, TRUE) )
		{// 2008-08-18 by dhjin, °ü¸®ŔÚ¸¸ »çżë°ˇ´É // °ü¸®ŔÚ/żîżµŔÚ/SCAdminTool¸¸ »çżë °ˇ´É
			return TRUE;
		}
		
		// "format: /[Ľ­ąö´ŮżîÁŘşń|PrepareShutdown|PrepareShutdown] [Start|Release] - Ľ­ąö Áľ·á ÁŘşń, ¸đµç ŔŻŔú ż¬°á °­Á¦ Áľ·áµĘ"
		token = strtok(NULL, seps);
		if (NULL == token)
		{// Parma1 ľřŔ˝
			SendString128(STRING_128_ADMIN_CMD, STRCMD_CS_COMMAND_PrepareShutdown_HELP);
			return TRUE;
		}

		INIT_MSG_WITH_BUFFER(MSG_FI_CONNECT_PREPARE_SHUTDOWN, T_FI_CONNECT_PREPARE_SHUTDOWN, pPreShutdown, SendBuf);
		if(0 == stricmp(token, STRCMD_CS_COMMAND_PrepareShutdown_P1Start))
		{
			// 2011-11-18 by shcho, Ľ­ąö´Ůżî ÇÁ¸®ĆäľîĽ­ąö´Ůżî Á¦°Ĺ Ăł¸® - Äż¸Çµĺ Ăł¸® Á¦°Ĺ
			 pPreShutdown->bPrepareShutdown		= TRUE;		// TURE¸é Ľ­ąö´Ůżî ÁŘşń ˝ĂŔŰ
		}
		else if(0 == stricmp(token, STRCMD_CS_COMMAND_PrepareShutdown_P1Release))
		{
			pPreShutdown->bPrepareShutdown		= FALSE;	// FALSEŔĚ¸é Ľ­ąö´Ůżî ÁŘşń ÇŘÁ¦
		}
		else
		{
			SendString128(STRING_128_ADMIN_CMD, STRCMD_CS_COMMAND_PrepareShutdown_HELP);
			return TRUE;
		}		
		
		// FieldServer·Î ŔüĽŰÇŃ´Ů.
		ms_pIMIOCP->SendMessageToFieldServer(SendBuf, MSG_SIZE(MSG_FI_CONNECT_PREPARE_SHUTDOWN));

		/*for (int i = 31; i < 0; i--)
		{
			Sleep(1000 * 1000);
			SendString128(STRING_128_ADMIN_CMD, "%d sec left%",i);
		}*/

		return TRUE;
	}
	else if(0 == stricmp(token, STRCMD_CS_COMMAND_STARTCALLGM_0)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_STARTCALLGM_1)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_STARTCALLGM_2))
	{// 2007-11-19 by cmkwon, ÁřÁ¤˝Ă˝şĹŰ ľ÷µĄŔĚĆ® - '/µµżěąĚ˝ĂŔŰ' ¸í·Éľî Ăł¸®
		if(FALSE == CheckCommandExecutability(RACE_OPERATION|RACE_GAMEMASTER))
		{// 2007-11-19 by cmkwon, °ü¸®ŔÚ/żîżµŔÚ¸¸ »çżë °ˇ´É
			return TRUE;
		}

		// "format: /µµżěąĚ˝ĂŔŰ [|˝Ă°Ł(´ÜŔ§:şĐ)] - ÁřÁ¤ ˝Ă˝şĹŰ ˝ĂŔŰ"
		int nTermMinute = 20160;		// 2007-11-19 by cmkwon, 20160şĐ = 336˝Ă°Ł = 14ŔĎ
		token = strtok(NULL, seps);
		if(token)
		{
			if(0 >= atoi(token))
			{
				SendString128(STRING_128_ADMIN_CMD, STRCMD_CS_COMMAND_STARTCALLGM_HELP);
				return TRUE;				
			}

			nTermMinute = atoi(token);
		}

		ATUM_DATE_TIME atSTime(TRUE);
		ATUM_DATE_TIME atETime;

		atETime = atSTime;
		atETime.AddDateTime(0, 0, 0, 0, nTermMinute);
		atETime.Second = 0;

		ms_pIMIOCP->SetCallGMSystemTime(&atSTime, &atETime);
		return TRUE;
	}
	else if(0 == stricmp(token, STRCMD_CS_COMMAND_ENDCALLGM_0)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_ENDCALLGM_1)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_ENDCALLGM_2))
	{// 2007-11-19 by cmkwon, ÁřÁ¤˝Ă˝şĹŰ ľ÷µĄŔĚĆ® - '/µµżěąĚÁľ·á' ¸í·Éľî Ăł¸®
		if(FALSE == CheckCommandExecutability(RACE_OPERATION|RACE_GAMEMASTER))
		{// 2007-11-19 by cmkwon, °ü¸®ŔÚ/żîżµŔÚ¸¸ »çżë °ˇ´É
			return TRUE;
		}

		// "format: /µµżěąĚÁľ·á - ÁřÁ¤ ˝Ă˝şĹŰ Áľ·á"
		ATUM_DATE_TIME atSTime;
		ATUM_DATE_TIME atETime;

		atSTime = ms_pIMIOCP->m_atCallGMStartTime;
		atETime.SetCurrentDateTime(TRUE);
		ms_pIMIOCP->SetCallGMSystemTime(&atSTime, &atETime);
		return TRUE;
	}
	else if(0 == stricmp(token, STRCMD_CS_COMMAND_CHATFORBIDRELEASE_LEADER_0)
		|| 0 == stricmp(token, STRCMD_CS_COMMAND_CHATFORBIDRELEASE_LEADER_1))
	{// 2008-12-30 by cmkwon, ÁöµµŔÚ Ă¤ĆĂ Á¦ÇŃ Ä«µĺ ±¸Çö - ÁöµµŔÚĂ¤±ÝÇŘÁ¦ ¸í·Éľî Ăł¸®
		if(FALSE == CheckCommandExecutability(RACE_OPERATION|RACE_GAMEMASTER, TRUE))
		{// 2008-12-30 by cmkwon, °ü¸®ŔÚ/żîżµŔÚ¸¸ »çżë °ˇ´É
			return TRUE;
		}

		// "format: /ÁöµµŔÚĂ¤±ÝÇŘÁ¦ [CharacterName] - ÁöµµŔÚ Ă¤±Ý ĽłÁ¤ŔĚ ÇŘÁ¦µČ´Ů."
		token = strtok(NULL, seps);
		if (NULL == token)
		{// CharacterName ŔĚ ľřŔ˝
			SendString128(STRING_128_ADMIN_CMD, STRCMD_CS_COMMAND_CHATFORBIDRELEASE_LEADER_HELP);
			return TRUE;
		}

		int nRemainMinutes = 0;
		if(FALSE == ms_pIMIOCP->m_chatBlockManagerByLeader.IsExistChatBlockList(&nRemainMinutes, token))
		{
			SendString128(STRING_128_ADMIN_CMD, "The CharacterName isn't in blocked list.");
			return TRUE;
		}

		// 2008-12-31 by cmkwon, Ă¤±Ý ¸®˝şĆ®żˇĽ­ »čÁ¦
		ms_pIMIOCP->m_chatBlockManagerByLeader.DeleteChatBlockList(token);

		// 2008-12-31 by cmkwon, Á˘ĽÓ ÁßŔÎ ŔŻŔú¸é ¸Ţ˝ĂÁö ŔüĽŰ
		CIMIOCPSocket *pIISock = ms_pIMIOCP->GetIMIOCPSocketByCharacterName(token);
		if(pIISock && pIISock->IsValidCharacter())
		{
			pIISock->SendString128(STRING_128_USER_NOTICE, STRMSG_081230_0003);
		}
		
		return TRUE;
	}
	else if (stricmp(token, STRCMD_CS_COMMAND_EVO_WHO) == 0)
	{
		//send info about all currently online users to client
		if (!CheckCommandExecutability(RACE_OPERATION | RACE_GAMEMASTER))
		{
			return FALSE;
		}

		ms_pIMIOCP->m_MapChactarterUniqueNumber.lock();


		for (const auto& charInfo : ms_pIMIOCP->m_MapChactarterUniqueNumber)
		{
			BYTE buffer[MSG_SIZE(MSG_IC_ADMIN_COMMAND_EVO_WHO_OK)];
			INIT_MSG(MSG_IC_ADMIN_COMMAND_EVO_WHO_OK, T_IC_ADMIN_COMMAND_EVO_WHO_OK, evowhoMsg, buffer);
			evowhoMsg->Character = charInfo.second->m_character;
			SendAddData(buffer, MSG_SIZE(MSG_IC_ADMIN_COMMAND_EVO_WHO_OK));
		}


		ms_pIMIOCP->m_MapChactarterUniqueNumber.unlock();
		return TRUE;
	}
	// ... 
	// Ăß°ˇµÇ´Â Äż¸Çµĺ´Â ż©±â´Ů !! by hskim

	/*
	// start 2011-06-22 by hskim, »çĽł Ľ­ąö ąćÁö
	else
	{
		if( TRUE == ms_pIMIOCP->m_pSecurityManager->ExecCommand(token) )
		{
			return TRUE;
		}
		}
		*/
	// end 2011-06-22 by hskim, »çĽł Ľ­ąö ąćÁö
	
	// ÇĘµĺĽ­ąöżˇµµ ŔüĽŰÇŘľß ÇĎąÇ·Î FALSE¸¦ returnÇŃ´Ů.
	// ÇŘ´ç ¸í·Éľî°ˇ ľř´Â °ćżě --> Field Ľ­ąö°ˇ łŞ¸ÓÁö¸¦ ąŢŔ˝
	return FALSE;
}

void CIMIOCPSocket::DBGOUT2(BitFlag8_t bType, char* pFormat, ...)
{
	if (bType == DO2_NONE)
	{
		return;
	}

	char szLogMsg[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(szLogMsg, pFormat, args);

	if (COMPARE_BIT_FLAG(bType, DO2_DBGOUT)) DBGOUT(szLogMsg);
	if (COMPARE_BIT_FLAG(bType, DO2_SYSLOG)) g_pGlobal->WriteSystemLog(szLogMsg, TRUE);
	if (COMPARE_BIT_FLAG(bType, DO2_STR128)) SendString128(STRING_128_DEBUG_L3, szLogMsg);
}

// error ¸ŢĽĽÁö¸¦ ŔüĽŰ
void CIMIOCPSocket::SendErrorMessage(MessageType_t msgType, Err_t err, int errParam1, int errParam2, char* errMsg, BOOL bCloseConnection)
{
	MSG_ERROR	*pMsgError;
	BYTE		SendBuf[SIZE_MAX_PACKET];

	char szSystemLog[256];
	sprintf(szSystemLog, "  SendErrorMsg to %s %s ==> %s(%#04x) AT %s Param1(%d) Param2(%d) Msg(%s)\r\n",
		GetCharacterString(&m_character, string()), m_szPeerIP,
		GetErrorString(err), err, GetProtocolTypeString(msgType), errParam1, errParam2, errMsg);
	g_pIMGlobal->WriteSystemLog(szSystemLog);
#ifdef _DEBUG
//	if (err != ERR_PROTOCOL_INVALID_MAP_EVENT_INFO)
	{
		DBGOUT(szSystemLog);
	}
#endif

	*(MessageType_t*)SendBuf = T_ERROR;
	pMsgError = (MSG_ERROR*)(SendBuf+SIZE_FIELD_TYPE_HEADER);
	pMsgError->CloseConnection = bCloseConnection;
	pMsgError->ErrorCode = err;
	pMsgError->MsgType = msgType;
	pMsgError->ErrParam1 = errParam1;
	pMsgError->ErrParam2 = errParam2;
	if (errMsg == NULL)
	{
		pMsgError->StringLength = 0;
	}
	else
	{
		pMsgError->StringLength = (USHORT)(strlen(errMsg) + 1);
		STRNCPY_MEMSET((char*)SendBuf + MSG_SIZE(MSG_ERROR), errMsg, SIZE_MAX_ERROR_STRING);
	}
	SendAddData(SendBuf, MSG_SIZE(MSG_ERROR) + pMsgError->StringLength);
}

BOOL CIMIOCPSocket::IsExistPremiumCard()
{
	if(FALSE != m_MembershipInfo.nCardItemNum1)
	{
		return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CIMIOCPSocket::SendFieldCreateGuildBySubLeader()
/// \brief		şÎÁöµµŔÚ°ˇ ż©´Ü »ýĽş˝Ă ÇĘµĺĽ­ąö·Î Á¤ş¸ ŔüĽŰ
/// \author		dhjin
/// \date		2007-10-06 ~ 2007-10-06
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CIMIOCPSocket::SendFieldCreateGuildBySubLeader()
{
	if (FALSE == this->IsUsing())
	{
		return;
	}

	INIT_MSG_WITH_BUFFER(MSG_FI_CREATE_GUILD_BY_SUBLEADER, T_FI_CREATE_GUILD_BY_SUBLEADER, SendMsg, SendBuf);
	SendMsg->Influence			= this->m_character.InfluenceType;
	SendMsg->CharacterUID		= this->m_character.CharacterUniqueNumber;
	if(ms_pIMIOCP->CheckInfluenceSubLeader1(this->m_character.InfluenceType, this->m_character.CharacterUniqueNumber))
	{
		SendMsg->SubLeaderRank	= 1;
	}
	else if(ms_pIMIOCP->CheckInfluenceSubLeader2(this->m_character.InfluenceType, this->m_character.CharacterUniqueNumber))
	{
		SendMsg->SubLeaderRank	= 2;
	}
	else
	{
		return;
	}

	this->SendMsgToCurrentFieldServerSocket(SendBuf, MSG_SIZE(MSG_FI_CREATE_GUILD_BY_SUBLEADER));
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			void CIMIOCPSocket::SendPartyListInfo(PartyID_t i_nStartNum)
/// \brief		EP3 Ćí´ë ĽöÁ¤ - Ćí´ë Á¤ş¸ ¸®˝şĆ®
/// \author		dhjin
/// \date		2008-06-02 ~ 2008-06-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CIMIOCPSocket::SendPartyListInfo(PartyID_t i_nStartNum)
{
	INIT_MSG_WITH_BUFFER(MSG_IC_PARTY_LIST_INFO_OK, T_IC_PARTY_LIST_INFO_OK, pSMsg, SendBuf);
	pSMsg->PartyInfoListCount	= this->MakeMSG_IC_PARTY_LIST_INFO_OK((SPARTY_LIST_INFO*)(SendBuf+MSG_SIZE(MSG_IC_PARTY_LIST_INFO_OK)), i_nStartNum, &pSMsg->PartyInfoTotalCount);
	if(0 >= pSMsg->PartyInfoListCount)
	{
		this->SendErrorMessage(T_IC_PARTY_LIST_INFO, ERR_PROTOCOL_NO_SUCH_PARTY);
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 2008-06-02 by dhjin, Ćí´ë ¸ń·Ď ŔüĽŰ
	this->SendAddData(SendBuf, MSG_SIZE(MSG_IC_PARTY_LIST_INFO_OK)+pSMsg->PartyInfoListCount*sizeof(SPARTY_LIST_INFO));
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			INT	 CIMIOCPSocket::MakeMSG_IC_PARTY_LIST_INFO_OK(SPARTY_LIST_INFO * o_pPartyListInfo, PartyID_t i_nStartNum, INT * o_nTotalCount)
/// \brief		EP3 Ćí´ë ĽöÁ¤ - Ćí´ë Á¤ş¸ ¸®˝şĆ®	
/// \author		dhjin
/// \date		2008-06-02 ~ 2008-06-02
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
INT	 CIMIOCPSocket::MakeMSG_IC_PARTY_LIST_INFO_OK(SPARTY_LIST_INFO * o_pPartyListInfo, PartyID_t i_nStartNum, INT * o_nTotalCount)
{
	INT	nPartyListCount	= 0;
	INT	Count = 0;
	
	ms_pIMIOCP->m_MapParties.lock();

	if(ms_pIMIOCP->m_MapParties.empty())
	{
		ms_pIMIOCP->m_MapParties.unlock();
		return nPartyListCount;
	}
	
	mt_map<PartyID_t, CIMParty*>::iterator itr = ms_pIMIOCP->m_MapParties.begin();
	for(; itr != ms_pIMIOCP->m_MapParties.end(); itr++)
	{
		if(this->m_character.InfluenceType != itr->second->m_SPartyInfo.PartyInfluence
			|| PARTY_JOIN_TYPE_INVITE_MASTER == itr->second->m_SPartyInfo.PartyJoinType)
		{
			continue;
		}

		(*o_nTotalCount)++;
	}

	itr = ms_pIMIOCP->m_MapParties.begin();
	for(; itr != ms_pIMIOCP->m_MapParties.end(); itr++)
	{
		if(i_nStartNum >= itr->first
			|| Count >= 13		// 2008-07-18 by cmkwon, ŔÓ˝Ă·Î ąö±× ĽöÁ¤
			|| this->m_character.InfluenceType != itr->second->m_SPartyInfo.PartyInfluence
			|| PARTY_JOIN_TYPE_INVITE_MASTER == itr->second->m_SPartyInfo.PartyJoinType)
		{
			continue;
		}

		o_pPartyListInfo[nPartyListCount].PartyNum				=   itr->first;
		o_pPartyListInfo[nPartyListCount].ExpDistributeType		=	itr->second->m_SPartyInfo.ExpDistributeType;
		o_pPartyListInfo[nPartyListCount].ItemDistributeType	=	itr->second->m_SPartyInfo.ItemDistributeType;
		o_pPartyListInfo[nPartyListCount].MinLevel				=	itr->second->m_SPartyInfo.MinLevel;	
		o_pPartyListInfo[nPartyListCount].MaxLevel				=	itr->second->m_SPartyInfo.MaxLevel;
		o_pPartyListInfo[nPartyListCount].PartyLock				=	itr->second->m_SPartyInfo.PartyLock;
		STRNCPY_MEMSET(o_pPartyListInfo[nPartyListCount].PartyName, itr->second->m_SPartyInfo.PartyName, SIZE_MAX_PARTY_NAME);
		STRNCPY_MEMSET(o_pPartyListInfo[nPartyListCount].PartyPW,	itr->second->m_SPartyInfo.PartyPW, SIZE_MAX_TEAM_PW);
		STRNCPY_MEMSET(o_pPartyListInfo[nPartyListCount].PartyMasterName,	itr->second->GetMasterCharacterName(), SIZE_MAX_CHARACTER_NAME);
		o_pPartyListInfo[nPartyListCount].Membercount			=	itr->second->GetMemberCount(FALSE);
		o_pPartyListInfo[nPartyListCount].VoipCheck				=	itr->second->CheckVoipUse();

		nPartyListCount++;
		Count++;
	}
	ms_pIMIOCP->m_MapParties.unlock();	

	return nPartyListCount;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CIMIOCPSocket::SendVoipType(EN_CHECK_TYPE i_NewVoipType)
/// \brief		EP3 VOIP -
/// \author		dhjin
/// \date		2008-06-17 ~ 2008-06-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CIMIOCPSocket::SendVoipType(EN_CHECK_TYPE i_NewVoipType)
{
	switch(i_NewVoipType)
	{
	case CHECK_TYPE_VOIP_PARTY:
		{
			CIMParty *ptmParty = this->GetIMParty();
			if (ptmParty == NULL)
			{
				SendErrorMessage(T_IC_VOIP_SET, ERR_PROTOCOL_NO_SUCH_PARTY);
				return;
			}
			ptmParty->LockIMParty();
			INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_SET_OK, T_IC_VOIP_SET_OK, pSendMsg, pSendBuf);
			pSendMsg->CharacterUID	= this->m_character.CharacterUniqueNumber;
			pSendMsg->VoipType		= CHECK_TYPE_VOIP_PARTY;
			ptmParty->SendMsgToMembers(pSendBuf, MSG_SIZE(MSG_IC_VOIP_SET_OK));
			ptmParty->UnlockIMParty();			
		}
		break;
	case CHECK_TYPE_VOIP_GUILD:
		{
			ms_pIMIOCP->m_MapGuild.lock();

			CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
			if(NULL == ptmGuild)
			{
				ms_pIMIOCP->m_MapGuild.unlock();
				SendErrorMessage(T_IC_VOIP_SET, ERR_PROTOCOL_NOT_HAVE_GUILD);
				return;
			}
			
//			if (m_character.CharacterUniqueNumber != ptmGuild->m_CommanderUniqueNumber)
//			{
//				ms_pIMIOCP->m_MapGuild.unlock();				
//				SendErrorMessage(T_IC_VOIP_SET, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
//				return;
//			}

			ptmGuild->SetMemberVoipType(m_character.CharacterUniqueNumber, CHECK_TYPE_VOIP_GUILD);				// 2008-07-16 by dhjin, EP3 - Voip Á¤ş¸
			
			INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_SET_OK, T_IC_VOIP_SET_OK, pSMsg, pBuffer);
			pSMsg->CharacterUID	= this->m_character.CharacterUniqueNumber;
			pSMsg->VoipType		= CHECK_TYPE_VOIP_GUILD;
			ptmGuild->SendMsgToGuildMembers(pBuffer, MSG_SIZE(MSG_IC_VOIP_SET_OK));

			ms_pIMIOCP->m_MapGuild.unlock();
		}
		break;
	default:
		{
		}
	}
	
	if(CHECK_TYPE_VOIP_NONE == i_NewVoipType)
	{
		switch(this->m_VoipType)
		{
		case CHECK_TYPE_VOIP_PARTY:
			{
				CIMParty *ptmParty = this->GetIMParty();
				if (ptmParty == NULL)
				{
					SendErrorMessage(T_IC_VOIP_SET, ERR_PROTOCOL_NO_SUCH_PARTY);
					return;
				}
				ptmParty->LockIMParty();
				INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_SET_OK, T_IC_VOIP_SET_OK, pSendMsg, pSendBuf);
				pSendMsg->CharacterUID	= this->m_character.CharacterUniqueNumber;
				pSendMsg->VoipType		= CHECK_TYPE_VOIP_NONE;
				ptmParty->SendMsgToMembers(pSendBuf, MSG_SIZE(MSG_IC_VOIP_SET_OK));
				ptmParty->UnlockIMParty();			
			}
			break;
		case CHECK_TYPE_VOIP_GUILD:
			{
				ms_pIMIOCP->m_MapGuild.lock();

				CGuild *ptmGuild = ms_pIMIOCP->m_MapGuild.findLock(m_uidGuildUniqueNumber);
				if(NULL == ptmGuild)
				{
					ms_pIMIOCP->m_MapGuild.unlock();
					SendErrorMessage(T_IC_VOIP_SET, ERR_PROTOCOL_NOT_HAVE_GUILD);
					return;
				}
				
//				if (m_character.CharacterUniqueNumber != ptmGuild->m_CommanderUniqueNumber)
//				{
//					ms_pIMIOCP->m_MapGuild.unlock();				
//					SendErrorMessage(T_IC_VOIP_SET, ERR_PROTOCOL_GUILD_PERMISSION_DENIED);
//					return;
//				}
				
				INIT_MSG_WITH_BUFFER(MSG_IC_VOIP_SET_OK, T_IC_VOIP_SET_OK, pSMsg, pBuffer);
				pSMsg->CharacterUID	= this->m_character.CharacterUniqueNumber;
				pSMsg->VoipType		= CHECK_TYPE_VOIP_NONE;
				ptmGuild->SendMsgToGuildMembers(pBuffer, MSG_SIZE(MSG_IC_VOIP_SET_OK));

				ms_pIMIOCP->m_MapGuild.unlock();
			}
			break;
		default:
			{
			}
		}		
	}

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CIMIOCPSocket::SendChatRoomListInfo(INT i_nStartNum)
/// \brief		EP3 Ă¤ĆĂąć -
/// \author		dhjin
/// \date		2008-06-17 ~ 2008-06-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CIMIOCPSocket::SendChatRoomListInfo(INT i_nStartNum)
{
	INIT_MSG_WITH_BUFFER(MSG_IC_CHATROOM_LIST_INFO_OK, T_IC_CHATROOM_LIST_INFO_OK, pSMsg, SendBuf);
	pSMsg->ChatRoomInfoListCount	= this->MakeMSG_IC_CHATROOM_LIST_INFO_OK((SCHATROOM_LIST_INFO*)(SendBuf+MSG_SIZE(MSG_IC_CHATROOM_LIST_INFO_OK)), i_nStartNum, &pSMsg->ChatRoomInfoTotalCount);
	if(0 >= pSMsg->ChatRoomInfoListCount)
	{
		this->SendErrorMessage(T_IC_CHATROOM_LIST_INFO, ERR_CHATROOM_NO_SUCH);
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 2008-06-02 by dhjin, Ćí´ë ¸ń·Ď ŔüĽŰ
	this->SendAddData(SendBuf, MSG_SIZE(MSG_IC_CHATROOM_LIST_INFO_OK)+pSMsg->ChatRoomInfoListCount*sizeof(SCHATROOM_LIST_INFO));
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			INT	CIMIOCPSocket::MakeMSG_IC_CHATROOM_LIST_INFO_OK(SCHATROOM_LIST_INFO * o_pChatRoomListInfo, INT i_nStartNum, INT * o_nTotalCount)
/// \brief		EP3 Ă¤ĆĂąć -
/// \author		dhjin
/// \date		2008-06-17 ~ 2008-06-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
INT	CIMIOCPSocket::MakeMSG_IC_CHATROOM_LIST_INFO_OK(SCHATROOM_LIST_INFO * o_pChatRoomListInfo, INT i_nStartNum, INT * o_nTotalCount)
{
	INT	nChatRoomListCount	= 0;
	INT	Count = 0;
	
	mt_auto_lock mta(&(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom));

	if(ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.empty())
	{
		return nChatRoomListCount;
	}
	
	// °°Ŕş ĽĽ·Â ŔüĂĽ Ă¤ĆĂąć Ľö 
	mtListChatRoom::iterator itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
	{
		if(this->m_character.InfluenceType != (*itr)->GetChatRoomInfluenceType())
		{
			continue;
		}

		(*o_nTotalCount)++;
	}

	// °°Ŕş ĽĽ·Â ˝ĂŔŰ Ă¤ĆĂąć ş¸ł»±â Ŕ§ÇŘ ĂłŔ˝ ş¸łľ ąć »ýĽş ˝Ă°ŁŔ¸·Î ĂŁ±â
	ATUM_DATE_TIME CheckTime;
	itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	if(i_nStartNum)
	{
		CheckTime = *((*itr)->GetChatRoomCreateTime());
	}
	else
	{
		for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
		{
			if(i_nStartNum == (*itr)->GetChatRoomNum())
			{
				CheckTime = *((*itr)->GetChatRoomCreateTime());
				break;
			}
		}
	}


	// ˝Ă°ŁŔ¸·Î şń±łÇĎż© Ă¤ĆĂąć ¸ń·Ď ş¸ł»±â
	itr = ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.begin();
	for(; itr != ms_pIMIOCP->m_ChatRoomManager.m_mtlistChatRoom.end(); itr++)
	{
		if(0 < CheckTime.GetTimeDiffTimeInSeconds(*((*itr)->GetChatRoomCreateTime()))
			|| Count >= ARENA_MAX_REQUEST_TEAM
			|| this->m_character.InfluenceType != (*itr)->GetChatRoomInfluenceType()
			)
		{
			continue;
		}

		o_pChatRoomListInfo[nChatRoomListCount].ChatRoomNum				=   (*itr)->GetChatRoomNum();
		o_pChatRoomListInfo[nChatRoomListCount].ChatRoomLock				=	(*itr)->GetChatRoomLock();
		o_pChatRoomListInfo[nChatRoomListCount].Membercount				=	(*itr)->GetChatRoomMemberCount();	
		o_pChatRoomListInfo[nChatRoomListCount].MaxMembercount				=	(*itr)->GetChatRoomMaxMemberCount();
		STRNCPY_MEMSET(o_pChatRoomListInfo[nChatRoomListCount].ChatRoomName, (*itr)->GetChatRoomName(), SIZE_MAX_CHATROOM_NAME);
		STRNCPY_MEMSET(o_pChatRoomListInfo[nChatRoomListCount].ChatRoomMasterName,	(*itr)->GetChatRoomMasterCharacterName(), SIZE_MAX_CHARACTER_NAME);

		nChatRoomListCount++;
		Count++;
	}

	return nChatRoomListCount;	
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		// 2009-08-03 by cmkwon, EP3-4 Ćí´ë ´ëÇü ˝şĹł ±¸Çö - 
/// \author		cmkwon
/// \date		2009-08-12 ~ 2009-08-12
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CIMIOCPSocket::SendFieldServPartyChangeFormationSkill(UID32_t i_PartyMasterCharcUID, BOOL i_bOnFlag, BYTE i_byFormationTy/*=FLIGHT_FORM_NONE*/)
{	
	INIT_MSG_WITH_BUFFER(MSG_FI_PARTY_CHANGE_FORMATION_SKILL, T_FI_PARTY_CHANGE_FORMATION_SKILL, pSendFS, SendBuf);
	pSendFS->PartyMasterCharcUID	= i_PartyMasterCharcUID;
	pSendFS->Is_On_or_Off			= i_bOnFlag;
	pSendFS->FlightFormation		= i_byFormationTy;
	ms_pIMIOCP->SendMessageToFieldServer(SendBuf, MSG_SIZE(MSG_FI_PARTY_CHANGE_FORMATION_SKILL));
}