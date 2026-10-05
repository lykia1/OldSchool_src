// INFStateEffect.cpp: implementation of the CINFStageEffect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "INFStageEffect.h"
#include "AtumApplication.h"
#include "INFImage.h"
#include "GameDataLast.h"
#include "dxutil.h"

#define SCALE_X			((float)g_pD3dApp->GetBackBufferDesc().Width/1024)
#define SCALE_Y			((float)g_pD3dApp->GetBackBufferDesc().Height/768)
#define START_X_1		0
#define START_Y_1		0
#define START_X_2		(512*SCALE)
#define START_Y_2		0
#define START_X_3		0
#define START_Y_3		(384*SCALE)
#define START_X_4		(512*SCALE)
#define START_Y_4		(384*SCALE)

#define STATE_FADE_IN		0
#define STATE_SHOW			1
#define STATE_FADE_OUT		2

#define CHANGE_TIME			1.0f
#define DEFAULT_SIZE		512.0f
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CINFStageEffect::CINFStageEffect(int nImageIndex, float fPlayTime, BOOL bSkipEnable)
{
	m_bRunning = FALSE;
	m_nIndex = nImageIndex;
	m_nState = STATE_FADE_IN;
	m_fPlayTime = fPlayTime;
	m_bSkipEnable = bSkipEnable;
	m_fCurrentTime = 0;
//	memset( m_pImage, 0x00, sizeof(DWORD)*STAGE_IMG_NUM );
//	for(int i=0;i<STAGE_IMG_NUM;i++)
//	{
//		m_pDataHeader[i] = NULL;
//	}
	m_pImage = NULL;
	m_pDataHeader = NULL;
	m_bEndWork = FALSE;
	m_bRestored = FALSE;
}

CINFStageEffect::~CINFStageEffect()
{
//	SAFE_DELETE(m_pImage[0]);
//	SAFE_DELETE(m_pImage[1]);
//	SAFE_DELETE(m_pImage[2]);
//	SAFE_DELETE(m_pImage[3]);
//	for(int i=0;i<STAGE_IMG_NUM;i++)
//	{
//		SAFE_DELETE(m_pDataHeader[i]);
//	}
	SAFE_DELETE(m_pImage);
	SAFE_DELETE(m_pDataHeader);
}

HRESULT CINFStageEffect::InitDeviceObjects()
{
	CGameData gameData;
	gameData.SetFile( ".\\Res-Tex\\steff.tex", FALSE, nullptr, 0, FALSE );
	char szName[32];
	wsprintf(szName, "%08d", m_nIndex);
	m_pDataHeader = gameData.FindFromFile(szName);
	if(m_pDataHeader == nullptr) {
		DBGOUT("StageEffect => Not found (%d)", m_nIndex);
		return E_FAIL;
	}
	m_pImage = new CINFImage;
	m_pImage->InitDeviceObjects( m_pDataHeader->m_pData, m_pDataHeader->m_DataSize );
	m_pImage->SetColor(0x00FFFFFF);
	m_bRunning = TRUE;

	return S_OK;
}

