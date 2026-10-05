#include "D3DSpriteFont.h"
#include "AtumApplication.h"
#include "DXUtil.h"

#define SPRITE_FONT_MAX_TEXTURE_SIZE 2048
#define SPRITE_FONT_TILE_SIZE 24
#define SPRITE_FONT_START_CHARACTER 0x20
#define SPRITE_FONT_END_CHARACTER 0xff


SpriteFontState::SpriteFontState(CD3DSpriteFont* pSpriteFont, char fontSize,
                                 SPRITEFONTFLAGS flags, D3DCOLOR color, DWORD layer)
{
	m_pSpriteFont = pSpriteFont;
	m_fontSize = fontSize;
	m_flags = flags;
	m_color = color;
	m_layer = layer;
}

/*HRESULT SpriteFontState::DrawString(FLOAT x, FLOAT y, LPCSTR text) const
{
	return DrawString(D3DXVECTOR2(x, y), text, m_color, m_layer);
}*/

HRESULT SpriteFontState::DrawString(FLOAT x, FLOAT y, LPCSTR text, D3DCOLOR color, DWORD layer, 
									PRECT fillRect) const
{
	return DrawString(D3DXVECTOR2(x, y), text, color, layer, fillRect);
}

HRESULT SpriteFontState::DrawString(D3DXVECTOR2 dst, LPCSTR text, D3DCOLOR color, DWORD layer, 
									PRECT fillRect) const
{
	return m_pSpriteFont->DrawString(dst, text, m_fontSize, m_flags, color, layer, fillRect);
}

SIZE SpriteFontState::MeasureString(LPCSTR text) const
{
	return m_pSpriteFont->MeasureString(text, m_fontSize, m_flags);
}

SpriteFontTexture::SpriteFontTexture(CD3DSpriteFont* pSpriteFont, char fontSize,
									 SPRITEFONTFLAGS flags, int texWidth, int texHeight)
{
	m_pTexture = NULL;
	m_wColor = 0;
	m_pSpriteFont = pSpriteFont;
	m_nFontSize = fontSize;
	m_cFlags = flags;
	m_nTexWidth = texWidth;
	m_nTexHeight = texHeight;
}

HRESULT SpriteFontTexture::InitDeviceObjects()
{
	return S_OK;
}

HRESULT SpriteFontTexture::RestoreDeviceObjects()
{
	HRESULT hr = m_pSpriteFont->m_pd3dDevice->CreateTexture(m_nTexWidth, m_nTexHeight,
		1,	0, D3DFMT_A4R4G4B4,
		D3DPOOL_MANAGED, &m_pTexture, nullptr);

	if (FAILED(hr))
	{
#ifdef CLIENT_CONSOLE
		printf("SpriteFontTexture::RestoreDeviceObjects for m_pd3dDevice->CreateTexture failed 0x%08lx\n", hr);
#endif
		return hr;
	}

	if(strlen(m_pText) > 0)
	{
		return RenderText();
	}
	
	return S_OK;
}

HRESULT SpriteFontTexture::InvalidateDeviceObjects()
{
	SAFE_RELEASE(m_pTexture);
	return S_OK;
}

HRESULT SpriteFontTexture::DeleteDeviceObjects()
{
	MEMSET_ZERO(m_pText, sizeof(m_pText));
	return S_OK;
}

HRESULT SpriteFontTexture::SetText(LPCSTR text, DWORD color)
{
	m_wColor = color;
	if(_tcscmp(m_pText, text) != 0)
	{
		_tcscpy(m_pText, text);
		return RenderText();
	}
	
	return S_OK;
}

