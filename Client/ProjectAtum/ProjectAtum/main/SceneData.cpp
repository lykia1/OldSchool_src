// SceneData.cpp: implementation of the CSceneData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneData.h"
#include "AtumApplication.h"


#include "Frustum.h"
#include "Cinema.h"
#include "Background.h"
//#include "ObjectRender.h"
#include "ObjectChild.h"
//#include "MonsterRender.h"
#include "SunRender.h"
#include "SunData.h"
#include "RainRender.h"
#include "RainData.h"
#include "ETCRender.h"
#include "Weapon.h"
#include "ItemData.h"
#include "MonsterData.h"
#include "EnemyData.h"
#include "UnitRender.h"
#include "CharacterRender.h"					// 2005-07-21 by ispark
#include "Camera.h"
#include "QuadGround.h"
#include "ShuttleChild.h"
#include "CharacterChild.h"				// 2005-07-21 by ispark
#include "dxutil.h"
//#include "WeaponMonsterData.h"
#include "FieldWinSocket.h"
#include "ObjRender.h"
#include "MonRender.h"
#include "AtumDatabase.h"
#include "WeaponItemInfo.h"
#include "INFGameMain.h"
#include "INFSkill.h"
#include "FxSystem.h"
#include "TutorialSystem.h"
#include "WeaponMissileData.h"			// 2005-07-19 by ispark
#include "Water.h"
#include "INFCityBoard.h"			// 2006-04-11 by ispark
#include "Chat.h"
#include "StoreData.h"
#include "Shaders/VShader.h"
#include "Shaders/PShader.h"
#define _DEBUG_SHADERS
#define BIG_OBJECT_RADIUS_TO_I_DISTANCE			320
#define NORMAL_OBJECT_RADIUS_TO_I_DISTANCE		320
#include "EvoAdminPanel.h"
#define OPTION_GAMMA_PLUS 0.05f

inline DWORD F2DW(FLOAT f) { return *((DWORD*)&f); }

float g_fAltitudeApplyMin = 600.0f;
float g_fAltitudeApplyMax = 1600.0f;
float g_fAltitudeDestRate = 0.2f;

// 2007-05-17 by bhsohn żŔşęÁ§Ć® µÚżˇ ĽűľúŔ»˝Ă żˇ ´ëÇŃ Ăł °Ë»ç Ăł¸®
#define TILE_CHECK_DISTANCE			100.0f		// 100ąĚĹÍ
#define MAX_TILE_CHECK_CNT			100			// 100ąĚĹÍ*100 = 10000ąĚĹÍ(ĂÖ´ë»ç°Ĺ¸®)
#define MIN_TILE_CHECK_DISTANCE		20.0f		// ĂÖĽŇ °Ĺ¸®

#define	MIN_TILE_DIFF_CHECK_HEIGHT		100.0f		// ĂÖĽŇ łôŔĚ 100ąĚĹÍ
#define	MAX_TILE_DIFF_CHECK_HEIGHT		500.0f		// ĂÖĽŇ łôŔĚ 100ąĚĹÍ
#define INVENTORY_MESSAGE_GAP			30.0f


// 2008. 12. 11 by ckPark ą°·»´ő¸µ
#define WATER_BUMP_TEXTURE_SIZE_X		256
#define WATER_BUMP_TEXTURE_SIZE_Y		256
// end 2008. 12. 11 by ckPark ą°·»´ő¸µ


struct CompareUnit {
	template<typename T>
	bool operator()(const T* pPtr1, const T* pPtr2)
	{
		if (pPtr1->m_fDistanceCamera > pPtr2->m_fDistanceCamera) {
			return true;
		}
		return false;
	}
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneData::CSceneData()
{
	FLOG("CSceneData::CSceneData()");
	g_pScene = this;
	m_pFrustum = NULL;
	m_pItemData = NULL;
	m_pWeaponData = NULL;
	m_pGround = NULL;
	m_pObjectRender = NULL;
	m_pMonsterRender = NULL;
	m_pETCRender = NULL;
	m_pWater = NULL;
	m_dwFogColor = D3DCOLOR_ARGB(0, 0, 0, 0);
	m_fFogStartValue = WEATHER_SUNNY_FOG_START;
	m_fFogEndValue = WEATHER_SUNNY_FOG_END;
	m_fBeforeFogStartValue = 0;
	m_fBeforeFogEndValue = 0;
	m_bFog = TRUE;
	m_bFogStay = FALSE;
	memset((char*)&m_light0, 0x00, sizeof(D3DLIGHT9));
	memset((char*)&m_light1, 0x00, sizeof(D3DLIGHT9));
	memset((char*)&m_light2, 0x00, sizeof(D3DLIGHT9));
	memset((char*)&m_light3, 0x00, sizeof(D3DLIGHT9));
	m_bNight = FALSE;

	m_pSunRender = NULL;
	m_pSunData = NULL;
	m_pData = NULL;
	memset((char*)m_pCreateTexture, 0x00, sizeof(DWORD) * TEXTILE_NUM);
	m_bIsRestore = TRUE;
	m_fSkyRedColor = 0;
	m_fSkyGreenColor = 0;
	m_fSkyBlueColor = 0;
	m_pRainRender = NULL;
	m_pRainList = NULL;
	m_byWeatherType = 0;
	m_dwStartTime = 0;
	m_nBaseTime = 0;
	m_byMapType = 0;
	m_vecEnemyBlockList = NULL;
	m_vecMonsterList = NULL;
	m_nBlockSizeX = 0;
	m_nBlockSizeY = 0;

	m_bChangeWeather = TRUE;
	m_fChangeWeatherCheckTime = 0;

	//	m_fFogDestStartValue = WEATHER_SUNNY_FOG_START;
	//	m_fFogDestEndValue = WEATHER_SUNNY_FOG_END;
	m_fOrgFogStartValue = WEATHER_SUNNY_FOG_START;
	m_fOrgFogEndValue = WEATHER_SUNNY_FOG_END;
	m_pCinemaData = NULL;		/// ˝Ăł×¸¶ µĄŔĚĹÍ 

	// 2005-01-20 by jschoi 
	m_fAlphaSky = 1.0f;		// ˝şÄ«ŔĚ ąÚ˝ş ł·,ąă ČĄÇŐşńŔ˛

	// 2005-07-11 by ispark
	m_nMaxAtitudeHeight = ALTITUDE_APPLY_MAX;
	m_bWaterShaderRenderFlag = FALSE;

	m_fGetItemAllDelay = 0.0f;
	m_fGetItemMessage = 0.0f;

	m_vecAlphaEffectRender.clear();
	m_vecAlphaUnitRender.clear();
	m_vecScanData.clear();

	// 2008. 12. 11 by ckPark ą°·»´ő¸µ
	m_pWaterBumpTexture = NULL;
	// end 2008. 12. 11 by ckPark ą°·»´ő¸µ

	m_pSimpleVS = nullptr;
	m_pSimplePS = nullptr;
	m_pVertexDeclaration = nullptr;
	m_pBackBufferTemp = nullptr;
	m_ReflectionTexture = nullptr;
	m_RefractionTexture = nullptr;
}

CSceneData::~CSceneData()
{
	FLOG("CSceneData::~CSceneData()");
	g_pScene = NULL;

	SAFE_DELETE(m_pFrustum);
	SAFE_DELETE(m_pCinemaData);	/// ˝Ăł×¸¶ µĄŔĚĹÍ
	SAFE_DELETE(m_pItemData);
	SAFE_DELETE(m_pWeaponData);
	SAFE_DELETE(m_pRainRender);
	SAFE_DELETE(m_pRainList);
	SAFE_DELETE(m_pData);
	SAFE_DELETE(m_pSunRender);
	SAFE_DELETE(m_pSunData);
	SAFE_DELETE(m_pGround);
	SAFE_DELETE(m_pObjectRender);
	SAFE_DELETE(m_pMonsterRender);
	SAFE_DELETE(m_pETCRender);
	SAFE_DELETE(m_pWater);
	if (m_vecEnemyBlockList) {
		for (int i = 0; i < m_nBlockSizeX; i++) {
			for (int j = 0; j < m_nBlockSizeY; j++) {
				m_vecEnemyBlockList[i * m_nBlockSizeY + j].clear();
				//SAFE_DELETE(m_vecEnemyBlockList[i*m_nBlockSizeY + j]);
			}
		}
		SAFE_DELETE_ARRAY(m_vecEnemyBlockList);
	}
	if (m_vecMonsterList) {
		for (int i = 0; i < m_nBlockSizeX; i++) {
			for (int j = 0; j < m_nBlockSizeY; j++) {
				m_vecMonsterList[i * m_nBlockSizeY + j].clear();
				//SAFE_DELETE(m_vecMonsterList[i*m_nBlockSizeY + j]);
			}
		}
		SAFE_DELETE_ARRAY(m_vecMonsterList);
	}
	CMapMonsterIterator itMonster = m_mapMonsterList.begin();
	while (itMonster != m_mapMonsterList.end()) {
		itMonster->second->InvalidateDeviceObjects();
		itMonster->second->DeleteDeviceObjects();
		SAFE_DELETE(itMonster->second);
		itMonster++;
	}
	m_mapMonsterList.clear();
	CMapEnemyIterator itEnemy = m_mapEnemyList.begin();
	while (itEnemy != m_mapEnemyList.end()) {
		itEnemy->second->InvalidateDeviceObjects();
		itEnemy->second->DeleteDeviceObjects();
		SAFE_DELETE(itEnemy->second);
		itEnemy++;
	}
	m_mapEnemyList.clear();

	// 2008. 12. 11 by ckPark ą°·»´ő¸µ
	SAFE_RELEASE(m_pWaterBumpTexture);
	// end 2008. 12. 11 by ckPark ą°·»´ő¸µ

}

HRESULT CSceneData::InitDeviceObjects()
{
	FLOG("CSceneData::InitDeviceObjects()");
	//	m_pObjectRender = new CObjectRender();
	m_pObjectRender = new CObjRender();
	m_pMonsterRender = new CMonRender();
	m_pSunRender = new CSunRender();
	m_pRainRender = new CRainRender();
	m_pETCRender = new CETCRender();
	m_pFrustum = new CFrustum();

	if (m_bWaterShaderRenderFlag) {
		m_pWater = new CWater();
	}
	LoadCinemaFile();						/// ˝Ăł×¸¶ ĆÄŔĎ ·Îµĺ ÇĎ±â

	return S_OK;
}

HRESULT CSceneData::RestoreDeviceObjects()
{
	FLOG("CSceneData::RestoreDeviceObjects()");
	if (g_pD3dApp->m_dwGameState == _GAME ||
		g_pD3dApp->m_dwGameState == _SHOP ||
		g_pD3dApp->m_dwGameState == _CITY ||
		g_pD3dApp->m_dwGameState == _MAPLOAD) {
		if (m_pGround)
			m_pGround->RestoreDeviceObjects();
		if (m_pETCRender)
			m_pETCRender->RestoreDeviceObjects();
		if (m_pWater && m_bWaterShaderRenderFlag)
			m_pWater->RestoreDeviceObjects();
		RestoreRes();
	}
	if (g_pD3dApp->m_dwGameState == _GAME || g_pD3dApp->m_dwGameState == _SHOP) {
		CMapMonsterIterator itMonster = m_mapMonsterList.begin();
		while (itMonster != m_mapMonsterList.end()) {
			//			itMonster->second->RestoreChat();
			itMonster->second->RestoreDeviceObjects();
			itMonster++;
		}
		CMapEnemyIterator itEnemy = m_mapEnemyList.begin();
		while (itEnemy != m_mapEnemyList.end()) {
			//			itEnemy->second->RestoreChat();
			itEnemy->second->RestoreDeviceObjects();
			itEnemy++;
		}

		// 2004-10-20 by jschoi µđąŮŔĚ˝ş¸¦ ŔŇŔş ČÄżˇ´Â ľČ°ł ĽĽĆĂŔ» ´Ů˝Ă ÇŘľßÇŃ´Ů.
		m_bChangeWeather = TRUE;
	}


	// 2008. 12. 11 by ckPark ą°·»´ő¸µ
	// ąüÇÁ ĹŘ˝şĂÄ »ýĽş 256 * 256 »çŔĚÁî
	if (NULL == m_pWaterBumpTexture) {
		if (FAILED(g_pD3dDev->CreateTexture(WATER_BUMP_TEXTURE_SIZE_X, WATER_BUMP_TEXTURE_SIZE_Y, 1, 0,
											D3DFMT_V8U8, D3DPOOL_MANAGED, &m_pWaterBumpTexture, NULL))) {
			SAFE_RELEASE(m_pWaterBumpTexture);
			return E_FAIL;
		}
	}

	D3DLOCKED_RECT lrDst;
	m_pWaterBumpTexture->LockRect(0, &lrDst, 0, 0);
	BYTE* pDst = (BYTE*)lrDst.pBits;


	for (DWORD y = 0; y < WATER_BUMP_TEXTURE_SIZE_Y; y++) {
		for (DWORD x = 0; x < WATER_BUMP_TEXTURE_SIZE_X; x++) {
			CHAR iDu = (CHAR)(sinf(x / (float)(WATER_BUMP_TEXTURE_SIZE_X)*D3DX_PI * 2.0f) * 16.0f);
			CHAR iDv = (CHAR)(cosf(x / (float)(WATER_BUMP_TEXTURE_SIZE_X)*D3DX_PI * 2.0f) * 32.0f);

			pDst[2 * x + 0] = iDu;
			pDst[2 * x + 1] = iDv;
		}
		pDst += lrDst.Pitch;
	}

	m_pWaterBumpTexture->UnlockRect(0);
	// end 2008. 12. 11 by ckPark ą°·»´ő¸µ
#ifdef _DEBUG_SHADERS
	if (FAILED(g_pD3dDev->CreateVertexShader((DWORD*)g_VShader, &m_pSimpleVS))) {
		return E_FAIL;
	}
	if (FAILED(g_pD3dDev->CreatePixelShader((DWORD*)g_PShader, &m_pSimplePS))) {
		return E_FAIL;
	}
	D3DVERTEXELEMENT9 dwDecl3 [] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		D3DDECL_END()
	};
    
    if( FAILED( g_pD3dDev->CreateVertexDeclaration( dwDecl3, &m_pVertexDeclaration ) ) ) {
        return E_FAIL;
    }

	if (!SUCCEEDED(g_pD3dDev->GetRenderTarget(0, &m_pBackBufferTemp))) {
		return false;
	}

	if (FAILED(g_pD3dDev->CreateTexture(g_pD3dApp->m_nWidth, g_pD3dApp->m_nHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_RefractionTexture, NULL))) {
			SAFE_RELEASE(m_RefractionTexture)
			return E_FAIL;
		}
	if (FAILED(g_pD3dDev->CreateTexture(g_pD3dApp->m_nWidth, g_pD3dApp->m_nHeight, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_ReflectionTexture, NULL))) {
			SAFE_RELEASE(m_ReflectionTexture)
			return E_FAIL;
		}
#endif
	return S_OK;
}

HRESULT CSceneData::InvalidateDeviceObjects()
{
	FLOG("CSceneData::InvalidateDeviceObjects()");
	if (g_pD3dApp->m_dwGameState == _GAME ||
		g_pD3dApp->m_dwGameState == _SHOP ||
		g_pD3dApp->m_dwGameState == _CITY ||
		g_pD3dApp->m_dwGameState == _MAPLOAD) {
		InvalidateRes();
	}

	m_vecAlphaEffectRender.clear();
	m_vecAlphaUnitRender.clear();
	m_vecScanData.clear();

	// 2008. 12. 11 by ckPark ą°·»´ő¸µ
	SAFE_RELEASE(m_pWaterBumpTexture);
	// end 2008. 12. 11 by ckPark ą°·»´ő¸µ

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			HRESULT CSceneData::DeleteDeviceObjects()
/// \brief		
/// \author		dhkwon
/// \date		2004-03-25 ~ 2004-03-25
/// \warning	CBackground¸¦ DeleteRes()żˇĽ­ ÁöżěÁö ľĘ°í ąŰżˇĽ­ Áöżě´Â ŔĚŔŻ´Â
///				°ÔŔÓ ¸®˝şĹ¸Ć® ČÄ ąŮ·Î Ŕç˝ĂŔŰÇŇ °ćżě ¸ĘŔĚ °°Ŕ» °ćżě¸¦ ´ëşńÇĎż© ¸Ţ¸đ¸®żˇĽ­
///				ł»¸®Áö ľĘ°Ô ÇĎ±â Ŕ§ÇŘĽ­ŔĚ´Ů.
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
HRESULT CSceneData::DeleteDeviceObjects()
{
	FLOG("CSceneData::DeleteDeviceObjects()");
	DBGOUT("Pre CSceneData::DeleteDeviceObjects()\n");
	SAFE_DELETE(m_pItemData);
	SAFE_DELETE(m_pWeaponData);

	DeleteCinemaFile();							/// ˝Ăł×¸¶ ĆÄŔĎ ŔüşÎ Áöżě±â
	DeleteRes();
	DeleteTexTileDevice();
	if (m_pGround) {
		m_pGround->DeleteDeviceObjects();
		//SAFE_DELETE(m_pGround)
	}


	// 2008. 12. 11 by ckPark ą°·»´ő¸µ
	SAFE_RELEASE(m_pWaterBumpTexture);
	// end 2008. 12. 11 by ckPark ą°·»´ő¸µ

	DBGOUT("End CSceneData::DeleteDeviceObjects()\n");
	return S_OK;
}

void CSceneData::RenderCity()
{
	g_pD3dDev->LightEnable(1, FALSE);
	g_pD3dDev->LightEnable(2, FALSE);
	g_pD3dDev->LightEnable(3, FALSE);
	g_pD3dDev->SetRenderState(D3DRS_FOGENABLE, TRUE);
	// 2005-01-03 by jschoi
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );		
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );	
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);


	// Vertex Fog Use
	g_pD3dDev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
	g_pD3dDev->SetRenderState(D3DRS_FOGSTART, FtoDW(m_fOrgFogStartValue)); //
	g_pD3dDev->SetRenderState(D3DRS_FOGEND, FtoDW(m_fOrgFogEndValue));
	g_pD3dDev->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
	g_pD3dDev->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
	g_pD3dDev->SetFVF(D3DFVF_FOGVERTEX);
	////////////////////////////////////////////////////////////////////////
	//	m_dwFogColor = GetFogColor(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex,!m_bNight);
	if (m_bNight) {
#ifdef MAP_EDITOR		
		m_dwFogColor = g_pDatabase->GetMapInfo(g_pD3dApp->m_nMapNumber)->NightFogColor;
#else
		m_dwFogColor = g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)->NightFogColor;
#endif
	}
	else {
#ifdef MAP_EDITOR
		m_dwFogColor = g_pDatabase->GetMapInfo(g_pD3dApp->m_nMapNumber)->DayFogColor;
#else
		m_dwFogColor = g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)->DayFogColor;
#endif
	}

	g_pD3dDev->SetRenderState(D3DRS_FOGCOLOR, m_dwFogColor);



	// Object Rendering List -> Render
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	g_pD3dDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	g_pD3dDev->SetRenderState(D3DRS_ALPHAREF, 0x08);

	vectorCObjectChildPtr::iterator itObj(m_vectorCulledObjectPtrList.begin());
	while (itObj != m_vectorCulledObjectPtrList.end()) {
		(*itObj)->Render();
		itObj++;
	}
#ifdef  _OLD_SHADOW_SYSTEM
	// Shadow Render
	// 2005-01-03 by jschoi
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );		
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );		
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);


	g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	g_pD3dDev->SetFVF(D3DFVF_SPRITEVERTEX);

	g_pD3dApp->m_pUnitRender->RenderShadow(g_pShuttleChild);
#endif
}

void CSceneData::Render()
{
	FLOG("CSceneData::Render()");
	g_pD3dDev->SetClipPlane(0,m_reflectPlane);
	g_pD3dDev->LightEnable(2, FALSE);
	g_pD3dDev->LightEnable(3, FALSE);
#ifdef MAP_EDITOR	
	g_pD3dDev->SetRenderState(D3DRS_FOGENABLE, IsFogEnableMap(g_pD3dApp->m_nMapNumber));
#else
	g_pD3dDev->SetRenderState(D3DRS_FOGENABLE, IsFogEnableMap(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex));
#endif
	g_pD3dDev->SetFVF(D3DFVF_FOGVERTEX);
	g_pD3dDev->SetRenderState(D3DRS_FOGCOLOR, m_dwFogColor);
#ifdef  _OLD_SHADOW_SYSTEM
	// Shadow Render
	// 2005-01-03 by jschoi
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );		
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );		
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);


	g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	g_pD3dDev->SetFVF(D3DFVF_SPRITEVERTEX);

	if (g_pSOption->sShadowState > 0) {
		CVecMonsterIterator itMonsterShadowRender = m_vecMonsterShadowRenderList.begin();
		while (itMonsterShadowRender != m_vecMonsterShadowRenderList.end()) {
			(*itMonsterShadowRender)->RenderShadow();
			itMonsterShadowRender++;
		}
	}
#endif
	// 2005-01-03 by jschoi
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );		
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );		
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);


	// Object Rendering List -> Render
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);
#ifdef MAP_EDITOR	
	g_pD3dDev->SetRenderState(D3DRS_FOGENABLE, IsFogEnableMap(g_pD3dApp->m_nMapNumber));
#else
	g_pD3dDev->SetRenderState(D3DRS_FOGENABLE, IsFogEnableMap(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex));
#endif
	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	g_pD3dDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	g_pD3dDev->SetRenderState(D3DRS_ALPHAREF, 0x08);

	g_bDetailDrawFrame = TRUE;	// 2005-01-06 by jschoi - ĽĽşÎ ÄĂ¸µ ±â´ÉŔ» »çżë
	vectorCObjectChildPtr::iterator itObj(m_vectorCulledObjectPtrList.begin());
	while (itObj != m_vectorCulledObjectPtrList.end()) {
		//		if(g_pCharacterChild->GetPickingObj() == (*itObj)->m_pObjectInfo->RenderIndex)
		if (g_pCharacterChild->GetPickingObj() == (*itObj)) {
			g_pD3dDev->LightEnable(2, TRUE);
			g_pD3dDev->LightEnable(3, TRUE);
		}
		(*itObj)->Render();
		itObj++;

		g_pD3dDev->LightEnable(2, FALSE);
		g_pD3dDev->LightEnable(3, FALSE);
	}
	g_bDetailDrawFrame = FALSE;	// 2005-01-06 by jschoi - ĽĽşÎ ÄĂ¸µ ±â´ÉŔ» şą±¸(»çżë ľČÇÔ)



#ifdef _DEBUG
	// 2005-04-12 by jschoi - ŔĚşĄĆ® żŔşęÁ§Ć® ş¸ż©ÁÖ±â
	// 2005-11-16 by ispark - °ü¸®ŔÚ¸¸ ş¸ż©ÁÖ±â
	if (g_pD3dApp->m_bEventObjectRender &&
		COMPARE_RACE(g_pShuttleChild->m_myShuttleInfo.Race, RACE_OPERATION) &&
		g_pGround->m_pObjectEvent && g_pGround->m_pObjectEvent->m_pChild) {
		CObjectChild* pObject = (CObjectChild*)g_pGround->m_pObjectEvent->m_pChild;
		while (pObject) {
			pObject->Render();
			pObject = (CObjectChild*)pObject->m_pNext;
		}
	}
#endif

	// Shadow Render
	// 2005-01-03 by jschoi
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );		
	//	g_pD3dDev->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );		
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	//	g_pD3dDev->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);


	g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pD3dDev->SetRenderState(D3DRS_ALPHAREF, 0x00000008);
	g_pD3dDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	g_pD3dDev->SetFVF(D3DFVF_SPRITEVERTEX);

	// 2005-08-09 by ispark
	// ±×¸˛ŔÚ ·»´ő¸µŔ» Ŕ§ÇŃ ÄÚµĺ
	g_pD3dDev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
	g_pD3dDev->SetRenderState(D3DRS_DEPTHBIAS, 0);

	float fOffset = 0.0f;
	if (g_pGameMain->m_pCityBoard) {
		g_pGameMain->m_pCityBoard->Render(&fOffset);
	}

#ifdef  _OLD_SHADOW_SYSTEM
	// 2005-07-13 by ispark
	// Äł¸ŻĹÍ ±×¸˛ŔÚŔÎÁö ŔŻ´Ö ±×¸˛ŔÚŔÎÁö ĆÇ´Ü
	if (g_pD3dApp->m_bCharacter) {
		// Äł¸ŻĹÍ ±×¸˛ŔÚ´Â ZąöĆŰ¸¦ »çżëÇŃ´Ů.
		// 2006-04-07 by ispark, ąŘżˇ ĽŇ˝ş¸¦ Áöżě°í Ŕ§żˇ ľËĆÄ 2ÁŮ·ÎĽ­ ŔĚĆĺĆ®żÍ Á¶Č­°ˇ ŔĚ·çľîÁł´Ů.
		g_pD3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);
		g_pD3dDev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f));
		fOffset += -0.00002f;
		g_pD3dDev->SetRenderState(D3DRS_DEPTHBIAS, F2DW(fOffset));
		g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		g_pD3dApp->m_pCharacterRender->RenderShadow();
		g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
	else {
		if (g_pShuttleChild->m_nAlphaValue == SKILL_OBJECT_ALPHA_NONE) {
			g_pD3dApp->m_pUnitRender->RenderShadow(g_pShuttleChild);
		}
	}

	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, FALSE);				// 2005-08-04 by ispark
	g_pD3dDev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
	g_pD3dDev->SetRenderState(D3DRS_DEPTHBIAS, 0);

	if (g_pSOption->sShadowState) {
		CVecEnemyIterator itEnemy = m_vecEnemyShadowRenderList.begin();
		while (itEnemy != m_vecEnemyShadowRenderList.end()) {
			g_pD3dDev->SetRenderState(D3DRS_ZENABLE, FALSE);		// 2005-08-04 by ispark
			g_pD3dDev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
			g_pD3dDev->SetRenderState(D3DRS_DEPTHBIAS, 0);

			// 2005-07-13 by ispark
			// Ŕű Äł¸ŻĹÍ ±×¸˛ŔÚŔÎÁö ŔŻ´Ö ±×¸˛ŔÚŔÎÁö ĆÇ´Ü
			CUnitData* pData = (CUnitData*)(*itEnemy);
			CEnemyData* pEnemy = (CEnemyData*)(*itEnemy);
			if (pEnemy->m_bEnemyCharacter) {
				// 2006-04-07 by ispark, ąŘżˇ ĽŇ˝ş¸¦ Áöżě°í Ŕ§żˇ ľËĆÄ 2ÁŮ·ÎĽ­ ŔĚĆĺĆ®żÍ Á¶Č­°ˇ ŔĚ·çľîÁł´Ů.
				g_pD3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);
				g_pD3dDev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f));
				fOffset += -0.00002f;
				g_pD3dDev->SetRenderState(D3DRS_DEPTHBIAS, F2DW(fOffset));
				g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
				g_pD3dApp->m_pCharacterRender->RenderShadow(*itEnemy);
				g_pD3dDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			}
			else {
				if ((*itEnemy)->m_nAlphaValue == SKILL_OBJECT_ALPHA_NONE) {
					g_pD3dApp->m_pUnitRender->RenderShadow(*itEnemy);
				}
			}

			itEnemy++;
		}
	}
#endif

	g_pD3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pD3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pD3dDev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f));
	g_pD3dDev->SetRenderState(D3DRS_DEPTHBIAS, F2DW(0.0f));

	/*
	// Enemy Rendering List -> Render
	CVecEnemyIterator itEnemyRender = m_vecEnemyRenderList.begin();
	while(itEnemyRender != m_vecEnemyRenderList.end())
	{
	(*itEnemyRender)->Render();
	itEnemyRender++;
	}

	// Monster Rendering List -> Render
	CVecMonsterIterator itMonsterRender = m_vecMonsterRenderList.begin();
	while(itMonsterRender != m_vecMonsterRenderList.end())
	{
	(*itMonsterRender)->Render();
	itMonsterRender++;
	}
	*/
	// 2006-07-29 by ispark, °łŔÎ »óÁˇ Picking

	LPDIRECT3DSURFACE9 RenderTarget, Surface;
	m_RefractionTexture->GetSurfaceLevel(0,&Surface);
	g_pD3dDev->GetRenderTarget(0,&RenderTarget);
	g_pD3dDev->StretchRect(RenderTarget,NULL, Surface,NULL,D3DTEXF_NONE);
	RenderTarget->Release();
	g_pD3dDev->SetClipPlane(1,m_reflectPlane);
	m_vecAlphaUnitRender.clear();
	vector<CUnitData*>::iterator itUnit = m_vecUnitRenderList.begin();
	while (itUnit != m_vecUnitRenderList.end()) {
		// 2006-11-16 by ispark, ľËĆÄ ·»´ő¸µ ĂŁ±â
		if ((*itUnit)->m_nAlphaValue < SKILL_OBJECT_ALPHA_NONE) {
			m_vecAlphaUnitRender.push_back(*itUnit);
		}
		else {
			if (g_pCharacterChild->GetPickingBazaar() == (*itUnit)) {
				g_pD3dDev->LightEnable(2, TRUE);
				g_pD3dDev->LightEnable(3, TRUE);
			}
			(*itUnit)->Render();

			g_pD3dDev->LightEnable(2, FALSE);
			g_pD3dDev->LightEnable(3, FALSE);
		}

		itUnit++;
	}

	// 2005-07-22 by ispark
	if (g_pD3dApp->m_bCharacter)
		g_pCharacterChild->Render();

	if (!IS_CITY_MAP_INDEX(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)) {
		if (m_bWaterShaderRenderFlag) {
			m_pWater->Render();
		}
		else {
			RenderWater();
		}
	}
	if (m_pRainRender->bRenderRain)//m_pRainRender) //by Inet
		m_pRainRender->Render();
	if (m_byWeatherType != WEATHER_RAINY &&			// şń
		m_byWeatherType != WEATHER_SNOWY &&			// ´«
		m_byWeatherType != WEATHER_CLOUDY &&		// ±¸¸§ł¤
		m_byWeatherType != WEATHER_FOGGY &&			// ľČ°łł¤
#ifdef MAP_EDITOR		
		IsSunRenderEnable(g_pD3dApp->m_nMapNumber))
#else
		IsSunRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex))
#endif
	{
		if (m_pSunData && !m_bNight)	// ĹÂľçŔÇ ·»ÁîÇĂ·ąľî
			m_pSunData->RenderRens();
	}


	if (g_pShuttleChild) {
		if (g_pShuttleChild->m_pSecondaryWeapon) {
			if (g_pShuttleChild->m_pSecondaryWeapon->GetAttackMode() == ATT_TYPE_GROUND_BOMBING_SEC ||
				g_pShuttleChild->m_pSecondaryWeapon->GetAttackMode() == ATT_TYPE_AIR_BOMBING_SEC)
				g_pGameMain->m_pInfSkill->RenderGroundTarget();
		}
	}
}

