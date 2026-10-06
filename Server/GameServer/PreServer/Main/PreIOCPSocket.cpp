// PreIOCPSocket.cpp: implementation of the CPreIOCPSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PreIOCPSocket.h"
#include "PreIOCP.h"
#include "PreGlobal.h"
#include "AtumError.h"
#include "AccountBlockManager.h"
#include "sha256.h"
#include <time.h>
#include <fstream>
//#include "SecurityManager.h"		// 2011-06-22 by hskim, 사설 서버 방지


CPreIOCP		*CPreIOCPSocket::ms_pPreIOCP = NULL;

static void AceTRSha256(const unsigned char* data, unsigned int len, unsigned char out[32])
{
	sha256_context ctx;
	sha256_starts(&ctx);
	sha256_update(&ctx, (uint8*)data, len);
	sha256_finish(&ctx, out);
}

static void AceTRHmacSha256(const unsigned char* key, unsigned int keyLen,
	const unsigned char* data, unsigned int dataLen, unsigned char out[32])
{
	unsigned char keyBlock[64];
	unsigned char innerPad[64];
	unsigned char outerPad[64];
	MEMSET_ZERO(keyBlock, sizeof(keyBlock));

	if (keyLen > 64)
	{
		AceTRSha256(key, keyLen, keyBlock);
	}
	else if (keyLen > 0)
	{
		memcpy(keyBlock, key, keyLen);
	}

	for (int i = 0; i < 64; ++i)
	{
		innerPad[i] = keyBlock[i] ^ 0x36;
		outerPad[i] = keyBlock[i] ^ 0x5c;
	}

	sha256_context ctx;
	unsigned char innerHash[32];
	sha256_starts(&ctx);
	sha256_update(&ctx, innerPad, 64);
	sha256_update(&ctx, (uint8*)data, dataLen);
	sha256_finish(&ctx, innerHash);

	sha256_starts(&ctx);
	sha256_update(&ctx, outerPad, 64);
	sha256_update(&ctx, innerHash, 32);
	sha256_finish(&ctx, out);
}

static void AceTRHex(const unsigned char* data, int len, char* out, int outSize)
{
	static const char* HEX = "0123456789abcdef";
	if (outSize < (len * 2 + 1)) return;
	for (int i = 0; i < len; ++i)
	{
		out[i * 2] = HEX[(data[i] >> 4) & 0x0F];
		out[i * 2 + 1] = HEX[data[i] & 0x0F];
	}
	out[len * 2] = 0;
}