HRESULT SpriteFontTexture::RenderText()
{
	HDC hdc = g_pApp->GetHDC();
	DWORD* pBitmapBits;
	BITMAPINFO bmi;
	ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = (int)m_nTexWidth;
	bmi.bmiHeader.biHeight = -(int)m_nTexHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount = 32;

	HBITMAP hbmBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS,
	                                     (void**)&pBitmapBits, nullptr, 0);
	if (!hbmBitmap || !pBitmapBits)
	{
#ifdef CLIENT_CONSOLE
		printf("CD3DSpriteFont::RestoreDeviceObjects for !hbmBitmap || !pBitmapBits failed\n");
#endif
		return E_FAIL;
	}

	int nHeight = -MulDiv(m_nFontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	HFONT font = CreateFont(nHeight, 0, 0, 0,
	                  (m_cFlags & SPRITEFONTFLAG_BOLD) ? FW_BOLD : 0,
	                  (m_cFlags & SPRITEFONTFLAG_ITALIC) ? 1 : 0,
	                  0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
	                  CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
	                  FIXED_PITCH, m_pSpriteFont->m_sFontName);

	SelectObject(hdc, font);
	SetBkMode(hdc,TRANSPARENT);
	SetMapMode(hdc, MM_TEXT);
	SelectObject(hdc, hbmBitmap);
	SetBkColor(hdc, 0x00000000);
	SetTextAlign(hdc, TA_TOP | TA_LEFT);

	int x = 0;
	long pos = 0;
	
	m_pSpriteFont->ForEachGlyph(m_pText, m_nFontSize, m_cFlags,
		[&](RECT box, D3DCOLOR overwriteColor)
	{
		if(overwriteColor > 0)
		{
			// GDI uses BGR, so have to convert it from RGB
			SetTextColor(hdc, RGB(overwriteColor >> 16 & 0xFF, overwriteColor >> 8 & 0xFF, overwriteColor & 0xFF));
		}
		else
		{
			SetTextColor(hdc, m_wColor);
		}
		
		ExtTextOut(hdc, x+1, 1, ETO_OPAQUE, nullptr, &m_pText[pos], 1, nullptr);

		x += box.right - box.left;
		pos++;
	});

	D3DLOCKED_RECT d3dlr;
	m_pTexture->LockRect( 0, &d3dlr, nullptr, 0 );
	BYTE* pDstRow = (BYTE*)d3dlr.pBits;
	WORD* pDst16;

	for(int y = 0; y < m_nTexHeight; y++ )
	{
		pDst16 = (WORD*)pDstRow;
		for(x = 0; x < m_nTexWidth; x++ )
		{
			if ((pBitmapBits[m_nTexWidth*y + x]) & 0x00ffffff)
			{
				*pDst16++ = 0xf000
					| (WORD)(((pBitmapBits[m_nTexWidth*y + x]) & 0x00f00000) >> 12)
					| (WORD)(((pBitmapBits[m_nTexWidth*y + x]) & 0x0000f000) >> 8)
					| (WORD)(((pBitmapBits[m_nTexWidth*y + x]) & 0x000000f0) >> 4);
			}
			else
			{
				*pDst16++ = 0x0000;
			}
		}
		pDstRow += d3dlr.Pitch;
	}

	m_pTexture->UnlockRect(0);

	DeleteObject(font);
	DeleteObject(hbmBitmap);

	return S_OK;
}

CD3DSpriteFont::CD3DSpriteFont(LPCSTR fontName, char minFontSize, char maxFontSize)
{
	m_pSpriteBatch = nullptr;
	m_pd3dDevice = nullptr;
	m_pTexFontAtlas = nullptr;
	m_pSelectionPixel = nullptr;
	_tcscpy(m_sFontName, fontName);
	m_cMinFontSize = minFontSize;
	m_cMaxFontSize = maxFontSize;
	m_dMaxTextureSize = 0;
}

CD3DSpriteFont::~CD3DSpriteFont()
{
	m_mapGlyphs.clear();
	m_pd3dDevice = nullptr;
	m_pSpriteBatch = nullptr;
}

HRESULT CD3DSpriteFont::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;

	D3DCAPS9 d3dCaps;
	m_pd3dDevice->GetDeviceCaps(&d3dCaps);

	m_dMaxTextureSize = min(d3dCaps.MaxTextureWidth, SPRITE_FONT_MAX_TEXTURE_SIZE);

#ifdef CLIENT_CONSOLE
	printf("CD3DSpriteFont::InitDeviceObjects maxtexture: %lu, device can:%lu\n", m_dMaxTextureSize, d3dCaps.MaxTextureWidth);
#endif
	
	for (char iFontSize = m_cMaxFontSize; iFontSize >= m_cMinFontSize; iFontSize--)
	{
		GenerateGlyphSet(iFontSize, SPRITE_FONT_START_CHARACTER, SPRITE_FONT_END_CHARACTER, SPRITEFONTFLAG_NORMAL);
		GenerateGlyphSet(iFontSize, SPRITE_FONT_START_CHARACTER, SPRITE_FONT_END_CHARACTER,
		                 SPRITEFONTFLAG_NORMAL | SPRITEFONTFLAG_OUTLINE);
		GenerateGlyphSet(iFontSize, SPRITE_FONT_START_CHARACTER, SPRITE_FONT_END_CHARACTER, SPRITEFONTFLAG_BOLD);
		GenerateGlyphSet(iFontSize, SPRITE_FONT_START_CHARACTER, SPRITE_FONT_END_CHARACTER,
		                 SPRITEFONTFLAG_BOLD | SPRITEFONTFLAG_OUTLINE);
	}

	return S_OK;
}