void CSceneData::RenderObjectNames()
{
	// 2005-02-16 by jschoi - ¸Ę »ó żŔşęÁ§Ć®ŔÇ ŔĚ¸§ ·»´ő¸µ
	auto itNameObj(m_vectorCulledObjectPtrList.begin());
	while (itNameObj != m_vectorCulledObjectPtrList.end())
	{
		(*itNameObj)->ObjectNameRender();
		++itNameObj;
	}

	if (m_pItemData)
	{
		auto* pItem = (CItemData*)m_pItemData->m_pChild;
		while (pItem)
		{
			pItem->RenderItemName(); // ľĆŔĚĹŰ ŔĚ¸§ŔĚ ŔÖŔ» °ćżě ·»´ő¸µ
			pItem = (CItemData*)pItem->m_pNext;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void TickScanObeject()
/// \brief		˝şÄµ żŔşęÁ§Ć®ŔÇ ŔŻČż˝Ă°ŁŔĚ ´ŮµÇ¸é »čÁ¦˝ĂĹ˛´Ů.
/// \author		dgwoo
/// \date		2007-02-12 ~ 2007-02-12
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CSceneData::TickScanObeject()
{
	vector<CItemData*>::iterator it = m_vecScanData.begin();
	while (it != m_vecScanData.end()) {
		if ((*it)->m_fItemCheckTime <= 0) {// ŔŻČż˝Ă°ŁŔĚ ´ŮµÇľî »čÁ¦ ˝ĂĹ˛´Ů.
			SAFE_DELETE(*it);
			//it = m_vecScanData.erase(it);
			it = m_vecScanData.erase(it); //14-03-2022 by Inet
		}
		else {// 
			(*it)->Tick();
			it++;
		}
	}

}

void CSceneData::Tick()
{
	FLOG("CSceneData::Tick()");

	// 2006-07-18 by ispark, ľĆŔĚĹŰ ˝Ŕµć µô·ąŔĚ ˝Ă°Ł
	m_fGetItemAllDelay += g_pD3dApp->GetElapsedTime();
	if (m_fGetItemAllDelay > GET_ITEM_IN_TIME + 1.0f) {
		// 2ĂĘ¸¦ łŃÁö ľĘ°Ô ÇŃ´Ů.
		m_fGetItemAllDelay = GET_ITEM_IN_TIME + 1.0f;
	}
	if (m_fGetItemMessage >= 0) {
		m_fGetItemMessage -= g_pD3dApp->GetElapsedTime();

	}



	// 2005-04-01 by jschoi
	if (g_pTutorial->IsTutorialMode() == FALSE ||
		g_pTutorial->IsUseShuttleTick() == TRUE) {
		if (m_pItemData) {
			m_pItemData->Tick();
		}
		if (m_pWeaponData) {
			m_pWeaponData->Tick();
		}
	}

	// 2010. 03. 05 by jskim ¸Ę ·Îµů Áß Ŕ©µµżě Č­¸é ŔüČŻ˝Ă ÁöÇü »ç¶óÁö´Â ąö±× ĽöÁ¤
	// 	if(m_pGround)
	// 	{
	// 		m_pGround->Tick(g_pD3dApp->GetElapsedTime());
	// 		if(m_pGround->m_pQuad  && IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) )
	// 		{
	// 			m_pGround->m_pQuad->Tick();
	// 		}
	// 	}
	if (m_pGround) {
		m_pGround->Tick(g_pD3dApp->GetElapsedTime());
		if (m_pGround->m_pQuad && IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)) {
			if (!g_pD3dApp->IsDeviceLost())
				m_pGround->m_pQuad->Tick();
		}
	}
	//end 2010. 03. 05 by jskim ¸Ę ·Îµů Áß Ŕ©µµżě Č­¸é ŔüČŻ˝Ă ÁöÇü »ç¶óÁö´Â ąö±× ĽöÁ¤
	CheckDay();
	CheckWeather();
	if (m_bWaterShaderRenderFlag)
		m_pWater->Tick();

	//2004 - 06 - 4 add by jsy
	//CheckObjectRenderList();


	//	if(m_pSunData)
	if (m_pSunData &&
		m_byWeatherType != WEATHER_RAINY &&			// şń
		m_byWeatherType != WEATHER_SNOWY &&			// ´«
		//		m_byWeatherType != WEATHER_CLOUDY &&		// ±¸¸§ł¤
		m_byWeatherType != WEATHER_FOGGY &&			// ľČ°łł¤
#ifdef MAP_EDITOR		
		IsSunRenderEnable(g_pD3dApp->m_nMapNumber))
#else
		IsSunRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex))
#endif
	{
		m_pSunData->Tick();
	}
	if (m_pRainList) {
		m_pRainList->Tick();
	}


	// 2005-04-21 by jschoi - Tutorial
	if (g_pTutorial->IsTutorialMode() == FALSE ||
		g_pTutorial->IsUseShuttleTick() == TRUE) {
		int nDel = 0;
		m_vecUnitRenderList.clear();
		m_vecEnemyRenderList.clear();
#ifdef  _OLD_SHADOW_SYSTEM
		m_vecEnemyShadowRenderList.clear();
#endif
		CMapEnemyIterator itEnemy = m_mapEnemyList.begin();
		while (itEnemy != m_mapEnemyList.end()) {
			CEnemyData* pEnemy = itEnemy->second;
			ASSERT_ASSERT(pEnemy);
			pEnemy->Tick();
			if (!pEnemy->m_bUsing) {
				if (pEnemy->m_infoCharacter.CharacterInfo.ClientIndex == g_pShuttleChild->m_stObserve.ClientIndex) {// 2007-06-19 by dgwoo ŔŻŔú°ˇ °­Á¦ Áľ·áłŞ ŔĚµż˝Ă żÉŔúąö¸¦ ľČŔüÇĎ°Ô ÇŘÁ¦ÇĎ±â Ŕ§ÇŘ.
					g_pShuttleChild->ObserveCancelUpdateInfo();
					g_pShuttleChild->ObserveEnd();
				}
				nDel = pEnemy->m_infoCharacter.CharacterInfo.ClientIndex;
			}
			itEnemy++;
		}
		if (nDel) {
			itEnemy = m_mapEnemyList.find(nDel);
			if (itEnemy != m_mapEnemyList.end()) {
				DeleteFieldItemOfUnitData(itEnemy->second);
				DeleteToBlockData(itEnemy->second);
				itEnemy->second->InvalidateDeviceObjects();
				itEnemy->second->DeleteDeviceObjects();
				SAFE_DELETE(itEnemy->second);
				m_mapEnemyList.erase(itEnemy);
			}
		}
		nDel = 0;
		m_vecMonsterRenderList.clear();
#ifdef  _OLD_SHADOW_SYSTEM
		m_vecMonsterShadowRenderList.clear();
#endif
		CMapMonsterIterator itMonster = m_mapMonsterList.begin();
		while (itMonster != m_mapMonsterList.end()) {
			itMonster->second->Tick();
			if (!itMonster->second->m_bUsing)
				nDel = itMonster->second->m_info.MonsterIndex;
			itMonster++;
		}
		if (nDel) {
			itMonster = m_mapMonsterList.find(nDel);
			if (itMonster != m_mapMonsterList.end()) {
				DeleteFieldItemOfUnitData(itMonster->second);
				DeleteToBlockData(itMonster->second);
				itMonster->second->InvalidateDeviceObjects();
				itMonster->second->DeleteDeviceObjects();
				SAFE_DELETE(itMonster->second);
				m_mapMonsterList.erase(itMonster);
			}
		}
		// 2004-12-16 by jschoi - ShuttleChildµµ Ć÷ÇÔ
		// 2005-07-28 by ispark Äł¸ŻĹÍŔĎ¶§´Â ÂďÁö ¸¶¶ó
		if (g_pD3dApp->m_bCharacter == FALSE) {
			m_vecUnitRenderList.push_back((CUnitData*)g_pShuttleChild);
			sort(m_vecUnitRenderList.begin(), m_vecUnitRenderList.end(), CompareUnit());
		}

		// 2007-02-12 by dgwoo ˝şÄµ°ü·Ă żŔşęÁ§Ć® Tick
		TickScanObeject();
	}

	// 2005-04-22 by jschoi - Tutorial
	if (m_vecUnitRenderList.empty()) {
		if (g_pD3dApp->m_bCharacter == FALSE) {
			m_vecUnitRenderList.push_back((CUnitData*)g_pShuttleChild);
		}
	}

}
#define MAPNAME_CHECK_SUCCESS 0
BOOL CSceneData::InitBackground()
{
	FLOG("CSceneData::InitBackground()");
	int i;
	// <Game Start>   or   <Game ReStart>  ˝Ă ż©·Ż ĹŘ˝şĂÄµéŔÇ Restore
	if (m_bIsRestore) {
		// żŔşęÁ§Ć® ą× ¸ó˝şĹÍ ±âĹ¸ EffectŔÇ ·Îµĺ
		InitRes();
		RestoreRes();
		m_bIsRestore = FALSE;
	}
	FILE* readMap;
	int re = -1;
	WORKSPACE iWorkspace;
	PROJECTINFO iProject;
	char strPath[256];
	g_pD3dApp->LoadPath(strPath, IDS_DIRECTORY_MAP, "ms.wok");//RC_MAP_WORKSPACE );//
	if (strlen(strPath) > 0) {
		readMap = fopen(strPath, "rb");
		fseek(readMap, 20, SEEK_SET);
		fread(&iWorkspace, sizeof(WORKSPACE), 1, readMap);
		int a = iWorkspace.numberOfProject;
		for (i = 0; i < a; i++) {
			fread(&iProject, sizeof(PROJECTINFO), 1, readMap);
			char buf[32];
#ifdef MAP_EDITOR
			wsprintf(buf, "%04d", g_pD3dApp->m_nMapNumber);
#else
			wsprintf(buf, "%04d", g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex);
#endif
			re = strcmp(iProject.strProjectName, buf);

			if (re == MAPNAME_CHECK_SUCCESS) {
				DBGOUT("MAP Project Info[name:%s]\n   [Water Height:%.2f]\n   Day[D:%.2f,%.2f,%.2f][A:%.2f,%.2f,%.2f]\n   Night[D:%.2f,%.2f,%.2f][A:%.2f,%.2f,%.2f]\n",
					   iProject.strProjectName, iProject.fWaterHeight,
					   iProject.fDiffuseR1, iProject.fDiffuseG1, iProject.fDiffuseB1,
					   iProject.fAmbientR1, iProject.fAmbientG1, iProject.fAmbientB1,
					   iProject.fDiffuseR2, iProject.fDiffuseG2, iProject.fDiffuseB2,
					   iProject.fAmbientR2, iProject.fAmbientG2, iProject.fAmbientB2
				);

				// map type °áÁ¤
				int nTemp = atoi(iProject.strProjectName);
				if (nTemp > 1000) {
					// 2006-07-19 by ispark, żľłŻ µµ˝Ă¸¦ ĆÇ´ÜÇĎ±â Ŕ§ÇŃ°Ĺ »čÁ¦
					//					m_byMapType =  nTemp / 1000;
					m_byMapType = MAP_TYPE_NORMAL_FIELD;
				}
				else {
					if (0 == strcmp(iProject.strProjectName, "0100"))
						m_byMapType = MAP_TYPE_TUTORIAL;
					//					else if(0 == strcmp(iProject.strProjectName,"0106"))
					//						m_byMapType = MAP_TYPE_CITY;
					else
						m_byMapType = MAP_TYPE_NORMAL_FIELD;
				}
				break;
			}
		}
		fclose(readMap);
	}
	else
		return FALSE;// error
	if (re == MAPNAME_CHECK_SUCCESS) {
		if (m_pGround) {
			if (m_vecEnemyBlockList) {
				for (int i = 0; i < m_nBlockSizeX; i++) {
					for (int j = 0; j < m_nBlockSizeY; j++) {
						m_vecEnemyBlockList[i * m_nBlockSizeY + j].clear();
						//SAFE_DELETE(m_vecEnemyBlockList[i*m_nBlockSizeY + j]);
					}
				}
				SAFE_DELETE_ARRAY(m_vecEnemyBlockList);
			}
			if (m_vecMonsterList) {
				for (int i = 0; i < m_nBlockSizeX; i++) {
					for (int j = 0; j < m_nBlockSizeY; j++) {
						m_vecMonsterList[i * m_nBlockSizeY + j].clear();
						//SAFE_DELETE(m_vecMonsterList[i*m_nBlockSizeY + j]);
					}
				}
				SAFE_DELETE_ARRAY(m_vecMonsterList);
			}
			/*			if(m_vecObjectList)
			{
			for(int i = 0; i < m_nBlockSizeX;i++)
			{
			for(int j = 0;j < m_nBlockSizeY;j++)
			{
			m_vecObjectList[i*m_nBlockSizeY + j].clear();
			}
			}
			SAFE_DELETE_ARRAY(m_vecObjectList);
			}
			*/
		}
		m_nBlockSizeX = iProject.sXSize / 3;
		m_nBlockSizeY = iProject.sYSize / 3;
		if (iProject.sXSize % 3)
			m_nBlockSizeX++;
		if (iProject.sYSize % 3)
			m_nBlockSizeY++;
		m_vecEnemyBlockList = new CVecEnemyList[m_nBlockSizeX * m_nBlockSizeY];
		m_vecMonsterList = new CVecMonsterList[m_nBlockSizeX * m_nBlockSizeY];
		//		m_vecObjectList = new CVecObjectList[m_nBlockSizeX*m_nBlockSizeY];

		int checkMapname = -1;
		if (m_pGround) {
			char buf[32];
#ifdef MAP_EDITOR
			wsprintf(buf, "%04d", g_pD3dApp->m_nMapNumber);
#else
			wsprintf(buf, "%04d", g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex);
#endif

			checkMapname = strcmp(m_pGround->m_projectInfo.strProjectName, buf);
		}
		//	if(checkMapname != MAPNAME_CHECK_SUCCESS)
		{
			EnterCriticalSection(&g_pD3dApp->m_cs);
			if (m_pGround) {
				m_pGround->DeleteDeviceObjects();
				SAFE_DELETE(m_pGround);
			}
			if (m_pETCRender) {
				m_pETCRender->DeleteDeviceObjects();
			}
			if (m_pWater && m_bWaterShaderRenderFlag)
				m_pWater->DeleteDeviceObjects();
			m_pGround = new CBackground(iProject);//,usetex);
			if (m_pGround) {
				if (FAILED(m_pGround->InitDeviceObjects())) {
					SAFE_DELETE(m_pGround);
					LeaveCriticalSection(&g_pD3dApp->m_cs);
					return TRUE;							// 2006-12-19 by ispark, ´Ů¸Ą ¸Ţ˝ĂÁö¸¦ ¶çżě±â Ŕ§ÇŘĽ­ ±âş»ŔűŔÎ ¸Ţ˝ĂÁö¸¦ »ý·«°í TRUE°ŞŔ» ÁŘ´Ů.
				}
			}
			if (m_pGround)
				m_pGround->RestoreDeviceObjects();
			if (m_pETCRender)
				m_pETCRender->InitDeviceObjects();
			if (m_pWater && m_bWaterShaderRenderFlag)
				m_pWater->InitDeviceObjects();
			if (m_pETCRender)
				m_pETCRender->RestoreDeviceObjects();
			if (m_pWater && m_bWaterShaderRenderFlag)
				m_pWater->RestoreDeviceObjects();

			// 2007-11-20 by bhsohn ¸Ę·Îµů ÇĎ´Â ąć˝Ä şŻ°ć
			{
				g_pD3dApp->UpdateGameStartMapInfo();
			}
			// end 2007-11-20 by bhsohn ¸Ę·Îµů ÇĎ´Â ąć˝Ä şŻ°ć
			LeaveCriticalSection(&g_pD3dApp->m_cs);
			return TRUE;
			// ŔĚÇĎ Ŕý»č
		}

		EnterCriticalSection(&g_pD3dApp->m_cs);
		m_pGround->RestoreDeviceObjects();
		if (m_pETCRender) {
			m_pETCRender->InvalidateDeviceObjects();
			m_pETCRender->DeleteDeviceObjects();
			m_pETCRender->InitDeviceObjects();
			m_pETCRender->RestoreDeviceObjects();
		}
		if (m_pWater && m_bWaterShaderRenderFlag) {
			m_pWater->InvalidateDeviceObjects();
			m_pWater->DeleteDeviceObjects();
			m_pWater->InitDeviceObjects();
			m_pWater->RestoreDeviceObjects();
		}
		// 2007-11-20 by bhsohn ¸Ę·Îµů ÇĎ´Â ąć˝Ä şŻ°ć
		{
			g_pD3dApp->UpdateGameStartMapInfo();
		}
		// end 2007-11-20 by bhsohn ¸Ę·Îµů ÇĎ´Â ąć˝Ä şŻ°ć

		LeaveCriticalSection(&g_pD3dApp->m_cs);
		return TRUE;
	}
	else {
		DBGOUT("Map Loading State : ERROR [Map Number = %d]\n", g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex);
		g_pD3dApp->NetworkErrorMsgBox(STRERR_C_RESOURCE_0011);// "¸Ę·Îµůżˇ ˝ÇĆĐÇĎż´˝Ŕ´Ď´Ů. Ĺ¬¶óŔĚľđĆ®¸¦ Áľ·áÇŐ´Ď´Ů."
	}
	return FALSE;
}

HRESULT CSceneData::LoadTex()
{
	FLOG("CSceneData::LoadTex()");
	int i;
	for (i = 0; i < TEXTILE_NUM; i++)
		SAFE_RELEASE(m_pCreateTexture[i]);

	DataHeader* pHeader;
	CGameData* pData = new CGameData;
	char strPath[256];
	int nCont = 0;
	wsprintf(strPath, ".\\Res-Map\\mud");
	if (pData->SetFile(strPath, FALSE, NULL, 0)) {
		char* p;
		pHeader = pData->Find(m_pGround->m_projectInfo.strProjectName);
		if (pHeader) {
			p = pHeader->m_pData;
			p += 20;
			memcpy_s(&nCont, sizeof(int), p, sizeof(int));
			p += sizeof(int);
			for (i = 0; i < nCont; i++) {
				int nTexnum;
				memcpy_s(&nTexnum, sizeof(int), p, sizeof(int));

				wsprintf(strPath, "05%06d", nTexnum);
				pHeader = m_pData->Find(strPath);
				if (pHeader) {
					if (FAILED(D3DXCreateTextureFromFileInMemory(g_pD3dDev,
																 pHeader->m_pData,
																 pHeader->m_DataSize,
																 &m_pCreateTexture[nTexnum])))
						return E_FAIL;
				}

				p += sizeof(int);
			}
			SAFE_DELETE(pData);
		}
		else {
			SAFE_DELETE(pData);
			return E_FAIL;
		}
	}
	else {
		SAFE_DELETE(pData);
		return E_FAIL;
	}

	return S_OK;
}

VOID CSceneData::InitRes()
{
	FLOG("CSceneData::InitRes()");
	////////////////////////////////////// Restart , Game Start , Warf ˝Ă ¸®ĽŇ˝ş ///////////////////////////////////
	EnterCriticalSection(&g_pD3dApp->m_cs);// by dhkwon 2002.12.16
	if (m_pObjectRender)
		m_pObjectRender->InitDeviceObjects();
	if (m_pMonsterRender)
		m_pMonsterRender->InitDeviceObjects();
	//	if(m_pTraceRender)
	//		m_pTraceRender->InitDeviceObjects();

	if (!m_pWeaponData)
		m_pWeaponData = new CWeapon;
	if (!m_pItemData)
		m_pItemData = new CAtumNode;

	if (m_pSunRender)
		m_pSunRender->InitDeviceObjects();
	if (!m_pSunData)
		m_pSunData = new CSunData();

	if (m_pRainRender)
		m_pRainRender->InitDeviceObjects();
	if (!m_pRainList)
		m_pRainList = new CAtumNode;
	LeaveCriticalSection(&g_pD3dApp->m_cs);
}

VOID CSceneData::RestoreRes()
{
	FLOG("CSceneData::RestoreRes()");
	////////////////////////////////////// Restart , Game Start , Warf ˝Ă ¸®ĽŇ˝ş ///////////////////////////////////
	EnterCriticalSection(&g_pD3dApp->m_cs);
	if (m_pObjectRender)
		m_pObjectRender->RestoreDeviceObjects();
	if (m_pMonsterRender)
		m_pMonsterRender->RestoreDeviceObjects();
	//	if(m_pTraceRender)
	//		m_pTraceRender->RestoreDeviceObjects();

	if (m_pSunRender)
		m_pSunRender->RestoreDeviceObjects();

	if (m_pRainRender)
		m_pRainRender->RestoreDeviceObjects();
	LeaveCriticalSection(&g_pD3dApp->m_cs);
}

VOID CSceneData::InvalidateRes()
{
	FLOG("CSceneData::InvalidateRes()");
	////////////////////////////////////// Restart , Game End , Warf ˝Ă ¸®ĽŇ˝ş ///////////////////////////////////
		EnterCriticalSection(&g_pD3dApp->m_cs);
	// Item Ŕ» Áöżî´Ů.(¸Ę»óŔÇ ¶°µąľĆ´Ů´Ď´Â ľĆŔĚĹŰµé)
	if (m_pItemData) {
		CAtumNode* pItem = (CAtumNode*)m_pItemData->m_pChild;
		while (pItem) {
			pItem->m_bUsing = FALSE;
			pItem = pItem->m_pNext;
		}
		m_pItemData->Tick();
	}
	// Weapon Ŕ» Áöżî´Ů.
	if (m_pWeaponData) {
		CAtumNode* pWeapon = (CAtumNode*)m_pWeaponData->m_pChild;
		while (pWeapon) {
			pWeapon->m_bUsing = FALSE;
			//			if(pWeapon->m_dwPartType == _MINE)
			//			{
			//				((CWeaponMineData *)pWeapon)->CheckDeleteMineSendData();
			//			}
			pWeapon = pWeapon->m_pNext;
		}
		m_pWeaponData->Tick();
	}
	CMapMonsterIterator itMonster = m_mapMonsterList.begin();
	while (itMonster != m_mapMonsterList.end()) {
		//		itMonster->second->InvalidateChat();
		itMonster->second->InvalidateDeviceObjects();
		itMonster++;
	}
	CMapEnemyIterator itEnemy = m_mapEnemyList.begin();
	while (itEnemy != m_mapEnemyList.end()) {
		//		itEnemy->second->InvalidateChat();
		itEnemy->second->InvalidateDeviceObjects();
		itEnemy++;
	}
	if (m_pObjectRender)
		m_pObjectRender->InvalidateDeviceObjects();
	if (m_pMonsterRender)
		m_pMonsterRender->InvalidateDeviceObjects();
	//	if(m_pTraceRender)
	//		m_pTraceRender->InvalidateDeviceObjects();
	if (m_pSunRender)
		m_pSunRender->InvalidateDeviceObjects();

	if (m_pRainRender)
		m_pRainRender->InvalidateDeviceObjects();
	if (m_pGround) {
		m_pGround->InvalidateDeviceObjects();
	}
	if (m_pETCRender) {
		m_pETCRender->InvalidateDeviceObjects();
	}
	if (m_pWater && m_bWaterShaderRenderFlag) {
		m_pWater->InvalidateDeviceObjects();
	}
		LeaveCriticalSection(&g_pD3dApp->m_cs);
}


VOID CSceneData::DeleteRes()
{
	FLOG("CSceneData::DeleteRes()");
	////////////////////////////////////// Restart , Game End , Warf ˝Ă ¸®ĽŇ˝ş ///////////////////////////////////
//		EnterCriticalSection(&g_pD3dApp->m_cs);
	CMapMonsterIterator itMonster = m_mapMonsterList.begin();
	while (itMonster != m_mapMonsterList.end()) {
		itMonster->second->DeleteDeviceObjects();
		SAFE_DELETE(itMonster->second);
		itMonster++;
	}
	m_mapMonsterList.clear();
	CMapEnemyIterator itEnemy = m_mapEnemyList.begin();
	while (itEnemy != m_mapEnemyList.end()) {
		itEnemy->second->DeleteDeviceObjects();
		SAFE_DELETE(itEnemy->second);
		itEnemy++;
	}
	m_mapEnemyList.clear();

	m_vecUnitRenderList.clear();
	m_vecEnemyRenderList.clear();
	m_vecMonsterRenderList.clear();

#ifdef  _OLD_SHADOW_SYSTEM
	m_vecEnemyShadowRenderList.clear();
	m_vecMonsterShadowRenderList.clear();
#endif
	if (m_vecEnemyBlockList) {
		for (int i = 0; i < m_nBlockSizeX; i++) {
			for (int j = 0; j < m_nBlockSizeY; j++) {
				m_vecEnemyBlockList[i * m_nBlockSizeY + j].clear();
			}
		}
		//		SAFE_DELETE_ARRAY(m_vecEnemyBlockList);
	}
	if (m_vecMonsterList) {
		for (int i = 0; i < m_nBlockSizeX; i++) {
			for (int j = 0; j < m_nBlockSizeY; j++) {
				m_vecMonsterList[i * m_nBlockSizeY + j].clear();
			}
		}
	}
	/*	if(m_vecObjectList)
	{
	for(int i = 0; i < m_nBlockSizeX;i++)
	{
	for(int j = 0;j < m_nBlockSizeY;j++)
	{
	m_vecObjectList[i*m_nBlockSizeY + j].clear();
	}
	}
	SAFE_DELETE_ARRAY(m_vecObjectList);
	}
	*/
	m_vectorRangeObjectPtrList.clear();
	m_vectorCollisionObjectPtrList.clear();
	m_vectorCulledObjectPtrList.clear();

	if (m_pObjectRender) {
		m_pObjectRender->DeleteDeviceObjects();
	}
	if (m_pMonsterRender) {
		m_pMonsterRender->DeleteDeviceObjects();
	}
	//	if(m_pTraceRender)
	//		m_pTraceRender->DeleteDeviceObjects();
	if (m_pSunRender) {
		m_pSunRender->DeleteDeviceObjects();
	}
	/*	if(m_pGround)
	{
	m_pGround->DeleteDeviceObjects();
	}
	*/
	if (g_pD3dApp->m_pEffectList) {
		g_pD3dApp->DeleteEffectList();
		g_pD3dApp->m_pEffectList->Tick();
		//		SAFE_DELETE(m_pEffectList);
	}

	if (m_pETCRender) {
		m_pETCRender->DeleteDeviceObjects();
	}
	if (m_pWater && m_bWaterShaderRenderFlag)
		m_pWater->DeleteDeviceObjects();
	if (m_pRainList) {
		CAtumNode* pRain = (CAtumNode*)m_pRainList->m_pChild;
		while (pRain) {
			pRain->m_bUsing = FALSE;
			pRain = pRain->m_pNext;
		}
		m_pRainList->Tick();
		SAFE_DELETE(m_pRainList);
	}
	if (m_pRainRender) {
		m_pRainRender->DeleteDeviceObjects();
	}
	m_bIsRestore = TRUE;
	//	LeaveCriticalSection(&g_pD3dApp->m_cs);
}


VOID CSceneData::DeleteTexTileDevice()
{
	FLOG("CSceneData::DeleteTexTileDevice()");
	int i;
	for (i = 0; i < TEXTILE_NUM; i++) {
		SAFE_RELEASE(m_pCreateTexture[i]);
	}
}

VOID CSceneData::SetShuttleLandState(CAtumData* pNode)
{
	FLOG("CSceneData::SetShuttleLandState(CAtumData * pNode)");
	D3DXMATRIX mat;
	D3DXVECTOR3 dir, vSide;//, orig;
	int x = ((int)pNode->m_vPos.x) / TILE_SIZE;
	int z = ((int)pNode->m_vPos.z) / TILE_SIZE;
	if (m_pGround->m_pTileInfo[x * m_pGround->m_projectInfo.sYSize + z].bEnableLand) {
		/*		dir = D3DXVECTOR3(0,-1,0);
		vSide = D3DXVECTOR3(0,0,1);
		D3DXMatrixLookAtLH(&mat,&pNode->m_vPos,&(pNode->m_vPos + dir),&vSide);
		float fTemp1 = pNode->m_vPos.y - g_pD3dApp->m_pObjectRender->CheckCollMesh(mat,pNode->m_vPos);//+ 5.0f;
		float fTemp2 = m_pGround->CheckHeightMap(pNode->m_vPos);
		if(fTemp2 < fTemp1)
		pNode->m_vPos.y =  fTemp1;
		else
		pNode->m_vPos.y = fTemp2;
		*/		pNode->m_dwState = _LANDED;
		((CShuttleChild*)pNode)->m_bIsAir = FALSE;
		((CShuttleChild*)pNode)->m_bFirstStart = TRUE;
	}

}



