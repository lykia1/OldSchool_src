#include "stdafx.h"
#include "INFEvoKillfeed.h"
#include "AtumApplication.h"
#include "D3DHanFont.h"
#include "AtumDefine.h"
#include "ShuttleChild.h"
#include "GameDataLast.h"
#include "INFImageEx.h"
#include "d3dx9core.h"
#include "dxutil.h"
#define  D3DFVF_KFITEMVERTEX (D3DFVF_XYZRHW  | D3DFVF_TEX1)
struct KillfeedItemVertex
{
	D3DXVECTOR4 p;  
	FLOAT tu, tv;
};

INFEvoKillfeed::INFEvoKillfeed()
{
	m_vecItems.reserve(40);
	
	m_kfBeginX = g_pApp->GetWidth() - 230;
	m_kfBeginY = 200;

	m_pFont = nullptr;
	m_pMissleIcon = nullptr;
	m_pCrashIcon = nullptr;
	m_pFuelIcon = nullptr;
	m_pMonsterIcon = nullptr;
	m_pBigBoomIcon = nullptr;
}

INFEvoKillfeed::~INFEvoKillfeed()
{
	m_vecItems.clear();
	SAFE_DELETE(m_pMissleIcon);
	SAFE_DELETE(m_pCrashIcon);
	SAFE_DELETE(m_pFuelIcon);
	SAFE_DELETE(m_pMonsterIcon);
	SAFE_DELETE(m_pBigBoomIcon);
	SAFE_DELETE(m_pFont);
}

HRESULT INFEvoKillfeed::InitDeviceObjects()
{
	// thanks to Salz_mich_ein for help with the icons
	DataHeader* pDataHeader = NULL;
	pDataHeader = m_pGameData->Find("evokft");
	if (pDataHeader)
	{
		m_pMissleIcon = new CINFImageEx;
		m_pMissleIcon->InitDeviceObjects(pDataHeader);
	}
	pDataHeader = m_pGameData->Find("evokfc");
	if (pDataHeader)
	{
		m_pCrashIcon = new CINFImageEx;
		m_pCrashIcon->InitDeviceObjects(pDataHeader);
	}
	pDataHeader = m_pGameData->Find("evokff");
	if (pDataHeader)
	{
		m_pFuelIcon = new CINFImageEx;
		m_pFuelIcon->InitDeviceObjects(pDataHeader);
	}
	pDataHeader = m_pGameData->Find("evokfm");
	if (pDataHeader)
	{
		m_pMonsterIcon = new CINFImageEx;
		m_pMonsterIcon->InitDeviceObjects(pDataHeader);
	}

	pDataHeader = m_pGameData->Find("evokfbb");
	if (pDataHeader)
	{
		m_pBigBoomIcon = new CINFImageEx;
		m_pBigBoomIcon->InitDeviceObjects(pDataHeader);
	}

	m_pFont = new CD3DHanFont(_T(g_pD3dApp->GetFontStyle()), 10, D3DFONT_BOLD, TRUE, 1024, 32);
	m_pFont->InitDeviceObjects(g_pD3dDev);
	m_pFont->RestoreDeviceObjects();
	return S_OK;
}

HRESULT INFEvoKillfeed::RestoreDeviceObjects()
{
	if (m_pMissleIcon)
		m_pMissleIcon->RestoreDeviceObjects();

	if (m_pCrashIcon)
		m_pCrashIcon->RestoreDeviceObjects();

	if (m_pFuelIcon)
		m_pFuelIcon->RestoreDeviceObjects();

	if (m_pMonsterIcon)
		m_pMonsterIcon->RestoreDeviceObjects();

	if (m_pBigBoomIcon)
		m_pBigBoomIcon->RestoreDeviceObjects();

	m_pFont->RestoreDeviceObjects();

	for (auto& item : m_vecItems)
	{
		item->RestoreDeviceObjects();
	}

	return S_OK;
}

