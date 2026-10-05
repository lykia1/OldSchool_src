// INFImageFile.cpp: implementation of the CINFImageFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "INFImageFile.h"
#include "AtumApplication.h"
#include "dxutil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CINFImageFile::CINFImageFile()
{
	memset(m_szFileName, 0x00, sizeof(m_szFileName));
	memset(&m_srcInfo, 0x00, sizeof(D3DXIMAGE_INFO));
}

CINFImageFile::~CINFImageFile()
{

}

HRESULT CINFImageFile::InitDeviceObjects(char* pData, int nSize)
{
	FLOG( "CINFImageFile::InitDeviceObjects(char* pData, int nSize)" );
	strncpy(m_szFileName, pData, nSize);
	m_v2Trans = D3DXVECTOR2( 0, 0);
	return S_OK;
}

HRESULT CINFImageFile::RestoreDeviceObjects()
{
	FLOG( "CINFImageFile::RestoreDeviceObjects()" );
	if(FAILED(D3DXCreateTextureFromFileEx(g_pD3dDev, m_szFileName, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 
		0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
		0, &m_srcInfo, NULL, &m_pTexture)))
	{

		return E_FAIL;
	}
	return S_OK;
}