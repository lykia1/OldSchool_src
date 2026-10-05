// INFImage.cpp: implementation of the CINFImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "INFImage.h"
#include "DXUtil.h"
#include "AtumApplication.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CINFImage::CINFImage()
	:m_v2Scaling(1,1), m_v2Rcenter(0,0), m_v2Trans(0,0)
{
	FLOG( "CINFImage()" );
	m_pRect = NULL;
	m_pTexture = NULL;
	m_bSpriteCrate = FALSE;
	//m_pd3dxSprite = NULL;
	m_fAngle = 0.0f;
	m_pData = NULL;
	m_nDataSize = 0;
	m_dwColor = 0xFFFFFFFF;
	m_poImgSize.x = 0.0f;
	m_poImgSize.y = 0.0f;
}

CINFImage::~CINFImage()
{
	FLOG( "~CINFImage()" );
	if(m_pRect)
	{
		delete m_pRect;
		m_pRect = NULL;
	}

}

void CINFImage::Attach(LPDIRECT3DTEXTURE9 pTexture)
{
	m_pTexture = pTexture;
}

LPDIRECT3DTEXTURE9 CINFImage::Detach()
{
	LPDIRECT3DTEXTURE9 pTexture = m_pTexture;
	m_pTexture = NULL;
	return pTexture;
}

HRESULT CINFImage::InitDeviceObjects(char* pData, int nSize)
{
	FLOG( "CINFImage::InitDeviceObjects(char* pData, int nSize)" );
	m_pData = pData;
	m_nDataSize = nSize;
	D3DXVECTOR2 tmVec {0,0};
	m_v2Trans = tmVec;//D3DXVECTOR2( 0, 0);
	return S_OK;
}

void CINFImage::Move( float x, float y ) 
{ 
	FLOG( "CINFImage::Move( float x, float y )" );
	D3DXVECTOR2 tmVec {x,y};
	m_v2Trans = tmVec;//D3DXVECTOR2(x, y);
}

void CINFImage::Rotate( float x, float y, float angle ) 
{ 
	FLOG( "CINFImage::Rotate( float x, float y, float angle )" );
	D3DXVECTOR2 tmVec {x,y};
	m_v2Rcenter = tmVec;//D3DXVECTOR2(x, y);
	m_fAngle = angle;
}

void CINFImage::Scaling(float x, float y)
{
	D3DXVECTOR2 tmVec {x,y};
	m_v2Scaling = tmVec;//D3DXVECTOR2(x, y);
}

HRESULT CINFImage::RestoreDeviceObjects()
{
	FLOG( "CINFImage::RestoreDeviceObjects()" );
	if(!m_pData)
		return E_FAIL;

	D3DXIMAGE_INFO SrcInfo;
	if(FAILED(D3DXCreateTextureFromFileInMemoryEx( g_pD3dDev, (LPCVOID)m_pData, m_nDataSize,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,
		0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 
		0, &SrcInfo, NULL, &m_pTexture)))
	{
		return E_FAIL;
	}

	m_poImgSize.x = SrcInfo.Width;
	m_poImgSize.y = SrcInfo.Height;

	return S_OK;
}

HRESULT CINFImage::InvalidateDeviceObjects()
{
	FLOG( "CINFImage::InvalidateDeviceObjects()" );
	SAFE_RELEASE(m_pTexture);

	return S_OK;
}

HRESULT CINFImage::DeleteDeviceObjects()
{
	FLOG( "CINFImage::DeleteDeviceObjects()" );
	return S_OK;
}

void CINFImage::Render()
{
	FLOG( "CINFImage::Render()" );
	if(m_pTexture && m_pTexture != NULL)
	{
		g_pD3dApp->GetSpriteBatch()->Draw(m_pTexture, m_pRect, &m_v2Scaling, &m_v2Rcenter, m_fAngle, &m_v2Trans, m_dwColor);
	}
}

void CINFImage::SetRect(long left,long top, long right, long bottom)
{
	FLOG( "CINFImage::SetRect(long left,long top, long right, long bottom)" );
	if(m_pRect) 
	{
		delete m_pRect;
		m_pRect = NULL;
	}
	m_pRect = new RECT;
	m_pRect->left = left;
	m_pRect->top = top;
	m_pRect->right = right;
	m_pRect->bottom = bottom;
}


void CINFImage::InitRect()
{
	if(m_pRect) 
	{
		delete m_pRect;
		m_pRect = NULL;
	}
}


// 2010. 05. 12 by hsLee 인피니티 필드 2차 UI 추가 수정. (인게임 거점 방어 단계 표시.)

/**************************************************************
**
**	Scale값 적용된 이미지 사이즈 값 리턴.
**
**	Create Info : 2010. 05. 12. by hsLee.
**
***************************************************************/
POINT CINFImage :: GetCurrentScale ( void )
{

	POINT rtn_pt;
		rtn_pt.x = m_poImgSize.x * m_v2Scaling.x;
		rtn_pt.y = m_poImgSize.y * m_v2Scaling.y;

	return rtn_pt;
}


/**************************************************************
**
**	Scale값 적용된 이미지 사이즈 값 리턴.
**
**	Create Info : 2010. 05. 12. by hsLee.
**
****************************************************************/
D3DXVECTOR2 CINFImage :: GetCenterTransVector ( void )
{

	D3DXVECTOR2 rtn_vector;

	rtn_vector.x = m_v2Trans.x + GetCurrentScale().x/2;
	rtn_vector.y = m_v2Trans.y + GetCurrentScale().y/2;

	return rtn_vector;

}

// End 2010. 05. 12 by hsLee 인피니티 필드 2차 UI 추가 수정. (인게임 거점 방어 단계 표시.)


///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFImage::RestoreDeviceObjectsEx(int nImageSizeX, int nImageSizeY)
/// \brief		특수 목적으로 데이타 없이 텍스쳐를 생성하는 함수 
/// \author		ispark
/// \date		2005-09-28 ~ 2005-09-28
/// \warning	
///
/// \param		
/// \return		HRESULT
///////////////////////////////////////////////////////////////////////////////
HRESULT CINFImage::RestoreDeviceObjectsEx(int nImageSizeX, int nImageSizeY)
{
	FLOG( "CINFImage::RestoreDeviceObjects()" );
	
	if(FAILED(D3DXCreateTexture(g_pD3dDev, nImageSizeX, nImageSizeY, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture)))
	{
		return E_FAIL;
	}

	m_poImgSize.x = nImageSizeX;
	m_poImgSize.y = nImageSizeY;

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CINFImage::InitDeviceObjectsEx()
/// \brief		특수 목적으로 쓰는 위 함수와 같이 쓰이는 초기화하는 함수
/// \author		ispark
/// \date		2005-09-28 ~ 2005-09-28
/// \warning	
///
/// \param		
/// \return		HRESULT
///////////////////////////////////////////////////////////////////////////////
HRESULT CINFImage::InitDeviceObjectsEx()
{
	FLOG( "CINFImage::InitDeviceObjects(char* pData, int nSize)" );
	m_pData = NULL;
	m_nDataSize = 0;
	D3DXVECTOR2 tmVec {0,0};
	m_v2Trans = tmVec;//D3DXVECTOR2( 0, 0);
	return S_OK;
}