///////////////////////////////////////////////////////////////////////////////
/// \fn			CSceneData::ApplyFogDistanceAsHeight( float fOriginStart , float fOriginEnd )
/// \brief		ŔĎÁ¤ °íµµżˇ µű¸Ą ˝Ăľß Á¶Ŕý
/// \author		dhkwon
/// \date		2004-06-18 ~ 2004-06-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CSceneData::ApplyFogDistanceAsHeight(float fOriginStart, float fOriginEnd)
{
	// ŔĎÁ¤ °íµµ ŔĚ»ó ŔÎÁö ĂĽĹ©
	/*	float fAltitudeApplyMin = ALTITUDE_APPLY_MIN + g_pGround->m_projectInfo.fWaterHeight;

	if( g_pShuttleChild->m_vPos.y <= fAltitudeApplyMin )
	return FALSE;

	float fAltitudeApplyMax	= ALTITUDE_APPLY_MAX + g_pGround->m_projectInfo.fWaterHeight;

	//°íµµ Ŕűżë ˝ĂŔŰ °ú łˇ ŔÇ Â÷ŔĚ
	float fDifferDistance =   g_pShuttleChild->m_vPos.y - fAltitudeApplyMin;
	if( fDifferDistance > (fAltitudeApplyMax-fAltitudeApplyMin) )
	fDifferDistance = (fAltitudeApplyMax-fAltitudeApplyMin);

	//°íµµżˇ µű¸Ą ™VĆĂÇŇ Ć÷±× ˝ĂŔŰ ¸¶Áö¸·
	//	float fDestStart, fDestEnd;


	float fDestStart, fDestEnd, fApplyDistanceRate;
	fApplyDistanceRate = (fDifferDistance / (fAltitudeApplyMax - fAltitudeApplyMin));
	fDestStart = fOriginStart - fOriginStart * fApplyDistanceRate;
	if( fDestStart < 0 )
	fDestStart = 0.0f;
	fDestEnd = fOriginEnd - ( fOriginEnd * ALTITUDE_DEST_RATE ) * fApplyDistanceRate * fApplyDistanceRate;

	//	fDestStart = fOriginStart - fOriginStart * ( fDifferDistance / (fAltitudeApplyMax - fAltitudeApplyMin)) *
	//					( fDifferDistance / (fAltitudeApplyMax - fAltitudeApplyMin));
	//	if( fDestStart < 0 )
	//		fDestStart = 0.0f;
	//	fDestEnd = fOriginEnd - ( fOriginEnd * ALTITUDE_DEST_RATE ) * (fDifferDistance / (fAltitudeApplyMax - fAltitudeApplyMin)) * (fDifferDistance / (fAltitudeApplyMax - fAltitudeApplyMin) );
	if( fDestEnd < 10.0f )
	fDestEnd = 10.0f;

	m_fFogDestStartValue = fDestStart;
	m_fFogDestEndValue = fDestEnd;

	return TRUE;
	*/
	//	if(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex == 2001)
	//	{
	//		m_fFogDestStartValue = fOriginStart;
	//		m_fFogDestEndValue = fOriginEnd;
	//		return TRUE;
	//	}

	float fAltitudeApplyMin = g_fAltitudeApplyMin + g_pGround->m_projectInfo.fWaterHeight;

	if (g_pShuttleChild->m_vPos.y <= fAltitudeApplyMin)
		return FALSE;

	float fAltitudeApplyMax = g_fAltitudeApplyMax + g_pGround->m_projectInfo.fWaterHeight;


	float fDifferDistance = g_pShuttleChild->m_vPos.y - fAltitudeApplyMin;
	if (fDifferDistance > (fAltitudeApplyMax - fAltitudeApplyMin))
		fDifferDistance = (fAltitudeApplyMax - fAltitudeApplyMin);

	float fDestStart, fDestEnd, fApplyDistanceRate;
	fApplyDistanceRate = (fDifferDistance / (fAltitudeApplyMax - fAltitudeApplyMin));

	fDestStart = fOriginStart - fOriginStart * fApplyDistanceRate;
	if (fDestStart < 0)
		fDestStart = 0.0f;

	fDestEnd = fOriginEnd - (fOriginEnd * g_fAltitudeDestRate) * fApplyDistanceRate * fApplyDistanceRate;
	if (fDestEnd < 10.0f)
		fDestEnd = 10.0f;

	//	m_fFogDestStartValue = fDestStart;
	//	m_fFogDestEndValue = fDestEnd;


	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CSceneData::CheckWeather()
/// \brief		łŻľľżˇ µű¸Ą ľČ°ł °Ĺ¸®¸¦ ĽĽĆĂÇŃ´Ů.
/// \author		jschoi
/// \date		2004-10-20 ~ 2004-10-20
/// \warning	±âÁ¸żˇ´Â ľČ°ł °Ĺ¸®żÍ ľČ°ł »ö»ó ¸đµÎ¸¦ ĽĽĆĂÇßÁö¸¸
///				CheckWeather() żˇĽ­ ľČ°ł °Ĺ¸®¸¦ ´ă´çÇĎ°í
///				CheckDay() żˇĽ­ ľČ°ł »ö»óŔ» ´ă´çÇŃ´Ů.
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
VOID CSceneData::CheckWeather()
{
	FLOG("CSceneData::CheckWeather()");
	if (!m_bChangeWeather) {
		return;
	}
	// 2004-10-20 by jschoi ľĆ·ˇÄÚµĺ´Â łŻľľ şŻČ­°ˇ ÁřÇŕÁßŔĎ¶§ µżŔŰÇŃ´Ů.

	const float fTotalChangeWeatherTime = 5.0f;	// łŻľľŔÇ şŻČ­´Â 10ĂĘ µżľČ ŔĚ·çľîÁř´Ů.
	if (m_fChangeWeatherCheckTime == 0.0f) {
		m_fChangeWeatherCheckTime = fTotalChangeWeatherTime;
		m_fBeforeFogStartValue = m_fFogStartValue;
		m_fBeforeFogEndValue = m_fFogEndValue;
	}

	float fTargetFogStartValue, fTargetFogEndValue;

	switch (m_byWeatherType) {
		case WEATHER_DEFAULT:
			{
				fTargetFogStartValue = m_fOrgFogStartValue;
				fTargetFogEndValue = m_fOrgFogEndValue;
			}
			break;
		case WEATHER_SUNNY:
			{
				fTargetFogStartValue = m_fOrgFogStartValue;
				fTargetFogEndValue = m_fOrgFogEndValue;
			}
			break;
		case WEATHER_RAINY:
			{
				fTargetFogStartValue = WEATHER_RAINY_FOG_START;
				fTargetFogEndValue = WEATHER_RAINY_FOG_END;
			}
			break;
		case WEATHER_SNOWY:
			{
				fTargetFogStartValue = WEATHER_SNOWY_FOG_START;
				fTargetFogEndValue = WEATHER_SNOWY_FOG_END;
			}
			break;
		case WEATHER_CLOUDY:
			{
				fTargetFogStartValue = WEATHER_CLOUDY_FOG_START;
				fTargetFogEndValue = WEATHER_CLOUDY_FOG_END;
			}
			break;
		case WEATHER_FOGGY:
			{
				fTargetFogStartValue = WEATHER_FOGGY_FOG_START;
				fTargetFogEndValue = WEATHER_FOGGY_FOG_END;
			}
			break;
		default:
			break;
	}

	m_fChangeWeatherCheckTime -= g_pD3dApp->GetElapsedTime();

	m_fFogStartValue = fTargetFogStartValue * (1 - m_fChangeWeatherCheckTime / fTotalChangeWeatherTime)
		+ m_fBeforeFogStartValue * (m_fChangeWeatherCheckTime / fTotalChangeWeatherTime);
	m_fFogEndValue = fTargetFogEndValue * (1 - m_fChangeWeatherCheckTime / fTotalChangeWeatherTime)
		+ m_fBeforeFogEndValue * (m_fChangeWeatherCheckTime / fTotalChangeWeatherTime);

	if (m_fChangeWeatherCheckTime <= 0) {
		//		m_bChangeWeather = FALSE;
		m_fChangeWeatherCheckTime = 0;
		m_fFogStartValue = fTargetFogStartValue;
		m_fFogEndValue = fTargetFogEndValue;
	}


	static float fSnowCheckTime = 0.0f;
	if (fSnowCheckTime >= 0.0f)
		fSnowCheckTime -= g_pD3dApp->GetElapsedTime();
	DWORD dwFogColor = 0;
	switch (m_byWeatherType) {
		case WEATHER_DEFAULT:
			{
				/*			m_fFogStartValue += ((200.0f + (g_pSOption->sTerrainRender-8)*0.25f*TILE_SIZE) - m_fFogStartValue)*g_pD3dApp->GetElapsedTime();
				m_fFogEndValue += ((650.0f + (g_pSOption->sTerrainRender-8)*1.99f*TILE_SIZE) - m_fFogEndValue)*g_pD3dApp->GetElapsedTime();
				DWORD dwColor;
				g_pD3dDev->GetRenderState( D3DRS_FOGCOLOR,  &dwColor );
				BYTE bRed, bGreen, bBlue;
				bRed = (BYTE)(dwColor >> 16);
				bGreen = (BYTE)(dwColor >> 8);
				bBlue = (BYTE)(dwColor);
				dwFogColor = D3DCOLOR_ARGB(0x00,(BYTE)(bRed + (m_fSkyRedColor*255.0f*0.4f - bRed)*g_pD3dApp->GetElapsedTime())
				,(BYTE)(bGreen + (m_fSkyGreenColor*255.0f*0.4f - bGreen)*g_pD3dApp->GetElapsedTime())
				,(BYTE)(bBlue + (m_fSkyBlueColor*255.0f*0.4f - bBlue)*g_pD3dApp->GetElapsedTime()));
				*/
				/*			m_fFogStartValue += ((300.0f + (g_pSOption->sTerrainRender-8)*0.3f*TILE_SIZE) - m_fFogStartValue)*g_pD3dApp->GetElapsedTime();
				m_fFogEndValue += ((650.0f + (g_pSOption->sTerrainRender-8)*1.99f*TILE_SIZE) - m_fFogEndValue)*g_pD3dApp->GetElapsedTime();
				DWORD dwColor;
				g_pD3dDev->GetRenderState( D3DRS_FOGCOLOR,  &dwColor );
				BYTE bRed, bGreen, bBlue;
				bRed = (BYTE)(dwColor >> 16);
				bGreen = (BYTE)(dwColor >> 8);
				bBlue = (BYTE)(dwColor);
				dwFogColor = D3DCOLOR_ARGB(0x00,(BYTE)(bRed + (m_fSkyRedColor*255.0f*0.4f - bRed)*g_pD3dApp->GetElapsedTime())
				,(BYTE)(bGreen + (m_fSkyGreenColor*255.0f*0.4f - bGreen)*g_pD3dApp->GetElapsedTime())
				,(BYTE)(bBlue + (m_fSkyBlueColor*255.0f*0.4f - bBlue)*g_pD3dApp->GetElapsedTime()));
				*/			m_fFogStartValue = WEATHER_DEFAULT_FOG_START;
				m_fFogEndValue = WEATHER_DEFAULT_FOG_END;
				dwFogColor = D3DCOLOR_ARGB(0, 55, 60, 90);
				m_bChangeWeather = FALSE;
			}
			break;
		case WEATHER_SUNNY:
			{
				//			ApplyFogDistanceAsHeight( WEATHER_SUNNY_FOG_START , WEATHER_SUNNY_FOG_END );
				ApplyFogDistanceAsHeight(m_fOrgFogStartValue, m_fOrgFogEndValue);
				//			m_fFogStartValue += (m_fFogDestStartValue - m_fFogStartValue)*g_pD3dApp->GetElapsedTime();
				//			m_fFogEndValue += (m_fFogDestEndValue - m_fFogEndValue)*g_pD3dApp->GetElapsedTime();
				//			dwFogColor = GetFogColor(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex, !m_bNight);
				//			DWORD dwColor;
				//			g_pD3dDev->GetRenderState( D3DRS_FOGCOLOR,  &dwColor );
				//			BYTE bRed, bGreen, bBlue;
				//			bRed = (BYTE)(dwColor >> 16);
				//			bGreen = (BYTE)(dwColor >> 8);
				//			bBlue = (BYTE)(dwColor);
				//			dwFogColor = D3DCOLOR_ARGB(0x00,(BYTE)(bRed + (m_fSkyRedColor*255.0f - bRed)*g_pD3dApp->GetElapsedTime())
				//				,(BYTE)(bGreen + (m_fSkyGreenColor*255.0f - bGreen)*g_pD3dApp->GetElapsedTime())
				//				,(BYTE)(bBlue + (m_fSkyBlueColor*255.0f - bBlue)*g_pD3dApp->GetElapsedTime()));
			}
			break;
		case WEATHER_RAINY:
			{
				ApplyFogDistanceAsHeight(WEATHER_RAINY_FOG_START, WEATHER_RAINY_FOG_END);
				//			m_fFogStartValue += (m_fFogDestStartValue - m_fFogStartValue)*g_pD3dApp->GetElapsedTime();
				//			m_fFogEndValue += (m_fFogDestEndValue - m_fFogEndValue)*g_pD3dApp->GetElapsedTime();
				// Rain Test
				if (fSnowCheckTime <= 0.0f) {
					fSnowCheckTime = SNOW_CHECKTIME;
					CRainData* pRain;
					D3DXVECTOR3 vPos, vVel, vUp, vTemp;
					vUp = D3DXVECTOR3(0, 1, 0);
					float fVelRate;
					vPos = g_pD3dApp->m_pCamera->GetEyePt() + 200.0f * vUp + 250.0f * g_pD3dApp->m_pCamera->GetViewDir();
					for (int i = 0; i < MAX_SNOW_AMOUNT * 10; i++) {
						vTemp = vPos;
						fVelRate = rand() % 100 + 350;

						vTemp.x += rand() % 500 - 250;
						vTemp.z += rand() % 500 - 250;

						vVel.x = rand() % 100 - 50;
						vVel.y = -500.0f;
						vVel.z = rand() % 100 - 50;
						D3DXVec3Normalize(&vVel, &vVel);
						pRain = new CRainData(vTemp, fVelRate, vVel);
						pRain = (CRainData*)m_pRainList->AddChild(pRain);
					}
				}
				DWORD dwColor;
				g_pD3dDev->GetRenderState(D3DRS_FOGCOLOR, &dwColor);
				BYTE bRed, bGreen, bBlue;
				bRed = (BYTE)(dwColor >> 16);
				bGreen = (BYTE)(dwColor >> 8);
				bBlue = (BYTE)(dwColor);
				dwFogColor = D3DCOLOR_ARGB(0x00, (BYTE)(bRed + (m_fSkyRedColor * 255.0f * 0.4f - bRed) * g_pD3dApp->GetElapsedTime())
										   , (BYTE)(bGreen + (m_fSkyGreenColor * 255.0f * 0.4f - bGreen) * g_pD3dApp->GetElapsedTime())
										   , (BYTE)(bBlue + (m_fSkyBlueColor * 255.0f * 0.4f - bBlue) * g_pD3dApp->GetElapsedTime()));
			}
			break;
		case WEATHER_SNOWY:
			{
				ApplyFogDistanceAsHeight(WEATHER_SNOWY_FOG_START, WEATHER_SNOWY_FOG_END);
				//			m_fFogStartValue += (m_fFogDestStartValue - m_fFogStartValue)*g_pD3dApp->GetElapsedTime();
				//			m_fFogEndValue += (m_fFogDestEndValue - m_fFogEndValue)*g_pD3dApp->GetElapsedTime();
				// Snow Test
				if (fSnowCheckTime <= 0.0f) {
					fSnowCheckTime = SNOW_CHECKTIME;
					CAppEffectData* pEff;
					D3DXVECTOR3 vPos, vVel, vUp, vTemp;
					vUp = D3DXVECTOR3(0, 1, 0);
					float fVelRate;
					vPos = g_pD3dApp->m_pCamera->GetEyePt() + 200.0f * vUp + 350.0f * g_pD3dApp->m_pCamera->GetViewDir();
					for (int i = 0; i < MAX_SNOW_AMOUNT; i++) {
						vTemp = vPos;
						fVelRate = rand() % 50 + 20;

						vTemp.x += rand() % 400 - 200;
						vTemp.z += rand() % 400 - 200;

						vVel.x = rand() % 100 - 50;
						vVel.y = -500.0f;
						vVel.z = rand() % 100 - 50;
						D3DXVec3Normalize(&vVel, &vVel);
						pEff = new CAppEffectData(RC_EFF_SNOW, vTemp, fVelRate, vVel);
						pEff = (CAppEffectData*)g_pD3dApp->m_pEffectList->AddChild(pEff);
					}
				}
				DWORD dwColor;
				g_pD3dDev->GetRenderState(D3DRS_FOGCOLOR, &dwColor);
				BYTE bRed, bGreen, bBlue;
				bRed = (BYTE)(dwColor >> 16);
				bGreen = (BYTE)(dwColor >> 8);
				bBlue = (BYTE)(dwColor);
				dwFogColor = D3DCOLOR_ARGB(0x00, (BYTE)(bRed + (m_fSkyRedColor * 255.0f * 0.4f - bRed) * g_pD3dApp->GetElapsedTime())
										   , (BYTE)(bGreen + (m_fSkyGreenColor * 255.0f * 0.4f - bGreen) * g_pD3dApp->GetElapsedTime())
										   , (BYTE)(bBlue + (m_fSkyBlueColor * 255.0f * 0.4f - bBlue) * g_pD3dApp->GetElapsedTime()));
			}
			break;
		case WEATHER_CLOUDY:
			{
				ApplyFogDistanceAsHeight(WEATHER_CLOUDY_FOG_START, WEATHER_CLOUDY_FOG_END);
				//			m_fFogStartValue += (m_fFogDestStartValue - m_fFogStartValue)*g_pD3dApp->GetElapsedTime();
				//			m_fFogEndValue += (m_fFogDestEndValue - m_fFogEndValue)*g_pD3dApp->GetElapsedTime();
				DWORD dwColor;
				g_pD3dDev->GetRenderState(D3DRS_FOGCOLOR, &dwColor);
				BYTE bRed, bGreen, bBlue;
				bRed = (BYTE)(dwColor >> 16);
				bGreen = (BYTE)(dwColor >> 8);
				bBlue = (BYTE)(dwColor);
				dwFogColor = D3DCOLOR_ARGB(0x00, (BYTE)(bRed + (m_fSkyRedColor * 255.0f * 0.4f - bRed) * g_pD3dApp->GetElapsedTime())
										   , (BYTE)(bGreen + (m_fSkyGreenColor * 255.0f * 0.4f - bGreen) * g_pD3dApp->GetElapsedTime())
										   , (BYTE)(bBlue + (m_fSkyBlueColor * 255.0f * 0.4f - bBlue) * g_pD3dApp->GetElapsedTime()));
			}
			break;
		case WEATHER_FOGGY:
			{
				ApplyFogDistanceAsHeight(WEATHER_FOGGY_FOG_START, WEATHER_FOGGY_FOG_END);
				//			m_fFogStartValue += (m_fFogDestStartValue - m_fFogStartValue)*g_pD3dApp->GetElapsedTime();
				if (m_fFogStartValue < 0.0f)
					m_fFogStartValue = 0.0f;
				//			m_fFogEndValue += (m_fFogDestEndValue - m_fFogEndValue)*g_pD3dApp->GetElapsedTime();
				DWORD dwColor;
				g_pD3dDev->GetRenderState(D3DRS_FOGCOLOR, &dwColor);
				BYTE bRed, bGreen, bBlue;
				bRed = (BYTE)(dwColor >> 16);
				bGreen = (BYTE)(dwColor >> 8);
				bBlue = (BYTE)(dwColor);
				dwFogColor = D3DCOLOR_ARGB(0x00,
										   (BYTE)(bRed + (200 - bRed) * g_pD3dApp->GetElapsedTime()),
										   (BYTE)(bGreen + (200 - bGreen) * g_pD3dApp->GetElapsedTime()),
										   (BYTE)(bBlue + (200 - bBlue) * g_pD3dApp->GetElapsedTime()));
			}
			break;
	}
	// ŔÓ˝Ă ÄÚµĺ łŞÁßżˇ ¸Ę żˇµđĹÍżˇĽ­ °ˇÁ®żČ
	if (g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex == 3067) {
		m_fFogStartValue = 200.0f;
		m_fFogEndValue = 2200.0f;
	}
	else if (g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex == 3003) {
		m_fFogStartValue = 1600.0f;
		m_fFogEndValue = 2400.0f;
	}
	else if (g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex == 4001) {
		m_fFogStartValue = 200.0f;
		m_fFogEndValue = 1000.0f;
	}


	// 	m_dwFogColor = dwFogColor;

	///////////////////////// FOG //////////////////////////////////////////
#ifdef MAP_EDITOR

	g_pD3dDev->SetRenderState(D3DRS_FOGENABLE, IsFogEnableMap(g_pD3dApp->m_nMapNumber));
#else
	g_pD3dDev->SetRenderState(D3DRS_FOGENABLE, IsFogEnableMap(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex));

#endif
	// Vertex Fog Use
	g_pD3dDev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
	g_pD3dDev->SetRenderState(D3DRS_FOGSTART, FtoDW(m_fFogStartValue)); //
	g_pD3dDev->SetRenderState(D3DRS_FOGEND, FtoDW(m_fFogEndValue));
	g_pD3dDev->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
	g_pD3dDev->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
	g_pD3dDev->SetFVF(D3DFVF_FOGVERTEX);
	////////////////////////////////////////////////////////////////////////
	g_pD3dDev->SetRenderState(D3DRS_FOGCOLOR, m_dwFogColor);
}
#ifdef _DEBUG_MAPSETTING
VOID CSceneData::CheckDay()
{
}
#else
VOID CSceneData::CheckDay()
{
	FLOG("CSceneData::CheckDay()");
	ATUM_DATE_TIME dt;
	dt = dt.GetCurrentDateTime();
	DWORD dwDayFogColor = g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)->DayFogColor;
	DWORD dwNightFogColor = g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)->NightFogColor;
	BYTE byDayRed, byDayGreen, byDayBlue, byNightRed, byNightGreen, byNightBlue, byFogRed, byFogGreen, byFogBlue;

	byDayRed = (BYTE)(dwDayFogColor >> 16);
	byDayGreen = (BYTE)(dwDayFogColor >> 8);
	byDayBlue = (BYTE)(dwDayFogColor);
	byNightRed = (BYTE)(dwNightFogColor >> 16);
	byNightGreen = (BYTE)(dwNightFogColor >> 8);
	byNightBlue = (BYTE)(dwNightFogColor);

	float fBlur = ((float)g_pD3dApp->m_pFxSystem->GetSourceAlpha() / 255) * 0.2f;

	byDayRed -= byDayRed * fBlur;
	byDayGreen -= byDayGreen * fBlur;
	byDayBlue -= byDayBlue * fBlur;
	byNightRed -= byNightRed * fBlur;
	byNightGreen -= byNightGreen * fBlur;
	byNightBlue -= byNightBlue * fBlur;

	dwDayFogColor = D3DCOLOR_ARGB(0, byDayRed, byDayGreen, byDayBlue);
	dwNightFogColor = D3DCOLOR_ARGB(0, byNightRed, byNightGreen, byNightBlue);
#ifdef _DEBUG
	char szTemp[1024];
	sprintf(szTemp, "Current time: %d:%d:%d\n", dt.Hour, dt.Minute, dt.Second);
	OutputDebugString(szTemp);