HRESULT CD3DSpriteFont::RestoreDeviceObjects()
{
	HRESULT hr;

	hr = m_pd3dDevice->CreateTexture(m_dMaxTextureSize, m_dMaxTextureSize, 1,
	                                 0, D3DFMT_A4R4G4B4,
	                                 D3DPOOL_MANAGED, &m_pTexFontAtlas, nullptr);

	if (FAILED(hr))
	{
#ifdef CLIENT_CONSOLE
		printf("CD3DSpriteFont::RestoreDeviceObjects for m_pd3dDevice->CreateTexture(m_pTexFontAtlas) failed 0x%08lx\n", hr);
#endif
		return hr;
	}
		
	hr = m_pd3dDevice->CreateTexture(1, 1, 1,
	                                 0, D3DFMT_A4R4G4B4,
	                                 D3DPOOL_MANAGED, &m_pSelectionPixel, nullptr);

	if (FAILED(hr))
	{
#ifdef CLIENT_CONSOLE
		printf("CD3DSpriteFont::RestoreDeviceObjects for m_pd3dDevice->CreateTexture(m_pSelectionPixel) failed 0x%08lx\n", hr);
#endif
		return hr;
	}

	DWORD* pBitmapBits;
	BITMAPINFO bmi;
	ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = (int)m_dMaxTextureSize;
	bmi.bmiHeader.biHeight = -(int)m_dMaxTextureSize;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount = 32;

	HDC hdc = g_pApp->GetHDC();

	HBITMAP hbmBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS,
	                                     (void**)&pBitmapBits, nullptr, 0);
	if (!hbmBitmap || !pBitmapBits)
	{
#ifdef CLIENT_CONSOLE
		printf("CD3DSpriteFont::RestoreDeviceObjects for !hbmBitmap || !pBitmapBits failed\n");
#endif
		return E_FAIL;
	}

	SetBkMode(hdc,TRANSPARENT);
	SetMapMode(hdc, MM_TEXT);
	SelectObject(hdc, hbmBitmap);
	SetTextColor(hdc, 0x00ffffff);
	SetBkColor(hdc, 0x00000000);
	SetTextAlign(hdc, TA_TOP | TA_LEFT);

	map<DWORD, HFONT> mapFonts;
	TCHAR str[2] = _T("x");
	
#ifdef CLIENT_CONSOLE
	printf("CD3DSpriteFont::RestoreDeviceObjects generating for %d glyphs\n", m_mapGlyphs.size());
#endif
	
	for (auto pair : m_mapGlyphs)
	{
		str[0] = (TCHAR)(pair.first >> 16 & 0xFFFF);
		char fontSize = (char)(pair.first >> 8 & 0xFF);
		SPRITEFONTFLAGS flag = (SPRITEFONTFLAGS)(pair.first & 0xFF);
		RECT box = pair.second;
		HFONT font = mapFonts[(fontSize << 8 | flag)];

		if (font == nullptr)
		{
			int nHeight = -MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
			font = CreateFont(nHeight, 0, 0, 0,
			                  (flag & SPRITEFONTFLAG_BOLD) ? FW_BOLD : 0,
			                  (flag & SPRITEFONTFLAG_ITALIC) ? 1 : 0,
			                  0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			                  CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
			                  FIXED_PITCH, m_sFontName);
			mapFonts.emplace((fontSize << 8 | flag), font);
		}
		SelectObject(hdc, font);

		if (flag & SPRITEFONTFLAG_OUTLINE)
		{
			ExtTextOut(hdc, box.left+1, box.top, ETO_CLIPPED, nullptr, str, 1, nullptr);
			ExtTextOut(hdc, box.left, box.top+1, ETO_CLIPPED, nullptr, str, 1, nullptr);
			ExtTextOut(hdc, box.left+2, box.top+1, ETO_CLIPPED, nullptr, str, 1, nullptr);
			ExtTextOut(hdc, box.left+1, box.top+2, ETO_CLIPPED, nullptr, str, 1, nullptr);
		}
		else
		{
			ExtTextOut(hdc, box.left, box.top, ETO_OPAQUE, nullptr, str, 1, nullptr);
		}
	}
	
#ifdef CLIENT_CONSOLE
	printf("CD3DSpriteFont::RestoreDeviceObjects %d fonts created\n", mapFonts.size());
#endif

	for (auto pair : mapFonts)
	{
		DeleteObject(pair.second);
	}

	mapFonts.clear();

	D3DLOCKED_RECT d3dlr;
	m_pTexFontAtlas->LockRect(0, &d3dlr, nullptr, 0);
	BYTE* pDstRow = (BYTE*)d3dlr.pBits;
	WORD* pDst16 = nullptr;
	BYTE bAlpha;

