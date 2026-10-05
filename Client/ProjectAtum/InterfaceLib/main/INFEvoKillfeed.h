#pragma once	
#include "AtumProtocol.h"
#include <vector>
#include <chrono>
#include "INFBase.h"
#include <memory>
#include "AtumParam.h"


#define KFITEM_ICON_PLACEHOLDER_WIDTH 40
#define KFITEM_ICON_WIDTH 24
#define KFITEM_ICON_HEIGHT 24
#define KFITEM_ICON_MARGIN_X ((KFITEM_ICON_PLACEHOLDER_WIDTH - KFITEM_ICON_WIDTH) / 2)
#define KFITEM_TEXT_MARGIN_X 5
#define KFITEM_TEXT_MARGIN_Y 2

#define KFITEM_LIFETIME 12s	 // total lifetime
#define KFITEM_FADEOUT_TIME 2s
#define KFITEM_HEIGHT (KFITEM_ICON_HEIGHT + 2 * KFITEM_TEXT_MARGIN_Y)

#define KFITEM_TEXT_COLOR_ENEMY		   COLOR_CHARACTER_ID01
#define KFITEM_TEXT_COLOR_ALLY		   COLOR_CHARACTER_ID00
#define KFITEM_TEXT_COLOR_ME           0x42C4F9

#define KFITEM_BACKGROUND_COLOR_NORMAL D3DCOLOR_ARGB(0, 0, 0, 0)    //D3DCOLOR_ARGB(100, 25, 92, 140)
#define KFITEM_BACKGROUND_COLOR_PLAYER D3DCOLOR_ARGB(0, 0, 0, 0)    //D3DCOLOR_ARGB(170, 245, 184, 32)
#define KILLFEED_ITEM_SPACING 2	//vertical space between two items

class CD3DHanFont;
class CINFImageEx;
class IDirect3DTexture9;
class IDirect3DSurface9;
class INFEvoKillfeed;

class KillFeedItem : public CINFBase
{
public:
	KillFeedItem(INFEvoKillfeed* parent, MSG_FC_CHARACTER_DEAD_NOTIFY_MAP* msg);
	~KillFeedItem();
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual void Render();
	virtual void Tick();
	void Move(int x, int y);
	bool ShouldBeRemoved() { return m_remove; };
	int GetWidth() { return m_coordinates.right - m_coordinates.left; };
	int GetHeight() { return m_coordinates.bottom - m_coordinates.top; };

private:
	void UpdateVertexBuffer(int x, int y);

private:
	INFEvoKillfeed* m_pParent;
	std::chrono::time_point<std::chrono::system_clock> m_createdTime;
	byte m_alpha;
	MSG_FC_CHARACTER_DEAD_NOTIFY_MAP m_data;
	bool m_attackerIsMe;
	bool m_targetIsMe;

	IDirect3DSurface9* m_pSurface;
	IDirect3DTexture9* m_pTexture;
	IDirect3DVertexBuffer9* m_pVB;

	RECT m_coordinates;
	bool m_remove;
	bool m_initialised;
};											  


class INFEvoKillfeed : public CINFBase
{
public:
	INFEvoKillfeed();
	virtual ~INFEvoKillfeed();
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual void Render();
	virtual void Tick();

	CD3DHanFont* GetFont() { return m_pFont; };
	CINFImageEx* GetPlayerIcon() { return m_pMissleIcon; };
	CINFImageEx* GetCrashIcon() { return m_pCrashIcon; };
	CINFImageEx* GetFuelIcon() { return m_pFuelIcon; };
	CINFImageEx* GetMonsterIcon() { return m_pMonsterIcon; };
	CINFImageEx* GetBigBoomIcon() { return m_pBigBoomIcon; };

	void AddKillFeedItem(MSG_FC_CHARACTER_DEAD_NOTIFY_MAP* msg);

private:
	std::vector<std::unique_ptr<KillFeedItem>> m_vecItems;
	int m_kfBeginX;
	int m_kfBeginY;
	CD3DHanFont* m_pFont;
	CINFImageEx* m_pMissleIcon;
	CINFImageEx* m_pCrashIcon;
	CINFImageEx* m_pFuelIcon;
	CINFImageEx* m_pMonsterIcon;
	CINFImageEx* m_pBigBoomIcon;
};