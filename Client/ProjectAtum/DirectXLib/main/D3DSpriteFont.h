#pragma once
#include "D3DSpriteBatch.h"
#include "stdafx.h"

class CD3DSpriteFont;
class CD3DSpriteBatch;

enum SPRITEFONTFLAGS : char
{
	SPRITEFONTFLAG_NORMAL		= 0,
	SPRITEFONTFLAG_BOLD			= 1 << 0,
	SPRITEFONTFLAG_ITALIC		= 1 << 1, // italic not used now
	SPRITEFONTFLAG_OUTLINE		= 1 << 2,
};
DEFINE_ENUM_FLAG_OPERATORS(SPRITEFONTFLAGS)

/*
 * lightweight class for simplify API
 * call to the SpriteFont->GetState(...)
 */
class SpriteFontState
{
public:
	//HRESULT DrawString(FLOAT x, FLOAT y, LPCSTR text) const;
	
	HRESULT DrawString(FLOAT x, FLOAT y, LPCSTR text, D3DCOLOR color = D3DCOLOR_FULL, DWORD layer = 0,
	                   PRECT fillRect = nullptr) const;

	HRESULT DrawString(D3DXVECTOR2 dst, LPCSTR text, D3DCOLOR color = D3DCOLOR_FULL, DWORD layer = 0,
	                   PRECT fillRect = nullptr) const;

	SIZE MeasureString(LPCSTR text) const;

	friend CD3DSpriteFont;
private:
	SpriteFontState(CD3DSpriteFont* pSpriteFont,
	                char fontSize,
	                SPRITEFONTFLAGS flags,
	                D3DCOLOR color,
	                DWORD layer);

	CD3DSpriteFont* m_pSpriteFont;
	char m_fontSize;
	SPRITEFONTFLAGS m_flags;
	D3DCOLOR m_color;
	DWORD m_layer;
};

/*
 * Specially used for BoardData
 * uses the slow GDI api
 */
class SpriteFontTexture
{
public:
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	// color is the gdi color palette (BGR)
	HRESULT SetText(LPCSTR text, DWORD color);

	friend CD3DSpriteFont;

	LPDIRECT3DTEXTURE9 m_pTexture;
	
private:
	SpriteFontTexture(
		CD3DSpriteFont* pSpriteFont,
		char fontSize,
		SPRITEFONTFLAGS flags,
		int texWidth,
		int texHeight);

	HRESULT RenderText();

	CD3DSpriteFont* m_pSpriteFont;
	TCHAR m_pText[256]{};
	char m_nFontSize;
	SPRITEFONTFLAGS m_cFlags;
	DWORD m_wColor; //BGR
	int m_nTexWidth;
	int m_nTexHeight;
};

/*
 * almost every call will use g_pD3dApp->GetFontStyle() except few exceptions
 * font size ranges according my search from 6-12
 * possible variants: italic, bold, outline, selection-border
 */
class CD3DSpriteFont
{
public:
	/*
	 * minFontSize and maxFontSize will be pre generate the glyph table
	 * when it restore the device, it will generate the atlas
	 */
	CD3DSpriteFont(LPCSTR fontName, char minFontSize, char maxFontSize);
	~CD3DSpriteFont();
	
    HRESULT InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();

	HRESULT DrawString(
		D3DXVECTOR2 dst,
		LPCSTR text, 
		char fontSize, 
		SPRITEFONTFLAGS flags = SPRITEFONTFLAG_NORMAL,
		D3DCOLOR color = D3DCOLOR_FULL,
		DWORD layer = 0,
		PRECT fillRect = nullptr);

	/*
	 * Warning slow GDI calls
	 */
	SpriteFontTexture* GetTexture(char fontSize, SPRITEFONTFLAGS flags, int texWidth, int texHeight)
	{
		return new SpriteFontTexture(this, fontSize, flags, texWidth, texHeight);
	}
	
	SIZE MeasureString(
		LPCSTR text,
		char fontSize,
		SPRITEFONTFLAGS flags = SPRITEFONTFLAG_NORMAL);

	SpriteFontState* GetState(
		char fontSize,
		SPRITEFONTFLAGS flags = SPRITEFONTFLAG_NORMAL,
		D3DCOLOR color = D3DCOLOR_FULL,
		DWORD layer = 0)
	{
		return new SpriteFontState(this, fontSize, flags, color, layer);
	}
	
	RECT FindGlyph(
		TCHAR character,
		char fontSize,
		SPRITEFONTFLAGS flags = SPRITEFONTFLAG_NORMAL);

	void AttachSpriteBatch(CD3DSpriteBatch* pSpriteBatch){ m_pSpriteBatch = pSpriteBatch; }

	friend SpriteFontTexture;
	//friend SpriteFontState;
	
private:
	template<typename TAction>
	void ForEachGlyph(
		LPCSTR text,
		char fontSize,
		SPRITEFONTFLAGS flags,
		TAction action);

	HRESULT GenerateGlyphSet(char fontSize, USHORT startCharacter, USHORT endCharacter, SPRITEFONTFLAGS flag);
	
	CD3DSpriteBatch* m_pSpriteBatch;
	LPDIRECT3DTEXTURE9 m_pTexFontAtlas;
	LPDIRECT3DTEXTURE9 m_pSelectionPixel;
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	
	map<DWORD, RECT> m_mapGlyphs;
	TCHAR m_sFontName[80]{};
	char m_cMinFontSize;
	char m_cMaxFontSize;
	POINT m_pAtlasMarker{0,0};
	DWORD m_dMaxTextureSize;
};