// INFIcon.cpp: implementation of the CINFIcon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "INFIcon.h"
#include "AtumApplication.h"
#include "GameDataLast.h"
#include "dxutil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CINFIcon::CINFIcon(LPCSTR szResourceFile, long defaultIcon)
{
	FLOG("CINFIcon()");
	strncpy(m_szResourceFile, szResourceFile, sizeof(m_szResourceFile));
	m_nDefaultIcon = defaultIcon;
	m_pArrTextureAtlas = nullptr;
	m_pGameData = nullptr;
	m_pTexInterfaceInfo = nullptr;
}

CINFIcon::~CINFIcon()
{
	FLOG("~CINFIcon()");
	if (m_pTexInterfaceInfo != nullptr && m_pArrTextureAtlas != nullptr) {
		for (int i = 0; i < m_pTexInterfaceInfo->m_iNumOfAtlas; i++)
		{
			SAFE_RELEASE(m_pArrTextureAtlas[i]);
		}
	}
	SAFE_DELETE_ARRAY(m_pArrTextureAtlas);
	SAFE_DELETE(m_pTexInterfaceInfo);
	SAFE_DELETE(m_pGameData);
	m_mapIcons.clear();
}

HRESULT CINFIcon::InitDeviceObjects()
{
	FLOG("CINFIcon::InitDeviceObjects()");
	//HRESULT hr;
	char strPath[256];
	g_pD3dApp->LoadPath(strPath, IDS_DIRECTORY_TEXTURE, m_szResourceFile);
	m_pGameData = new CGameData;

	if (false == m_pGameData->SetFile(strPath, FALSE, NULL, 0))
	{
		return E_FAIL;
	}

	m_pTexInterfaceInfo = new TexInterfaceInfo;
	auto* header = m_pGameData->Find("info");

	if (header == nullptr)
	{
		return E_FAIL;
	}

	memcpy(m_pTexInterfaceInfo, header->m_pData, sizeof(TexInterfaceInfo));

	header = m_pGameData->Find("mapping");

	if (header == nullptr)
	{
		return E_FAIL;
	}

	auto* pMapping = reinterpret_cast<TexMappingInfo*>(header->m_pData);
	for (int i = 0; i < m_pTexInterfaceInfo->m_iNumOfImages; ++i)
	{
		long itemNum = atol(pMapping->m_sImageName);
		m_mapIcons.emplace(itemNum, AtlasRECT{ pMapping->m_iAtlasNum, pMapping->m_rRect });
		pMapping++;
	}

	return S_OK;
}

HRESULT CINFIcon::RestoreDeviceObjects()
{
	FLOG("CINFIcon::RestoreDeviceObjects()");
	HRESULT hr;

	// 	if(FAILED(hr))
	// 	{
	// #ifdef CLIENT_CONSOLE
	// 		printf("CINFIcon::RestoreDeviceObjects at g_pD3dDev->CreateTexture(TextureAtlas) failed 0x%08lx\n", hr);
	// #endif
	// 		return hr;
	// 	}
	// 	
	m_pArrTextureAtlas = new LPDIRECT3DTEXTURE9[m_pTexInterfaceInfo->m_iNumOfAtlas];

	if (m_pArrTextureAtlas == nullptr)
	{
		return E_OUTOFMEMORY;
	}

	TCHAR buf[10];
	for (int i = 0; i < m_pTexInterfaceInfo->m_iNumOfAtlas; ++i)
	{
		sprintf(buf, "%d", i);
		auto* header = m_pGameData->Find(buf);

		D3DXIMAGE_INFO SrcInfo;
		hr = D3DXCreateTextureFromFileInMemoryEx(g_pD3dDev, (LPCVOID)header->m_pData, header->m_DataSize, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
			0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT,
			0, &SrcInfo, nullptr, &m_pArrTextureAtlas[i]);

		if (FAILED(hr))
		{
#ifdef CLIENT_CONSOLE
			printf("CINFIcon::RestoreDeviceObjects at D3DXCreateTexture(TextureAtlas) %s #%d failed 0x%08lx\n", m_szResourceFile, i, hr);
#endif
			return hr;
		}

#ifdef CLIENT_CONSOLE
		printf("CINFIcon::RestoreDeviceObjects at D3DXCreateTexture(TextureAtlas) %s #%d loaded with %dx%d\n", m_szResourceFile, i, SrcInfo.Height, SrcInfo.Width);
#endif
	}

	return S_OK;
}

HRESULT CINFIcon::InvalidateDeviceObjects()
{
	FLOG("CINFIcon::InvalidateDeviceObjects()");
	for (int i = 0; i < m_pTexInterfaceInfo->m_iNumOfAtlas; i++)
	{
		SAFE_RELEASE(m_pArrTextureAtlas[i]);
	}
	SAFE_DELETE_ARRAY(m_pArrTextureAtlas);
	return S_OK;
}

HRESULT CINFIcon::DeleteDeviceObjects()
{
	FLOG("CINFIcon::DeleteDeviceObjects()");
	m_mapIcons.clear();
	SAFE_DELETE(m_pGameData);
	return S_OK;
}

BOOL CINFIcon::Exists(long iconNum)
{
	FLOG("CINFIcon::FindIcon(char* strName)");

	return m_mapIcons.find(iconNum) != m_mapIcons.end();
}

SIZE CINFIcon::GetIconSize(long iconNum)
{
	if (iconNum == 0 && m_nDefaultIcon == 0) return SIZE{ 0, 0 };

	if (iconNum == 0)
	{
		iconNum = m_nDefaultIcon;
	}

	auto it = m_mapIcons.find(iconNum);
	if (it == m_mapIcons.end())
	{
		return SIZE{ 0, 0 };
	}

	auto rect = it->second.m_rRect;
	return SIZE{ rect.right - rect.left, rect.bottom - rect.top };
}

void CINFIcon::Render(long iconNum, int x, int y, float fScale, D3DCOLOR color)
{
	// since no icon will habe number zero
	if (iconNum == 0 && m_nDefaultIcon == 0) return;

	auto it = m_mapIcons.find(iconNum);
	if (it != m_mapIcons.end())
	{
		auto* rect = &it->second;
		g_pD3dApp->GetSpriteBatch()->Draw(
			m_pArrTextureAtlas[rect->m_iAtlasNum],
			&rect->m_rRect,
			&D3DXVECTOR2(fScale, fScale),
			nullptr,
			0,
			&D3DXVECTOR2(float(x), float(y)),
			color
		);
	}
	else
	{
		auto* rect = &m_mapIcons[m_nDefaultIcon];
		g_pD3dApp->GetSpriteBatch()->Draw(
			m_pArrTextureAtlas[rect->m_iAtlasNum],
			&rect->m_rRect,
			&D3DXVECTOR2(fScale, fScale),
			nullptr,
			0,
			&D3DXVECTOR2(float(x), float(y)),
			color
		);
	}
}