HRESULT CINFStageEffect::RestoreDeviceObjects()
{
	if(!m_bRestored)
	{
//		for(int i=0;i<STAGE_IMG_NUM;i++)
//		{
//			m_pImage[i]->RestoreDeviceObjects();
//			m_pImage[i]->SetScale(g_pD3dApp->GetBackBufferDesc().Width / DEFAULT_SIZE, 
//				(float)g_pD3dApp->GetBackBufferDesc().Width / DEFAULT_SIZE);
//		}
		if (m_pImage)
			m_pImage->RestoreDeviceObjects();
			//			m_pImage->SetScale(g_pD3dApp->GetBackBufferDesc().Width / DEFAULT_SIZE, 
			//				(float)g_pD3dApp->GetBackBufferDesc().Width / DEFAULT_SIZE);

			m_bRestored = TRUE;
			//		m_pImage[0]->SetScale(SCALE, SCALE);
			//		m_pImage[0]->Move(START_X_1, START_Y_1);
			//		m_pImage[1]->SetScale(SCALE, SCALE);
			//		m_pImage[1]->Move(START_X_2, START_Y_2);
			//		m_pImage[2]->SetScale(SCALE, SCALE);
			//		m_pImage[2]->Move(START_X_3, START_Y_3);
			//		m_pImage[3]->SetScale(SCALE, SCALE);
			//		m_pImage[3]->Move(START_X_4, START_Y_4);
			if (m_pImage){
			m_pImage->SetScale(SCALE_X, SCALE_Y);
			m_pImage->Move(START_X_1, START_Y_1);
		}

	}
	return S_OK;

}
HRESULT CINFStageEffect::InvalidateDeviceObjects()
{
	if(m_bRestored)
	{
//		for(int i=0;i<STAGE_IMG_NUM;i++)
//		{
//			if(m_pImage[i]->GetTexture() != NULL)
//			{
//				m_pImage[i]->InvalidateDeviceObjects();
//			}
//		}

		if(m_pImage && m_pImage->GetTexture() != NULL)
		{
			m_pImage->InvalidateDeviceObjects();
		}
		m_bRestored = FALSE;
	}
	return S_OK;
}
HRESULT CINFStageEffect::DeleteDeviceObjects()
{
//	for(int i=0;i<STAGE_IMG_NUM;i++)
//	{
//		m_pImage[i]->DeleteDeviceObjects();
//		SAFE_DELETE(m_pImage[i]);
//		SAFE_DELETE(m_pDataHeader[i]);
//	}

	if (m_pImage) {
		m_pImage->DeleteDeviceObjects();
		SAFE_DELETE(m_pImage);
	}
		SAFE_DELETE(m_pDataHeader);
	
	m_bRunning = FALSE;
	return S_OK;
}

void CINFStageEffect::Tick()
{
	if(g_pD3dApp->GetElapsedTime() > 0.1)
	{
		return;
	}
	switch(m_nState)
	{
	case STATE_FADE_IN:
		{
			m_fCurrentTime += g_pD3dApp->GetElapsedTime();
			if(m_fCurrentTime > CHANGE_TIME)
			{
				m_fCurrentTime = CHANGE_TIME;
			}
			D3DCOLOR dwColor = (D3DCOLOR)((int)(0xFF*m_fCurrentTime) << 24 | 0x00FFFFFF);
//			for(int i=0;i<STAGE_IMG_NUM;i++)
//			{
//				m_pImage[i]->SetColor(dwColor);
//			}
			if (m_pImage)
			m_pImage->SetColor(dwColor);

			if(m_fCurrentTime >= CHANGE_TIME)
			{
				m_nState = STATE_SHOW;
			}
		}
		break;
	case STATE_FADE_OUT:
		{
			m_fCurrentTime -= g_pD3dApp->GetElapsedTime();
			if(m_fCurrentTime < 0)
			{
				m_fCurrentTime = 0;
			}
			D3DCOLOR dwColor = (D3DCOLOR)((int)(0xFF*m_fCurrentTime) << 24 | 0x00FFFFFF);
//			for(int i=0;i<STAGE_IMG_NUM;i++)
//			{
//				m_pImage[i]->SetColor(dwColor);
//			}
			if (m_pImage)
			m_pImage->SetColor(dwColor);

			if(m_fCurrentTime <= 0)
			{
				m_bRunning = FALSE;
			}
		}
		break;
	case STATE_SHOW:
		{
			m_fPlayTime -= g_pD3dApp->GetElapsedTime();
			if(m_fPlayTime < 0)
			{
				m_nState = STATE_FADE_OUT;
			}
		}
		break;
	}
}
void CINFStageEffect::Render()
{
	g_pD3dDev->SetRenderState( D3DRS_FOGENABLE, FALSE );
	g_pD3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_pD3dDev->SetRenderState( D3DRS_ZENABLE, FALSE );
//	m_pImage[0]->Render();
//	m_pImage[1]->Render();
//	m_pImage[2]->Render();
//	m_pImage[3]->Render();
	if (m_pImage)
	m_pImage->Render();
}

int CINFStageEffect::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_bSkipEnable)
	{
		switch(uMsg)
		{
		case WM_KEYDOWN:
		case WM_LBUTTONDOWN:
			{
				m_nState = STATE_FADE_OUT;
			}
			break;
		}
	}
	return INF_MSGPROC_NORMAL;
}