#endif
	int nMonrningBegin = 6;
	int nNightBegin = 20;
	float fAddAdminHour = g_pD3dApp->GetAdminPanel() ? g_pD3dApp->GetAdminPanel()->timer7 : 0;

	if (dt.Hour+fAddAdminHour >= nMonrningBegin && dt.Hour+fAddAdminHour < nNightBegin) //is day
		m_bNight = FALSE;
	else
		m_bNight = TRUE;

	if (m_bNight) {
		if (g_pShuttleChild->GetCurrentBodyCondition() & ~BODYCON_NIGHTFLY_MASK)
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition(BODYCON_NIGHTFLY_MASK, TRUE);

		if (dt.Hour == nNightBegin) {
			float fLight = (float)dt.Minute / 60.0f;

			m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR1 * (1.0f - fLight) + m_pGround->m_projectInfo.fDiffuseR2 * (fLight);
			m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG1 * (1.0f - fLight) + m_pGround->m_projectInfo.fDiffuseG2 * (fLight);
			m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB1 * (1.0f - fLight) + m_pGround->m_projectInfo.fDiffuseB2 * (fLight);
			m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR1 * (1.0f - fLight) + m_pGround->m_projectInfo.fAmbientR2 * (fLight);
			m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG1 * (1.0f - fLight) + m_pGround->m_projectInfo.fAmbientG2 * (fLight);
			m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB1 * (1.0f - fLight) + m_pGround->m_projectInfo.fAmbientB2 * (fLight);

			m_fAlphaSky = 1.0f - fLight;

			fLight = 1.0f - fLight;
			if (fLight < 0.4f)
				fLight = 0.4f;

			m_light0.Specular.r = fLight;
			m_light0.Specular.g = fLight;
			m_light0.Specular.b = fLight;
		}
		else {
			m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR2;
			m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG2;
			m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB2;
			m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR2;
			m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG2;
			m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB2;

			m_fAlphaSky = 0.0f;

			m_light0.Specular.r = 0.4f;
			m_light0.Specular.g = 0.4f;
			m_light0.Specular.b = 0.4f;
		}
		m_dwFogColor = dwNightFogColor;
	}
	else {
		if (g_pShuttleChild->GetCurrentBodyCondition() & BODYCON_NIGHTFLY_MASK)
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition(BODYCON_NIGHTFLY_MASK, FALSE);

		if (dt.Hour == nMonrningBegin) {
			float fLight = (float)dt.Minute / 60.0f;

			m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR2 * (1.0f - fLight) + m_pGround->m_projectInfo.fDiffuseR1 * (fLight);
			m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG2 * (1.0f - fLight) + m_pGround->m_projectInfo.fDiffuseG1 * (fLight);
			m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB2 * (1.0f - fLight) + m_pGround->m_projectInfo.fDiffuseB1 * (fLight);
			m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR2 * (1.0f - fLight) + m_pGround->m_projectInfo.fAmbientR1 * (fLight);
			m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG2 * (1.0f - fLight) + m_pGround->m_projectInfo.fAmbientG1 * (fLight);
			m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB2 * (1.0f - fLight) + m_pGround->m_projectInfo.fAmbientB1 * (fLight);

			m_fAlphaSky = fLight;

			m_light0.Specular.r = fLight;
			m_light0.Specular.g = fLight;
			m_light0.Specular.b = fLight;
		}
		else {
			m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR1;
			m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG1;
			m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB1;
			m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR1;
			m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG1;
			m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB1;

			m_fAlphaSky = 1.0f;

			m_light0.Specular.r = 1.0f;
			m_light0.Specular.g = 1.0f;
			m_light0.Specular.b = 1.0f;
		}
		m_dwFogColor = dwDayFogColor;

		D3DVECTOR dvX = SetLightDirection();
		int nHoursOfDay = nNightBegin - nMonrningBegin;

		float fPerc = (dt.Hour+fAddAdminHour - nMonrningBegin) / (float)nHoursOfDay;
		float fFull = 6.0f * fPerc - 3.0f;
#ifdef _DEBUG
		sprintf(szTemp, "Tick: %.2f\n", fFull);
		OutputDebugString(szTemp);
#endif
		dvX.x = fFull;
		dvX.z = fFull;

		m_light0.Direction = dvX;
		g_pD3dDev->SetLight(0, &m_light0);
	}

	//const DWORD dwTime = 1;			// ˝Ă°ŁŔ» dwTimeąč ¸¸Ĺ­ »ˇ¸® Čĺ¸Ł°Ô ÇŃ´Ů.				DEFAULT = 1
	//const DWORD dwChangingTime = 1;	// ąă ł· şŻČ­˝Ă°ŁŔ» dwChangingTimeąč żŔ·ˇ ÁöĽÓµÇ°Ô ÇŃ´Ů.DEFAULT = 1

	////	int nH = 4, nM = 60, nS = 60; // nH:˝Ă°Ł nM:şĐ nS:ĂĘ ŔÇ ·® ĽÂĆĂÇŇĽö ŔÖ´Ů.(żą> ÇĎ·ç->4˝Ă°Ł 1˝Ă°Ł->60şĐ 1şĐ->60ĂĘ)-°ÔŔÓ»óŔÇ ˝Ă°Ł
	//int nTime = (GetTickCount() - m_dwStartTime) / 1000 + m_nBaseTime;
	//nTime = (nTime * dwTime) % (TIME_HOUR * TIME_MINUTE * TIME_SECOND);

	//// 2004-10-20 by jschoi
	//float fChangingCheckTime;
	//float fChangingTime = 10 * dwChangingTime * TIME_SECOND;

	////	dwDayFogColor = GetFogColor(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex,TRUE);
	////	dwNightFogColor = GetFogColor(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex,FALSE);


	////	if(g_pSOption->sReflactive)
	//{
	//	byDayRed = (BYTE)(dwDayFogColor >> 16);
	//	byDayGreen = (BYTE)(dwDayFogColor >> 8);
	//	byDayBlue = (BYTE)(dwDayFogColor);
	//	byNightRed = (BYTE)(dwNightFogColor >> 16);
	//	byNightGreen = (BYTE)(dwNightFogColor >> 8);
	//	byNightBlue = (BYTE)(dwNightFogColor);

	//	float fBlur = ((float)g_pD3dApp->m_pFxSystem->GetSourceAlpha() / 255) * 0.2f;

	//	byDayRed -= byDayRed * fBlur;
	//	byDayGreen -= byDayGreen * fBlur;
	//	byDayBlue -= byDayBlue * fBlur;
	//	byNightRed -= byNightRed * fBlur;
	//	byNightGreen -= byNightGreen * fBlur;
	//	byNightBlue -= byNightBlue * fBlur;

	//	dwDayFogColor = D3DCOLOR_ARGB(0, byDayRed, byDayGreen, byDayBlue);
	//	dwNightFogColor = D3DCOLOR_ARGB(0, byNightRed, byNightGreen, byNightBlue);
	//}

	//if (nTime >= 0 && nTime < TIME_MINUTE * TIME_SECOND) { // night
	//	if (g_pShuttleChild->GetCurrentBodyCondition() & ~BODYCON_NIGHTFLY_MASK) {
	//		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition(BODYCON_NIGHTFLY_MASK, TRUE);
	//	}
	//	if (nTime < fChangingTime) { // ąăŔĚ µČÁö 10şĐŔĚł»
	//		fChangingCheckTime = (float)nTime;

	//		m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR1 * (1.0f - (fChangingCheckTime / fChangingTime)) + m_pGround->m_projectInfo.fDiffuseR2 * (fChangingCheckTime / fChangingTime);
	//		m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG1 * (1.0f - (fChangingCheckTime / fChangingTime)) + m_pGround->m_projectInfo.fDiffuseG2 * (fChangingCheckTime / fChangingTime);
	//		m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB1 * (1.0f - (fChangingCheckTime / fChangingTime)) + m_pGround->m_projectInfo.fDiffuseB2 * (fChangingCheckTime / fChangingTime);
	//		m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR1 * (1.0f - (fChangingCheckTime / fChangingTime)) + m_pGround->m_projectInfo.fAmbientR2 * (fChangingCheckTime / fChangingTime);
	//		m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG1 * (1.0f - (fChangingCheckTime / fChangingTime)) + m_pGround->m_projectInfo.fAmbientG2 * (fChangingCheckTime / fChangingTime);
	//		m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB1 * (1.0f - (fChangingCheckTime / fChangingTime)) + m_pGround->m_projectInfo.fAmbientB2 * (fChangingCheckTime / fChangingTime);

	//		m_fAlphaSky = 1.0f - fChangingCheckTime / fChangingTime;	// ąă, ł· ˝şÄ«ŔĚ ąÚ˝ş ČĄÇŐ şńŔ˛ 

	//		// 2004-11-15 by ydkim
	//		//			if(g_pSOption->sGammaCtrl > 0)
	//		ChangeGammaOption(g_pSOption->sGammaCtrl);

	//		byDayRed = (BYTE)(dwDayFogColor >> 16);
	//		byDayGreen = (BYTE)(dwDayFogColor >> 8);
	//		byDayBlue = (BYTE)(dwDayFogColor);
	//		byNightRed = (BYTE)(dwNightFogColor >> 16);
	//		byNightGreen = (BYTE)(dwNightFogColor >> 8);
	//		byNightBlue = (BYTE)(dwNightFogColor);
	//		byFogRed = byDayRed * (1.0f - fChangingCheckTime / fChangingTime) + byNightRed * (fChangingCheckTime / fChangingTime);
	//		byFogGreen = byDayGreen * (1.0f - fChangingCheckTime / fChangingTime) + byNightGreen * (fChangingCheckTime / fChangingTime);
	//		byFogBlue = byDayBlue * (1.0f - fChangingCheckTime / fChangingTime) + byNightBlue * (fChangingCheckTime / fChangingTime);
	//		m_dwFogColor = D3DCOLOR_ARGB(0x00, byFogRed, byFogGreen, byFogBlue);
	//	}
	//	else { // ąă 10şĐŔĚ Áöł­ ŔĚČÄ
	//		m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR2;
	//		m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG2;
	//		m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB2;
	//		m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR2;
	//		m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG2;
	//		m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB2;

	//		m_fAlphaSky = 0.0f;	// ąă, ł· ˝şÄ«ŔĚ ąÚ˝ş ČĄÇŐ şńŔ˛ 

	//		// 2004-11-15 by ydkim
	//		//			if(g_pSOption->sGammaCtrl > 0)
	//		ChangeGammaOption(g_pSOption->sGammaCtrl);

	//		m_dwFogColor = dwNightFogColor;
	//	}
	//	//		m_bNight = TRUE;
	//	m_light0.Specular.r = 0.4f;
	//	m_light0.Specular.g = 0.4f;
	//	m_light0.Specular.b = 0.4f;
	//}
	//else { // ł·
	//	if (g_pShuttleChild->GetCurrentBodyCondition() & BODYCON_NIGHTFLY_MASK) {
	//		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition(BODYCON_NIGHTFLY_MASK, FALSE);
	//	}
	//	if (nTime >= TIME_MINUTE * TIME_SECOND && nTime < TIME_MINUTE * TIME_SECOND + fChangingTime) { // ł·ŔĚ µČÁö 10şĐŔĚł»
	//		fChangingCheckTime = (float)(nTime - TIME_MINUTE * TIME_SECOND);
	//		m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR2 * (1.0f - fChangingCheckTime / fChangingTime) + m_pGround->m_projectInfo.fDiffuseR1 * (fChangingCheckTime / fChangingTime);
	//		m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG2 * (1.0f - fChangingCheckTime / fChangingTime) + m_pGround->m_projectInfo.fDiffuseG1 * (fChangingCheckTime / fChangingTime);
	//		m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB2 * (1.0f - fChangingCheckTime / fChangingTime) + m_pGround->m_projectInfo.fDiffuseB1 * (fChangingCheckTime / fChangingTime);
	//		m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR2 * (1.0f - fChangingCheckTime / fChangingTime) + m_pGround->m_projectInfo.fAmbientR1 * (fChangingCheckTime / fChangingTime);
	//		m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG2 * (1.0f - fChangingCheckTime / fChangingTime) + m_pGround->m_projectInfo.fAmbientG1 * (fChangingCheckTime / fChangingTime);
	//		m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB2 * (1.0f - fChangingCheckTime / fChangingTime) + m_pGround->m_projectInfo.fAmbientB1 * (fChangingCheckTime / fChangingTime);

	//		m_fAlphaSky = fChangingCheckTime / fChangingTime;	// ąă, ł· ˝şÄ«ŔĚ ąÚ˝ş ČĄÇŐ şńŔ˛ 

	//		// 2004-11-15 by ydkim
	//		//			if(g_pSOption->sGammaCtrl > 0)
	//		ChangeGammaOption(g_pSOption->sGammaCtrl);

	//		byDayRed = (BYTE)(dwDayFogColor >> 16);
	//		byDayGreen = (BYTE)(dwDayFogColor >> 8);
	//		byDayBlue = (BYTE)(dwDayFogColor);
	//		byNightRed = (BYTE)(dwNightFogColor >> 16);
	//		byNightGreen = (BYTE)(dwNightFogColor >> 8);
	//		byNightBlue = (BYTE)(dwNightFogColor);
	//		byFogRed = byNightRed * (1.0f - fChangingCheckTime / fChangingTime) + byDayRed * (fChangingCheckTime / fChangingTime);
	//		byFogGreen = byNightGreen * (1.0f - fChangingCheckTime / fChangingTime) + byDayGreen * (fChangingCheckTime / fChangingTime);
	//		byFogBlue = byNightBlue * (1.0f - fChangingCheckTime / fChangingTime) + byDayBlue * (fChangingCheckTime / fChangingTime);
	//		m_dwFogColor = D3DCOLOR_ARGB(0x00, byFogRed, byFogGreen, byFogBlue);
	//	}
	//	else { // ł· 10şĐŔĚ Áöł­ ŔĚČÄ
	//		m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR1;
	//		m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG1;
	//		m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB1;
	//		m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR1;
	//		m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG1;
	//		m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB1;

	//		m_fAlphaSky = 1.0f;	// ąă, ł· ˝şÄ«ŔĚ ąÚ˝ş ČĄÇŐ şńŔ˛ 

	//		// 2004-11-15 by ydkim
	//		//			if(g_pSOption->sGammaCtrl > 0)
	//		ChangeGammaOption(g_pSOption->sGammaCtrl);

	//		m_dwFogColor = dwDayFogColor;
	//	}
	//	//		m_bNight = FALSE;
	//	m_light0.Specular.r = 1.0f;
	//	m_light0.Specular.g = 1.0f;
	//	m_light0.Specular.b = 1.0f;
	//}

	//// 2004-10-22 by jschoi
	//// ľîµÎżöÁö±â Á÷Ŕüżˇ ąŮ·Î ´ŢŔĚ ¶á´Ů.
	//// Ľ­Ľ­Č÷ ąŕľĆÁö¸éĽ­ Áß°ŁÂë ąŕľĆÁö¸é ĹÂľçŔĚ ¶á´Ů.	
	//if ((nTime >= 0) &&
	//	(nTime < (TIME_MINUTE * TIME_SECOND + fChangingTime / 2))) {
	//	m_bNight = TRUE;
	//}
	//else {
	//	m_bNight = FALSE;
	//}


	//// 2005-04-18 by jschoi - Tutorial
	//if (g_pTutorial->IsTutorialMode() == TRUE) {// ą«Á¶°Ç ÇŃ ł·ŔĚ´Ů.
	//	if (g_pShuttleChild->GetCurrentBodyCondition() & BODYCON_NIGHTFLY_MASK) {
	//		g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition(BODYCON_NIGHTFLY_MASK, FALSE);
	//	}
	//	m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR1;
	//	m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG1;
	//	m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB1;
	//	m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR1;
	//	m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG1;
	//	m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB1;
	//	m_fAlphaSky = 1.0f;
	//	ChangeGammaOption(g_pSOption->sGammaCtrl);
	//	m_dwFogColor = dwDayFogColor;

	//	m_light0.Specular.r = 1.0f;
	//	m_light0.Specular.g = 1.0f;
	//	m_light0.Specular.b = 1.0f;
	//	m_bNight = FALSE;
	//}


	//	if(nTime >= 0 && nTime < 1*TIME_MINUTE*TIME_SECOND)	// ąă
	//	{
	//		if(!m_bNight)
	//		{
	//			if(	m_fSkyRedColor != m_pGround->m_projectInfo.fDiffuseR2 ||
	//				m_fSkyGreenColor != m_pGround->m_projectInfo.fDiffuseG2 ||
	//				m_fSkyBlueColor != m_pGround->m_projectInfo.fDiffuseB2)
	//			{
	//				if(g_pShuttleChild->GetCurrentBodyCondition() & ~BODYCON_NIGHTFLY_MASK)
	//				{
	//					g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_NIGHTFLY_MASK, TRUE );
	//				}
	//				if(m_fSkyRedColor > m_pGround->m_projectInfo.fDiffuseR2)
	//					m_fSkyRedColor -= (m_pGround->m_projectInfo.fDiffuseR1 - 
	//					m_pGround->m_projectInfo.fDiffuseR2)/TIME_DAY_CHANGE;
	//				if(m_fSkyRedColor < m_pGround->m_projectInfo.fDiffuseR2)
	//					m_fSkyRedColor = m_pGround->m_projectInfo.fDiffuseR2;
	//				if(m_fSkyGreenColor > m_pGround->m_projectInfo.fDiffuseG2)
	//					m_fSkyGreenColor -= (m_pGround->m_projectInfo.fDiffuseG1 - 
	//					m_pGround->m_projectInfo.fDiffuseG2)/TIME_DAY_CHANGE;
	//				if(m_fSkyGreenColor < m_pGround->m_projectInfo.fDiffuseG2)
	//					m_fSkyGreenColor = m_pGround->m_projectInfo.fDiffuseG2;
	//				if(m_fSkyBlueColor > m_pGround->m_projectInfo.fDiffuseB2)
	//					m_fSkyBlueColor -= (m_pGround->m_projectInfo.fDiffuseB1 - 
	//					m_pGround->m_projectInfo.fDiffuseB2)/TIME_DAY_CHANGE;
	//				if(m_fSkyBlueColor < m_pGround->m_projectInfo.fDiffuseB2)
	//					m_fSkyBlueColor = m_pGround->m_projectInfo.fDiffuseB2;
	//
	//				m_light0.Diffuse.r = m_fSkyRedColor;
	//				m_light0.Diffuse.g = m_fSkyGreenColor;
	//				m_light0.Diffuse.b = m_fSkyBlueColor;
	//
	//				if(m_light0.Ambient.r > m_pGround->m_projectInfo.fAmbientR2)
	//					m_light0.Ambient.r -= (m_pGround->m_projectInfo.fAmbientR1 - 
	//					m_pGround->m_projectInfo.fAmbientR2)/TIME_DAY_CHANGE;
	//				if(m_light0.Ambient.r < m_pGround->m_projectInfo.fAmbientR2)
	//					m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR2;
	//				if(m_light0.Ambient.g > m_pGround->m_projectInfo.fAmbientG2)
	//					m_light0.Ambient.g -= (m_pGround->m_projectInfo.fAmbientG1 - 
	//					m_pGround->m_projectInfo.fAmbientG2)/TIME_DAY_CHANGE;
	//				if(m_light0.Ambient.g < m_pGround->m_projectInfo.fAmbientG2)
	//					m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG2;
	//				if(m_light0.Ambient.b > m_pGround->m_projectInfo.fAmbientB2)
	//					m_light0.Ambient.b -= (m_pGround->m_projectInfo.fAmbientB1 - 
	//					m_pGround->m_projectInfo.fAmbientB2)/TIME_DAY_CHANGE;
	//				if(m_light0.Ambient.b < m_pGround->m_projectInfo.fAmbientB2)
	//					m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB2;
	//			}
	//			else
	//			{
	//				m_bNight = TRUE;//ąă
	//			}
	//		}
	//	}
	//	else if(nTime >= 1*TIME_MINUTE*TIME_SECOND && nTime < TIME_HOUR*TIME_MINUTE*TIME_SECOND)
	//	{
	//		if(m_bNight)
	//		{
	//			if(m_fSkyRedColor != m_pGround->m_projectInfo.fDiffuseR1 ||
	//				m_fSkyGreenColor != m_pGround->m_projectInfo.fDiffuseG1 ||
	//				m_fSkyBlueColor != m_pGround->m_projectInfo.fDiffuseB1)
	//			{
	//				if(g_pShuttleChild->GetCurrentBodyCondition() & BODYCON_NIGHTFLY_MASK)
	//				{
	//					g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_NIGHTFLY_MASK, FALSE );
	//				}
	//				if(m_fSkyRedColor < m_pGround->m_projectInfo.fDiffuseR1)
	//					m_fSkyRedColor += (m_pGround->m_projectInfo.fDiffuseR1 - 
	//					m_pGround->m_projectInfo.fDiffuseR2)*g_pD3dApp->GetElapsedTime()/TIME_DAY_CHANGE;
	//				if(m_fSkyRedColor > m_pGround->m_projectInfo.fDiffuseR1)
	//					m_fSkyRedColor = m_pGround->m_projectInfo.fDiffuseR1;
	//				if(m_fSkyGreenColor < m_pGround->m_projectInfo.fDiffuseG1)
	//					m_fSkyGreenColor += (m_pGround->m_projectInfo.fDiffuseG1 - 
	//					m_pGround->m_projectInfo.fDiffuseG2)*g_pD3dApp->GetElapsedTime()/TIME_DAY_CHANGE;
	//				if(m_fSkyGreenColor > m_pGround->m_projectInfo.fDiffuseG1)
	//					m_fSkyGreenColor = m_pGround->m_projectInfo.fDiffuseG1;
	//				if(m_fSkyBlueColor < m_pGround->m_projectInfo.fDiffuseB1)
	//					m_fSkyBlueColor += (m_pGround->m_projectInfo.fDiffuseB1 - 
	//					m_pGround->m_projectInfo.fDiffuseB2)*g_pD3dApp->GetElapsedTime()/TIME_DAY_CHANGE;
	//				if(m_fSkyBlueColor > m_pGround->m_projectInfo.fDiffuseB1)
	//					m_fSkyBlueColor = m_pGround->m_projectInfo.fDiffuseB1;
	//
	//				m_light0.Diffuse.r = m_fSkyRedColor;
	//				m_light0.Diffuse.g = m_fSkyGreenColor;
	//				m_light0.Diffuse.b = m_fSkyBlueColor;
	//
	//				if(m_light0.Ambient.r < m_pGround->m_projectInfo.fAmbientR1)
	//					m_light0.Ambient.r += (m_pGround->m_projectInfo.fAmbientR1 - 
	//					m_pGround->m_projectInfo.fAmbientR2)*g_pD3dApp->GetElapsedTime()/TIME_DAY_CHANGE;
	//				if(m_light0.Ambient.r > m_pGround->m_projectInfo.fAmbientR1)
	//					m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR1;
	//				if(m_light0.Ambient.g < m_pGround->m_projectInfo.fAmbientG1)
	//					m_light0.Ambient.g += (m_pGround->m_projectInfo.fAmbientG1 - 
	//					m_pGround->m_projectInfo.fAmbientG2)*g_pD3dApp->GetElapsedTime()/TIME_DAY_CHANGE;
	//				if(m_light0.Ambient.g > m_pGround->m_projectInfo.fAmbientG1)
	//					m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG1;
	//				if(m_light0.Ambient.b < m_pGround->m_projectInfo.fAmbientB1)
	//					m_light0.Ambient.b += (m_pGround->m_projectInfo.fAmbientB1 - 
	//					m_pGround->m_projectInfo.fAmbientB2)*g_pD3dApp->GetElapsedTime()/TIME_DAY_CHANGE;
	//				if(m_light0.Ambient.b > m_pGround->m_projectInfo.fAmbientB1)
	//					m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB1;
	//			}
	//			else
	//			{
	//				m_bNight = FALSE;
	//			}
	//		}
	//	}

	//	ŔÓ˝ĂÄÚµĺ - łŞÁßżˇ ¸Ę żˇµđĹÍżˇĽ­ µĄŔĚĹÍ¸¦ °ˇÁ®żĂ °Í.. - 2004-07-06 - jschoi
	/*	if (g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex == 3067)
	{
	m_light0.Diffuse.r = 0.64f;
	m_light0.Diffuse.g = 0.80f;
	m_light0.Diffuse.b = 0.77f;
	}
	else if(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex == 3003)
	{
	m_light0.Diffuse.r = 0.60f;
	m_light0.Diffuse.g = 0.59f;
	m_light0.Diffuse.b = 0.70f;
	}
	*/
	// ż©±â±îÁö ŔÓ˝ĂÄÚµĺ 

	/*m_light0.Direction = SetLightDirection();
	g_pD3dDev->SetLight(0, &m_light0);*/
	//	DBGOUT("LIGHT[D:%.2f,%.2f,%.2f][A:%.2f,%.2f,%.2f]\n", 
	//		m_light0.Diffuse.r, m_light0.Diffuse.g, m_light0.Diffuse.b, 
	//		m_light0.Ambient.r, m_light0.Ambient.g, m_light0.Ambient.b);
}
#endif
VOID CSceneData::SetDay()
{
	FLOG("CSceneData::CheckDay()");
	ATUM_DATE_TIME dt;
	dt = dt.GetCurrentDateTime();

	char szTemp[1024];
	sprintf(szTemp, "Current time: %d:%d:%d\n", dt.Hour, dt.Minute, dt.Second);
	OutputDebugString(szTemp);
	if (dt.Hour >= 6 && dt.Hour <= 20) //is day
		m_bNight = TRUE;
	else
		m_bNight = FALSE;

	if (m_bNight) {
		if (g_pShuttleChild->GetCurrentBodyCondition() & ~BODYCON_NIGHTFLY_MASK) {
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition(BODYCON_NIGHTFLY_MASK, TRUE);
		}


	}
	else {
		if (g_pShuttleChild->GetCurrentBodyCondition() & BODYCON_NIGHTFLY_MASK) {
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition(BODYCON_NIGHTFLY_MASK, FALSE);
		}


	}
	return;
	FLOG("CSceneData::SetDay()");
	//	int nH = 4, nM = 60, nS = 60; // nH:˝Ă°Ł nM:şĐ nS:ĂĘ ŔÇ ·® ĽÂĆĂÇŇĽö ŔÖ´Ů.(żą> ÇĎ·ç->4˝Ă°Ł 1˝Ă°Ł->60şĐ 1şĐ->60ĂĘ)-°ÔŔÓ»óŔÇ ˝Ă°Ł
	int nTime = (GetTickCount() - m_dwStartTime) / 1000 + m_nBaseTime;
	nTime = nTime % (TIME_HOUR * TIME_MINUTE * TIME_SECOND);

	// 2004-10-20 by jschoi
	float fChangingCheckTime;
	float fChangingTime = 5 * TIME_SECOND;
	DWORD dwDayFogColor, dwNightFogColor;
	BYTE byDayRed, byDayGreen, byDayBlue, byNightRed, byNightGreen, byNightBlue, byFogRed, byFogGreen, byFogBlue;
	//	dwDayFogColor = GetFogColor(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex,TRUE);
	//	dwNightFogColor = GetFogColor(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex,FALSE);
#ifdef MAP_EDITOR
	dwDayFogColor = g_pDatabase->GetMapInfo(g_pD3dApp->m_nMapNumber)->DayFogColor;
	dwNightFogColor = g_pDatabase->GetMapInfo(g_pD3dApp->m_nMapNumber)->NightFogColor;
#else
	dwDayFogColor = g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)->DayFogColor;
	dwNightFogColor = g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)->NightFogColor;
#endif
	if (nTime >= 0 && nTime < TIME_MINUTE * TIME_SECOND) { // ąă
		if (g_pShuttleChild->GetCurrentBodyCondition() & ~BODYCON_NIGHTFLY_MASK) {
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition(BODYCON_NIGHTFLY_MASK, TRUE);
		}
		if (nTime < fChangingTime) { // ąăŔĚ µČÁö 5şĐŔĚł»
			fChangingCheckTime = (float)nTime;

			m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR1 * (1.0f - (fChangingCheckTime / fChangingTime)) + m_pGround->m_projectInfo.fDiffuseR2 * (fChangingCheckTime / fChangingTime);
			m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG1 * (1.0f - (fChangingCheckTime / fChangingTime)) + m_pGround->m_projectInfo.fDiffuseG2 * (fChangingCheckTime / fChangingTime);
			m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB1 * (1.0f - (fChangingCheckTime / fChangingTime)) + m_pGround->m_projectInfo.fDiffuseB2 * (fChangingCheckTime / fChangingTime);
			m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR1 * (1.0f - (fChangingCheckTime / fChangingTime)) + m_pGround->m_projectInfo.fAmbientR2 * (fChangingCheckTime / fChangingTime);
			m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG1 * (1.0f - (fChangingCheckTime / fChangingTime)) + m_pGround->m_projectInfo.fAmbientG2 * (fChangingCheckTime / fChangingTime);
			m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB1 * (1.0f - (fChangingCheckTime / fChangingTime)) + m_pGround->m_projectInfo.fAmbientB2 * (fChangingCheckTime / fChangingTime);

			// 2004-11-15 by ydkim
			//			if(g_pSOption->sGammaCtrl > 0)
			ChangeGammaOption(g_pSOption->sGammaCtrl);

			byDayRed = (BYTE)(dwDayFogColor >> 16);
			byDayGreen = (BYTE)(dwDayFogColor >> 8);
			byDayBlue = (BYTE)(dwDayFogColor);
			byNightRed = (BYTE)(dwNightFogColor >> 16);
			byNightGreen = (BYTE)(dwNightFogColor >> 8);
			byNightBlue = (BYTE)(dwNightFogColor);
			byFogRed = byDayRed * (1.0f - fChangingCheckTime / fChangingTime) + byNightRed * (fChangingCheckTime / fChangingTime);
			byFogGreen = byDayGreen * (1.0f - fChangingCheckTime / fChangingTime) + byNightGreen * (fChangingCheckTime / fChangingTime);
			byFogBlue = byDayBlue * (1.0f - fChangingCheckTime / fChangingTime) + byNightBlue * (fChangingCheckTime / fChangingTime);
			m_dwFogColor = D3DCOLOR_ARGB(0x00, byFogRed, byFogGreen, byFogBlue);
		}
		else { // ąă 5şĐŔĚ Áöł­ ŔĚČÄ
			m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR2;
			m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG2;
			m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB2;
			m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR2;
			m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG2;
			m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB2;

			// 2004-11-15 by ydkim
			//			if(g_pSOption->sGammaCtrl > 0)
			ChangeGammaOption(g_pSOption->sGammaCtrl);

			m_dwFogColor = dwNightFogColor;
		}
		//		m_bNight = TRUE;
		m_light0.Specular.r = 0.4f;
		m_light0.Specular.g = 0.4f;
		m_light0.Specular.b = 0.4f;
	}
	else { // ł·
		if (g_pShuttleChild->GetCurrentBodyCondition() & BODYCON_NIGHTFLY_MASK) {
			g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition(BODYCON_NIGHTFLY_MASK, FALSE);
		}
		if (nTime >= TIME_MINUTE * TIME_SECOND && nTime < TIME_MINUTE * TIME_SECOND + fChangingTime) { // ł·ŔĚ µČÁö 5şĐŔĚł»
			fChangingCheckTime = (float)(nTime - TIME_MINUTE * TIME_SECOND);
			m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR2 * (1.0f - fChangingCheckTime / fChangingTime) + m_pGround->m_projectInfo.fDiffuseR1 * (fChangingCheckTime / fChangingTime);
			m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG2 * (1.0f - fChangingCheckTime / fChangingTime) + m_pGround->m_projectInfo.fDiffuseG1 * (fChangingCheckTime / fChangingTime);
			m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB2 * (1.0f - fChangingCheckTime / fChangingTime) + m_pGround->m_projectInfo.fDiffuseB1 * (fChangingCheckTime / fChangingTime);
			m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR2 * (1.0f - fChangingCheckTime / fChangingTime) + m_pGround->m_projectInfo.fAmbientR1 * (fChangingCheckTime / fChangingTime);
			m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG2 * (1.0f - fChangingCheckTime / fChangingTime) + m_pGround->m_projectInfo.fAmbientG1 * (fChangingCheckTime / fChangingTime);
			m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB2 * (1.0f - fChangingCheckTime / fChangingTime) + m_pGround->m_projectInfo.fAmbientB1 * (fChangingCheckTime / fChangingTime);

			// 2004-11-15 by ydkim
			//			if(g_pSOption->sGammaCtrl > 0)
			ChangeGammaOption(g_pSOption->sGammaCtrl);

			byDayRed = (BYTE)(dwDayFogColor >> 16);
			byDayGreen = (BYTE)(dwDayFogColor >> 8);
			byDayBlue = (BYTE)(dwDayFogColor);
			byNightRed = (BYTE)(dwNightFogColor >> 16);
			byNightGreen = (BYTE)(dwNightFogColor >> 8);
			byNightBlue = (BYTE)(dwNightFogColor);
			byFogRed = byNightRed * (1.0f - fChangingCheckTime / fChangingTime) + byDayRed * (fChangingCheckTime / fChangingTime);
			byFogGreen = byNightGreen * (1.0f - fChangingCheckTime / fChangingTime) + byDayGreen * (fChangingCheckTime / fChangingTime);
			byFogBlue = byNightBlue * (1.0f - fChangingCheckTime / fChangingTime) + byDayBlue * (fChangingCheckTime / fChangingTime);
			m_dwFogColor = D3DCOLOR_ARGB(0x00, byFogRed, byFogGreen, byFogBlue);
		}
		else { // ł· 5şĐŔĚ Áöł­ ŔĚČÄ
			m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR1;
			m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG1;
			m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB1;
			m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR1;
			m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG1;
			m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB1;

			// 2004-11-15 by ydkim
			//			if(g_pSOption->sGammaCtrl > 0)
			ChangeGammaOption(g_pSOption->sGammaCtrl);

			m_dwFogColor = dwDayFogColor;
		}
		//		m_bNight = FALSE;
		m_light0.Specular.r = 1.0f;
		m_light0.Specular.g = 1.0f;
		m_light0.Specular.b = 1.0f;
	}

	// 2004-10-22 by jschoi
	// ľîµÎżöÁö±â Á÷Ŕüżˇ ąŮ·Î ´ŢŔĚ ¶á´Ů.
	// Ľ­Ľ­Č÷ ąŕľĆÁö¸éĽ­ Áß°ŁÂë ąŕľĆÁö¸é ĹÂľçŔĚ ¶á´Ů.	
	if ((nTime >= 0) &&
		(nTime < (TIME_MINUTE * TIME_SECOND + fChangingTime / 2))) {
		m_bNight = TRUE;
	}
	else {
		m_bNight = FALSE;
	}
	m_light0.Direction = SetLightDirection();
	g_pD3dDev->SetLight(0, &m_light0);
	//	int nH = 4, nM = 5, nS = 5; // nH:˝Ă°Ł nM:şĐ nS:ĂĘ ŔÇ ·® ĽÂĆĂÇŇĽö ŔÖ´Ů.(żą> ÇĎ·ç->4˝Ă°Ł 1˝Ă°Ł->60şĐ 1şĐ->60ĂĘ)-°ÔŔÓ»óŔÇ ˝Ă°Ł
	/*	int nTime = (GetTickCount() - m_dwStartTime)/1000 + m_nBaseTime;
	nTime = nTime%(TIME_HOUR*TIME_MINUTE*TIME_SECOND);
	if(nTime >= 0 && nTime < 1*TIME_MINUTE*TIME_SECOND)
	{
	if(g_pShuttleChild->GetCurrentBodyCondition() & ~BODYCON_NIGHTFLY_MASK)
	{
	g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_NIGHTFLY_MASK, TRUE );
	}
	float fChangeTime = (float)(TIME_DAY_CHANGE - nTime);
	if(fChangeTime >= 0)
	{ // ł·żˇĽ­ ąăŔ¸·Î ąŮ˛î´ÂÁß
	m_fSkyRedColor = m_pGround->m_projectInfo.fDiffuseR2 +
	(m_pGround->m_projectInfo.fDiffuseR1 - m_pGround->m_projectInfo.fDiffuseR2)*
	(fChangeTime/TIME_DAY_CHANGE);
	m_fSkyGreenColor = m_pGround->m_projectInfo.fDiffuseG2 +
	(m_pGround->m_projectInfo.fDiffuseG1 - m_pGround->m_projectInfo.fDiffuseG2)*
	(fChangeTime/TIME_DAY_CHANGE);
	m_fSkyBlueColor = m_pGround->m_projectInfo.fDiffuseB2 +
	(m_pGround->m_projectInfo.fDiffuseB1 - m_pGround->m_projectInfo.fDiffuseB2)*
	(fChangeTime/TIME_DAY_CHANGE);
	m_light0.Diffuse.r = m_fSkyRedColor;
	m_light0.Diffuse.g = m_fSkyGreenColor;
	m_light0.Diffuse.b = m_fSkyBlueColor;
	m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR2 +
	(m_pGround->m_projectInfo.fAmbientR1 - m_pGround->m_projectInfo.fAmbientR2)*
	(fChangeTime/TIME_DAY_CHANGE);
	m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG2 +
	(m_pGround->m_projectInfo.fAmbientG1 - m_pGround->m_projectInfo.fAmbientG2)*
	(fChangeTime/TIME_DAY_CHANGE);
	m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB2 +
	(m_pGround->m_projectInfo.fAmbientB1 - m_pGround->m_projectInfo.fAmbientB2)*
	(fChangeTime/TIME_DAY_CHANGE);
	m_light0.Direction = SetLightDirection();
	g_pD3dDev->SetLight( 0, &m_light0 );
	m_bNight = FALSE;
	}
	else
	{ // ąă
	m_fSkyRedColor = m_pGround->m_projectInfo.fDiffuseR2;
	m_fSkyGreenColor = m_pGround->m_projectInfo.fDiffuseG2;
	m_fSkyBlueColor = m_pGround->m_projectInfo.fDiffuseB2;
	m_light0.Diffuse.r = m_fSkyRedColor;
	m_light0.Diffuse.g = m_fSkyGreenColor;
	m_light0.Diffuse.b = m_fSkyBlueColor;
	m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR2;
	m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG2;
	m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB2;
	m_light0.Direction = SetLightDirection();
	g_pD3dDev->SetLight( 0, &m_light0 );
	m_bNight = TRUE;
	}
	}
	else if(nTime >= 1*TIME_MINUTE*TIME_SECOND && nTime < TIME_HOUR*TIME_MINUTE*TIME_SECOND)
	{
	if(g_pShuttleChild->GetCurrentBodyCondition() & BODYCON_NIGHTFLY_MASK)
	{
	g_pD3dApp->SendFieldSocketChangeCharacterBodyCondition( BODYCON_NIGHTFLY_MASK, FALSE );
	}
	float fChangeTime = (float)(TIME_DAY_CHANGE - (nTime - 1*TIME_MINUTE*TIME_SECOND));
	if(fChangeTime >= 0)
	{ // ąăżˇĽ­ ł·Ŕ¸·Î ąŮ˛î´ÂÁß
	m_fSkyRedColor = m_pGround->m_projectInfo.fDiffuseR1 -
	(m_pGround->m_projectInfo.fDiffuseR1 - m_pGround->m_projectInfo.fDiffuseR2)*
	(fChangeTime/TIME_DAY_CHANGE);
	m_fSkyGreenColor = m_pGround->m_projectInfo.fDiffuseG1 -
	(m_pGround->m_projectInfo.fDiffuseG1 - m_pGround->m_projectInfo.fDiffuseG2)*
	(fChangeTime/TIME_DAY_CHANGE);
	m_fSkyBlueColor = m_pGround->m_projectInfo.fDiffuseB1 -
	(m_pGround->m_projectInfo.fDiffuseB1 - m_pGround->m_projectInfo.fDiffuseB2)*
	(fChangeTime/TIME_DAY_CHANGE);
	m_light0.Diffuse.r = m_fSkyRedColor;
	m_light0.Diffuse.g = m_fSkyGreenColor;
	m_light0.Diffuse.b = m_fSkyBlueColor;
	m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR1 -
	(m_pGround->m_projectInfo.fAmbientR1 - m_pGround->m_projectInfo.fAmbientR2)*
	(fChangeTime/TIME_DAY_CHANGE);
	m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG1 -
	(m_pGround->m_projectInfo.fAmbientG1 - m_pGround->m_projectInfo.fAmbientG2)*
	(fChangeTime/TIME_DAY_CHANGE);
	m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB1 -
	(m_pGround->m_projectInfo.fAmbientB1 - m_pGround->m_projectInfo.fAmbientB2)*
	(fChangeTime/TIME_DAY_CHANGE);
	m_light0.Direction = SetLightDirection();
	g_pD3dDev->SetLight( 0, &m_light0 );
	m_bNight = TRUE;
	}
	else
	{ // ł·
	m_fSkyRedColor = m_pGround->m_projectInfo.fDiffuseR1;
	m_fSkyGreenColor = m_pGround->m_projectInfo.fDiffuseG1;
	m_fSkyBlueColor = m_pGround->m_projectInfo.fDiffuseB1;
	m_light0.Diffuse.r = m_fSkyRedColor;
	m_light0.Diffuse.g = m_fSkyGreenColor;
	m_light0.Diffuse.b = m_fSkyBlueColor;
	m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR1;
	m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG1;
	m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB1;
	m_light0.Direction = SetLightDirection();
	g_pD3dDev->SetLight( 0, &m_light0 );
	m_bNight = FALSE;
	}
	}
	::SetFogLevel( g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex, !m_bNight );
	#ifdef _DEBUG
	char buf[256];
	sprintf( buf, "LIGHT DIRECTION : [%.1f, %.1f, %.1f]\n", m_light0.Direction.x, m_light0.Direction.y, m_light0.Direction.z);
	DBGOUT(buf);
	#endif
	*/
}

D3DXVECTOR3 CSceneData::SetLightDirection()
{
	FLOG("CSceneData::SetLightDirection()");

	D3DXVECTOR3 vDirection = GetMapDirection(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex, !m_bNight);

	D3DXVec3Normalize(&vDirection, &vDirection);
	return vDirection;
	//#endif
	/*
	//	int nH = 4, nM = 5, nS = 5; // nH:˝Ă°Ł nM:şĐ nS:ĂĘ ŔÇ ·® ĽÂĆĂÇŇĽö ŔÖ´Ů.(żą> ÇĎ·ç->4˝Ă°Ł 1˝Ă°Ł->60şĐ 1şĐ->60ĂĘ)-°ÔŔÓ»óŔÇ ˝Ă°Ł
	int nTime = (GetTickCount() - m_dwStartTime) + m_nBaseTime*1000;
	nTime = nTime%(TIME_HOUR*TIME_MINUTE*TIME_SECOND*1000);
	D3DXMATRIX mat;
	D3DXVECTOR3 vVel = D3DXVECTOR3(1,0,0);
	D3DXVECTOR3 vDir;
	float fAngle;
	if(nTime >= 0 && nTime < 1*TIME_MINUTE*TIME_SECOND*1000)
	{
	//		vDir = D3DXVECTOR3(0.0f,0.8f,-1.0f);
	vDir = D3DXVECTOR3(0.0f,-1.0f,1.0f);
	D3DXVec3Normalize(&vDir,&vDir);
	//		fAngle = ((float)nTime)*((4.5f)/(float)(1*TIME_MINUTE*TIME_SECOND*1000));
	fAngle = ((float)nTime/(float)(1*TIME_MINUTE*TIME_SECOND*1000))*(PI/4);
	}
	else if(nTime >= 1*TIME_MINUTE*TIME_SECOND*1000 && nTime < TIME_HOUR*TIME_MINUTE*TIME_SECOND*1000)
	{
	//		vDir = D3DXVECTOR3(0.3f,0.8f,-1.0f);
	vDir = D3DXVECTOR3(0.0f,-1.0f,1.0f);
	D3DXVec3Normalize(&vDir,&vDir);
	//		fAngle = ((float)(nTime - 1*TIME_MINUTE*TIME_SECOND*1000))*((4.5f)/(float)((TIME_HOUR-1)*TIME_MINUTE*TIME_SECOND*1000));
	fAngle = ((float)(nTime - 1*TIME_MINUTE*TIME_SECOND*1000)/(float)((TIME_HOUR-1)*TIME_MINUTE*TIME_SECOND*1000))*(PI/4);
	}
	//	D3DXMatrixRotationAxis(&mat,&vVel,-fAngle);
	D3DXMatrixRotationAxis(&mat,&vVel,fAngle);
	D3DXVec3TransformCoord(&vDir,&vDir,&mat);
	D3DXVec3Normalize(&vDir,&vDir);
	return vDir;
	*/
}