HRESULT INFEvoKillfeed::DeleteDeviceObjects()
{
	if (m_pMissleIcon)
		m_pMissleIcon->DeleteDeviceObjects();

	if (m_pCrashIcon)
		m_pCrashIcon->DeleteDeviceObjects();

	if (m_pFuelIcon)
		m_pFuelIcon->DeleteDeviceObjects();

	if (m_pMonsterIcon)
		m_pMonsterIcon->DeleteDeviceObjects();

	if (m_pBigBoomIcon)
		m_pBigBoomIcon->DeleteDeviceObjects();

	m_pFont->DeleteDeviceObjects();

	SAFE_DELETE(m_pMissleIcon);
	SAFE_DELETE(m_pCrashIcon);
	SAFE_DELETE(m_pFuelIcon);
	SAFE_DELETE(m_pMonsterIcon);
	SAFE_DELETE(m_pFont);

	for (auto& item : m_vecItems)
	{
		item->DeleteDeviceObjects();
	}

	return S_OK;
}

HRESULT INFEvoKillfeed::InvalidateDeviceObjects()
{
	if (m_pMissleIcon)
		m_pMissleIcon->InvalidateDeviceObjects();

	if (m_pCrashIcon)
		m_pCrashIcon->InvalidateDeviceObjects();

	if (m_pFuelIcon)
		m_pFuelIcon->InvalidateDeviceObjects();

	if (m_pMonsterIcon)
		m_pMonsterIcon->InvalidateDeviceObjects();

	if (m_pBigBoomIcon)
		m_pBigBoomIcon->InvalidateDeviceObjects();

	m_pFont->InvalidateDeviceObjects();

	for (auto& item : m_vecItems)
	{
		item->InvalidateDeviceObjects();
	}

	return S_OK;
}

void INFEvoKillfeed::Render()
{
	if (MAP_INFLUENCE_PVP_ALL == g_pD3dApp->GetMyShuttleMapInfo()->MapInfluenceType) {
		return;
	}

	int offset_y = 0;
	auto& rev_iter = m_vecItems.rbegin();
	while (rev_iter != m_vecItems.rend())
	{
		auto item = (*rev_iter).get();
		if(item){
			item->Move(m_kfBeginX - item->GetWidth(), m_kfBeginY + offset_y);
			item->Render();
			++rev_iter;
			offset_y += KFITEM_HEIGHT + KILLFEED_ITEM_SPACING;
		}
	}
}

