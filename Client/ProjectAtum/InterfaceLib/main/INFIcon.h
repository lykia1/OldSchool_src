// INFIcon.h: interface for the CINFIcon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFICON_H__76C63A2B_57D9_485B_BE09_08B60678507E__INCLUDED_)
#define AFX_INFICON_H__76C63A2B_57D9_485B_BE09_08B60678507E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AtumNode.h"
#include "D3DSpriteBatch.h"

struct TexInterfaceInfo
{
	long m_iKeyAsNumber;
	long m_iTextureSize;
	long m_iNumOfImages;
	long m_iNumOfAtlas;

	TCHAR m_sDefaultImage[12]; // reserved for future
};

struct TexMappingInfo
{
	TCHAR m_sImageName[12];

	long m_iAtlasNum;
	RECT m_rRect;
};

struct AtlasRECT // lightweight version of TexMappingInfo
{
	long m_iAtlasNum;
	RECT m_rRect;
};

// TODO: create a template for string and number keyed indexes
// the texturemapping is already suitable to replace the whole interface image
// processing/rendering

class CINFImage;
class CINFImageEx;		// 2011. 10. 10 by jskim UI시스템 변경
class CGameData;
class CINFIcon : public CAtumNode
{
protected:
	LPDIRECT3DTEXTURE9* m_pArrTextureAtlas;
	map<long, AtlasRECT>		m_mapIcons;
	CGameData* m_pGameData;
	TCHAR						m_szResourceFile[20];
	long						m_nDefaultIcon;
	TexInterfaceInfo* m_pTexInterfaceInfo;

public:
	CINFIcon(LPCSTR szResourceFile, long defaultIcon = 0);
	virtual ~CINFIcon();

	BOOL Exists(long iconNum);

	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();

	SIZE GetIconSize(long iconNum = 0);
	virtual void Render(long iconNum, int x, int y, float fScale = 1.0f, D3DCOLOR color = D3DCOLOR_FULL);


};
#endif // !defined(AFX_INFICON_H__76C63A2B_57D9_485B_BE09_08B60678507E__INCLUDED_)