int CSceneData::CheckMove()
{
	int Oldx, Oldy, Curx, Cury;
	Oldx = g_pD3dApp->m_pShuttleChild->m_vOldPos.x / (MAP_BLOCK_SIZE / 2);
	Oldy = g_pD3dApp->m_pShuttleChild->m_vOldPos.z / (MAP_BLOCK_SIZE / 2);
	Curx = g_pD3dApp->m_pShuttleChild->m_vPos.x / (MAP_BLOCK_SIZE / 2);
	Cury = g_pD3dApp->m_pShuttleChild->m_vPos.z / (MAP_BLOCK_SIZE / 2);

	if (Curx % 2) {
		//żŔ¸ĄÂĘ
		if (Cury % 2) {
			//ľĆ·ˇ
			return BOTTOMRIGHT;
		}
		else {
			//Ŕ§
			return TOPRIGHT;
		}
	}
	else {
		//żŢÂĘ
		if (Cury % 2) {
			//ľĆ·ˇ
			return BOTTOMLEFT;
		}
		else {
			// Ŕ§
			return TOPLEFT;
		}
	}

	return -1;	// error
}

void CSceneData::CalcCollisionRange(int nMoveType)
{
	// Ăćµą Ăł¸® żµżŞ °č»ę
	//	int nTestNormal=0, nTestBig=0;

	m_vectorCollisionObjectPtrList.clear();
	TWO_BLOCK_INDEXES blockIdx;
	int x, z;

	switch (nMoveType) {
		case TOPLEFT:
			blockIdx.sMinX = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.x / MAP_BLOCK_SIZE) - 1);
			blockIdx.sMaxX = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.x / MAP_BLOCK_SIZE));
			blockIdx.sMinZ = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.z / MAP_BLOCK_SIZE) - 1);
			blockIdx.sMaxZ = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.z / MAP_BLOCK_SIZE));
			break;
		case TOPRIGHT:
			blockIdx.sMinX = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.x / MAP_BLOCK_SIZE));
			blockIdx.sMaxX = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.x / MAP_BLOCK_SIZE) + 1);
			blockIdx.sMinZ = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.z / MAP_BLOCK_SIZE) - 1);
			blockIdx.sMaxZ = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.z / MAP_BLOCK_SIZE));
			break;
		case BOTTOMLEFT:
			blockIdx.sMinX = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.x / MAP_BLOCK_SIZE) - 1);
			blockIdx.sMaxX = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.x / MAP_BLOCK_SIZE));
			blockIdx.sMinZ = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.z / MAP_BLOCK_SIZE));
			blockIdx.sMaxZ = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.z / MAP_BLOCK_SIZE) + 1);
			break;
		case BOTTOMRIGHT:
			blockIdx.sMinX = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.x / MAP_BLOCK_SIZE));
			blockIdx.sMaxX = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.x / MAP_BLOCK_SIZE) + 1);
			blockIdx.sMinZ = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.z / MAP_BLOCK_SIZE));
			blockIdx.sMaxZ = (int)((g_pD3dApp->m_pShuttleChild->m_vPos.z / MAP_BLOCK_SIZE) + 1);
			break;
	}

	int nMaxX = (int)m_pGround->m_fSizeMap_X / MAP_BLOCK_SIZE;
	int nMaxZ = (int)m_pGround->m_fSizeMap_Z / MAP_BLOCK_SIZE;
	if (blockIdx.sMinX < 0)
		blockIdx.sMinX = 0;
	if (blockIdx.sMaxX >= nMaxX)
		blockIdx.sMaxX = nMaxX - 1;
	if (blockIdx.sMinZ < 0)
		blockIdx.sMinZ = 0;
	if (blockIdx.sMaxZ >= nMaxZ)
		blockIdx.sMaxZ = nMaxZ - 1;

	x = blockIdx.sMinX;
	while (x <= blockIdx.sMaxX) {
		z = blockIdx.sMinZ;
		while (z <= blockIdx.sMaxZ) {
			CObjectChild* pObj = (CObjectChild*)m_pGround->m_ppObjectList[x][z].m_pChild;

			while (pObj) {

				if (pObj->m_pObjectInfo) {
					float fRadius = 0;
					if (pObj->m_pObjMesh) {
						fRadius = pObj->m_pObjMesh->m_fRadius;
					}
					if (D3DXVec3Length(&(pObj->m_vOriPos - g_pShuttleChild->m_vPos)) - fRadius < NORMAL_OBJECT_RADIUS_TO_I_DISTANCE) {
						m_vectorCollisionObjectPtrList.push_back(pObj);
					}
				}
				pObj = (CObjectChild*)pObj->m_pNext;
			}
			z++;
		}
		x++;
	}

	//	nTestNormal = m_vectorCollisionObjectPtrList.size();

	// 2005-02-11 by jschoi  Big Object ¸¦ Ăćµą ¸®˝şĆ®żˇ Ăß°ˇ
	CObjectChild* pBigObj = (CObjectChild*)m_pGround->m_pBigObject->m_pChild;

	while (pBigObj) {
		if (pBigObj->m_pObjectInfo) {
			float fRadius = 0;
			if (pBigObj->m_pObjMesh) {
				fRadius = pBigObj->m_pObjMesh->m_fRadius;
			}

			if (g_pShuttleChild && pBigObj->m_pObjectInfo->ObjectRenderType != OBJECT_BIG_NO_COLLISION &&		//2013-05-22 by ssjung ÄłłŞ´Ů ŔÍĽÁĽÇ żŔ·ů żąąć ÄÚµĺ Ăß°ˇ
				pBigObj->m_pObjectInfo->ObjectRenderType != OBJECT_BIG_CONTOUR &&
				D3DXVec3Length(&(pBigObj->m_vOriPos - g_pShuttleChild->m_vPos)) - fRadius < BIG_OBJECT_RADIUS_TO_I_DISTANCE) {

				// 2005-04-12 by jschoi - Tutorial
				if (g_pTutorial && g_pTutorial->IsTutorialMode() == TRUE)		//2013-05-22 by ssjung ÄłłŞ´Ů ŔÍĽÁĽÇ żŔ·ů żąąć ÄÚµĺ Ăß°ˇ
				{
					if (pBigObj->m_nCode == TUTORIAL_GATE) {
						if (g_pTutorial->GetChapter() == L1 &&
							g_pTutorial->IsEnableTutorialGate(pBigObj->m_vPos)) {
							m_vectorCollisionObjectPtrList.push_back(pBigObj);
						}
					}
					else {
						m_vectorCollisionObjectPtrList.push_back(pBigObj);
					}
				}
				else {
					m_vectorCollisionObjectPtrList.push_back(pBigObj);
				}
			}
		}
		pBigObj = (CObjectChild*)pBigObj->m_pNext;
	}

	// Water Object ¸¦ Ăćµą ¸®˝şĆ®żˇ Ăß°ˇ
	CObjectChild* pWaterObj = (CObjectChild*)m_pGround->m_pWaterObject->m_pChild;

	while (pWaterObj) {
		if (pWaterObj->m_pObjectInfo) {
			float fRadius = 0;
			if (pWaterObj->m_pObjMesh) {
				fRadius = pWaterObj->m_pObjMesh->m_fRadius;
			}

			if (pWaterObj->m_pObjectInfo->ObjectRenderType != OBJECT_BIG_NO_COLLISION &&
				pWaterObj->m_pObjectInfo->ObjectRenderType != OBJECT_BIG_CONTOUR &&
				D3DXVec3Length(&(pWaterObj->m_vOriPos - g_pShuttleChild->m_vPos)) - fRadius < BIG_OBJECT_RADIUS_TO_I_DISTANCE) {
				m_vectorCollisionObjectPtrList.push_back(pWaterObj);
			}
		}
		pWaterObj = (CObjectChild*)pWaterObj->m_pNext;
	}


	// 2004-11-29 by jschoi - żŔşęÁ§Ć® ¸ó˝şĹÍ żŔşęÁ§Ć®¸¦ Ăćµą ¸®˝şĆ®żˇ Ăß°ˇ
	CObjectChild* pObjectMonster = (CObjectChild*)m_pGround->m_pObjectMonster->m_pChild;

	while (pObjectMonster) {
		if (pObjectMonster->m_pObjectInfo &&
			pObjectMonster->m_bEnableObjectMonsterObject == TRUE) {
			float fRadius = 0;
			if (pObjectMonster->m_pObjMesh) {
				fRadius = pObjectMonster->m_pObjMesh->m_fRadius;
			}

			if (D3DXVec3Length(&(pObjectMonster->m_vOriPos - g_pShuttleChild->m_vPos)) - fRadius < BIG_OBJECT_RADIUS_TO_I_DISTANCE) {
				m_vectorCollisionObjectPtrList.push_back(pObjectMonster);
			}
		}
		pObjectMonster = (CObjectChild*)pObjectMonster->m_pNext;
	}


}