#ifdef CLIENT_CONSOLE
	printf("CD3DSpriteFont::RestoreDeviceObjects copying vram - marker[%ld,%ld]\n", m_pAtlasMarker.x, m_pAtlasMarker.y);
#endif

	for (int y = 0; /*y < m_pAtlasMarker.y + SPRITE_FONT_TILE_SIZE &&*/ y < m_dMaxTextureSize; y++)
	{
		pDst16 = (WORD*)pDstRow;
		for (int x = 0; x < m_dMaxTextureSize; x++)
		{
			bAlpha = (BYTE)((pBitmapBits[m_dMaxTextureSize * y + x] & 0xff) >> 4);
			if (bAlpha > 0)
			{
				*pDst16++ = (bAlpha << 12) | 0x0fff;
			}
			else
			{
				*pDst16++ = 0x0000;
			}
		}
		pDstRow += d3dlr.Pitch;
	}

	m_pTexFontAtlas->UnlockRect(0);
	DeleteObject(hbmBitmap);
	
	D3DLOCKED_RECT rect;
    m_pSelectionPixel->LockRect( 0, &rect, nullptr, 0 );
	DWORD* pDword = (DWORD*)rect.pBits;
	*pDword = (D3DCOLOR)0xffffffff;
    m_pSelectionPixel->UnlockRect(0);

	return S_OK;
}

HRESULT CD3DSpriteFont::InvalidateDeviceObjects()
{
	SAFE_RELEASE(m_pTexFontAtlas);
	SAFE_RELEASE(m_pSelectionPixel);
	return S_OK;
}

HRESULT CD3DSpriteFont::DeleteDeviceObjects()
{
	m_pd3dDevice = nullptr;
	m_mapGlyphs.clear();
	return S_OK;
}

HRESULT CD3DSpriteFont::DrawString(D3DXVECTOR2 dst, LPCSTR text, char fontSize,
                                   SPRITEFONTFLAGS flags, D3DCOLOR color,
								   DWORD layer, PRECT fillRect)
{
	if (m_pSpriteBatch == nullptr || m_pTexFontAtlas == nullptr)
	{
		return E_FAIL;
	}

	HRESULT hr;

	if (fillRect != nullptr)
	{
		RECT rect{0,0,0,0};
		long pos = 0;
		
		ForEachGlyph(text, fontSize, flags & ~SPRITEFONTFLAG_OUTLINE,
		[&](RECT box, D3DCOLOR overwriteColor)
		{
			UNREFERENCED_PARAMETER(overwriteColor);

			long h = box.bottom - box.top;
			if(h > rect.bottom)
			{
				rect.bottom = h;
			}

			if(pos < fillRect->left)
			{
				rect.left += box.right - box.left;
			}
			else if(pos < fillRect->right)
			{
				rect.right += box.right - box.left;
			}

			pos++;
		});
				
		hr = m_pSpriteBatch->Draw(
			m_pSelectionPixel,
			nullptr,
			&D3DXVECTOR2(rect.right, rect.bottom),
			nullptr,
			0,
			&D3DXVECTOR2(dst.x + rect.left, dst.y + rect.top),
			0xAAAAAAAA);

		if (FAILED(hr))
		{
#ifdef CLIENT_CONSOLE
			printf("CD3DSpriteFont::DrawString for Draw Rect failed %d\n", hr);
#endif
			return hr;
		}
	}

	long x = 0;

	if (flags & SPRITEFONTFLAG_OUTLINE)
	{
		ForEachGlyph(text, fontSize, flags,
		[&](RECT box, D3DCOLOR overwriteColor)
		{
			UNREFERENCED_PARAMETER(overwriteColor);

			m_pSpriteBatch->Draw(
				m_pTexFontAtlas,
				&box,
				nullptr,
				nullptr,
				0,
				&D3DXVECTOR2(dst.x + float(x), dst.y),
				D3DCOLOR_ARGB(255, 1, 1, 1), // full black
				layer);

			x += box.right - box.left - 2;
		});

		flags &= ~SPRITEFONTFLAG_OUTLINE;
	}

	x = 0;
	
	ForEachGlyph(text, fontSize, flags,
	[&](RECT box, D3DCOLOR overwriteColor)
	{
		m_pSpriteBatch->Draw(
			m_pTexFontAtlas,
			&box,
			nullptr,
			nullptr,
			0,
			&D3DXVECTOR2(dst.x + float(x) + 1, dst.y + 1),
			overwriteColor > 0 ? overwriteColor : color,
			layer);

		x += box.right - box.left;
	});
	return S_OK;
}

