// UdpateSocket.cpp: implementation of the CUpdateWinSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UpdateWinSocket.h"
#include "AtumProtocol.h"
#ifdef _ATUM_LAUNCHER
#include "FtpDownload.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUpdateWinSocket::CUpdateWinSocket(HWND hWnd, UINT wmSock, UINT wmNotify)
:CWinSocket(hWnd, wmSock, wmNotify, TRUE)
{
}

CUpdateWinSocket::~CUpdateWinSocket()
{

}
BOOL CUpdateWinSocket::OnCloseSocket(int nErrorCode)
{
	return CWinSocket::OnCloseSocket(nErrorCode);
}

BOOL CUpdateWinSocket::OnConnect(int nErrorCode)
{
	CWinSocket::OnConnect(nErrorCode);
	//////////////////////////////////////////////////////////////////////
	// TODO: Add your specialized code here

	//
	//////////////////////////////////////////////////////////////////////
	return	TRUE;
}
#ifdef _ATUM_LAUNCHER
int CUpdateWinSocket::CreateDirectory(CString strParent, CList<CString, CString&> &lsDir)
{
	POSITION pos = lsDir.GetHeadPosition();
	strParent += lsDir.GetNext(pos);
	if(pos)// 마지막은 디렉토리가 아닌 복사할 파일
	{
		CFileFind finder;
		if(finder.FindFile(strParent))
		{

		    finder.FindNextFile();
			if(!finder.IsDirectory())
			{
				DeleteFile(strParent); // 파일로 존재하면 지운다.
				if(!::CreateDirectory(strParent,NULL))
					return -1;
			}
		} else
			if(!::CreateDirectory(strParent,NULL))
				return -1;

		lsDir.GetPrev(pos);
		lsDir.RemoveAt(pos);
		strParent += '/';
		if(CreateDirectory(strParent,lsDir)==-1)
			return -1;
	}
	return 0;

}
#endif
BOOL CUpdateWinSocket::OnRecvdPacket(LPSTR pPacket, int nLength, BYTE nSeq)
{
	int		nBytesUsed	= 0;
	int		nType		= 0;
	int		nMsgSize	= 0;
	char*	pRMsg		= NULL;
	MessageType_t		prevMsgType = 0;

	while(nBytesUsed < nLength)
	{
		memcpy(&nType, pPacket + nBytesUsed, SIZE_FIELD_TYPE_HEADER);
		switch(nType)
		{
		case T_PC_CONNECT_VERSION_OK:	// No Body
			{
				nMsgSize = SIZE_FIELD_TYPE_HEADER;
				pRMsg = new char[nMsgSize];
				memcpy(pRMsg, pPacket + nBytesUsed, nMsgSize);
				EnterCriticalSection(&m_criticalRecvMessageQueue);
				m_queueRecvMessage.push(pRMsg);
				LeaveCriticalSection(&m_criticalRecvMessageQueue);
				pRMsg = NULL;
				//  처리
				PostNotify(WS_RECEIVED, 0, (char*)this);
				nBytesUsed += nMsgSize;
			}
			break;
		case T_PC_CONNECT_UPDATE_INFO:
			{
				nMsgSize = MSG_SIZE(MSG_PC_CONNECT_UPDATE_INFO);
				pRMsg = new char[nMsgSize];
				memcpy(pRMsg, pPacket + nBytesUsed, nMsgSize);
				EnterCriticalSection(&m_criticalRecvMessageQueue);
				m_queueRecvMessage.push(pRMsg);
				LeaveCriticalSection(&m_criticalRecvMessageQueue);
				pRMsg = NULL;
				//  처리
				PostNotify(WS_RECEIVED, 0, (char*)this);
				nBytesUsed += nMsgSize;
			}
			break;
		case T_PC_CONNECT_REINSTALL_CLIENT:
			{
				nMsgSize = MSG_SIZE(MSG_PC_CONNECT_REINSTALL_CLIENT);
				pRMsg = new char[nMsgSize];
				memcpy(pRMsg, pPacket + nBytesUsed, nMsgSize);
				EnterCriticalSection(&m_criticalRecvMessageQueue);
				m_queueRecvMessage.push(pRMsg);
				LeaveCriticalSection(&m_criticalRecvMessageQueue);
				pRMsg = NULL;
				//  처리
				PostNotify(WS_RECEIVED, 0, (char*)this);
				nBytesUsed += nMsgSize;
			}
			break;
#ifdef _INET_MAC_ADDRESS_CHECKER
		case T_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR_BLOCK:
		{
			nMsgSize = SIZE_FIELD_TYPE_HEADER;
			pRMsg = new char[nMsgSize];
			memcpy(pRMsg, pPacket + nBytesUsed, nMsgSize);
			EnterCriticalSection(&m_criticalRecvMessageQueue);
			m_queueRecvMessage.push(pRMsg);
			LeaveCriticalSection(&m_criticalRecvMessageQueue);
			pRMsg = NULL;
			PostNotify(WS_RECEIVED, 0, (char*)this);
			nBytesUsed += nMsgSize;
		}
		case T_PC_CONNECT_SEND_GET_BLOCKED_MAC_ADDR_OK:
		{
			nMsgSize = SIZE_FIELD_TYPE_HEADER;
			pRMsg = new char[nMsgSize];
			memcpy(pRMsg, pPacket + nBytesUsed, nMsgSize);
			EnterCriticalSection(&m_criticalRecvMessageQueue);
			m_queueRecvMessage.push(pRMsg);
			LeaveCriticalSection(&m_criticalRecvMessageQueue);
			pRMsg = NULL;
			PostNotify(WS_RECEIVED, 0, (char*)this);
			nBytesUsed += nMsgSize;
		}
		break;
#endif
		case T_PC_CONNECT_SINGLE_FILE_VERSION_CHECK_OK:
			{
				nMsgSize = SIZE_FIELD_TYPE_HEADER;
				pRMsg = new char[nMsgSize];
				memcpy(pRMsg, pPacket + nBytesUsed, nMsgSize);
				EnterCriticalSection(&m_criticalRecvMessageQueue);
				m_queueRecvMessage.push(pRMsg);
				LeaveCriticalSection(&m_criticalRecvMessageQueue);
				pRMsg = NULL;
				//  처리
				PostNotify(WS_RECEIVED, 0, (char*)this);
				nBytesUsed += nMsgSize;
			}
			break;
		case T_PC_CONNECT_SINGLE_FILE_UPDATE_INFO:
			{
				nMsgSize = MSG_SIZE(MSG_PC_CONNECT_SINGLE_FILE_UPDATE_INFO);
				pRMsg = new char[nMsgSize];
				memcpy(pRMsg, pPacket + nBytesUsed, nMsgSize);
				EnterCriticalSection(&m_criticalRecvMessageQueue);
				m_queueRecvMessage.push(pRMsg);
				LeaveCriticalSection(&m_criticalRecvMessageQueue);
				pRMsg = NULL;
				//  처리
				PostNotify(WS_RECEIVED, 0, (char*)this);
				nBytesUsed += nMsgSize;
			}
			break;
		case T_PC_CONNECT_LOGIN_OK:
			{
				nMsgSize = MSG_SIZE(MSG_PC_CONNECT_LOGIN_OK);
				pRMsg = new char[nMsgSize];
				memcpy(pRMsg, pPacket + nBytesUsed, nMsgSize);
				EnterCriticalSection(&m_criticalRecvMessageQueue);
				m_queueRecvMessage.push(pRMsg);
				LeaveCriticalSection(&m_criticalRecvMessageQueue);
				pRMsg = NULL;
				//  처리
				PostNotify(WS_RECEIVED, 0, (char*)this);
				nBytesUsed += nMsgSize;
			}
			break;
		case T_PC_CONNECT_LAUNCHER_SESSION:
			{
				nMsgSize = MSG_SIZE(MSG_PC_CONNECT_LAUNCHER_SESSION);
				pRMsg = new char[nMsgSize];
				memcpy(pRMsg, pPacket + nBytesUsed, nMsgSize);
				EnterCriticalSection(&m_criticalRecvMessageQueue);
				m_queueRecvMessage.push(pRMsg);
				LeaveCriticalSection(&m_criticalRecvMessageQueue);
				pRMsg = NULL;
				PostNotify(WS_RECEIVED, 0, (char*)this);
				nBytesUsed += nMsgSize;
			}
			break;
		case T_PC_CONNECT_GET_SERVER_GROUP_LIST_OK:
			{
				nMsgSize = MSG_SIZE(MSG_PC_CONNECT_GET_SERVER_GROUP_LIST_OK)
					+ ((MSG_PC_CONNECT_GET_SERVER_GROUP_LIST_OK*)(pPacket+nBytesUsed+SIZE_FIELD_TYPE_HEADER))->NumOfServerGroup * sizeof(MEX_SERVER_GROUP_INFO_FOR_LAUNCHER);
				pRMsg = new char[nMsgSize];
				memcpy(pRMsg, pPacket + nBytesUsed, nMsgSize);
				EnterCriticalSection(&m_criticalRecvMessageQueue);
				m_queueRecvMessage.push(pRMsg);
				LeaveCriticalSection(&m_criticalRecvMessageQueue);
				pRMsg = NULL;
				//  처리
				PostNotify(WS_RECEIVED, 0, (char*)this);
				nBytesUsed += nMsgSize;
			}
			break;
		case T_PC_CONNECT_NETWORK_CHECK_OK:		// 2007-06-18 by cmkwon, 네트워크 상태 체크
			{
				nMsgSize = MSG_SIZE(MSG_PC_CONNECT_NETWORK_CHECK_OK);
				pRMsg = new char[nMsgSize];
				memcpy(pRMsg, pPacket + nBytesUsed, nMsgSize);
				EnterCriticalSection(&m_criticalRecvMessageQueue);
				m_queueRecvMessage.push(pRMsg);
				LeaveCriticalSection(&m_criticalRecvMessageQueue);
				pRMsg = NULL;
				//  처리
				PostNotify(WS_RECEIVED, 0, (char*)this);
				nBytesUsed += nMsgSize;
			}
			break;

		case T_PC_CONNECT_LOGIN_BLOCKED:
			{
				nMsgSize = MSG_SIZE(MSG_PC_CONNECT_LOGIN_BLOCKED);
				pRMsg = new char[nMsgSize];
				memcpy(pRMsg, pPacket + nBytesUsed, nMsgSize);
				EnterCriticalSection(&m_criticalRecvMessageQueue);
				m_queueRecvMessage.push(pRMsg);
				LeaveCriticalSection(&m_criticalRecvMessageQueue);
				pRMsg = NULL;
				//  처리
				PostNotify(WS_RECEIVED, 0, (char*)this);
				nBytesUsed += nMsgSize;
			}
			break;
		case T_ERROR:
			{
				nMsgSize = MSG_SIZE(MSG_ERROR) + ((MSG_ERROR*)(pPacket+nBytesUsed+SIZE_FIELD_TYPE_HEADER))->StringLength;
				pRMsg = new char[nMsgSize];
				memcpy(pRMsg, pPacket + nBytesUsed, nMsgSize);
				EnterCriticalSection(&m_criticalRecvMessageQueue);
				m_queueRecvMessage.push(pRMsg);
				LeaveCriticalSection(&m_criticalRecvMessageQueue);
				pRMsg = NULL;
				//  처리
				PostNotify(WS_RECEIVED, 0, (char*)this);
				nBytesUsed += nMsgSize;
			}
			break;
		default:
			{
				char msg[256];
#ifdef _ATUM_DEVELOP
				sprintf(msg, "[Error] Unhandled Message Type: %s(%#04X)\n", GetProtocolTypeString(nType), nType);
#else
				sprintf(msg, "[Error] Unhandled Message Type!\n");
#endif
				MessageBox(NULL, msg, NULL, MB_OK);
				exit(0);
			}
		} // end - switch

		if(NULL != pRMsg)
		{
			SAFE_DELETE(pRMsg);
		}
		prevMsgType = nType;
	}
	return TRUE;
}