void INFEvoKillfeed::Tick()
{
	auto iter = m_vecItems.begin();
	while (iter != m_vecItems.end())
	{
		(*iter)->Tick();

		if ((*iter)->ShouldBeRemoved())
		{
			iter = m_vecItems.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void INFEvoKillfeed::AddKillFeedItem(MSG_FC_CHARACTER_DEAD_NOTIFY_MAP* msg)
{	
	std::unique_ptr<KillFeedItem> item = std::make_unique<KillFeedItem>(this, msg);
	if (item->RestoreDeviceObjects() == S_OK)
	{
		m_vecItems.push_back(std::move(item));
	}
}

KillFeedItem::KillFeedItem(INFEvoKillfeed* parent, MSG_FC_CHARACTER_DEAD_NOTIFY_MAP* msg)
{
	m_initialised = false; 
	m_remove = false;

	if (!parent || !msg)
	{
		m_remove = true;
		return;
	}

	m_pParent = parent;
	m_data = *msg;

	m_createdTime = std::chrono::system_clock::now();
	m_alpha = 255;


	if (strcmp(msg->AttackerName, g_pShuttleChild->m_myShuttleInfo.CharacterName) == 0)
	{
		m_attackerIsMe = true;
		STRNCPY_MEMSET(m_data.AttackerName, "Me",SIZE_MAX_CHARACTER_NAME);
	}
	else
	{
		m_attackerIsMe = false;
	}

	if (strcmp(msg->TargetName, g_pShuttleChild->m_myShuttleInfo.CharacterName) == 0)
	{
		m_targetIsMe = true;
		STRNCPY_MEMSET(m_data.TargetName, "Me",SIZE_MAX_CHARACTER_NAME);
	}
	else
	{
		m_targetIsMe = false;
	}
}

KillFeedItem::~KillFeedItem()
{
	DeleteDeviceObjects();
}

HRESULT KillFeedItem::InitDeviceObjects()
{
	return S_OK;
}

HRESULT KillFeedItem::RestoreDeviceObjects()
{
	if (m_initialised) 
    {
        return S_OK;
    }
	short nMMEventHelpMe = 101;
	short nMMEventMarkForm = 102;
	SIZE attackerstringsize = m_pParent->GetFont()->GetStringSize(m_data.AttackerName);
	SIZE targetstringsize = m_pParent->GetFont()->GetStringSize(m_data.TargetName);
	char szHelpMeMsg[100];

	if (m_data.DamageType == nMMEventHelpMe) {
		if (m_targetIsMe)
			sprintf(szHelpMeMsg, "You have asked for help!");
		else
			sprintf(szHelpMeMsg, "%s is asking for help!", m_data.TargetName);

		targetstringsize = m_pParent->GetFont()->GetStringSize(szHelpMeMsg);
	}
	if (m_data.DamageType == nMMEventMarkForm) {
		if (m_targetIsMe)
			sprintf(szHelpMeMsg, "You have setted marker!");
		else
			sprintf(szHelpMeMsg, "%s have setted marker!", m_data.TargetName);

		targetstringsize = m_pParent->GetFont()->GetStringSize(szHelpMeMsg);
	}
	int width = targetstringsize.cx + KFITEM_ICON_WIDTH + KFITEM_ICON_MARGIN_X + 2 * KFITEM_TEXT_MARGIN_X;
	width += (m_data.DamageType == DAMAGE_BY_PK) ? attackerstringsize.cx + KFITEM_ICON_MARGIN_X : 0;
	int height = KFITEM_HEIGHT;
	

	if (!SUCCEEDED(g_pD3dDev->CreateTexture(width, height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture, 0)))
	{
		return S_FALSE;
	}

	if (!SUCCEEDED(m_pTexture->GetSurfaceLevel(0, &m_pSurface)))
	{
		return S_FALSE;
	}

	IDirect3DSurface9* pBackbuffer = nullptr;
	if (!SUCCEEDED(g_pD3dDev->GetRenderTarget(0, &pBackbuffer)))
	{
		return S_FALSE;
	}
		
	if (!SUCCEEDED(g_pD3dDev->SetRenderTarget(0, m_pSurface)))
	{
		return S_FALSE;
	}

	D3DVIEWPORT9 vp, vp_orig;
	vp.Height = height;
	vp.Width = width;
	vp.X = 0;
	vp.Y = 0;
	vp.MaxZ = 1.0f;
	vp.MinZ = 0.0f;
	g_pD3dDev->GetViewport(&vp_orig);
	HRESULT hr = g_pD3dDev->SetViewport(&vp);
	
	DWORD clearColor = (m_attackerIsMe || m_targetIsMe) ? KFITEM_BACKGROUND_COLOR_PLAYER  : KFITEM_BACKGROUND_COLOR_NORMAL;
	//background box
	if (!SUCCEEDED(g_pD3dDev->Clear(0, 0, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, clearColor, 1.0f, 0)))
	{
		return S_FALSE;
	}

	BYTE myInfluence = g_pShuttleChild->m_myShuttleInfo.InfluenceType;
	//DWORD attackercolor = m_attackerIsMe ? COLOR_CHARACTER_ID02 : ((myInfluence == m_data.AttackerInfluence) ? COLOR_CHARACTER_ID00 : COLOR_CHARACTER_ID01);
	    //DWORD targetcolor    = m_targetIsMe ? COLOR_CHARACTER_ID02 : ((myInfluence == m_data.TargetInfluence) ? COLOR_CHARACTER_ID00 : COLOR_CHARACTER_ID01);
    DWORD attackercolor = m_attackerIsMe ? KFITEM_TEXT_COLOR_ME : ((myInfluence == m_data.AttackerInfluence) ? KFITEM_TEXT_COLOR_ALLY : KFITEM_TEXT_COLOR_ENEMY);
    DWORD targetcolor    = m_targetIsMe     ? KFITEM_TEXT_COLOR_ME : ((myInfluence == m_data.TargetInfluence)     ? KFITEM_TEXT_COLOR_ALLY : KFITEM_TEXT_COLOR_ENEMY);

	CINFImageEx* icon = nullptr;
	int attackername_offset = 0;

	switch (m_data.DamageType)
	{
	case DAMAGE_BY_COLLISION:
		icon = m_pParent->GetCrashIcon();
		break;
	case DAMAGE_BY_MONSTER:
		icon = m_pParent->GetMonsterIcon();
		break;
	case DAMAGE_BY_FUEL_ALLIN:
		icon = m_pParent->GetFuelIcon();
		break;
	case DAMAGE_BY_NA:
		icon = m_pParent->GetBigBoomIcon();
		break;
	case DAMAGE_BY_PK:
		icon = m_pParent->GetPlayerIcon();
		int attacker_text_offset_y = static_cast<int>(static_cast<float>(KFITEM_HEIGHT - attackerstringsize.cy) / 2 - 0.5f);
		m_pParent->GetFont()->DrawText(KFITEM_TEXT_MARGIN_X, attacker_text_offset_y-4, attackercolor, m_data.AttackerName, 0);
		attackername_offset = attackerstringsize.cx + KFITEM_ICON_MARGIN_X;
		break;
	}

	if (icon)
	{
		icon->Move(attackername_offset + KFITEM_TEXT_MARGIN_X, KFITEM_TEXT_MARGIN_Y);
		icon->SetScale(1.0f, 1.0f);
		icon->Render();
	}

	int target_text_offset_y = static_cast<int>(static_cast<float>(KFITEM_HEIGHT - targetstringsize.cy) / 2 - 0.5f);

	if(m_data.DamageType == nMMEventHelpMe)
		m_pParent->GetFont()->DrawTextA(width - targetstringsize.cx - KFITEM_TEXT_MARGIN_X, target_text_offset_y - 4, RGB(0,125,255), szHelpMeMsg, 0);
	else if (m_data.DamageType == nMMEventMarkForm)
		m_pParent->GetFont()->DrawTextA(width - targetstringsize.cx - KFITEM_TEXT_MARGIN_X, target_text_offset_y - 4, RGB(0, 255, 0), szHelpMeMsg, 0);
	else
		m_pParent->GetFont()->DrawTextA(width - targetstringsize.cx - KFITEM_TEXT_MARGIN_X, target_text_offset_y - 4, targetcolor, m_data.TargetName, 0);
	
	    g_pD3dDev->SetViewport(&vp_orig);
	if (!SUCCEEDED(g_pD3dDev->SetRenderTarget(0, pBackbuffer)))
	{
		return S_FALSE;
	}
	    pBackbuffer->Release();
	// Create Vertex Buffer
	if (!SUCCEEDED(g_pD3dDev->CreateVertexBuffer( /*MAX_NUM_VERTICES*/6 * sizeof(KillfeedItemVertex),
		D3DUSAGE_WRITEONLY, D3DFVF_KFITEMVERTEX,
		D3DPOOL_MANAGED, &m_pVB, NULL)))
	{
		return S_FALSE;
	}

	m_coordinates.left = 0;
	m_coordinates.right = width;
	m_coordinates.top = 0;
	m_coordinates.bottom = height;
	m_initialised = true;
	return S_OK;
}

HRESULT KillFeedItem::DeleteDeviceObjects()
{
	SAFE_RELEASE(m_pVB);         
    SAFE_RELEASE(m_pSurface);
    SAFE_RELEASE(m_pTexture);
    m_initialised = false;
    m_remove = true;
	
	return S_OK;
}

HRESULT KillFeedItem::InvalidateDeviceObjects()
{
	SAFE_RELEASE(m_pVB);             
    SAFE_RELEASE(m_pSurface);
    SAFE_RELEASE(m_pTexture);
    m_initialised = false;
    m_remove = true;
    return S_OK;
}

void KillFeedItem::Render()
{
	if (m_initialised && !m_remove)
	{
		/*g_pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		g_pD3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pD3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		g_pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pD3dDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pD3dDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		g_pD3dDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		g_pD3dDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

		g_pD3dDev->SetTextureStageState(0, D3DTSS_CONSTANT, D3DCOLOR_ARGB(m_alpha, 255, 255, 255));
		g_pD3dDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CONSTANT);
		g_pD3dDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pD3dDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		
		g_pD3dDev->SetRenderState(D3DRS_ZENABLE, FALSE);
		*/
		g_pD3dDev->SetTexture(0, m_pTexture);
		g_pD3dDev->SetFVF(D3DFVF_KFITEMVERTEX);
		g_pD3dDev->SetStreamSource(0, m_pVB, 0, sizeof(KillfeedItemVertex));

		g_pD3dDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	}
}

void KillFeedItem::Tick()
{
	if (m_initialised)
	{
		auto deltaTime = std::chrono::system_clock::now() - m_createdTime;		   		
		if (deltaTime >= KFITEM_LIFETIME)
		{
			m_remove = true;
			return;
		}
		/*else if (deltaTime >= KFITEM_LIFETIME - KFITEM_FADEOUT_TIME) 
		{
			auto fadeTime = std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime) - std::chrono::milliseconds(KFITEM_LIFETIME - KFITEM_FADEOUT_TIME);
			m_alpha = 255 - fadeTime.count() * 255 / std::chrono::milliseconds(KFITEM_FADEOUT_TIME).count();
		}*/
	}
}

void KillFeedItem::Move(int x, int y)
{
	if (m_remove)
	{
		return;
	}

	if (x != m_coordinates.left || y != m_coordinates.top)
	{
		UpdateVertexBuffer(x, y);
	}
}

void KillFeedItem::UpdateVertexBuffer(int x, int y)
{
	int width = GetWidth();
	int height = GetHeight();

	KillfeedItemVertex* vertices;
	HRESULT hr = 0;
	if (SUCCEEDED(m_pVB->Lock(0, 0, reinterpret_cast<void**>(&vertices), 0)))
	{
		vertices[0] = KillfeedItemVertex{ D3DXVECTOR4(x - 0.5f			, y + height - 0.5f	,0.9f	,1.0f),  0, 1 };
		vertices[1] = KillfeedItemVertex{ D3DXVECTOR4(x - 0.5f			, y - 0.5f			,0.9f	,1.0f),  0, 0 };
		vertices[2] = KillfeedItemVertex{ D3DXVECTOR4(x + width - 0.5f	, y + height - 0.5f	,0.9f	,1.0f),  1, 1 };
		vertices[3] = KillfeedItemVertex{ D3DXVECTOR4(x + width - 0.5f	, y - 0.5f			,0.9f	,1.0f),  1, 0 };
		vertices[4] = KillfeedItemVertex{ D3DXVECTOR4(x + width - 0.5f	, y + height - 0.5f	,0.9f	,1.0f),  1, 1 };
		vertices[5] = KillfeedItemVertex{ D3DXVECTOR4(x - 0.5f			, y - 0.5f			,0.9f	,1.0f),  0, 0 };
		m_pVB->Unlock();
		
		m_coordinates.left		= x;
		m_coordinates.top		= y;
		m_coordinates.right		= x + width;
		m_coordinates.bottom	= y + height;
	}
}