static BOOL AceTRLoadLauncherSecret(char* outSecret, int outSize)
{
	MEMSET_ZERO(outSecret, outSize);

	DWORD envLen = GetEnvironmentVariableA("ACETR_LAUNCHER_API_SECRET", outSecret, outSize);
	if (envLen > 0 && envLen < (DWORD)outSize)
		return TRUE;

	// Prefer a secret placed next to PreServer.exe so the result does not
	// depend on the process working directory.
	char exePath[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, exePath, MAX_PATH);
	char* slash = strrchr(exePath, '\\');
	if (slash != NULL)
	{
		*(slash + 1) = 0;
		strncat(exePath, "launcher_api_secret.txt", MAX_PATH - strlen(exePath) - 1);

		std::ifstream exeSecret(exePath);
		if (exeSecret.is_open())
		{
			std::string line;
			std::getline(exeSecret, line);
			exeSecret.close();
			if (!line.empty() && (int)line.size() < outSize)
			{
				STRNCPY_MEMSET(outSecret, line.c_str(), outSize);
				return TRUE;
			}
		}
	}

	// Backward-compatible fallback for existing deployments.
	std::ifstream secretFile("launcher_api_secret.txt");
	if (!secretFile.is_open())
		return FALSE;

	std::string line;
	std::getline(secretFile, line);
	secretFile.close();
	if (line.empty() || (int)line.size() >= outSize)
		return FALSE;

	STRNCPY_MEMSET(outSecret, line.c_str(), outSize);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPreIOCPSocket::CPreIOCPSocket()
{
	SetClientState(CP_NOTCONNECTED, NULL);
	m_PeerSocketType	= ST_INVALID_TYPE;
	MEMSET_ZERO(m_szConnectedServerGroupName, SIZE_MAX_SERVER_NAME);

	m_eOtherPublisherConncect = CONNECT_PUBLISHER_DEFAULT;				// 2010-11 by dhjin, 아라리오 채널링 로그인.
	m_bHasSubmitMac = false;
}

CPreIOCPSocket::~CPreIOCPSocket()
{
}

BOOL CPreIOCPSocket::OnRecvdPacketPreServer(const char* pPacket, int nLength, BYTE nSeq, char* pPeerIP, int nPeerPort, SThreadInfo *i_pThreadInfo)
{
	BYTE			SendBuf[SIZE_MAX_PACKET];
	int				nBytesUsed		= 0;
	MessageType_t	nRecvType		= 0;
	ProcessResult	procRes			= RES_BREAK;
	int				tmpSeq;
	MessageType_t	nOldRecvType	= 0;			// 2007-04-02 by cmkwon

	// TCP 처리 루틴
	if(m_bPeerSequenceNumberInitFlag == FALSE)
	{
		tmpSeq = (nSeq + SEQNO_VAR_A) * SEQNO_VAR_B;
		if(tmpSeq > SEQNO_VAR_C)
		{
			tmpSeq = tmpSeq % SEQNO_VAR_C;
		}
		m_byPeerSequenceNumber = ++tmpSeq;
		m_bPeerSequenceNumberInitFlag = TRUE;
	}
	else
	{
		if(m_byPeerSequenceNumber != nSeq)
		{
			// Protocl Error 처리
			// - 받은 패킷의 Sequence Number가 유효하지 않음
			// Error Code : ERR_PROTOCOL_INVALID_SEQUENCE_NUMBER
			SendErrorMessage(T_PRE_IOCP, ERR_PROTOCOL_INVALID_SEQUENCE_NUMBER);
			Close(0x11000, TRUE);
			return FALSE;
		}
		tmpSeq = (nSeq + SEQNO_VAR_A) * SEQNO_VAR_B;
		if(tmpSeq > SEQNO_VAR_C)
		{
			tmpSeq = tmpSeq % SEQNO_VAR_C;
		}
		m_byPeerSequenceNumber = ++tmpSeq;
	} // end TCP 처리 루틴


	while(this->IsUsing() && nBytesUsed < nLength)
	{
		nRecvType = *(MessageType_t*)(pPacket+nBytesUsed);
		nBytesUsed += SIZE_FIELD_TYPE_HEADER;

		if(i_pThreadInfo)
		{
			i_pThreadInfo->dwSocketIndex = m_nClientArrayIndex;
			i_pThreadInfo->dwMessageType = nRecvType;
		}


#ifdef _DEBUG
		PrintExchangeMsg(RECV_TYPE, nRecvType, m_szPeerIP, m_PeerSocketType, GGetexchangeMsgPrintLevel());
#endif

		// 업데이트중일 때는 잠시 서비스를 중단한다.
		if(ms_pPreIOCP->m_bPauseService &&
			(HIBYTE(nRecvType) == T0_PC_CONNECT || HIBYTE(nRecvType) == T0_PC_DEFAULT_UPDATE)
		)
		{
			// 2008-06-05 by cmkwon, AdminTool, Monitor 접근 가능 IP를 server config file 에 설정하기 - 아래와 같이 수정 함
			//if(FALSE == IS_SCADMINTOOL_CONNECTABLE_IP(this->GetPeerIP()))
			if(FALSE == g_pPreGlobal->CheckAllowedToolIP(this->GetPeerIP()))	// 2008-06-05 by cmkwon, AdminTool, Monitor 접근 가능 IP를 server config file 에 설정하기 - 
			{
				SendErrorMessage(T_ERROR, ERR_COMMON_SERVICE_TEMPORARILY_PAUSED, 0, 0, this->GetPeerIP());
				return TRUE;
			}
		}

		switch(nRecvType)
		{
		///////////////////////////////////////////////////////////////////////
		// Pre Server <-- Client, PreServer <-- FieldServer, PreServer <-- IMServer, PreServer <-- MonitorServer
		case T_PC_CONNECT_ALIVE:
		case T_FP_CONNECT_ALIVE:
		case T_IP_CONNECT_ALIVE:
		case T_PM_CONNECT_ALIVE:
		case T_IM_CONNECT_ALIVE:
		case T_FM_CONNECT_ALIVE:
		case T_NM_CONNECT_ALIVE:
			OnRecvdAlivePacket();
			procRes = RES_RETURN_TRUE;
			break;
		//////////////////////////////////////////////////////////////////
		// Server <-- AdminTool
		case T_PA_ADMIN_CONNECT:
			{
				MSG_PA_ADMIN_CONNECT* msgAdminConnect = (MSG_PA_ADMIN_CONNECT*)(pPacket + nBytesUsed);		// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈 (추가 기존 버그 수정)
				nBytesUsed += sizeof(MSG_PA_ADMIN_CONNECT);		// 2011-07-21 by hskim, 인증 서버 구현 - 기존 서버와 호환 안되도록 구조체 크기 바꿈 (추가 기존 버그 수정)
				
				// 인증하기
				INIT_MSG(MSG_PA_ADMIN_CONNECT_OK, T_PA_ADMIN_CONNECT_OK, msgAdminConnectOK, SendBuf);
				// 2008-06-05 by cmkwon, AdminTool, Monitor 접근 가능 IP를 server config file 에 설정하기 - 아래와 같이 수정 함
				//if(IS_SCADMINTOOL_CONNECTABLE_IP(GetPeerIP()))
				if(g_pPreGlobal->CheckAllowedToolIP(this->GetPeerIP()))		// 2008-06-05 by cmkwon, AdminTool, Monitor 접근 가능 IP를 server config file 에 설정하기 - 
				{
					STRNCPY_MEMSET(m_szAdminAccountName, msgAdminConnect->UID, SIZE_MAX_ACCOUNT_NAME);		// 2007-06-20 by cmkwon, 계정 블럭정보 시스템 로그에 추가
					msgAdminConnectOK->AccountType0 = g_pGlobalGameServer->AuthAdminToolUser(msgAdminConnect->UID, msgAdminConnect->PWD);
				}
				ifstream fileInput;
				string line;

				fileInput.open("SupportedAT_versions.txt");
				bool bAllowedInFile = false;
				g_pPreGlobal->WriteSystemLogEX(TRUE, "[INET_AT_VERSIONS] AT connecting! Version: (%d) \r\n", msgAdminConnect->Padding);
				char szTmpVersion[20]; *szTmpVersion = '\0';
				sprintf(szTmpVersion,"%d", msgAdminConnect->Padding);
				for (unsigned int curLine = 0; getline(fileInput, line); curLine++) {
					if (line.find(szTmpVersion) != string::npos) {
						g_pPreGlobal->WriteSystemLogEX(TRUE, "[INET_AT_VERSIONS] Version allowed %s!\r\n", szTmpVersion);
						bAllowedInFile = true;
						break;
					}
				}
				fileInput.close();
				if (!bAllowedInFile || msgAdminConnect->Padding == 0) //check for AT version			
				{				
					msgAdminConnectOK->AccountType0 = 0;
					msgAdminConnectOK->Padding = 1;
					g_pPreGlobal->WriteSystemLogEX(TRUE, "[INET_AT_VERSIONS] Not allowed Admin Tool version! %s, IP(%s) ver(%s)!\r\n", msgAdminConnect->UID, GetPeerIP(), szTmpVersion);
				}
				///////////////////////////////////////////////////////////////////////////////
				// 2007-11-01 by cmkwon, 시스템 로그 추가
				g_pGlobal->WriteSystemLogEX(TRUE, "[Notify] SCAdminTool connected !!, AccountName(%s) IP(%s) AccountType(%d)\r\n"
													, msgAdminConnect->UID, GetPeerIP(), msgAdminConnectOK->AccountType0);

				SendAddData(SendBuf, MSG_SIZE(MSG_PA_ADMIN_CONNECT_OK));

//				if (!bRet)
//				{
//					procRes = RES_RETURN_FALSE;
//					Close();
//				}

				// 인증 성공
				m_PeerSocketType = ST_ADMIN_TOOL;

				ms_pPreIOCP->InsertMonitorIOCPSocketPtr(this);
			}
			break;
		case T_PA_ADMIN_GET_ACCOUNT_INFO:
			{
				MSG_PA_ADMIN_GET_ACCOUNT_INFO* msgAdminGetCharac = (MSG_PA_ADMIN_GET_ACCOUNT_INFO*)(pPacket + nBytesUsed);
				nBytesUsed += sizeof(MSG_PA_ADMIN_GET_ACCOUNT_INFO);

				INIT_MSG(MSG_PA_ADMIN_GET_ACCOUNT_INFO_OK, T_PA_ADMIN_GET_ACCOUNT_INFO_OK, msgAdminGetCharacOK, SendBuf);

				CAccountInfo tmAccountInfo;
				msgAdminGetCharacOK->IsBlocked			= ms_pPreIOCP->m_pAccountBlockManager->IsCheckBlockedAccountInfoByAccountName(&msgAdminGetCharacOK->BlockedAccountInfo, msgAdminGetCharac->AccountName, this);
				if (FALSE == ms_pPreIOCP->GetAccountInfo(&tmAccountInfo, msgAdminGetCharac->AccountName))
				{
					msgAdminGetCharacOK->IsOnline		= FALSE;
					STRNCPY_MEMSET(msgAdminGetCharacOK->AccountInfo.AccountName, msgAdminGetCharac->AccountName, SIZE_MAX_ACCOUNT_NAME);
				}
				else
				{
					msgAdminGetCharacOK->IsOnline		= TRUE;
					msgAdminGetCharacOK->AccountInfo	= tmAccountInfo;
				}
				SendAddData(SendBuf, MSG_SIZE(MSG_PA_ADMIN_GET_ACCOUNT_INFO_OK));
			}
			break;
// 2005-06-02 by cmkwon, 사용하지 않는 Protocol Type
//		case T_PA_ADMIN_DISCONNECT_USER:
//			{
//				MSG_PA_ADMIN_DISCONNECT_USER* msgDisconnect = (MSG_PA_ADMIN_DISCONNECT_USER*)(pPacket + nBytesUsed);
//				nBytesUsed += sizeof(MSG_PA_ADMIN_DISCONNECT_USER);
//
//				CAccountInfo tmAccountInfo;
//				if (FALSE == ms_pPreIOCP->GetAccountInfo(&tmAccountInfo, msgDisconnect->AccountName))
//				{
//					INIT_MSG(MSG_PA_ADMIN_GET_ACCOUNT_INFO_OK, T_PA_ADMIN_GET_ACCOUNT_INFO_OK, msgAdminGetCharacOK, SendBuf);
//					MEMSET_ZERO(msgAdminGetCharacOK, sizeof(MSG_PA_ADMIN_GET_ACCOUNT_INFO_OK));
//					msgAdminGetCharacOK->IsOnline = FALSE;
//					msgAdminGetCharacOK->IsBlocked = (ms_pPreIOCP->m_setBlockedAccounts.findLock_Ptr(msgDisconnect->AccountName) != NULL);
//					SendAddData(SendBuf, MSG_SIZE(MSG_PA_ADMIN_GET_ACCOUNT_INFO_OK));
//				}
//				else
//				{
//					if (!ms_pPreIOCP->BlockAccount(msgDisconnect->AccountName, this, TRUE))
//					{
//						// error
//					}
//				}
//			}
//			break;
		case T_PA_ADMIN_BLOCK_ACCOUNT:
			{
				MSG_PA_ADMIN_BLOCK_ACCOUNT* msgBlockAcc = (MSG_PA_ADMIN_BLOCK_ACCOUNT*)(pPacket + nBytesUsed);
				nBytesUsed += sizeof(MSG_PA_ADMIN_BLOCK_ACCOUNT);

				// 2011-11-18 by shcho, 서버다운 프리페어서버다운 제거 처리 - 블럭 언블럭도 툴 IP를 체크하도록 한다.
				if(FALSE ==g_pPreGlobal->CheckAllowedToolIP(this->GetPeerIP()))
				{
					g_pPreGlobal->WriteSystemLogEX(TRUE, "HACKUSER!! Connect Account Block Command Using: HackingIP(%15s) AdminAccountName(%20s), BlockedUserAccName(%20s) \r\n"
						, this->GetPeerIP(), m_szAdminAccountName, msgBlockAcc->szBlockedAccountName);
					break;
				}
				// end 2011-11-18 by shcho, 서버다운 프리페어서버다운 제거 처리 - 블럭 언블럭도 툴 IP를 체크하도록 한다.

				///////////////////////////////////////////////////////////////////////////////
				// 2007-06-20 by cmkwon, 계정 블럭정보 시스템 로그에 추가
				g_pPreGlobal->WriteSystemLogEX(TRUE, "[Notify] Account Block: AdminIP(%15s) AdminAccountName(%20s), BlockedUserAccName(%20s) Period(%s ~ %s)\r\n"
					, this->GetPeerIP(), m_szAdminAccountName, msgBlockAcc->szBlockedAccountName
					, msgBlockAcc->atimeStartTime.GetDateTimeString(STRNBUF(SIZE_MAX_ATUM_DATE_TIME_STRING))
					, msgBlockAcc->atimeEndTime.GetDateTimeString(STRNBUF(SIZE_MAX_ATUM_DATE_TIME_STRING)));

				ms_pPreIOCP->BlockAccount(msgBlockAcc, this, TRUE);
			}
			break;
		case T_PA_ADMIN_UNBLOCK_ACCOUNT:
			{
				MSG_PA_ADMIN_UNBLOCK_ACCOUNT* msgUnblockAcc = (MSG_PA_ADMIN_UNBLOCK_ACCOUNT*)(pPacket + nBytesUsed);
				nBytesUsed += sizeof(MSG_PA_ADMIN_UNBLOCK_ACCOUNT);

				// 2011-11-18 by shcho, 서버다운 프리페어서버다운 제거 처리 - 블럭 언블럭도 툴 IP를 체크하도록 한다.
				if(FALSE==g_pPreGlobal->CheckAllowedToolIP(this->GetPeerIP()))
				{
					g_pPreGlobal->WriteSystemLogEX(TRUE, "HACKUSER!! Connect Account UnBlock Command Using: HackingIP(%15s) AdminAccountName(%20s), UnBlockedUserAccName(%20s)\r\n"
						, this->GetPeerIP(), m_szAdminAccountName, msgUnblockAcc->szBlockedAccountName);
					break;
				}
				// end 2011-11-18 by shcho, 서버다운 프리페어서버다운 제거 처리 - 블럭 언블럭도 툴 IP를 체크하도록 한다.

				///////////////////////////////////////////////////////////////////////////////
				// 2007-06-20 by cmkwon, 계정 블럭정보 시스템 로그에 추가
				g_pPreGlobal->WriteSystemLogEX(TRUE, "[Notify] Account Block Cancellation: AdminIP(%15s) AdminAccountName(%20s), UserAccName(%20s)\r\n"
					, this->GetPeerIP(), m_szAdminAccountName, msgUnblockAcc->szBlockedAccountName);

				msgUnblockAcc->atimeEndTime.SetCurrentDateTime();	// 2008-01-30 by cmkwon, 계정 블럭 로그 남기기 구현 - 
				ms_pPreIOCP->UnblockAccount(msgUnblockAcc, this);
			}
			break;
		case T_PA_ADMIN_GET_SERVER_INFO:
			{
				// no body
				ms_pPreIOCP->SendServerInfoForAdmin(this);
			}
			break;
			////////////////////////////////////////////////////////////////////////////
			// 2012-11-13 by jhseol, 전쟁 시스템 리뉴얼 - 거점전
		case T_PA_ADMIN_STRATRGYPOINT_INFO_CHANGE:
		{
			if (FALSE == g_pPreGlobal->CheckAllowedToolIP(this->GetPeerIP()))
			{
				g_pPreGlobal->WriteSystemLogEX(TRUE, "HACKUSER!! Connect T_PA_ADMIN_STRATRGYPOINT_INFO_CHANGE Command Using: HackingIP(%15s)\r\n", this->GetPeerIP());
				break;
			}
			MSG_PA_ADMIN_STRATRGYPOINT_INFO_CHANGE* msgStratrgyPointInfoChange = (MSG_PA_ADMIN_STRATRGYPOINT_INFO_CHANGE*)(pPacket + nBytesUsed);
			nBytesUsed += sizeof(MSG_PA_ADMIN_STRATRGYPOINT_INFO_CHANGE);

			INIT_MSG_WITH_BUFFER(MSG_FP_ADMIN_STRATRGYPOINT_INFO_CHANGE, T_FP_ADMIN_STRATRGYPOINT_INFO_CHANGE, pSend, SendBuf);
			STRNCPY_MEMSET(pSend->DBName, msgStratrgyPointInfoChange->DBName, sizeof(pSend->DBName));
			ms_pPreIOCP->SendMessageToAllFieldServer(SendBuf, MSG_SIZE(MSG_FP_ADMIN_STRATRGYPOINT_INFO_CHANGE));
			g_pPreGlobal->WriteSystemLogEX(TRUE, "  [Notify] S_WAR_SYSTEM_RENEWAL_STRATEGYPOINT_JHSEOL #Recvd & FieldServer Send Packet - DBName(%s)\r\n", msgStratrgyPointInfoChange->DBName);
		}
		// end 2012-11-13 by jhseol, 전쟁 시스템 리뉴얼 - 거점전
		break;
		case T_PA_ADMIN_GET_ACCOUNT_LIST:
			{
				MSG_PA_ADMIN_GET_ACCOUNT_LIST * msgGetAccList = (MSG_PA_ADMIN_GET_ACCOUNT_LIST*)(pPacket + nBytesUsed);
				nBytesUsed += sizeof(MSG_PA_ADMIN_GET_ACCOUNT_LIST);

				INIT_MSG(MSG_PA_ADMIN_GET_ACCOUNT_LIST_OK, T_PA_ADMIN_GET_ACCOUNT_LIST_OK, msgGetOK, SendBuf);
				msgGetOK->NumOfAccounts = 0;
				CServerGroup *pServerGroup = ms_pPreIOCP->GetServerGroup(msgGetAccList->ServerName);

				pServerGroup->m_setLoginedAccount.lock();
				{
					mt_set<string>::iterator itr = pServerGroup->m_setLoginedAccount.begin();
					int offset = MSG_SIZE(MSG_PA_ADMIN_GET_ACCOUNT_LIST_OK);
					while (itr != pServerGroup->m_setLoginedAccount.end())
					{
						if (offset+sizeof(MEX_SIMPLE_ACCOUNT_INFO_FOR_ADMIN) >= SIZE_MAX_PACKET)
						{
							break;
						}

						MEX_SIMPLE_ACCOUNT_INFO_FOR_ADMIN *pAccInfo
							= (MEX_SIMPLE_ACCOUNT_INFO_FOR_ADMIN*)(SendBuf+offset);

						STRNCPY_MEMSET(pAccInfo->AccountName, (*itr).c_str(), SIZE_MAX_ACCOUNT_NAME);
						STRNCPY_MEMSET(pAccInfo->CharacterName, "", SIZE_MAX_CHARACTER_NAME);

						offset += sizeof(MEX_SIMPLE_ACCOUNT_INFO_FOR_ADMIN);