SIZE CD3DSpriteFont::MeasureString(LPCSTR text, char fontSize, SPRITEFONTFLAGS flags)
{
	SIZE size{0, 0};
	flags &= ~SPRITEFONTFLAG_OUTLINE;
	ForEachGlyph(text, fontSize, flags,
	[&](RECT box, D3DCOLOR overwriteColor)
	{
		UNREFERENCED_PARAMETER(overwriteColor);
		long h = box.bottom - box.top;
		if(h > size.cy)
		{
			size.cy = h;
		}
		size.cx += box.right - box.left;
	});
	return size;
}

RECT CD3DSpriteFont::FindGlyph(TCHAR character, char fontSize, SPRITEFONTFLAGS flags)
{
	return m_mapGlyphs[(character << 16 | fontSize << 8 | flags)];
}

template <typename TAction>
void CD3DSpriteFont::ForEachGlyph(LPCSTR text, char fontSize, SPRITEFONTFLAGS flags, TAction action)
{
	D3DCOLOR color = 0;

	for (; *text; text++)
	{
		TCHAR character = *text;

		switch (character)
		{
		case '\\':
			{
				TCHAR nextChar = *(text + 1);
				if (nextChar)
				{
					D3DCOLOR value = GetFontColor(nextChar);
					if (value > 0)
					{
						// this masking is temporary until everything is replaced
						value |= 0xff000000;
						if (color == value)
						{
							color = 0;
						}
						else
						{
							color = value;
						}
						text++;
						continue;
					}
				}
			}
			break;
		case '\r':
			// skip carriage returns
			continue;
		}

		RECT box = FindGlyph(character, fontSize, flags);
		bool isNotEmpty = !iswspace(character) || 
			(box.right - box.left) > 1 ||
			(box.bottom - box.top) > 1;
		if(isNotEmpty){
			action(box, color);
		}
	}
}

HRESULT CD3DSpriteFont::GenerateGlyphSet(char fontSize, USHORT startCharacter, USHORT endCharacter, SPRITEFONTFLAGS flag)
{
	HDC hdc = g_pApp->GetHDC();

	int nHeight = -MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);

	HFONT hFont = CreateFont(nHeight, 0, 0, 0,
	                         (flag & SPRITEFONTFLAG_BOLD) ? FW_BOLD : 0,
	                         (flag & SPRITEFONTFLAG_ITALIC) ? 1 : 0,
	                         0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
	                         CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
	                         FIXED_PITCH, m_sFontName);

	if (hFont == nullptr)
	{
#ifdef CLIENT_CONSOLE
		printf("CD3DSpriteFont::GenerateGlyphSet GDI CreateFont failed\n");
#endif
		return E_FAIL;
	}

	HGDIOBJ hOldFont = SelectObject(hdc, hFont);

	TCHAR str[2] = _T("x");
	SIZE size;
	
#ifdef CLIENT_CONSOLE
	printf("CD3DSpriteFont::GenerateGlyphSet from %c[%d] to %c[%d]\n", startCharacter, startCharacter, endCharacter, endCharacter);
#endif

	for (USHORT i = startCharacter; i <= endCharacter; i++)
	{
		str[0] = i;
		GetTextExtentPoint32(hdc, str, 1, &size);

		if (m_pAtlasMarker.x + SPRITE_FONT_TILE_SIZE >= SPRITE_FONT_MAX_TEXTURE_SIZE)
		{
			m_pAtlasMarker.x = 0;
			m_pAtlasMarker.y += SPRITE_FONT_TILE_SIZE;
		}

		if (flag & SPRITEFONTFLAG_OUTLINE)
		{
			size.cx += 2;
			size.cy += 2;
		}

		RECT box{
			m_pAtlasMarker.x,
			m_pAtlasMarker.y,
			m_pAtlasMarker.x + size.cx,
			m_pAtlasMarker.y + size.cy
		};

		/*glyph.offset.x = 0;
		glyph.offset.y = 0;*/
		m_mapGlyphs.emplace((str[0] << 16 | fontSize << 8 | flag), box);

		m_pAtlasMarker.x += SPRITE_FONT_TILE_SIZE;
	}
	
#ifdef CLIENT_CONSOLE
	printf("CD3DSpriteFont::GenerateGlyphSet atlasMarker[%ld,%ld] mapGlyphs.size(%d)\n", m_pAtlasMarker.x,
	       m_pAtlasMarker.y, m_mapGlyphs.size());
#endif

	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);

	return S_OK;
}