VOID CSceneData::CheckObjectRenderList(BOOL bCheckRange)
{
	FLOG("CSceneData::CheckObjectRenderList()");

	BOOL bIsMoveRange = TRUE;

	int nMoveType;

	if (g_pD3dApp->m_pShuttleChild->m_ptOldPoint.x == (int)g_pD3dApp->m_pShuttleChild->m_vPos.x / MAP_BLOCK_SIZE
		&& g_pD3dApp->m_pShuttleChild->m_ptOldPoint.y == (int)g_pD3dApp->m_pShuttleChild->m_vPos.z / MAP_BLOCK_SIZE) {
		bIsMoveRange = FALSE;
	}

	if (bIsMoveRange || bCheckRange == FALSE) // »ç°˘żµżŞŔ» ąţľîłµŔ» °ćżě
	{
		m_vectorRangeObjectPtrList.clear();

		TWO_BLOCK_INDEXES blockIdx;
		int x, z;
		//		int nViewRange = m_fFogStartValue > MAP_BLOCK_SIZE ? m_fFogStartValue : MAP_BLOCK_SIZE;


		// 2005-04-29 by jschoi
		float fOptionDistance;
		if (g_pTutorial->IsTutorialMode() == TRUE) {
			fOptionDistance = m_fFogEndValue;
		}
		else {
			fOptionDistance = (m_fFogEndValue * g_pSOption->sTerrainRender) / MAX_OPTION_VALUE;
		}

		int nViewRange = fOptionDistance > MAP_BLOCK_SIZE ?
			fOptionDistance : MAP_BLOCK_SIZE;

		m_pGround->GetBlockAdjacentToPosition(g_pD3dApp->m_pShuttleChild->m_vPos.x,
											  g_pD3dApp->m_pShuttleChild->m_vPos.z,
											  nViewRange/*SIZE_OBJECT_VISIBLERECT*/,
											  blockIdx);			//

										  // add by frustum
		x = blockIdx.sMinX;
		while (x <= blockIdx.sMaxX) {
			z = blockIdx.sMinZ;
			while (z <= blockIdx.sMaxZ) {
				CObjectChild* pObj = (CObjectChild*)m_pGround->m_ppObjectList[x][z].m_pChild;

				while (pObj) {

					if (pObj->m_pObjectInfo)//&& 
						//							pObj->m_pObjectInfo->Collision )
						//							(/*pObj->m_bCheckColl ||*/ g_pD3dApp->m_bDegree != 0 || g_pD3dApp->m_dwGameState != _GAME) )
					{
						m_vectorRangeObjectPtrList.push_back(pObj);
					}
					pObj = (CObjectChild*)pObj->m_pNext;
				}
				z++;
			}
			x++;
		}

		// 2004.06.08 syjun,jschoi
		// ŔýµÎĂĽ ÄĂ¸µÇŇ ¸®˝şĆ®żˇ BigObject Ăß°ˇ
		CObjectChild* pBigObj = (CObjectChild*)m_pGround->m_pBigObject->m_pChild;

		while (pBigObj) {
			if (pBigObj->m_pObjectInfo) {
				// 2005-04-12 by jschoi - Tutorial
				if (g_pTutorial->IsTutorialMode() == TRUE) {
					if (pBigObj->m_nCode == TUTORIAL_GATE) {
						if (g_pTutorial->GetLesson() == L1 &&
							g_pTutorial->IsEnableTutorialGate(pBigObj->m_vPos)) {
							m_vectorRangeObjectPtrList.push_back(pBigObj);
						}
					}
					else {
						m_vectorRangeObjectPtrList.push_back(pBigObj);
					}
				}
				else {
					m_vectorRangeObjectPtrList.push_back(pBigObj);
				}
			}
			pBigObj = (CObjectChild*)pBigObj->m_pNext;
		}


		// 2005-02-11 by jschoi  ŔýµÎĂĽ ÄĂ¸µÇŇ ¸®˝şĆ®żˇ ą° żŔşęÁ§Ć® Ăß°ˇ
		CObjectChild* pWaterObj = (CObjectChild*)m_pGround->m_pWaterObject->m_pChild;

		while (pWaterObj) {
			if (pWaterObj->m_pObjectInfo) {
				m_vectorRangeObjectPtrList.push_back(pWaterObj);
			}
			pWaterObj = (CObjectChild*)pWaterObj->m_pNext;
		}


		// 2004-11-29 by jschoi - żŔşęÁ§Ć® ¸ó˝şĹÍ¸¦ ŔýµÎĂĽ ÄĂ¸µÇŇ ¸®˝şĆ®żˇ Ăß°ˇ
		CObjectChild* pObjectMonster = (CObjectChild*)m_pGround->m_pObjectMonster->m_pChild;

		while (pObjectMonster) {
			if (pObjectMonster->m_pObjectInfo &&
				pObjectMonster->m_bEnableObjectMonsterObject == TRUE) {
				m_vectorRangeObjectPtrList.push_back(pObjectMonster);
			}
			pObjectMonster = (CObjectChild*)pObjectMonster->m_pNext;
		}

	}

	nMoveType = CheckMove();

	CalcCollisionRange(nMoveType);

	m_vectorCulledObjectPtrList.clear();

	// 2004.06.08 syjun,jschoi
	// ¸đµç BigObjectżÍ żµżŞ ÄĂ¸µµČ ¸®˝şĆ®¸¦ ŔýµÎĂĽ ÄĂ¸µ
	vectorCObjectChildPtr::iterator itObj = m_vectorRangeObjectPtrList.begin();
	while (itObj != m_vectorRangeObjectPtrList.end()) {
		ASSERT_ASSERT((*itObj));
		float fRadius = 0;
		if ((*itObj)->m_pObjMesh) {
			fRadius = (*itObj)->m_pObjMesh->m_fRadius;
		}
		if ((*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_BIG_EFFECT &&
			D3DXVec3Length(&((*itObj)->m_vOriPos - g_pCamera->m_vCamCurrentPos)) - fRadius < m_fFogEndValue) {
			m_vectorCulledObjectPtrList.push_back(*itObj);
			itObj++;
			continue;
		}
		BOOL bCheckIn = g_pFrustum->CheckSphere((*itObj)->m_vOriPos.x, (*itObj)->m_vOriPos.y, (*itObj)->m_vOriPos.z, fRadius);
		if (bCheckIn) {
			if ((*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_BIG) {
				m_vectorCulledObjectPtrList.push_back(*itObj);
			}
			else if (((*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_BIG_CULLED ||
					  (*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_BIG_NO_COLLISION ||
					  (*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_BIG_CONTOUR) &&
					 D3DXVec3Length(&((*itObj)->m_vOriPos - g_pCamera->m_vCamCurrentPos)) - fRadius < m_fFogEndValue) {
				if ((*itObj)->m_pObjectInfo->ObjectRenderType != OBJECT_BIG_CONTOUR
					|| FALSE == g_pSOption->sLowQuality) {// 2005-07-11 by ispark, ĂÖĽŇ ÇÁ·ąŔÓŔĎ¶§ żÜ°˘ żŔşęÁ§Ć® ·»´ő¸µ Á¦żÜ
					m_vectorCulledObjectPtrList.push_back(*itObj);
				}
			}
			else if (((*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_NORMAL ||
					  (*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_SHOP_NPC) &&
					 D3DXVec3Length(&((*itObj)->m_vOriPos - g_pCamera->m_vCamCurrentPos)) - fRadius < m_fFogEndValue) {
				m_vectorCulledObjectPtrList.push_back(*itObj);
			}
			else if ((*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_MONSTER_OBJECT &&
					 (*itObj)->m_bEnableObjectMonsterObject == TRUE) {
				m_vectorCulledObjectPtrList.push_back(*itObj);
			}
		}
		//		if( bCheckIn && 
		//			((*itObj)->m_pObjectInfo->RadiusForClient > BIG_OBJECT_SIZE || 
		//			D3DXVec3Length(&((*itObj)->m_vOriPos - g_pCamera->m_vCamCurrentPos)) - fRadius<m_fFogEndValue))
		//		{
		//			m_vectorCulledObjectPtrList.push_back((*itObj));
		//		}
		itObj++;
	}


	g_pShuttleChild->m_ptOldPoint.x = g_pShuttleChild->m_vPos.x / TILE_SIZE;
	g_pShuttleChild->m_ptOldPoint.y = g_pShuttleChild->m_vPos.z / TILE_SIZE;

	// 2004-12-15 by jschoi - °Ĺ¸®żˇ µű¶óĽ­ Á¤·Ä
	sort(m_vectorCulledObjectPtrList.begin(), m_vectorCulledObjectPtrList.end(), CompareUnit());
}

VOID CSceneData::SetObjectRenderList()
{
	FLOG("CSceneData::SetObjectRenderList()");

	int nMoveType;
	m_vectorRangeObjectPtrList.clear();
	TWO_BLOCK_INDEXES blockIdx;
	int x, z;
	//	int nVal  = ((int)m_fFogEndValue) % MAP_BLOCK_SIZE;
	int nViewRange = m_fFogStartValue > MAP_BLOCK_SIZE ? m_fFogStartValue : MAP_BLOCK_SIZE;

	m_pGround->GetBlockAdjacentToPosition(g_pD3dApp->m_pShuttleChild->m_vPos.x,
										  g_pD3dApp->m_pShuttleChild->m_vPos.z,
										  nViewRange/*SIZE_OBJECT_VISIBLERECT*/,
										  blockIdx);			//

									  // add by frustum
	x = blockIdx.sMinX;
	while (x <= blockIdx.sMaxX) {
		z = blockIdx.sMinZ;
		while (z <= blockIdx.sMaxZ) {
			CObjectChild* pObj = (CObjectChild*)m_pGround->m_ppObjectList[x][z].m_pChild;
			while (pObj) {
				if (pObj->m_pObjectInfo) {
					m_vectorRangeObjectPtrList.push_back(pObj);
				}
				pObj = (CObjectChild*)pObj->m_pNext;
			}
			z++;
		}
		x++;
	}

	// 2004.06.08 syjun,jschoi
	// ŔýµÎĂĽ ÄĂ¸µÇŇ ¸®˝şĆ®żˇ BigObject Ăß°ˇ
	CObjectChild* pBigObj = (CObjectChild*)m_pGround->m_pBigObject->m_pChild;

	while (pBigObj) {
		if (pBigObj->m_pObjectInfo) {
			m_vectorRangeObjectPtrList.push_back(pBigObj);
		}
		pBigObj = (CObjectChild*)pBigObj->m_pNext;
	}

	// 2005-02-11 by jschoi  ŔýµÎĂĽ ÄĂ¸µÇŇ ¸®˝şĆ®żˇ ą° żŔşęÁ§Ć® Ăß°ˇ
	CObjectChild* pWaterObj = (CObjectChild*)m_pGround->m_pWaterObject->m_pChild;

	while (pWaterObj) {
		if (pWaterObj->m_pObjectInfo) {
			m_vectorRangeObjectPtrList.push_back(pWaterObj);
		}
		pWaterObj = (CObjectChild*)pWaterObj->m_pNext;
	}

	// 2004-11-29 by jschoi - żŔşęÁ§Ć® ¸ó˝şĹÍ¸¦ ŔýµÎĂĽ ÄĂ¸µÇŇ ¸®˝şĆ®żˇ Ăß°ˇ 
	CObjectChild* pObjectMonster = (CObjectChild*)m_pGround->m_pObjectMonster->m_pChild;

	while (pObjectMonster) {
		if (pObjectMonster->m_pObjectInfo &&
			pObjectMonster->m_bEnableObjectMonsterObject == TRUE) {
			m_vectorRangeObjectPtrList.push_back(pObjectMonster);
		}
		pObjectMonster = (CObjectChild*)pObjectMonster->m_pNext;
	}


	nMoveType = CheckMove();

	CalcCollisionRange(nMoveType);

	m_vectorCulledObjectPtrList.clear();

	// 2004.06.08 syjun,jschoi
	// ¸đµç BigObjectżÍ żµżŞ ÄĂ¸µµČ ¸®˝şĆ®¸¦ ŔýµÎĂĽ ÄĂ¸µ
	vectorCObjectChildPtr::iterator itObj = m_vectorRangeObjectPtrList.begin();
	while (itObj != m_vectorRangeObjectPtrList.end()) {
		float fRadius = 0;
		if ((*itObj)->m_pObjMesh) {
			fRadius = (*itObj)->m_pObjMesh->m_fRadius;
		}
		BOOL bCheckIn = g_pFrustum->CheckSphere((*itObj)->m_vOriPos.x, (*itObj)->m_vOriPos.y, (*itObj)->m_vOriPos.z, fRadius);
		if (bCheckIn) {
			if ((*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_BIG) {
				m_vectorCulledObjectPtrList.push_back(*itObj);
			}
			else if (((*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_BIG_CULLED ||
					  (*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_BIG_NO_COLLISION ||
					  (*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_BIG_CONTOUR) &&
					 D3DXVec3Length(&((*itObj)->m_vOriPos - g_pShuttleChild->m_vPos)) - fRadius < m_fFogEndValue) {
				if ((*itObj)->m_pObjectInfo->ObjectRenderType != OBJECT_BIG_CONTOUR
					|| FALSE == g_pSOption->sLowQuality) {// 2005-07-11 by ispark, ĂÖĽŇ ÇÁ·ąŔÓŔĎ¶§ żÜ°˘ żŔşęÁ§Ć® ·»´ő¸µ Á¦żÜ
					m_vectorCulledObjectPtrList.push_back(*itObj);
				}
			}
			else if (((*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_NORMAL ||
					  (*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_SHOP_NPC) &&
					 D3DXVec3Length(&((*itObj)->m_vOriPos - g_pShuttleChild->m_vPos)) - fRadius < m_fFogEndValue) {
				m_vectorCulledObjectPtrList.push_back(*itObj);
			}
			else if ((*itObj)->m_pObjectInfo->ObjectRenderType == OBJECT_MONSTER_OBJECT &&
					 (*itObj)->m_bEnableObjectMonsterObject == TRUE) {
				m_vectorCulledObjectPtrList.push_back(*itObj);
			}

		}
		//		if( bCheckIn && 
		//			((*itObj)->m_pObjectInfo->RadiusForClient > BIG_OBJECT_SIZE || 
		//			D3DXVec3Length(&((*itObj)->m_vOriPos - g_pCamera->m_vCamCurrentPos)) - fRadius<m_fFogEndValue))
		//		{
		//			m_vectorCulledObjectPtrList.push_back((*itObj));
		//		}
		itObj++;
	}

	return;
}

VOID CSceneData::ChangeResource()
{
	FLOG("CSceneData::ChangeResource()");
	SAFE_DELETE(m_pData);
	m_pData = new CGameData;
	char strPath[256];
	wsprintf(strPath, ".\\Res-Map\\tileset%d%02d", g_pD3dApp->m_bDegree, m_pGround->m_projectInfo.useTileSetIndex);
	if (strlen(strPath) > 0) {
		m_pData->SetFile(strPath, FALSE, NULL, 0);
	}
	//	LoadTex();
}

VOID CSceneData::DeleteObjectList(int nIndex)
{
	//	FLOG("CSceneData::DeleteObjectList(int nIndex)");
	//	CMapObjectIterator itObj = m_mapObjectList.find(nIndex);
	//	if(itObj != m_mapObjectList.end())
	//	{
	//		m_mapObjectList.erase(itObj);
	//	}
}

VOID CSceneData::InsertToBlockData(CAtumNode* pNode)
{
	FLOG("CSceneData::InsertToBlockData(CAtumNode * pNode)");
	if (pNode->m_dwPartType == _ENEMY) {
		CEnemyData* pEnemy = (CEnemyData*)pNode;
		int nNew = pEnemy->m_ptCurRegion.x * m_nBlockSizeY + pEnemy->m_ptCurRegion.y;
		if (pEnemy->m_ptCurRegion.x >= 0 && pEnemy->m_ptCurRegion.x < m_nBlockSizeX
			&& pEnemy->m_ptCurRegion.y >= 0 && pEnemy->m_ptCurRegion.y < m_nBlockSizeY) {
			m_vecEnemyBlockList[nNew].push_back(pEnemy);
		}
		else {
			DBGOUT("Critical Error : InsertToBlockData(Enemy)\n");
		}
	}
	else if (pNode->m_dwPartType == _MONSTER) {
		CMonsterData* pMonster = (CMonsterData*)pNode;
		int nNew = pMonster->m_ptCurRegion.x * m_nBlockSizeY + pMonster->m_ptCurRegion.y;
		if (pMonster->m_ptCurRegion.x >= 0 && pMonster->m_ptCurRegion.x < m_nBlockSizeX
			&& pMonster->m_ptCurRegion.y >= 0 && pMonster->m_ptCurRegion.y < m_nBlockSizeY) {
			m_vecMonsterList[nNew].push_back(pMonster);
		}
		else {
			DBGOUT("Critical Error  : InsertToBlockData(Monster)\n");
		}
	}
}

VOID CSceneData::ChangeToBlockData(CAtumNode* pNode)
{
	FLOG("CSceneData::ChangeToBlockData(CAtumNode * pNode)");
	bool bChange = false;
	if (pNode->m_dwPartType == _ENEMY) {
		CEnemyData* pEnemy = (CEnemyData*)pNode;
		int nOld = pEnemy->m_ptOldRegion.x * m_nBlockSizeY + pEnemy->m_ptOldRegion.y;
		int nNew = pEnemy->m_ptCurRegion.x * m_nBlockSizeY + pEnemy->m_ptCurRegion.y;
		if (nOld < m_nBlockSizeX * m_nBlockSizeY) {
			CVecEnemyIterator it = m_vecEnemyBlockList[nOld].begin();
			while (it != m_vecEnemyBlockList[nOld].end()) {
				if (*it == pEnemy) {
					it = m_vecEnemyBlockList[nOld].erase(it);
					bChange = true;
					break;
				}
				it++;
			}
			if (!bChange) {
				DBGOUT("Critical Error: ChangeToBlockData(Enemy)\n");
			}
			if (nNew < m_nBlockSizeX * m_nBlockSizeY) {
				m_vecEnemyBlockList[nNew].push_back(pEnemy);
			}
		}
	}
	else if (pNode->m_dwPartType == _MONSTER) {
		CMonsterData* pMonster = (CMonsterData*)pNode;
		int nOld = pMonster->m_ptOldRegion.x * m_nBlockSizeY + pMonster->m_ptOldRegion.y;
		int nNew = pMonster->m_ptCurRegion.x * m_nBlockSizeY + pMonster->m_ptCurRegion.y;
		if (nOld < m_nBlockSizeX * m_nBlockSizeY) {
			CVecMonsterIterator it = m_vecMonsterList[nOld].begin();
			while (it != m_vecMonsterList[nOld].end()) {
				if (*it == pMonster) {
					m_vecMonsterList[nOld].erase(it);
					bChange = true;
					break;
				}
				it++;
			}
			if (!bChange) {
				DBGOUT("Critical Error : ChangeToBlockData(Monster)\n");
			}
			if (nNew < m_nBlockSizeX * m_nBlockSizeY) {
				m_vecMonsterList[nNew].push_back(pMonster);
			}
		}
	}
}

VOID CSceneData::DeleteToBlockData(CAtumNode* pNode)
{
	FLOG("CSceneData::DeleteToBlockData(CAtumNode * pNode)");
	if (!m_pWeaponData)
		return;
	// żţĆůÁß ŔĚ Äł¸ŻŔ» ÇâÇĎ´Â °Íµé
	CWeapon* pWeapon = (CWeapon*)m_pWeaponData->m_pChild;
	while (pWeapon) {
		if (pWeapon->m_pTarget == pNode) {
			pWeapon->m_pTarget = NULL;
		}
		else if (pWeapon->m_pAttacker == pNode) {
			// 2005-08-03 by ispark
			// Ŕű ąĚ»çŔĎŔ» łÖľî¶ó
			pWeapon->m_pAttacker = NULL;
			pWeapon->m_bUsing = FALSE;
		}
		pWeapon = (CWeapon*)pWeapon->m_pNext;
	}
	// shuttleŔÇ Ĺ¸°ŮŔÎ °ćżě
	if (g_pShuttleChild->m_pOrderTarget &&
		g_pShuttleChild->m_pOrderTarget == pNode) {
		g_pShuttleChild->m_pOrderTarget = NULL;
	}
	if (g_pShuttleChild->m_pTarget &&
		g_pShuttleChild->m_pTarget == pNode) {
		g_pShuttleChild->m_pTarget = NULL;
	}
	// ŔŻ´Öżˇ ŔĺÂřµČ ľĆŔĚĹŰ
	DeleteEffect(pNode);
	if (pNode->m_dwPartType == _ENEMY) {
		CEnemyData* pEnemy = (CEnemyData*)pNode;
		int nDel = pEnemy->m_ptCurRegion.x * m_nBlockSizeY + pEnemy->m_ptCurRegion.y;
		if (nDel < m_nBlockSizeY * m_nBlockSizeX) {
			CVecEnemyIterator it = m_vecEnemyBlockList[nDel].begin();
			while (it != m_vecEnemyBlockList[nDel].end()) {
				if (*it == pEnemy) {
					it = m_vecEnemyBlockList[nDel].erase(it);
					return;
				}
				it++;
			}
		}
		DBGOUT("Critical Error : DeleteToBlockData(Enemy)\n");
	}
	else if (pNode->m_dwPartType == _MONSTER) {
		CMonsterData* pMonster = (CMonsterData*)pNode;
		int nDel = pMonster->m_ptCurRegion.x * m_nBlockSizeY + pMonster->m_ptCurRegion.y;
		if (nDel < m_nBlockSizeY * m_nBlockSizeX) {
			CVecMonsterIterator it = m_vecMonsterList[nDel].begin();
			while (it != m_vecMonsterList[nDel].end()) {
				if (*it == pMonster) {
					m_vecMonsterList[nDel].erase(it);
					return;
				}
				it++;
			}
		}
		DBGOUT("Critical Error : DeleteToBlockData(Monster)\n");
	}
}

VOID CSceneData::SetupLights()
{
	FLOG("CSceneData::SetupLights()");
	// 0:ÁÖ¶óŔĚĆ® 1:ŔĚĆĺĆ®żë¶óŔĚĆ®(Á¤¸éżˇĽ­ ąŕ°Ô) 2:¸ó˝şĹÍ µĄąĚÁö˝Ă 3:ĽĹĆ˛żë ¶óŔĚĆ®(Ŕ§żˇĽ­ ľĆ·ˇ·Î)
	// Čň»öŔÇ Áˇ±¤żř ĽłÁ¤
	D3DXVECTOR3 vUp = D3DXVECTOR3(0, 1, 0);
	/////////   light0   /////////////////// ŔüĂĽ¸¦ şńĂá´Ů.
	ZeroMemory(&m_light0, sizeof(D3DLIGHT9));
	m_light0.Type = D3DLIGHT_DIRECTIONAL;
	if (m_pGround) {
		m_light0.Diffuse.r = m_pGround->m_projectInfo.fDiffuseR1;
		m_light0.Diffuse.g = m_pGround->m_projectInfo.fDiffuseG1;
		m_light0.Diffuse.b = m_pGround->m_projectInfo.fDiffuseB1;
		m_light0.Ambient.r = m_pGround->m_projectInfo.fAmbientR1;
		m_light0.Ambient.g = m_pGround->m_projectInfo.fAmbientG1;
		m_light0.Ambient.b = m_pGround->m_projectInfo.fAmbientB1;
		m_light0.Specular.r = 1.0f;
		m_light0.Specular.g = 1.0f;
		m_light0.Specular.b = 1.0f;
	}
	else {
		m_light0.Diffuse.r = 1.0f;
		m_light0.Diffuse.g = 1.0f;
		m_light0.Diffuse.b = 1.0f;
		m_light0.Ambient.r = 0.3f;
		m_light0.Ambient.g = 0.3f;
		m_light0.Ambient.b = 0.5f;
		m_light0.Specular.r = 1.0f;
		m_light0.Specular.g = 1.0f;
		m_light0.Specular.b = 1.0f;
	}
	m_light0.Direction = D3DXVECTOR3(-1, -0.1f, 0);
	g_pD3dDev->SetLight(0, &m_light0);
	//2007-02-28 dgwoo şń˝şĹ¸żˇĽ­ µđąŮŔĚ˝ş ¸®ĽÂ˝Ă Á¶¸íŔ» (ŔŻČż,ą«Čż) °ŞŔ» şŻ°ćÇŘÁŕľß Ŕç ĽÂĆĂµČ´Ů.
	//g_pD3dDev->LightEnable( 0, TRUE );
	g_pD3dDev->LightEnable(0, FALSE);

	/////////   light1   ///////////////////->Çěµĺ¶óŔĚĆ®
	ZeroMemory(&m_light1, sizeof(D3DLIGHT9));
	m_light1.Type = D3DLIGHT_SPOT;
	m_light1.Diffuse.r = 0.9f;
	m_light1.Diffuse.g = 0.9f;
	m_light1.Diffuse.b = 0.9f;
	m_light1.Ambient.r = 0.0f;
	m_light1.Ambient.g = 0.0f;
	m_light1.Ambient.b = 0.0f;
	m_light1.Specular.r = 1.0f;
	m_light1.Specular.g = 1.0f;
	m_light1.Specular.b = 1.0f;
	m_light1.Position = D3DXVECTOR3(0, 0, 0);
	m_light1.Direction = D3DXVECTOR3(0, 1, 0);
	m_light1.Theta = 0.5f;
	m_light1.Phi = 1.0f;
	m_light1.Falloff = 1.0f;
	m_light1.Attenuation0 = 1.0f;
	m_light1.Range = 0.0f;
	g_pD3dDev->SetLight(1, &m_light1);
	g_pD3dDev->LightEnable(1, FALSE);
	/////////   light2   ///////////////////->ŔĚĆĺĆ® ¶óŔĚĆ®
	ZeroMemory(&m_light2, sizeof(D3DLIGHT9));
	m_light2.Type = D3DLIGHT_DIRECTIONAL;
	m_light2.Diffuse.r = 1.0f;
	m_light2.Diffuse.g = 1.0f;
	m_light2.Diffuse.b = 1.0f;
	m_light2.Ambient.r = 1.0f;
	m_light2.Ambient.g = 1.0f;
	m_light2.Ambient.b = 1.0f;
	m_light2.Direction = D3DXVECTOR3(-1, 0, 0);
	/////////   light3   ///////////////////->ĽĹĆ˛ şńĂâ¶§ »çżë - Ŕ§żˇĽ­ ľĆ·ˇ·Î şńĂá´Ů.
	ZeroMemory(&m_light3, sizeof(D3DLIGHT9));
	m_light3.Type = D3DLIGHT_DIRECTIONAL;
	m_light3.Diffuse.r = 180.0f / 255.0f;
	m_light3.Diffuse.g = 180.0f / 255.0f;
	m_light3.Diffuse.b = 180.0f / 255.0f;
	m_light3.Ambient.r = 15.0f / 255.0f;
	m_light3.Ambient.g = 20.0f / 255.0f;
	m_light3.Ambient.b = 25.0f / 255.0f;
	m_light3.Direction = D3DXVECTOR3(0, -1, 0);
	g_pD3dDev->SetLight(3, &m_light3);
	g_pD3dDev->LightEnable(3, FALSE);

	g_pD3dDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CSceneData::CreateWeaponMonsterMissile(MSG_FC_MISSILE_MOVE_OK* pMsg)
/// \brief		Monster Weapon missileŔ» »ýĽşÇŃ´Ů.»çżëÇĎÁö ľĘŔ˝
/// \author		dhkwon
/// \date		2004-03-21 ~ 2004-03-21
/// \warning	CreateWeaponMonsterSecondaryżÍ ÄÚµĺ°ˇ °ĹŔÇ °°´Ů. °đ ÇĎłŞ·Î ÇŐĂÄľß ÇŃ´Ů.
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CSceneData::CreateWeaponMonsterMissile(MSG_FC_MISSILE_MOVE_OK* pMsg)
{
	//	FLOG("CSceneData::CreateMonsterMissileWeapon(MSG_FC_MISSILE_MOVE_OK* pMsg)");
	//	if(m_pWeaponData)
	//	{
	//		CMapMonsterIterator itMonster = m_mapMonsterList.find(pMsg->MonsterIndex);
	//		if(itMonster != m_mapMonsterList.end())
	//		{
	//			if(pMsg->ItemNum == 7900005)
	//			{
	//				CWeaponMonsterData * pWeapon = new CWeaponMonsterData(pMsg,itMonster->second);
	//				m_pWeaponData->AddChild(pWeapon);
	//			}
	//		}
	//	}
}
/*
///////////////////////////////////////////////////////////////////////////////
/// \fn			void CSceneData::CreateWeaponMonsterSecondary(MSG_FC_BATTLE_ATTACK_RESULT_SECONDARY* pMsg)
/// \brief		¸ó˝şĹÍ 2Çü ą«±â¸¦ »ýĽşÇŃ´Ů.
/// \author		dhkwon
/// \date		2004-03-21 ~ 2004-03-21
/// \warning	Áöżď°Í!
///
/// \param
/// \return
///////////////////////////////////////////////////////////////////////////////
void CSceneData::CreateWeaponMonsterSecondary(MSG_FC_BATTLE_ATTACK_RESULT_SECONDARY* pMsg)
{
//	FLOG("CSceneData::CreateWeaponMonsterSecondary(MSG_FC_BATTLE_ATTACK_RESULT_SECONDARY* pMsg)");
//	if(m_pWeaponData)
//	{
//		CMapMonsterIterator itMonster = m_mapMonsterList.find(pMsg->AttackIndex);
//		if(itMonster != m_mapMonsterList.end())
//		{
//			if(pMsg->WeaponItemNumber == 7900005)
//			{
//				CWeaponMonsterData * pWeapon = new CWeaponMonsterData(pMsg,itMonster->second);
//				m_pWeaponData->AddChild(pWeapon);
//			}
//		}
//	}
}
*/

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		dgwoo
/// \date		2007-02-09 ~ 2007-02-09
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CSceneData::AddFieldItemScanObject(CAtumNode* pUnitData, float fCheckTime, UINT uObjId, DWORD dwPartType,
										D3DXVECTOR3	vVel, D3DXVECTOR3 vUp, D3DXVECTOR3* i_pPos/*=NULL*/)
{
	if (pUnitData) {
		// 2007-04-19 by bhsohn Ľ­ÄˇľĆŔĚ ľĆŔĚĹŰ Ăß°ˇ
		//CItemData* pItemData = new CItemData(pUnitData,fCheckTime);
		CItemData* pItemData = new CItemData(pUnitData, fCheckTime, uObjId, dwPartType,
											 vVel, vUp, i_pPos);
		// end 2007-04-19 by bhsohn Ľ­ÄˇľĆŔĚ ľĆŔĚĹŰ Ăß°ˇ

		m_vecScanData.push_back(pItemData);
	}
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			void CSceneData::AddFieldItemItemSHowItem( MSG_FC_ITEM_SHOW_ITEM *pMsg )
/// \brief		ÇĘµĺżˇ ľĆŔĚĹŰ »ýĽş(ľĆ·ˇ ÇÔĽöµé ÇŐĂÄľß ÇÔ)
/// \author		dhkwon
/// \date		2004-03-22 ~ 2004-03-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CSceneData::AddFieldItemItemSHowItem(MSG_FC_ITEM_SHOW_ITEM* pMsg)
{
	FLOG("CSceneData::AddFieldItemItemSHowItem( MSG_FC_ITEM_SHOW_ITEM *pMsg )");
	//#ifdef _DEBUG
	//	char buf[128];
	//	ITEM* pItem = g_pDatabase->GetServerItemInfo(pMsg->ItemNum);
	//	wsprintf(buf,"ľĆŔĚĹŰ [%s,%d] (%d°ł)°ˇ ¶łľîÁł˝Ŕ´Ď´Ů.",pItem ? pItem->ItemName : "Á¤ş¸ľřŔ˝", pMsg->ItemNum, pMsg->Amount);
	//	g_pD3dApp->m_pChat->CreateChatChild(buf,COLOR_SKILL_USE);
	//#endif
	if (m_pItemData) {
		CItemData* pItem = FindFieldItemByFieldIndex(pMsg->ItemFieldIndex);
		if (!pItem) {
			pItem = new CItemData(pMsg);
			m_pItemData->AddChild(pItem);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CSceneData::AddFieldItemBattleDropDummyOk( CAtumNode* pUnitData, MSG_FC_BATTLE_DROP_DUMMY_OK* pMsg )
/// \brief		MSG_FC_BATTLE_DROP_DUMMY_OK ŔÎ °ćżě Item »ýĽş
/// \author		dhkwon
/// \date		2004-03-22 ~ 2004-03-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
//void CSceneData::AddFieldItemBattleDropDummyOk( CAtumNode* pUnitData, MSG_FC_BATTLE_DROP_DUMMY_OK* pMsg )
//{
//	FLOG("CSceneData::AddFieldItemBattleDropDummyOk( CAtumNode* pUnitData, MSG_FC_BATTLE_DROP_DUMMY_OK* pMsg )");
//	if(m_pItemData)
//	{
//		CItemData *pItem = FindFieldItemByFieldIndex( pMsg->ItemFieldIndex );
//		if(!pItem)
//		{
//			pItem = new CItemData(pUnitData, pMsg);
//			m_pItemData->AddChild(pItem);
//		}
//	}
//}

void CSceneData::AddFieldItemBattleDropFixerOk(CAtumNode* pTarget, CAtumNode* pAttack, MSG_FC_BATTLE_DROP_FIXER_OK* pMsg)
{
	FLOG("CSceneData::AddFieldItemBattleDropFixerOk(CAtumNode *pTarget,CAtumNode *pAttack,...");
	if (m_pItemData) {
		CItemData* pItemData = FindFieldItemByFieldIndex(pMsg->ItemFieldIndex);
		if (!pItemData) {
			pItemData = new CItemData(pTarget, (CAtumData*)pAttack, pMsg);
			m_pItemData->AddChild(pItemData);
			ITEM* pItem = g_pDatabase->GetServerItemInfo(pItemData->m_nItemNum);
			if (pMsg->TargetIndex == g_pShuttleChild->m_myShuttleInfo.ClientIndex) {// ł»°ˇ ÇČĽ­ŔÇ Ĺ¸ÄĎŔĚ¸é..
				// 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ
				// 				g_pShuttleChild->SetParamFactorDesParam(g_pShuttleChild->m_paramFactor,pItem->DestParameter1,pItem->ParameterValue1);
				// 				g_pShuttleChild->SetParamFactorDesParam(g_pShuttleChild->m_paramFactor,pItem->DestParameter2,pItem->ParameterValue2);
				// 				g_pShuttleChild->SetParamFactorDesParam(g_pShuttleChild->m_paramFactor,pItem->DestParameter3,pItem->ParameterValue3);
				// 				g_pShuttleChild->SetParamFactorDesParam(g_pShuttleChild->m_paramFactor,pItem->DestParameter4,pItem->ParameterValue4);
				int nArrParamCnt = 0;
				for (nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++) {
					g_pShuttleChild->SetParamFactorDesParam(g_pShuttleChild->m_paramFactor,
															pItem->ArrDestParameter[nArrParamCnt],
															pItem->ArrParameterValue[nArrParamCnt]);

				}
				// end 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ
			}
		}
	}
}

void CSceneData::DeleteFieldItemBattleDropFixerOk(UINT nItemFieldIndex)
{
	if (m_pItemData) {
		CItemData* pItemData = FindFieldItemByFieldIndex(nItemFieldIndex);
		if (pItemData) {
			ITEM* pItem = g_pDatabase->GetServerItemInfo(pItemData->m_nItemNum);
			// 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ
			// 			g_pShuttleChild->ReleaseParamFactorDesParam(g_pShuttleChild->m_paramFactor,pItem->DestParameter1,pItem->ParameterValue1);
			// 			g_pShuttleChild->ReleaseParamFactorDesParam(g_pShuttleChild->m_paramFactor,pItem->DestParameter2,pItem->ParameterValue2);
			// 			g_pShuttleChild->ReleaseParamFactorDesParam(g_pShuttleChild->m_paramFactor,pItem->DestParameter3,pItem->ParameterValue3);
			// 			g_pShuttleChild->ReleaseParamFactorDesParam(g_pShuttleChild->m_paramFactor,pItem->DestParameter4,pItem->ParameterValue4);
			int nArrParamCnt = 0;
			for (nArrParamCnt = 0; nArrParamCnt < SIZE_MAX_DESPARAM_COUNT_IN_ITEM; nArrParamCnt++) {
				g_pShuttleChild->ReleaseParamFactorDesParam(g_pShuttleChild->m_paramFactor,
															pItem->ArrDestParameter[nArrParamCnt],
															pItem->ArrParameterValue[nArrParamCnt]);

			}
			// end 2009-04-21 by bhsohn ľĆŔĚĹŰ DesParamĂß°ˇ
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CItemData * CSceneData::FindFieldItemByFieldIndex( UINT nFieldIndex )
/// \brief		FieldIndex·Î °Ë»ö
/// \author		dhkwon
/// \date		2004-03-22 ~ 2004-03-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CItemData* CSceneData::FindFieldItemByFieldIndex(UINT nFieldIndex)
{
	FLOG("CSceneData::FindFieldItemByFieldIndex( UINT nFieldIndex )");
	if (!m_pItemData) 
		return NULL;

	CItemData* pItem = (CItemData*)m_pItemData->m_pChild;
	while (pItem) {
		if (pItem->m_nItemIndex == nFieldIndex) {
			return pItem;
		}
		pItem = (CItemData*)pItem->m_pNext;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CItemData* CSceneData::FindFieldItemBy2DDistance( D3DXVECTOR2 vPos, float fDist )
/// \brief		2D»ó °Ĺ¸®·Î °Ë»ö(Ĺ¬¸Ż)
/// \author		dhkwon
/// \date		2004-03-22 ~ 2004-03-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CItemData* CSceneData::FindFieldItemBy2DDistance(D3DXVECTOR2 vPos, float fDist)
{
	FLOG("CSceneData::FindFieldItemBy2DDistance( D3DXVECTOR2 vPos, float fDist )");
	//if (!m_pItemData) return NULL;
	CItemData* pItem = (CItemData*)m_pItemData->m_pChild;
	while (pItem) {
		D3DXVECTOR2 vItemPos = D3DXVECTOR2(pItem->m_nObjScreenX, pItem->m_nObjScreenY);
		float fTemp = D3DXVec2Length(&(vItemPos - vPos));
		if (fTemp < fDist) {
			return pItem;
		}
		pItem = (CItemData*)pItem->m_pNext;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CSceneData::SendItemGetItemAll()
/// \brief		ÁÖşŻ ľĆŔĚĹŰ ˝Ŕµć ¸Ţ˝ĂÁö
/// \author		dhkwon
/// \date		2004-06-18 ~ 2004-06-18
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CSceneData::SendItemGetItemAll()
{
	// 2006-07-18 by ispark, ˝Ă°Ł µô·ąŔĚ. Ĺ°¸¦ ´©¸Ą ČÄ 1ĂĘµżľČŔş Ĺ° ŔÔ·Â Á¦ÇŃ
	if (m_fGetItemAllDelay < GET_ITEM_IN_TIME) {
		return;
	}

	m_fGetItemAllDelay = 0.0f;

	// ÁÖşŻ ľĆŔĚĹŰ ĂŃ Á¶»ç
	CItemData* pItem = (CItemData*)m_pItemData->m_pChild;
	vector<CItemData*> vecItemData;
	while (pItem) {
		vecItemData.push_back(pItem);
		pItem = (CItemData*)pItem->m_pNext;
	}
	BOOL bSendGetItem = FALSE;

	// ·»´ý 2°ł °áÁ¤
	for (int i = 0; i < GET_ITEM_NUMBER_IN_TICK; i++) {
		if (0 >= vecItemData.size()) {
			// 2008-09-02 by dgwoo ÁÖşŻżˇ ľĆŔĚĹŰŔĚ ľřŔ»°ćżě Ăł¸®.
			bSendGetItem = TRUE;
			break;
		}

		int nIndex = rand() % vecItemData.size();
		pItem = vecItemData[nIndex];

		if (pItem->m_dwPartType == _ITEMFIELD && pItem->m_bIsRender) {
			// 2009. 11. 3 by jskim Äł˝¬(ŔÎşĄ/Ă˘°í Č®Ŕĺ) ľĆŔĚĹŰ Ăß°ˇ ±¸Çö
			//if(g_pStoreData->GetTotalUseInven() < CAtumSJ::GetMaxInventorySize((BOOL)g_pD3dApp->GetPrimiumCardInfo()->nCardItemNum1) - 1, pMainInfo->GetAddedPermanentInventoryCount())
			// 2009. 12. 17 by jskim Äł˝¬(ŔÎşĄ/Ă˘°í Č®Ŕĺ) ŔçĽöÁ¤
			// 			CHARACTER* pMainInfo = g_pD3dApp->GetMFSMyShuttleInfo();
			// 			if(g_pStoreData->GetTotalUseInven() < CAtumSJ::GetMaxInventorySize((BOOL)g_pD3dApp->GetPrimiumCardInfo()->nCardItemNum1, pMainInfo->GetAddedPermanentInventoryCount()) - 1)
			if (g_pStoreData->GetTotalUseInven() < CAtumSJ::GetMaxInventorySize((BOOL)g_pD3dApp->GetPrimiumCardInfo()->nCardItemNum1, g_pShuttleChild->m_myShuttleInfo.GetAddedPermanentInventoryCount()) - 1)

				//end 2009. 12. 17 by jskim Äł˝¬(ŔÎşĄ/Ă˘°í Č®Ŕĺ) ŔçĽöÁ¤
				//end 2009. 11. 3 by jskim Äł˝¬(ŔÎşĄ/Ă˘°í Č®Ŕĺ) ľĆŔĚĹŰ Ăß°ˇ ±¸Çö
			{// ŔÎşĄĹä¸®żˇ ż©ŔŻ°ˇ ŔÖŔ»°ćżě´Â Ľ­ąö·Î ąŮ·Î żäĂ».
				MSG_FC_ITEM_GET_ITEM sMsg;
				memset(&sMsg, 0x00, sizeof(sMsg));
				sMsg.ClientIndex = g_pShuttleChild->m_myShuttleInfo.ClientIndex;
				sMsg.ItemFieldIndex = pItem->m_nItemIndex;
				g_pFieldWinSocket->SendMsg(T_FC_ITEM_GET_ITEM, (char*)&sMsg, sizeof(sMsg));
				bSendGetItem = TRUE;
			}
			else if (IS_COUNTABLE_ITEM(pItem->m_byKind)) {// Ä«żîĹÍşí ľĆŔĚĹŰŔĎ°ćżě ÇöŔç ŔÎşĄżˇ ŔÖ´ÂÁö ĂĽĹ©ČÄ żäĂ».
				CItemInfo* pItemInfo = g_pStoreData->FindItemInInventoryByItemNum(pItem->m_nItemNum);
				if (pItemInfo) {
					MSG_FC_ITEM_GET_ITEM sMsg;
					memset(&sMsg, 0x00, sizeof(sMsg));
					sMsg.ClientIndex = g_pShuttleChild->m_myShuttleInfo.ClientIndex;
					sMsg.ItemFieldIndex = pItem->m_nItemIndex;
					g_pFieldWinSocket->SendMsg(T_FC_ITEM_GET_ITEM, (char*)&sMsg, sizeof(sMsg));
					bSendGetItem = TRUE;
				}
			}
			//			else
			//			{
			//				g_pD3dApp->m_pChat->CreateChatChild(STRERR_ERROR_0022,COLOR_ERROR);
			//			}

		}

		vecItemData.erase(find(vecItemData.begin(), vecItemData.end(), pItem));
		//		if(!pItem)
		//			break;
		//		if(pItem->m_dwPartType == _ITEMFIELD && pItem->m_bIsRender)
		//		{
		//			MSG_FC_ITEM_GET_ITEM sMsg;
		//			memset(&sMsg,0x00,sizeof(sMsg));
		//			sMsg.ClientIndex = g_pShuttleChild->m_myShuttleInfo.ClientIndex;
		//			sMsg.ItemFieldIndex = pItem->m_nItemIndex;
		//			g_pFieldWinSocket->SendMsg(T_FC_ITEM_GET_ITEM, (char*)&sMsg, sizeof(sMsg) );
		//		}
		//		pItem = (CItemData *)pItem->m_pNext;
	}
	// 2008-10-14 by dgwoo 'q'ż¬Ĺ¸˝Ă ŔÎşĄŔĚ ˛ËÂ÷Áö ľĘľŇ´ÂµĄ ¸Ţ˝ĂÁö°ˇ ¶ß´Â ąö±×.
	//if(!bSendGetItem)
	if (!bSendGetItem && pItem && pItem->m_bIsRender)
		g_pD3dApp->m_pChat->CreateChatChild(STRERR_ERROR_0022, COLOR_ERROR);

	vecItemData.clear();
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			void CSceneData::DeleteItemInFieldItem( int x, int y )
/// \brief		°ü¸®ŔÚżˇ ŔÇÇŃ ÇĘµĺ Ŕ§ŔÇ ľĆŔĚĹŰ »čÁ¦
/// \author		dhkwon
/// \date		2004-03-22 ~ 2004-03-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CSceneData::SendItemDeleteItemAdmin(int x, int y)
{
	FLOG("SendItemDeleteItemAdmin( int x, int y )");
	CItemData* pItem = FindFieldItemBy2DDistance(D3DXVECTOR2(x, y), GET_INFO_DISTANCE_OF_2D);
	if (pItem) {
		MSG_FC_ITEM_DELETE_ITEM_ADMIN sMsg;
		memset(&sMsg, 0x00, sizeof(sMsg));
		sMsg.ItemFieldIndex = pItem->m_nItemIndex;
		sMsg.DropPosition = pItem->m_vPos;
		g_pD3dApp->m_pFieldWinSocket->SendMsg(T_FC_ITEM_DELETE_ITEM_ADMIN, (char*)&sMsg, sizeof(sMsg));
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CSceneData::DeleteFieldItemOfUnitData( CUnitData* pUnitData )
/// \brief		ŔŻ´Öµéżˇ ż¬°áµČ DUMMY, FIXER, WeaponŔ» »čÁ¦ÇŃ´Ů.
/// \author		dhkwon
/// \date		2004-03-22 ~ 2004-03-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CSceneData::DeleteFieldItemOfUnitData(CUnitData* pUnitData)
{
	FLOG("CSceneData::DeleteFieldItemOfUnitData( CUnitData* pUnitData )");

	if (!m_pItemData)
		return;

	CItemData* pItem = (CItemData*)m_pItemData->m_pChild;
	while (pItem) {
		if (pItem->m_pParent == pUnitData) {
			pItem->m_bUsing = FALSE;
		}
		// 2006-01-17 by ispark
		else if (pItem->m_pTarget == pUnitData) {
			pItem->m_bUsing = FALSE;
		}
		pItem = (CItemData*)pItem->m_pNext;
	}

}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CSceneData::DeleteFieldItemOfFieldIndex( UINT nFieldIndex )
/// \brief		nFieldIndex¸¦ Áöżî´Ů.
/// \author		dhkwon
/// \date		2004-03-22 ~ 2004-03-22
/// \warning	FieldSocketBattleAttackHideItem
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CSceneData::DeleteFieldItemOfFieldIndex(UINT nFieldIndex)
{
	FLOG("CSceneData::DeleteFieldItemOfFieldIndex( UINT nFieldIndex )");
	CItemData* pItem = FindFieldItemByFieldIndex(nFieldIndex);
	if (pItem) {
		pItem->m_bUsing = FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CSceneData::ExplodingFieldItemOfFieldIndex( UINT nFieldIndex )
/// \brief		nFieldIndexfmf ExplodingŔ¸·Î ¸¸µç´Ů.
/// \author		dhkwon
/// \date		2004-03-22 ~ 2004-03-22
/// \warning	FieldSocketBattleAttackExplodeItem
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CSceneData::ExplodingFieldItemOfFieldIndex(UINT nFieldIndex)
{
	FLOG("CSceneData::ExplodingFieldItemOfFieldIndex( UINT nFieldIndex )");
	CItemData* pItem = FindFieldItemByFieldIndex(nFieldIndex);
	if (pItem) {
		pItem->m_dwState = _EXPLODING;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CUnitData* CSceneData::FindUnitDataByClientIndex( ClientIndex_t nIndex )
/// \brief		ClientIndex¸¦ ŔĚżëÇŘ Shuttle,Enemy,MonsterÁßżˇĽ­ UnitData¸¦ ĂŁ´Â´Ů.
/// \author		dhkwon
/// \date		2004-03-23 ~ 2004-03-23
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CUnitData* CSceneData::FindUnitDataByClientIndex(ClientIndex_t nIndex)
{
	FLOG("CSceneData::FindUnitDataByClientIndex( ClientIndex_t nIndex )");
	if (g_pD3dApp->m_pShuttleChild == NULL) {
		return NULL;
	}
	if (nIndex == g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.ClientIndex) {
		return g_pD3dApp->m_pShuttleChild;
	}
	else if (IS_CHARACTER_CLIENT_INDEX(nIndex))//pMsg->AttackIndex < 10000)
	{
		CMapEnemyIterator itEnemy = m_mapEnemyList.find(nIndex);
		if (itEnemy != m_mapEnemyList.end()) {
			return itEnemy->second;
		}
	}
	else if (IS_MONSTER_CLIENT_INDEX(nIndex)) {
		CMapMonsterIterator itMonster = m_mapMonsterList.find(nIndex);
		if (itMonster != m_mapMonsterList.end()) {
			return itMonster->second;
		}
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int CSceneData::FindClientIndexByUnitData( CUnitData* pUnit )
/// \brief		UnitData¸¦ ŔĚżëÇŘ ClientIndex¸¦ ĂŁ´Â´Ů.
/// \author		dhkwon
/// \date		2004-07-22 ~ 2004-07-22
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int CSceneData::FindClientIndexByUnitData(CUnitData* pUnit)
{
	if (pUnit->m_dwPartType == _SHUTTLE) {
		return g_pShuttleChild->m_myShuttleInfo.ClientIndex;
	}
	else if (pUnit->m_dwPartType == _ENEMY || pUnit->m_dwPartType == _ADMIN) {
		return ((CEnemyData*)pUnit)->m_infoCharacter.CharacterInfo.ClientIndex;
	}
	else if (pUnit->m_dwPartType == _MONSTER) {
		return ((CMonsterData*)pUnit)->m_info.MonsterIndex;
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			CItemData * CSceneData::FindFieldItemByPartTypeAndParent( DWORD dwPartType, CAtumNode* pParent )
/// \brief		Field itemÁßżˇ UnitDatażˇ şŮŔş ItemÁßżˇ dwPartyType(_DUMMY,_FIXER)°ú °°Ŕş °Í ÇĎłŞ¸¦ ¸®ĹĎÇŃ´Ů.
/// \author		dhkwon
/// \date		2004-03-23 ~ 2004-03-23
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CItemData* CSceneData::FindFieldItemByPartTypeAndParent(DWORD dwPartType, CAtumNode* pParent)
{
	FLOG("CSceneData::FindFieldItemByPartTypeAndParent( DWORD dwPartType, CAtumNode* pParent )");
	CItemData* pItem = (CItemData*)m_pItemData->m_pChild;
	while (pItem) {
		if (pItem->m_dwPartType == dwPartType && pItem->m_pParent == pParent) {
			return pItem;
		}
		pItem = (CItemData*)pItem->m_pNext;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CItemData * CSceneData::FindFieldItemByParent( CAtumNode* pParent )
/// \brief		
/// \author		dhkwon
/// \date		2004-03-23 ~ 2004-03-23
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CItemData* CSceneData::FindFieldItemByParent(CAtumNode* pParent)
{
	FLOG("CSceneData::FindFieldItemByParent( CAtumNode* pParent )");
	CItemData* pItem = (CItemData*)m_pItemData->m_pChild;
	while (pItem) {
		if (pItem->m_pParent == pParent) {
			return pItem;
		}
		pItem = (CItemData*)pItem->m_pNext;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			int	CSceneData::GetDummyCountOfUnit(CAtumData* pUnit)
/// \brief		ŔŻ´ÖŔÇ ´őąĚ °ąĽö¸¦ ĆÄľÇÇŃ´Ů.
/// \author		dhkwon
/// \date		2004-05-31 ~ 2004-05-31
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
int	CSceneData::GetDummyCountOfUnit(CAtumData* pUnit)
{
	int nCount = 0;
	CItemData* pItem = (CItemData*)m_pItemData->m_pChild;
	while (pItem) {
		if (pItem->m_pParent == pUnit && pItem->m_dwPartType == _DUMMY) {
			nCount++;
		}
		pItem = (CItemData*)pItem->m_pNext;
	}
	return nCount;
}

CAppEffectData* CSceneData::FindEffect(int nType)
{
	CAppEffectData* pEffect = (CAppEffectData*)g_pD3dApp->m_pEffectList->m_pChild;
	while (pEffect) {
		if (pEffect->m_nType == nType) {
			break;
		}
		pEffect = (CAppEffectData*)pEffect->m_pNext;
	}
	return pEffect;

}

CAppEffectData* CSceneData::FindEffect(int nType, CUnitData* pParent)
{
	CAppEffectData* pEffect = (CAppEffectData*)g_pD3dApp->m_pEffectList->m_pChild;
	while (pEffect) {
		if (pEffect->m_nType == nType && pEffect->m_pParent == pParent) {
			break;
		}
		pEffect = (CAppEffectData*)pEffect->m_pNext;
	}
	return pEffect;

}

void CSceneData::DeleteEffect(CAtumNode* pParent)
{
	CAppEffectData* pEffect = (CAppEffectData*)g_pD3dApp->m_pEffectList->m_pChild;
	while (pEffect) {
		if (pEffect->m_pParent == pParent) {
			pEffect->m_bUsing = FALSE;
		}
		pEffect = (CAppEffectData*)pEffect->m_pNext;
	}
}

CCinema* CSceneData::LoadCinemaData(char* szFileName, int nFileName)
{
	// 2006-03-06 by ispark, µđĆúĆ® ĆĐĹĎ Ăß°ˇ
	/// »ő·Îżî ˝Ăł×¸¶ Ĺ¬·ˇ˝ş »ýĽş
	CCinema* pCinemaData = new CCinema;

	if (nFileName != 0) {
		/// m_pCinemaData·Î şÎĹÍ szFileNameżˇ ´ëÇŃ µĄŔĚĹÍ¸¦ °ˇÁ®żČ
		DataHeader* pDataHeader = m_pCinemaData->Find(szFileName);
		if (pDataHeader == NULL) {
			SAFE_DELETE(pCinemaData);
			return NULL;
		}
		/// »ő·Î ¸¸µç ˝Ăł×¸¶ Ĺ¬·ˇ˝şżˇ DataHeaderżˇ ´ă±ä µĄŔĚĹÍ·Î ĆĐĹĎ »ýĽş
		pCinemaData->InitData(pDataHeader->m_pData);
	}
	else {
		pCinemaData->InitDefaultData();
	}
	/// ĆĐĹĎŔĚ Á¤ŔÇµČ ˝Ăł×¸¶ Ĺ¬·ˇ˝ş ¸®ĹĎ
	return pCinemaData;
}

void CSceneData::LoadCinemaFile()
{
	SAFE_DELETE(m_pCinemaData);
	m_pCinemaData = new CGameData;
	char strPath[256];
	wsprintf(strPath, ".\\Res-Tex\\cinema.tex");
	if (strlen(strPath) > 0) {
		m_pCinemaData->SetFile(strPath, FALSE, NULL, 0);
	}
}

void CSceneData::DeleteCinemaFile()
{
	// ˝Ăł×¸¶ µĄŔĚĹÍ ŔüşÎ¸¦ Áöżî´Ů.
	SAFE_DELETE(m_pCinemaData);
}

//void CSceneData::SetExceptAllUnitTarget()
//{
//	CMapEnemyIterator itEnemy = m_mapEnemyList.begin();
//	while(itEnemy != m_mapEnemyList.end())
//	{
//		itEnemy->second->m_nEnemyTypeSecondary = ENEMYDATA_ENEMYLIST;
//		itEnemy++;
//	}
//	CMapMonsterIterator itMonster = m_mapMonsterList.begin();
//	while(itMonster != m_mapMonsterList.end())
//	{
//		itMonster->second->m_nMonsterTypeSecondary = MONSTERDATA_MONSTERLIST;
//		itMonster++;
//	}
//
//}

void CSceneData::SetPKSettingGuildWar(int nPeerGuildUniqueNumber, BOOL bPK)
{
	CMapEnemyIterator itEnemy = m_mapEnemyList.begin();
	while (itEnemy != m_mapEnemyList.end()) {
		if (itEnemy->second->m_infoCharacter.CharacterInfo.GuildUniqueNumber == nPeerGuildUniqueNumber) {
			//			itEnemy->second->m_bAttackEnemy = bPK;
			itEnemy->second->SetPkState(PK_GVG, bPK);
		}
		itEnemy++;
	}

}

CEnemyData* CSceneData::GetEnemyCharaterID(D3DXVECTOR2 vPos1)
{
	float fLength = 50.0f;
	map<INT, CEnemyData*>::iterator itEnemy = m_mapEnemyList.begin();
	CEnemyData* pEnemy = NULL;
	while (itEnemy != m_mapEnemyList.end()) {
		if (itEnemy->second->m_nObjScreenW > 0) {
			D3DXVECTOR2 vPos2 = D3DXVECTOR2(itEnemy->second->m_nObjScreenX, itEnemy->second->m_nObjScreenY);
			float fLengthTemp = D3DXVec2Length(&(vPos1 - vPos2));
			if (fLengthTemp < fLength) {
				fLength = fLengthTemp;
				pEnemy = itEnemy->second;
			}
		}
		itEnemy++;
	}

	return pEnemy;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CMonsterData * CSceneData::GetMonsterInfo2D(D3DXVECTOR2 vPos1)
/// \brief		ÁÖŔ§żˇ ŔÖ´Â ¸ó˝şĹÍŔÇ Á¤ş¸Áß vPos1żˇ °Şżˇ ÇŘ´çÇĎ´Â ¸ó˝şĹÍ Á¤ş¸¸¦ ąÝČŻ.
/// \author		dgwoo
/// \date		2007-09-05 ~ 2007-09-05
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CMonsterData* CSceneData::GetMonsterInfo2D(D3DXVECTOR2 vPos1)
{
	float fLength = 50.0f;
	map<INT, CMonsterData*>::iterator itMonster = m_mapMonsterList.begin();
	CMonsterData* pMonster = NULL;
	while (itMonster != m_mapMonsterList.end()) {// °Ë»ö˝Ă Ĺ¬¸ŻŔ» ŔÎ˝ÄÇĎ´Â ¸ó˝şĹÍŔÇ °ćżě¸¸ ¸®ĹĎ.
		if (itMonster->second->m_nObjScreenW > 0) {
			D3DXVECTOR2 vPos2 = D3DXVECTOR2(itMonster->second->m_nObjScreenX, itMonster->second->m_nObjScreenY);
			float fLengthTemp = D3DXVec2Length(&(vPos1 - vPos2));
			if (fLengthTemp < fLength) {
				fLength = fLengthTemp;
				pMonster = itMonster->second;
			}
		}
		itMonster++;
	}

	return pMonster;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CEnemyData * CSceneData::GetPickEnemy(D3DXVECTOR2 vPos)
/// \brief		Äł¸ŻĹÍżë Picking
/// \author		ispark
/// \date		2006-08-01 ~ 2006-08-01
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CEnemyData* CSceneData::GetPickEnemy(D3DXVECTOR2 vPos)
{
	float fLength = 70.0f;
	CEnemyData* pEnemyTemp = NULL;
	CEnemyData* pEnemy = NULL;
	CVecEnemyList::iterator itEnemy = m_vecEnemyRenderList.begin();
	int x = 0, y = 0, w = 0;
	while (itEnemy != m_vecEnemyRenderList.end()) {
		pEnemy = (*itEnemy);

		D3DXVECTOR3 vPosTemp = pEnemy->m_vPos;
		int nPilotNum = pEnemy->GetPilotNum();
		float fChaLeng = (GetCharacterHeight(nPilotNum) + 1.0f) / 2.0f;
		vPosTemp.y += fChaLeng;
		g_pD3dApp->CalcObjectSourceScreenCoords(vPosTemp, g_pD3dApp->GetBackBufferDesc().Width, g_pD3dApp->GetBackBufferDesc().Height,
												x, y, w);

		if (w > 0) {
			D3DXVECTOR2 vPos2 = D3DXVECTOR2(x, y);
			float fLengthTemp = D3DXVec2Length(&(vPos - vPos2));
			if (fLengthTemp < fLength) {
				fLength = fLengthTemp;
				pEnemyTemp = pEnemy;
			}
		}

		itEnemy++;
	}

	return pEnemyTemp;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CObjectChild * CSceneData::FindWarpGateByPosition( D3DXVECTOR3 vPos )
/// \brief		°ˇŔĺ °ˇ±îżî Big object¸¦ ¸®ĹĎÇŃ´Ů. 
/// \author		dhkwon
/// \date		2004-11-08 ~ 2004-11-08
///////////////////////////////////////////////////////////////////////////////
/// \fn			CSceneData::FindWarpGateByPosition( D3DXVECTOR3 vPos , float fCloseDistance )
/// \brief		fCloseDistance ş¸´Ů ŔŰŔş °ˇŔĺ °ˇ±îżî żöÇÁ°ÔŔĚĆ®¸¦ ¸®ĹĎÇŃ´Ů.
/// \author		jschoi
/// \date		2005-03-08 ~ 2005-03-08
/// \warning	
///
/// \param		µÎąřÂ° ŔÎŔÚ µđĆúĆ® °Ş : RANGE_OF_VISION
/// \return		
///////////////////////////////////////////////////////////////////////////////
CObjectChild* CSceneData::FindWarpGateByPosition(D3DXVECTOR3 vPos, float fCloseDistance)
{
	CObjectChild* pWarpGate = NULL;
	CObjectChild* pObj = (CObjectChild*)g_pGround->m_pBigObject->m_pChild;
	while (pObj != NULL) {
		if (pObj->m_pObjectInfo &&
			(pObj->m_pObjectInfo->Code == WARP_GATE_OBJECT_NUM || pObj->m_pObjectInfo->Code == WARP_GATE_OBJECT_NUM_2))  // 2008-06-24 by dgwoo żöÇÁ °ÔŔĚĆ® Ăß°ˇ.
		{
			float fDistance = D3DXVec3Length(&(pObj->m_vPos - vPos));
			if (fDistance < fCloseDistance) {
				fCloseDistance = fDistance;
				pWarpGate = pObj;
			}
		}
		pObj = (CObjectChild*)pObj->m_pNext;
	}
	return pWarpGate;
}

void CSceneData::ChangeGammaOption(int nGamma)
{
	// żÉĽÇ ĽłÁ¤(°¨¸¶)
	// 2004-11-15 by ydkim

	float fGammaPlus = nGamma * OPTION_GAMMA_PLUS;

	//m_light0.Diffuse.r += fGammaPlus;
	//m_light0.Diffuse.g += fGammaPlus;
	//m_light0.Diffuse.b += fGammaPlus;

	m_light0.Ambient.r += fGammaPlus;
	m_light0.Ambient.g += fGammaPlus;
	m_light0.Ambient.b += fGammaPlus;

	//if((m_light0.Diffuse.r) > 1.0f) m_light0.Diffuse.r = 1.0f;
	//if((m_light0.Diffuse.g) > 1.0f) m_light0.Diffuse.g = 1.0f;
	//if((m_light0.Diffuse.b) > 1.0f) m_light0.Diffuse.b = 1.0f;

	if ((m_light0.Ambient.r) > 1.0f) m_light0.Ambient.r = 1.0f;
	if ((m_light0.Ambient.g) > 1.0f) m_light0.Ambient.g = 1.0f;
	if ((m_light0.Ambient.b) > 1.0f) m_light0.Ambient.b = 1.0f;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CSceneData::FindEventObjectByIndex(UINT nEventObjectIndex)
/// \brief		ŔĚşĄĆ® żŔşęÁ§Ć® ŔÎµ¦˝ş·Î °ˇŔĺ °ˇ±îżî ÇŘ´ç ŔĚşĄĆ® żŔşęÁ§Ć®¸¦ ĂŁ´Â´Ů.
/// \author		jschoi
/// \date		2004-11-26 ~ 2004-11-26
/// \warning	°°Ŕş żŔşęÁ§Ć® ŔÎµ¦˝ş°ˇ ŔÖ´Â°ćżě °ˇŔĺ °ˇ±îżî ŔĚşĄĆ® żŔşęÁ§Ć®¸¦ ¸®ĹĎÇŃ´Ů.
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CObjectChild* CSceneData::FindEventObjectByIndex(D3DXVECTOR3 vPos, UINT nEventObjectIndex)
{
	float fDistance = 10000.0f;
	CObjectChild* pResultObjEvent = NULL;
	CObjectChild* pObjEvent = (CObjectChild*)g_pGround->m_pObjectEvent->m_pChild;
	while (pObjEvent) {
		if (pObjEvent->m_sEventIndexFrom == nEventObjectIndex) {
			float fTempDistance = D3DXVec3Length(&(pObjEvent->m_vPos - vPos));
			if (fDistance > fTempDistance) {
				fDistance = fTempDistance;
				pResultObjEvent = pObjEvent;
			}
		}
		pObjEvent = (CObjectChild*)pObjEvent->m_pNext;
	}
	return pResultObjEvent;
}

CObjectChild* CSceneData::FindEventObjectByWarp()
{
	CObjectChild* pResultObjEvent = NULL;
	CObjectChild* pObjEvent = (CObjectChild*)g_pGround->m_pObjectEvent->m_pChild;
	while (pObjEvent) {
		if (EVENT_TYPE_WARP == pObjEvent->m_bEventType) {
			pResultObjEvent = pObjEvent;
		}
		pObjEvent = (CObjectChild*)pObjEvent->m_pNext;
	}
	return pResultObjEvent;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CSceneData::FindEventObjectByIndex(UINT nEventObjectIndex)
/// \brief		ŔÎµ¦˝ş·Î żŔşęÁ§Ć®¸¦ ĂŁ´Â´Ů
/// \author		ydkim
/// \date		2005-07-20
/// \warning
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CObjectChild* CSceneData::FindObjectByIndex(UINT nEventObjectIndex)
{
	if (!g_pGround)
		return nullptr;

	CObjectChild* pResultObjEvent = NULL;
	CObjectChild* pObjEvent = (CObjectChild*)g_pGround->m_pObjectEvent->m_pChild;
	while (pObjEvent) {
		if (pObjEvent->m_sEventIndexFrom == nEventObjectIndex) {
			pResultObjEvent = pObjEvent;
		}
		pObjEvent = (CObjectChild*)pObjEvent->m_pNext;
	}
	return pResultObjEvent;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CSceneData::FindRandomObjectByEventType(BYTE bObjectType, BOOL bRand)
/// \brief		ŔĚşĄĆ® Ĺ¸ŔÔŔ¸·Î ĂŁ±â, ¶ÇÇŃ °°Ŕş Ĺ¸ŔÔŔĚ ż©·Ż°Ô ŔÖŔ» °ćżě ·Ł´ýŔ¸·Î »ĚľĆÁŘ´Ů.
/// \author		ispark
/// \date		2006-05-19 ~ 2006-05-19
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CObjectChild* CSceneData::FindRandomObjectByEventType(BYTE bObjectType, BOOL bRand)
{
	vector<CObjectChild*> vtObjectInfoPtr;
	CObjectChild* pResultObjEvent = NULL;
	CObjectChild* pObjEvent = (CObjectChild*)g_pGround->m_pObjectEvent->m_pChild;
	while (pObjEvent) {
		if (pObjEvent->m_bEventType == bObjectType) {
			if (bRand) {
				vtObjectInfoPtr.push_back(pObjEvent);
			}
			else {
				return pObjEvent;
			}
		}
		pObjEvent = (CObjectChild*)pObjEvent->m_pNext;
	}

	if (vtObjectInfoPtr.empty()) {
		return NULL;
	}

	///////////////////////////////////////////////////////////////////////////////
	// °°Ŕş EventIndex°ˇ ż©·Ż°ł ŔĎ¶§ ·Ł´ýŔ¸·Î ĽłÁ¤
	int i = rand() % vtObjectInfoPtr.size();
	CObjectChild* pTempObjEvent = vtObjectInfoPtr[i];
	vtObjectInfoPtr.clear();
	return pTempObjEvent;
}
///////////////////////////////////////////////////////////////////////////////
/// \fn			CSceneData::FindEventObjectByTypeAndPosition(BYTE bObjectType, D3DXVECTOR3 vPos)
/// \brief		ÇŘ´ç Ŕ§Äˇżˇ ÇŘ´çÇĎ´Â ŔĚşĄĆ® żŔşęÁ§Ć®¸¦ ĂŁ´Â´Ů.
/// \author		jschoi
/// \date		2004-11-26 ~ 2004-11-26
/// \warning	żŔşęÁ§Ć® Ĺ¸ŔÔŔĚ ŔĎÄˇÇĎ°í °Ĺ¸®°ˇ 10 ŔĚł»ŔĚľîľß ĂŁŔş°É·Î ÇŃ´Ů.
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CObjectChild* CSceneData::FindEventObjectByTypeAndPosition(BYTE bObjectType, D3DXVECTOR3 vPos, float fDist)
{
	CObjectChild* pObjEvent = (CObjectChild*)g_pGround->m_pObjectEvent->m_pChild;
	while (pObjEvent) {
		float dist = D3DXVec3Length(&(pObjEvent->m_vPos - vPos));
		if (pObjEvent->m_bEventType == bObjectType && dist < fDist) {
			return pObjEvent;
		}
		else {
			pObjEvent = (CObjectChild*)pObjEvent->m_pNext;
		}
	}
	return NULL;
}

// 2010. 05. 27 by jskim ˝Ăł×¸¶ Ŕűżë Ä«¸Ţ¶ó ±¸Çö
CObjectChild* CSceneData::FindEventObjectByTypeAndPositionIndex(BYTE bObjectType, short sEventIndex)
{
	CObjectChild* pObjEvent = (CObjectChild*)g_pGround->m_pObjectEvent->m_pChild;
	while (pObjEvent) {
		if (pObjEvent->m_bEventType == bObjectType && pObjEvent->m_sEventIndexTo == sEventIndex) {
			return pObjEvent;
		}
		else {
			pObjEvent = (CObjectChild*)pObjEvent->m_pNext;
		}
	}
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CSceneData::FindMapObjectByCodeAndPosition(int nCode, D3DXVECTOR3 vPos)
/// \brief		ÇŘ´çŔ§Äˇżˇ ÇŘ´ç ÄÚµĺŔÇ ¸Ę żŔşęÁ§Ć®¸¦ ĂŁ´Â´Ů.
/// \author		jschoi
/// \date		2004-11-27 ~ 2004-11-27
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
CObjectChild* CSceneData::FindMapObjectByCodeAndPosition(int nCode, D3DXVECTOR3 vPos)
{
	if (g_pGround == NULL)
	{
		FDBG("[Error] Find map object %d impossible due ground is not loaded!", nCode);
		return NULL;
	}

	int x = (int)((g_pGround->m_projectInfo.sXSize * TILE_SIZE) / MAP_BLOCK_SIZE);
	int z = (int)((g_pGround->m_projectInfo.sYSize * TILE_SIZE) / MAP_BLOCK_SIZE);
	CObjectChild* pMapObj = NULL;
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < z; j++) {
			pMapObj = (CObjectChild*)g_pGround->m_ppObjectList[i][j].m_pChild;
			while (pMapObj) {
				if (pMapObj->m_nCode == nCode && D3DXVec3Length(&(pMapObj->m_vPos - vPos)) < 10.0f) {
					return pMapObj;
				}
				else {
					pMapObj = (CObjectChild*)pMapObj->m_pNext;
				}
			}
		}
	}

	pMapObj = (CObjectChild*)m_pGround->m_pBigObject->m_pChild;
	while (pMapObj) {
		if (pMapObj->m_nCode == nCode && D3DXVec3Length(&(pMapObj->m_vPos - vPos)) < 10.0f) {
			return pMapObj;
		}
		else {
			pMapObj = (CObjectChild*)pMapObj->m_pNext;
		}
	}

	pMapObj = (CObjectChild*)m_pGround->m_pWaterObject->m_pChild;
	while (pMapObj) {
		if (pMapObj->m_nCode == nCode && D3DXVec3Length(&(pMapObj->m_vPos - vPos)) < 10.0f) {
			return pMapObj;
		}
		else {
			pMapObj = (CObjectChild*)pMapObj->m_pNext;
		}
	}

	pMapObj = (CObjectChild*)m_pGround->m_pObjectMonster->m_pChild;
	while (pMapObj) {
		if (pMapObj->m_nCode == nCode && D3DXVec3Length(&(pMapObj->m_vPos - vPos)) < 10.0f) {
			return pMapObj;
		}
		else {
			pMapObj = (CObjectChild*)pMapObj->m_pNext;
		}
	}


	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			ChangeAllEnemyCityWarTeamType(BYTE byCityWarTeamType)
/// \brief		¸đµç EnemyŔÇ µµ˝ĂÁˇ·ÉŔü ĆŔŔ» ĽÂĆĂÇŃ´Ů.(ľĆŔĚµđ »öŔ¸·Î ÇÇľĆ˝Äş°Ŕ» Ŕ§ÇŘ)
/// \author		jschoi
/// \date		2005-05-16 ~ 2005-05-16
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CSceneData::ChangeAllEnemyCityWarTeamType(BYTE byCityWarTeamType)
{
	map<INT, CEnemyData*>::iterator itEnemy = m_mapEnemyList.begin();
	while (itEnemy != m_mapEnemyList.end()) {
		itEnemy->second->m_infoCharacter.CharacterInfo.CityWarTeamType = byCityWarTeamType;
		itEnemy++;
	}
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CSceneData::SetFogLevel(DWORD dwFogColor, float fFogStart, float fFogEnd)
/// \brief		ľČ°ł¸¦ ĽÂĆĂÇŃ´Ů.
/// \author		ispark
/// \date		2005-06-29 ~ 2005-06-29
/// \warning	
///
/// \param		
/// \return		void
///////////////////////////////////////////////////////////////////////////////
void CSceneData::SetFogLevel(DWORD dwFogColor, float fFogStart, float fFogEnd)
{
	m_dwFogColor = dwFogColor;
	m_fOrgFogStartValue = fFogStart;
	m_fOrgFogEndValue = fFogEnd;
	DBGOUT("m_dwFogColor = %d \n m_fOrgFogStartValue = %f \n m_fOrgFogEndValue = %f \n", dwFogColor, fFogStart, fFogEnd);
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			CSceneData::SetMaxAtitudeHeight(int i_nMaxHeight)
/// \brief		¸Ę»ó ŔĚµż °ˇ´ÉÇŃ ĂÖ´ë łôŔĚ ĽÂĆĂ
/// \author		ispark
/// \date		2005-07-11 ~ 2005-07-11
/// \warning	
///
/// \param		
/// \return		void
///////////////////////////////////////////////////////////////////////////////
void CSceneData::SetMaxAtitudeHeight(int i_nMaxHeight)
{
	m_nMaxAtitudeHeight = i_nMaxHeight;
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			CSceneData::RenderWarter()
/// \brief		¸Ę»óŔÇ ą°ŔÇ ·»´ő¸µ
/// \author		ydkim
/// \date		2005-07-26 by ydkim
/// \warning	
///
/// \param		
/// \return		void
///////////////////////////////////////////////////////////////////////////////
void CSceneData::RenderWater()
{
	//		 IDirect3DSurface9* pSurface;
	//g_pD3dDev->CreateOffscreenPlainSurface(1600, 900,
	//	D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pSurface, NULL);
	//g_pD3dDev->GetFrontBufferData(0, pSurface);


	//IDirect3DSurface9* dest = NULL; // to be our level0 surface of the texture
	//m_Rtexture->GetSurfaceLevel(0, &dest);
	//g_pD3dDev->StretchRect(pSurface, NULL, dest, NULL, D3DTEXF_LINEAR);
	if (FAILED(g_pD3dDev->CreateStateBlock(D3DSBT_ALL, &m_pStateBlock))) {
		return;
	}
	//
	// 2005-02-11 by jschoi  물 오브젝트 렌더링
	CObjectChild* pWaterObj = (CObjectChild*)g_pGround->m_pWaterObject->m_pChild;
	while (pWaterObj)
	{
		float fRadius = 0;
		if (pWaterObj->m_pObjMesh)
		{
			fRadius = pWaterObj->m_pObjMesh->m_fRadius;
		}
		BOOL bCheckIn = g_pFrustum->CheckSphere(pWaterObj->m_vOriPos.x, pWaterObj->m_vOriPos.y, pWaterObj->m_vOriPos.z, fRadius);
		if (bCheckIn)
		{
			pWaterObj->Render();
		}
		pWaterObj = (CObjectChild*)pWaterObj->m_pNext;
	}

	// Water
	D3DMATERIAL9 mtrl;
	D3DUtil_InitMaterial(mtrl, 1, 1, 1);
	g_pD3dDev->SetMaterial(&mtrl);
	// 2008-12-04 by bhsohn 최소화 모드시, 물 랜더링 처리
	//if(g_pSOption->sLowQuality)
	{
		g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		g_pD3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		g_pD3dDev->SetRenderState(D3DRS_FOGENABLE, IsFogEnableMap(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex));
		g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);


	// 2008. 12. 11 by ckPark 물렌더링
	//g_pD3dDev->SetFVF( D3DFVF_GROUNDVERTEX );
	//g_pD3dDev->SetFVF(D3DFVF_WATERBUMPVERTEX);
	// end 2008. 12. 11 by ckPark 물렌더링
	 g_pD3dDev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);


	g_pD3dDev->SetTransform(D3DTS_WORLD, &mat);
	D3DXVECTOR3 vView;// = m_pCamera->GetEyePt();
	D3DXVECTOR3 vDir = g_pD3dApp->m_pCamera->GetViewDir();
	D3DXVec3Normalize(&vDir, &vDir);
	int i, j, cont1, cont2;
	int tempx, tempz;
	vView = g_pD3dApp->m_pCamera->GetEyePt();

	/// 2004.06.07 jschoi
	/// Water Region
	int nWaterCont = (int)(m_fFogEndValue / (TILE_SIZE * 2));
	vView += 2 * TILE_SIZE * nWaterCont * vDir;
	tempx = (int)((vView.x / TILE_SIZE) / 2);
	tempz = (int)((vView.z / TILE_SIZE) / 2);
	cont1 = 0;
	cont2 = 0;

	/// 2004.06.07 jschoi
	/// 물 이미지에 절두체 적용
	g_pFrustum->Construct(g_pD3dDev);
	int nWaterHeight = m_pGround->m_projectInfo.fWaterHeight;
	int nWaterRadius = TILE_SIZE * 4;
	// 2008-12-04 by bhsohn 최소화 모드시, 물 랜더링 처리
	//if(g_pSOption->sLowQuality)
	{
		g_pD3dDev->LightEnable(2, FALSE);
		g_pD3dDev->LightEnable(3, FALSE);
	}
	// end 2008-12-04 by bhsohn 최소화 모드시, 물 랜더링 처리

	// 2008. 12. 11 by ckPark 물렌더링



	// 범프맵 텍스쳐 스테이지 설정
	/*g_pD3dDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 1);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BUMPENVMAP);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pD3dDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	g_pD3dDev->SetTextureStageState(0, D3DTSS_BUMPENVMAT00, F2DW(1.0f / 16.0f));
	g_pD3dDev->SetTextureStageState(0, D3DTSS_BUMPENVMAT01, F2DW(0.0f));
	g_pD3dDev->SetTextureStageState(0, D3DTSS_BUMPENVMAT10, F2DW(0.0f));
	g_pD3dDev->SetTextureStageState(0, D3DTSS_BUMPENVMAT11, F2DW(1.0f / 16.0f));

	// 랩핑 모드 WRAP으로 변경
	g_pD3dDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	g_pD3dDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// 물 텍스쳐 스테이지 설정
	g_pD3dDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
	g_pD3dDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	g_pD3dDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pD3dDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	g_pD3dDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pD3dDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	g_pD3dDev->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	g_pD3dDev->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// 물 텍스쳐 텍스쳐 매트릭스 설정
	D3DXMATRIXA16 matTex;
	D3DXMatrixIdentity(&matTex);
	matTex._31 = timeGetTime() % 10000 / 10000.0f;
	g_pD3dDev->SetTransform(D3DTS_TEXTURE1, &matTex);
	g_pD3dDev->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	*/
	// 범프 맵핑과 텍스쳐 매트릭스가 동시에 안먹어서
	// 버텍스 버퍼에 직접 텍스쳐 좌표를 수정한다
		float tu2;
	tu2 = timeGetTime() % 4000 / 4000.0f;
	int		nRenderWaterTileCount = 0;	// 그려줄 물 타일 갯수
	// 최종적으로 그려줄 버텍스 버퍼 포인터
	WATERBUMPVERTEX* v;						//	GROUNDVERTEX -> WATERBUMPVERTEX

	// 2014-06-23 by ymjoo 물 버텍스 버퍼 락 익셉션 예외 처리 (if문) 2014-07-31 by ymjoo 롤백
	if(m_pGround && m_pGround->m_pToRenderWaterVB)
	{
		// 최종적 그릴 물 버텍스 버퍼 락
		// 2014-08-25 by ymjoo 물 버텍스 버퍼 코드 개선
		//m_pGround->m_pToRenderWaterVB->Lock(0, 0, (void**)&v, 0);
		LPDIRECT3DVERTEXBUFFER9 waterVB = m_pGround->m_pToRenderWaterVB;
		waterVB->Lock(0, 0, (void**)&v, 0);
		// END 2014-08-25 by ymjoo 물 버텍스 버퍼 코드 개선
		// end 2008. 12. 11 by ckPark 물렌더링

		for (i = tempx - nWaterCont; i < tempx + nWaterCont + 1; i++)
		{
			if (cont1 < nWaterCont / 2)
				cont2 = cont1;
			else if (cont1 < nWaterCont * 2 + 1 - nWaterCont / 2)
				cont2 = nWaterCont / 2;
			else
				cont2 = nWaterCont * 2 - cont1;

			if (i >= 0 && i < m_pGround->m_projectInfo.sXSize / 2)
			{
				// 2014-12-12 by jwLee 스트림으로 변환할 필요없는 물 버텍스 버퍼 소스 주석처리
// 				g_pD3dDev->SetStreamSource( 0, m_pGround->m_pVBWater[i],0, sizeof(GROUNDVERTEX) );
				// end 2014-12-12 by jwLee 스트림으로 변환할 필요없는 물 버텍스 버퍼 소스 주석처리
				for (j = tempz - (nWaterCont / 2 + cont2) - 1; j < tempz + (nWaterCont / 2 + cont2) + 2; j++)
				{
					if (j >= 0 && j < m_pGround->m_projectInfo.sYSize / 2)
					{
						int k = (i * m_pGround->m_projectInfo.sYSize / 2 + j);
						if (m_pGround->m_bWaterRender[k].useWater)
						{
							/// 2004.06.07 jschoi 절두체 적용
							if (g_pFrustum->CheckSphere(i * TILE_SIZE * 2 + TILE_SIZE, nWaterHeight, j * TILE_SIZE * 2 + TILE_SIZE, nWaterRadius))
							{
								int nWaterTexNumber = m_pGround->m_bWaterRender[k].waterTexNumber + m_pGround->m_bWaterTexCont;
								if (nWaterTexNumber >= WATER_TEXTURE_COUNT)
									nWaterTexNumber -= WATER_TEXTURE_COUNT;
								// 2008-12-04 by bhsohn 최소화 모드시, 물 랜더링 처리
	//							if(g_pSOption->sLowQuality)
// 				{
	//								nWaterTexNumber = 0;
	//							}
								// end 2008-12-04 by bhsohn 최소화 모드시, 물 랜더링 처리


							// 2008. 12. 11 by ckPark 물렌더링
							// 각 타일별로 텍스쳐 바꿔가면서 렌더링 하는 것들 한번 뭉쳐서 그린다
	// 						g_pD3dDev->SetTexture(0,m_pGround->m_pWaterTexture[nWaterTexNumber]);
	//						g_pD3dDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, j*4, 2 );
// 
								GROUNDVERTEX* vWaterVertices;
								m_pGround->m_pVBWater[i]->Lock(0, 0, (void**)&vWaterVertices, 0);

								// 최종적으로 그릴 버텍스 버퍼에 값 대입
								v[0].p = vWaterVertices[j * 4 + 0].p;
								v[0].n = vWaterVertices[j * 4 + 0].n;
								v[0].tu = vWaterVertices[j * 4 + 0].tu;
								v[0].tv = vWaterVertices[j * 4 + 0].tv;
								v[0].tu2 = tu2;			// 텍스쳐 좌표도 변경
								v[0].tv2 = 0.0f;
								
								v[1].p = vWaterVertices[j * 4 + 1].p;
								v[1].n = vWaterVertices[j * 4 + 1].n;
								v[1].tu = vWaterVertices[j * 4 + 1].tu;
								v[1].tv = vWaterVertices[j * 4 + 1].tv;
								v[1].tu2 = tu2;
								v[1].tv2 = 1.0f;

								v[2].p = vWaterVertices[j * 4 + 2].p;
								v[2].n = vWaterVertices[j * 4 + 2].n;
								v[2].tu = vWaterVertices[j * 4 + 2].tu;
								v[2].tv = vWaterVertices[j * 4 + 2].tv;
								v[2].tu2 = tu2 + 1.0f;
								v[2].tv2 = 0.0f;

								v[3].p = vWaterVertices[j * 4 + 3].p;
								v[3].n = vWaterVertices[j * 4 + 3].n;
								v[3].tu = vWaterVertices[j * 4 + 3].tu;
								v[3].tv = vWaterVertices[j * 4 + 3].tv;
								v[3].tu2 = tu2 + 1.0f;
								v[3].tv2 = 1.0f;
								
								m_pGround->m_pVBWater[i]->Unlock();

								// 최정적으로 그릴 버텍스 버퍼 포인터 증가
								v += 4;
								// 그리는 물 타일 갯수 증가
								++nRenderWaterTileCount;
								// end 2008. 12. 11 by ckPark 물렌더링
							}
						}
					}
				}
			}
			cont1++;
		}

		// 2008. 12. 11 by ckPark 물렌더링
		// 최종적으로 그릴 물 버텍스 버퍼 언락
		m_pGround->m_pToRenderWaterVB->Unlock();
	}
	// END 2014-06-23 by ymjoo 물 버텍스 버퍼 락 익셉션 예외 처리 (if문)

	// 타일 갯수 * 4만큼 랜더링
	g_pD3dDev->SetStreamSource(0, m_pGround->m_pToRenderWaterVB, 0, sizeof(WATERBUMPVERTEX));

	// 2009. 02. 11 by ckPark 물 렌더링 타일별로 렌더링
	//g_pD3dDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, nRenderWaterTileCount * 4);

#ifdef _DEBUG_SHADERS	
	D3DXMATRIX World, View, Projection,ReflectionMatView,ReflectionMatProj,ReflectionMatWorld;
	// get the current world, view and projection matrix
	g_pD3dDev->GetTransform(D3DTS_WORLD, &World);
	g_pD3dDev->GetTransform(D3DTS_VIEW, &View);
	g_pD3dDev->GetTransform(D3DTS_PROJECTION, &Projection);

	D3DXPLANE plane(0.0f, 1.0f, 0.0f, 0.0f); // Reflect across the XZ plane

	// Create the reflection matrix
	//D3DXMATRIX reflectionMatrix;
	//g_pD3dDev->GetTransform(D3DTS_VIEW, &reflectionMatrix);
	//D3DXMatrixReflect(&reflectionMatrix, &plane);


	//D3DXMatrixInverse(&ReflectionMatWorld,NULL,&World);
	//D3DXMatrixInverse(&ReflectionMatView,NULL,&View);
	//D3DXMatrixInverse(&ReflectionMatProj,NULL,&Projection);
	
	LPDIRECT3DSURFACE9 RenderTarget, Surface;
	D3DXMATRIX WorldViewProjection, WorldViewProjectionTransposed,FinalInverseRefl,FinalInverseReflTranspo;



	WorldViewProjection = World * View * Projection;

//	Projection._42 = g_pD3dApp->GetAdminPanel()->timer1;
	//Projection._32 = max(Projection._32,g_pD3dApp->GetAdminPanel()->timer1); //min(max(Projection._32,g_pD3dApp->GetAdminPanel()->timer1),g_pD3dApp->GetAdminPanel()->timer2);
	//View._32 =  max(View._32,g_pD3dApp->GetAdminPanel()->timer2); //min(max(View._32,g_pD3dApp->GetAdminPanel()->timer1),g_pD3dApp->GetAdminPanel()->timer2);
	//World._32 =  max(World._32,g_pD3dApp->GetAdminPanel()->timer3); //min(max(World._32,g_pD3dApp->GetAdminPanel()->timer1),g_pD3dApp->GetAdminPanel()->timer2);
	//Projection._22 = g_pD3dApp->GetAdminPanel()->timer3;

	FinalInverseRefl = World * View * Projection;
	//g_pD3dDev->SetTransform(D3DTS_VIEW, &reflectionMatrix);
	//D3DXMatrixMultiply(&FinalInverseRefl,NULL,&WorldViewProjection);
	//g_pD3dDev->SetTransform(D3DTS_WORLD, &FinalInverseRefl);
	//D3DXMatrixMultiply(&World,&ReflectionMat,&FinalInverseRefl);
	//D3DXMATRIX ReflectMat = {};
	//D3DXPLANE pl;
	//D3DXMatrixReflect(&ReflectMat,&m_reflectPlane );
	//D3DXMatrixMultiply(&FinalInverseRefl,&ReflectMat,&View);
	//g_pD3dDev->SetTransform(D3DTS_VIEW, &FinalInverseRefl);
	
	/*m_ReflectionTexture->GetSurfaceLevel(0,&Surface);
	g_pD3dDev->GetRenderTarget(0,&RenderTarget);
	g_pD3dDev->StretchRect(RenderTarget,NULL, Surface,NULL,D3DTEXF_NONE);

	RenderTarget->Release();*/
	D3DXMatrixTranspose(&WorldViewProjectionTransposed, &WorldViewProjection);
	D3DXMatrixTranspose(&FinalInverseReflTranspo, &FinalInverseRefl);

	//D3DXMatrixRotationYawPitchRoll(&FinalInverseReflTranspo, g_pD3dApp->GetAdminPanel()->timer4, g_pD3dApp->GetAdminPanel()->timer5, g_pD3dApp->GetAdminPanel()->timer6);
	//D3DXPLANE pl;
	//D3DXPlaneFromPoints(&pl,&g_pShuttleChild->m_vVel,&g_pShuttleChild->m_vSideVel,&g_pShuttleChild->m_vUp);

#ifdef _DEBUG_SHADERS
	//g_pD3dDev->SetTexture(0, m_pGround->m_pWaterTexture[0]);			// dudv
	g_pD3dDev->SetTexture(1, m_pGround->m_pWaterTexture[0]);			//water
	g_pD3dDev->SetTexture(2, m_RefractionTexture);			// reflect
	g_pD3dDev->SetTexture(3, m_RefractionTexture);			// refract
	g_pD3dDev->SetTexture(4, m_pGround->m_pWaterTexture[1]);			// normal
#else
	g_pD3dDev->SetTexture(0, m_pGround->m_pWaterTexture[0]);			// 범프 텍스쳐 0번
#endif
	
	
	   static float time = 0.f; time += 0.01f;
    float psConstant[] = { g_pD3dApp->GetAdminPanel()->timer1, g_pD3dApp->GetAdminPanel()->timer2, g_pD3dApp->GetAdminPanel()->timer3, time };
    float psConstant_Adm[] = { g_pD3dApp->GetAdminPanel()->timer4, g_pD3dApp->GetAdminPanel()->timer5, g_pD3dApp->GetAdminPanel()->timer6,  g_pD3dApp->GetAdminPanel()->timer7 };
	D3DVECTOR dvX = SetLightDirection();
	 float psConstant2[] = { m_light0.Direction.x, m_light0.Direction.y,m_light0.Direction.z, 0 };
	 float psConstantCam[] = { g_pCamera->GetViewDir().x, g_pCamera->GetViewDir().y,g_pCamera->GetViewDir().z, 1 };
	g_pD3dDev->SetVertexShaderConstantF(0, WorldViewProjectionTransposed, 4);
	g_pD3dDev->SetVertexShaderConstantF(4, World, 4);
	g_pD3dDev->SetVertexShaderConstantF(8, Projection, 4);
	g_pD3dDev->SetVertexShaderConstantF(12, FinalInverseReflTranspo, 4);
	g_pD3dDev->SetVertexShaderConstantF(16, psConstant, 1);
	g_pD3dDev->SetVertexShaderConstantF(20, psConstantCam, 1);
	g_pD3dDev->SetVertexShaderConstantF(24, View, 4);
	g_pD3dDev->SetVertexDeclaration(m_pVertexDeclaration);
	g_pD3dDev->SetVertexShader(m_pSimpleVS);

    g_pD3dDev->SetPixelShaderConstantF(0, psConstant, 1);
    g_pD3dDev->SetPixelShaderConstantF(4, psConstant_Adm, 1);
    g_pD3dDev->SetPixelShaderConstantF(8, psConstant2, 1);
	short FogStartDist = m_bNight ? g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)->NightFogStartDistance : g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)->DayFogStartDistance;
	short FogEndDist = m_bNight ? g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)->NightFogEndDistance : g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)->DayFogEndDistance;
	auto FogColor = m_bNight ?  g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)->DayFogColor : g_pDatabase->GetMapInfo(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex)->NightFogColor;
	auto byFogRed = (BYTE)(FogColor >> 16);
	auto byFogGreen = (BYTE)(FogColor >> 8);
	auto byFogBlue = (BYTE)(FogColor);
	
	float psMapParams[] = { 100, FogEndDist,g_pShuttleChild->m_vPos.y-nWaterHeight, 0 };
    g_pD3dDev->SetPixelShaderConstantF(12, psMapParams, 1);
	float psMapParamsFogCol[] = { byFogRed/255,  byFogGreen/255, byFogBlue/255, 0 };
	g_pD3dDev->SetPixelShaderConstantF(16, psMapParamsFogCol, 1);
	g_pD3dDev->SetPixelShader(m_pSimplePS);


#endif


	for (i = 0; i < nRenderWaterTileCount; ++i)
			g_pD3dDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, i * 4, 2);
	// end 2009. 02. 11 by ckPark 물 렌더링 타일별로 렌더링

	// 텍스쳐 해제
	/*g_pD3dDev->SetTexture(0, 0);
	g_pD3dDev->SetTexture(1, 0);

	// 텍스쳐 매트릭스 해제
	g_pD3dDev->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	// 1번 멀티 텍스쳐 해제	
	// 2009-01-15 by bhsohn ATI비디오 카드에서 이 옵션을 안쓰면 깨진다.
	g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	g_pD3dDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	g_pD3dDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	// 텍스쳐 좌표 원래대로 변경
	g_pD3dDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	g_pD3dDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

	g_pD3dDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pD3dDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	// 	g_pD3dDev->SetSamplerState( 1, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
	//  g_pD3dDev->SetSamplerState( 1, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );
	*/
		// end 2008. 12. 11 by ckPark 물렌더링
	m_pStateBlock->Apply();
	SAFE_RELEASE(m_pStateBlock)
}



///////////////////////////////////////////////////////////////////////////////
/// \fn			CSceneData::ChangeEnemyCharacterMode()
/// \brief		Ŕű Äł¸ŻĹÍżÍ ŔŻ´Ö ¸đµĺ¸¦ Ľ­·Î ąŮ˛Ű´Ů. Character -> Unit, Unit -> Character
/// \author		ispark
/// \date		2005-07-28 ~ 2005-07-28
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CSceneData::ChangeEnemyCharacterMode(MSG_FC_CHARACTER_CHANGE_CHARACTER_MODE_OK* pMsg)
{
	CMapEnemyIterator itEnemy = m_mapEnemyList.find(pMsg->ClientIndex);
	if (itEnemy != m_mapEnemyList.end()) {
		CEnemyData* pEnemy = itEnemy->second;

		if (pMsg->CharacterMode0 == FALSE) {
			// ±âľî¸đµĺ
			pEnemy->m_bEnemyCharacter = FALSE;
			// 2008-07-14 by bhsohn Äł¸ŻĹÍ »óĹÂ ąö±× ĽöÁ¤
			pEnemy->m_infoCharacter.CharacterInfo.CharacterMode0 = 0;
		}
		else {
			// Äł¸ŻĹÍ ¸đµĺ
			pEnemy->m_bEnemyCharacter = TRUE;
			// 2008-07-14 by bhsohn Äł¸ŻĹÍ »óĹÂ ąö±× ĽöÁ¤
			pEnemy->m_infoCharacter.CharacterInfo.CharacterMode0 = 1;

			// Äł¸ŻĹÍ ¸đµĺ´Â Upş¤ĹÍ ąŘżˇ˛¨ ĽÂĆĂ
			pEnemy->m_vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		}
		pEnemy->m_vPos = A2DX(pMsg->PositionAVec3);
		pEnemy->m_vVel = A2DX(pMsg->TargetAVec3);
		D3DXVec3Normalize(&pEnemy->m_vVel, &pEnemy->m_vVel);
		pEnemy->Init();
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void ChangeObjectBodyCondition(INT nCode, BodyCond_t body);
/// \brief		°°Ŕş żŔşęÁ§Ć®ŔÇ ąřČŁŔÇ ąŮµđÄÁµđĽÇ°ŞŔ» şŻ°ćÇŃ´Ů.
/// \author		dgwoo
/// \date		2007-04-23 ~ 2007-04-23
/// \warning	
///
/// \param		
/// \return		żŔşęÁ§Ć®ąřČŁ, şŻ°ćÇŇ ąŮµđÄÁµđĽÇ
///////////////////////////////////////////////////////////////////////////////
void CSceneData::ChangeObjectBodyCondition(INT nCode, BodyCond_t body)
{
	CObjectChild* pObj = (CObjectChild*)g_pGround->m_pBigObject->m_pChild;
	while (pObj) {
		if (pObj->m_nCode == nCode) {
			pObj->ChangeBodyconditionEvent(body);
		}
		pObj = (CObjectChild*)pObj->m_pNext;
	}
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			COLLISION_RESULT IsObjectCheckPosTOPos(D3DXVECTOR3	vMyShuttlePos, D3DXVECTOR3	vEmenyPos);
/// \brief		// Áˇ »çŔĚżˇ żŔşęÁ§Ć®°ˇ ŔÖ´ÂÁö ĂĽĹ©
/// \author		// 2007-05-17 by bhsohn żŔşęÁ§Ć® µÚżˇ ĽűľúŔ»˝Ă żˇ ´ëÇŃ Ăł °Ë»ç Ăł¸®
/// \date		2007-05-17 ~ 2007-05-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CSceneData::IsObjectCheckPosTOPos(D3DXVECTOR3	vMyShuttlePos, D3DXVECTOR3	vEmenyPos, D3DXVECTOR3	vEnemyUp)
{
	BOOL bCheckObject = FALSE;
	D3DXVECTOR3	vTargetPos;
	D3DXVECTOR3 vVel, vSide, vUpTemp;
	D3DXMATRIX mat;
	FLOAT fDistance = D3DXVec3Length(&(vEmenyPos - vMyShuttlePos));

	vVel = vEmenyPos - vMyShuttlePos;
	D3DXVec3Normalize(&vVel, &vVel);

	vTargetPos = vMyShuttlePos;

	D3DXVec3Cross(&vSide, &vEnemyUp, &vVel);
	D3DXVec3Cross(&vUpTemp, &vVel, &vSide);
	D3DXMatrixLookAtLH(&mat, &vTargetPos, &(vTargetPos + vVel), &vUpTemp);

	FLOAT fDist = CheckCollRenderListRangeObject(mat, vTargetPos, fDistance);
	if (fDist < fDistance) {
		bCheckObject = TRUE;
		//DBGOUT("ĂćµąŔĚ´ĺ[%s] ~\n", m_infoCharacter.CharacterInfo.CharacterName);			
	}
	return bCheckObject;

}

float CSceneData::CheckCollRenderListRangeObject(D3DXMATRIX mat, D3DXVECTOR3 vPos, float fMovingDistance)
{
	float fDist, fDistResult;
	fDist = fDistResult = DEFAULT_COLLISION_DISTANCE;
	vectorCObjectChildPtr::iterator itObj(m_vectorCulledObjectPtrList.begin());
	while (itObj != m_vectorCulledObjectPtrList.end()) {
		CObjectChild* pObject = *itObj;
		// 2009-04-21 by bhsohn ĂćµąĂĽĹ© ľČµÇ´Â Ĺ¸ŔÔ żŔşęÁ§Ć®¸é ą«Á¶°Ç Ĺ¸ÄĎĆĂ µÇ°Ô ĽöÁ¤
		BOOL bCheck = TRUE;
		if (pObject && pObject->m_pObjectInfo && pObject->m_pObjMesh) {
			if ((FALSE == pObject->m_pObjectInfo->Collision)
				|| (pObject->m_pObjectInfo->ObjectRenderType == OBJECT_BIG_EFFECT)) {
				bCheck = FALSE;
			}
		}
		// end 2009-04-21 by bhsohn ĂćµąĂĽĹ© ľČµÇ´Â Ĺ¸ŔÔ żŔşęÁ§Ć®¸é ą«Á¶°Ç Ĺ¸ÄĎĆĂ µÇ°Ô ĽöÁ¤
		if (pObject && pObject->m_pObjectInfo && pObject->m_pObjMesh &&
			//pObject->m_pObjectInfo->ObjectRenderType != OBJECT_BIG_EFFECT // 2008-10-17 by bhsohn Ĺ¸ÄĎĆĂ ˝Ă˝şĹŰżˇĽ­ Ĺő¸í żŔşęÁ§Ć®´Â ĂĽĹ© ÇĎÁö ľČ°Ô şŻ°ć
			bCheck)// 2009-04-21 by bhsohn ĂćµąĂĽĹ© ľČµÇ´Â Ĺ¸ŔÔ żŔşęÁ§Ć®¸é ą«Á¶°Ç Ĺ¸ÄĎĆĂ µÇ°Ô ĽöÁ¤			
		{
			float fRadius = pObject->m_pObjMesh->m_fRadius;
			if (D3DXVec3Length(&(pObject->m_vOriPos - vPos)) < fRadius * 2.0f + fMovingDistance) {
				//				pObject->m_pObjMesh->Tick(pObject->m_fCurrentTime);
				//				pObject->m_pObjMesh->SetWorldMatrix(pObject->m_mMatrix);
				fDistResult = pObject->m_pObjMesh->CheckCollision(mat, vPos, DEFAULT_COLLISION_DISTANCE, FALSE, FALSE).fDist;
				if (fDist > fDistResult) {
					fDist = fDistResult;
				}
			}
		}
		itObj++;
	}
	return fDist;


}

///////////////////////////////////////////////////////////////////////////////
/// \fn			COLLISION_RESULT IsObjectCheckPosTOPos(D3DXVECTOR3	vMyShuttlePos, D3DXVECTOR3	vEmenyPos);
/// \brief		// Áˇ »çŔĚżˇ ÁöÇü°ˇ ŔÖ´ÂÁö ĂĽĹ©
/// \author		// 2007-05-17 by bhsohn żŔşęÁ§Ć® µÚżˇ ĽűľúŔ»˝Ă żˇ ´ëÇŃ Ăł °Ë»ç Ăł¸®
/// \date		2007-05-17 ~ 2007-05-17
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CSceneData::IsTileCheckPosTOPos(D3DXVECTOR3	vMyShuttlePos, D3DXVECTOR3	vEmenyPos)
{
	BOOL bCheckObject = FALSE;
	// ÁöÇü°ú Ăćµą °Ë»ç
#ifdef MAP_EDITOR
	if (IsTileMapRenderEnable(g_pD3dApp->m_nMapNumber) == FALSE)
#else
	if (IsTileMapRenderEnable(g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex) == FALSE)
#endif
	{
		return bCheckObject;
	}

	D3DXVECTOR3	vTargetPos;
	D3DXVECTOR3 vVel, vSide, vUpTemp;
	D3DXMATRIX mat;
	FLOAT fDistance = D3DXVec3Length(&(vEmenyPos - vMyShuttlePos));
	FLOAT fTargetDistance = 0;
	FLOAT fMovingDistance = TILE_CHECK_DISTANCE;

	if (fDistance < MIN_TILE_CHECK_DISTANCE) {
		return bCheckObject;
	}

	vVel = vEmenyPos - vMyShuttlePos;
	D3DXVec3Normalize(&vVel, &vVel);

	if (0 == D3DXVec3Length(&vVel)) {
		return bCheckObject;
	}

	vTargetPos = vMyShuttlePos;
	DWORD dwCnt = 0;
	for (dwCnt = 0;; dwCnt++) {
		vTargetPos += vVel * fMovingDistance;
		fTargetDistance = D3DXVec3Length(&(vTargetPos - vMyShuttlePos));

		if (fTargetDistance > fDistance) {
			break;
		}
		// ąŮ´Ú Ăćµą
		float fHeight = 0.0f;
		D3DXVECTOR3 vNor;
		g_pGround->CheckCollMap(vTargetPos, &fHeight, &vNor);
		if (fHeight >= vTargetPos.y) {
			bCheckObject = TRUE;
			break;
		}
		long lDiff = abs(fHeight - vTargetPos.y);
		if (lDiff < MIN_TILE_DIFF_CHECK_HEIGHT) {
			// ĂĽĹ©·çĆľ°ú ¸ĘŔÇ łôŔĚ Â÷°ˇ 100ąĚĹÍ ¸¸
			fMovingDistance /= 2;
			if (fMovingDistance < MIN_TILE_CHECK_DISTANCE) {
				// ĂÖĽŇ´Â Ĺ¸ŔĎŔÇ ąÝ
				fMovingDistance = MIN_TILE_CHECK_DISTANCE;
			}
		}
		else if (lDiff > MAX_TILE_DIFF_CHECK_HEIGHT) {
			fMovingDistance = TILE_CHECK_DISTANCE;
		}
	}
	return bCheckObject;
}


///////////////////////////////////////////////////////////////////////////////
/// \fn			void CSceneData::DeleteRenderEnemy(ClientIndex_t nIndex)
/// \brief		
/// \author		// 2007-06-13 by bhsohn ¸Ţ¸đ¸® ąö±× µđąö±ë
/// \date		2007-06-13 ~ 2007-06-13
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CSceneData::DeleteRenderEnemy(ClientIndex_t nIndex)
{
	CVecEnemyIterator it = m_vecEnemyRenderList.begin();
	while (it != m_vecEnemyRenderList.end()) {
		CEnemyData* pEnemy = (CEnemyData*)(*it);
		ENEMYINFO stuEnemyinfo = pEnemy->GetEnemyInfo();
		if (stuEnemyinfo.CharacterInfo.ClientIndex == nIndex) {
			//it = m_vecEnemyRenderList.erase(it);
			it = m_vecEnemyRenderList.erase(it);//14-03-2022 by Inet
			continue;
		}
		it++;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			void CSceneData::ReLoadEnemyRenderList()
/// \brief		
/// \author		// 2007-08-29 by bhsohn ĂÖĽŇ ÇÁ·ąŔÓ˝Ă ±âş» ľĆ¸Ó¸¸ ·ÎµůÇĎ°Ô˛ű şŻ°ć
/// \date		2007-08-29 ~ 2007-08-29
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
void CSceneData::ReLoadEnemyRenderList()
{
	g_pD3dApp->m_pUnitRender->DeleteDeviceObjects();
}

///////////////////////////////////////////////////////////////////////////////
/// \fn			
/// \brief		
/// \author		// 2008-07-14 by bhsohn EP3 °ü·Ă Ăł¸®
/// \date		2008-07-14 ~ 2008-07-14
/// \warning	
///
/// \param		
/// \return		
///////////////////////////////////////////////////////////////////////////////
BOOL CSceneData::GetEmemyCharacterUniqueNumber(char* pName, UID32_t* o_CharacterUniqueNumber)
{
	CMapEnemyIterator itEnemy = m_mapEnemyList.begin();
	while (itEnemy != m_mapEnemyList.end()) {
		if (0 == strcmp(pName, itEnemy->second->m_infoCharacter.CharacterInfo.CharacterName)) {
			(*o_CharacterUniqueNumber) = itEnemy->second->m_infoCharacter.CharacterInfo.CharacterUniqueNumber;
			return TRUE;
		}
		itEnemy++;
	}
	(*o_CharacterUniqueNumber) = 0;
	return FALSE;
}
void CSceneData::InvectoryFullMessage()
{
	if (m_fGetItemMessage < 0.0f) {
		g_pD3dApp->m_pChat->CreateChatChild(STRERR_ERROR_0022, COLOR_ERROR);
		m_fGetItemMessage = INVENTORY_MESSAGE_GAP;
	}
}


// 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ
CObjectChild* CSceneData::FindMapObjectByIndex(int nIndex)
{
	int x = (int)((g_pGround->m_projectInfo.sXSize * TILE_SIZE) / MAP_BLOCK_SIZE);
	int z = (int)((g_pGround->m_projectInfo.sYSize * TILE_SIZE) / MAP_BLOCK_SIZE);
	CObjectChild* pMapObj = NULL;
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < z; j++) {
			pMapObj = (CObjectChild*)g_pGround->m_ppObjectList[i][j].m_pChild;
			while (pMapObj) {
				if (pMapObj->m_sEventIndexFrom == nIndex) {
					return pMapObj;
				}
				else {
					pMapObj = (CObjectChild*)pMapObj->m_pNext;
				}
			}
		}
	}

	pMapObj = (CObjectChild*)m_pGround->m_pBigObject->m_pChild;
	while (pMapObj) {
		if (pMapObj->m_sEventIndexFrom == nIndex) {
			return pMapObj;
		}
		else {
			pMapObj = (CObjectChild*)pMapObj->m_pNext;
		}
	}

	pMapObj = (CObjectChild*)m_pGround->m_pWaterObject->m_pChild;
	while (pMapObj) {
		if (pMapObj->m_sEventIndexFrom == nIndex) {
			return pMapObj;
		}
		else {
			pMapObj = (CObjectChild*)pMapObj->m_pNext;
		}
	}

	pMapObj = (CObjectChild*)m_pGround->m_pObjectMonster->m_pChild;
	while (pMapObj) {
		if (pMapObj->m_sEventIndexFrom == nIndex) {
			return pMapObj;
		}
		else {
			pMapObj = (CObjectChild*)pMapObj->m_pNext;
		}
	}

	return NULL;
}
// end 2009. 11. 02 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ ŔÎ˝şĹĎ˝ş ´řÁŻ ˝Ă˝şĹŰ

// 2010. 03. 15 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ż¬Ăâ Ăł¸®)
void	CSceneData::ChangeSkyBox(char* szSkyBoxName)
{
	m_pETCRender->ChangeSkyBox(szSkyBoxName);
}

void	CSceneData::ChangeEventObjectBodyCondition(short nEventParam, BodyCond_t body)
{
	CObjectChild* pObj = FindMapObjectByIndex(nEventParam);
	if (pObj && pObj->m_pCharacterInfo) {
		pObj->m_pCharacterInfo->SetCharacterAnimationBodyConditionMask(0xffffffffffffffff);
		pObj->ChangeBodyconditionEvent(body);
	}
}
// end 2010. 03. 15 by ckPark ŔÎÇÇ´ĎĆĽ ÇĘµĺ 2Â÷(ż¬Ăâ Ăł¸®)

// 2010. 10. 05 by jskim ¸Ę·Îµů ±¸Á¶ şŻ°ć
void	CSceneData::StepBackground_Step1()
{
	if (m_bIsRestore) {
		InitRes();
	}
}

void	CSceneData::StepBackground_Step2()
{
	if (m_bIsRestore) {
		RestoreRes();
		m_bIsRestore = FALSE;
	}
}

void	CSceneData::StepBackground_Step3()
{
//	EnterCriticalSection(&g_pD3dApp->m_cs);
	int i;
	FILE* readMap;
	int re = -1;
	WORKSPACE iWorkspace;
	char strPath[256];
	g_pD3dApp->LoadPath(strPath, IDS_DIRECTORY_MAP, "ms.wok");//RC_MAP_WORKSPACE );//
	if (strlen(strPath) > 0) {
		readMap = fopen(strPath, "rb");
		fseek(readMap, 20, SEEK_SET);
		fread(&iWorkspace, sizeof(WORKSPACE), 1, readMap);
		int a = iWorkspace.numberOfProject;
		for (i = 0; i < a; i++) {
			fread(&m_prProject, sizeof(PROJECTINFO), 1, readMap);
			char buf[32];

			wsprintf(buf, "%04d", g_pD3dApp->m_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex);

			re = strcmp(m_prProject.strProjectName, buf);

			if (re == MAPNAME_CHECK_SUCCESS) {
				DBGOUT("MAP Project Info[name:%s]\n   [Water Height:%.2f]\n   Day[D:%.2f,%.2f,%.2f][A:%.2f,%.2f,%.2f]\n   Night[D:%.2f,%.2f,%.2f][A:%.2f,%.2f,%.2f]\n",
					   m_prProject.strProjectName, m_prProject.fWaterHeight,
					   m_prProject.fDiffuseR1, m_prProject.fDiffuseG1, m_prProject.fDiffuseB1,
					   m_prProject.fAmbientR1, m_prProject.fAmbientG1, m_prProject.fAmbientB1,
					   m_prProject.fDiffuseR2, m_prProject.fDiffuseG2, m_prProject.fDiffuseB2,
					   m_prProject.fAmbientR2, m_prProject.fAmbientG2, m_prProject.fAmbientB2
				);

				// map type °áÁ¤
				int nTemp = atoi(m_prProject.strProjectName);
				if (nTemp > 1000) {
					// 2006-07-19 by ispark, żľłŻ µµ˝Ă¸¦ ĆÇ´ÜÇĎ±â Ŕ§ÇŃ°Ĺ »čÁ¦
					//					m_byMapType =  nTemp / 1000;
					m_byMapType = MAP_TYPE_NORMAL_FIELD;
				}
				else {
					if (0 == strcmp(m_prProject.strProjectName, "0100"))
						m_byMapType = MAP_TYPE_TUTORIAL;
					//					else if(0 == strcmp(iProject.strProjectName,"0106"))
					//						m_byMapType = MAP_TYPE_CITY;
					else
						m_byMapType = MAP_TYPE_NORMAL_FIELD;
				}
				break;
			}
		}
		fclose(readMap);
	}
	else {
		return;
	}
	//LeaveCriticalSection(&g_pD3dApp->m_cs);
}

void	CSceneData::StepBackground_Step4()
{
	//EnterCriticalSection(&g_pD3dApp->m_cs);
	//step 4
	if (m_pGround) {
		if (m_vecEnemyBlockList) {
			for (int i = 0; i < m_nBlockSizeX; i++) {
				for (int j = 0; j < m_nBlockSizeY; j++) {
					m_vecEnemyBlockList[i * m_nBlockSizeY + j].clear();
				}
			}
			SAFE_DELETE_ARRAY(m_vecEnemyBlockList);
		}
		if (m_vecMonsterList) {
			for (int i = 0; i < m_nBlockSizeX; i++) {
				for (int j = 0; j < m_nBlockSizeY; j++) {
					m_vecMonsterList[i * m_nBlockSizeY + j].clear();
				}
			}
			SAFE_DELETE_ARRAY(m_vecMonsterList);
		}
	}
	m_nBlockSizeX = m_prProject.sXSize / 3;
	m_nBlockSizeY = m_prProject.sYSize / 3;
	if (m_prProject.sXSize % 3)
		m_nBlockSizeX++;
	if (m_prProject.sYSize % 3)
		m_nBlockSizeY++;
	m_vecEnemyBlockList = new CVecEnemyList[m_nBlockSizeX * m_nBlockSizeY];
	m_vecMonsterList = new CVecMonsterList[m_nBlockSizeX * m_nBlockSizeY];
	//LeaveCriticalSection(&g_pD3dApp->m_cs);
}

void	CSceneData::StepBackground_Step5()
{
	//EnterCriticalSection(&g_pD3dApp->m_cs);

	if (m_pGround) {
		m_pGround->DeleteDeviceObjects();
		SAFE_DELETE(m_pGround);
	}
	if (m_pETCRender) {
		m_pETCRender->DeleteDeviceObjects();
	}
	if (m_pWater && m_bWaterShaderRenderFlag)
		m_pWater->DeleteDeviceObjects();

	m_pGround = new CBackground(m_prProject);

	if (m_pGround) {
		if (FAILED(m_pGround->InitDeviceObjects())) {
			SAFE_DELETE(m_pGround);
		}
	}
	if (m_pGround)
		m_pGround->RestoreDeviceObjects();
	if (m_pETCRender)
		m_pETCRender->InitDeviceObjects();
	if (m_pWater && m_bWaterShaderRenderFlag)
		m_pWater->InitDeviceObjects();
	if (m_pETCRender)
		m_pETCRender->RestoreDeviceObjects();
	if (m_pWater && m_bWaterShaderRenderFlag)
		m_pWater->RestoreDeviceObjects();
	{
		g_pD3dApp->UpdateGameStartMapInfo();
	}
	//LeaveCriticalSection(&g_pD3dApp->m_cs);
}
// end 2010. 10. 05 by jskim ¸Ę·Îµů ±¸Á¶ şŻ°ć