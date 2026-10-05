#pragma once
#include "stdafx.h"

constexpr auto D3DCOLOR_FULL = 0xFFFFFFFF;

/*enum SpriteEffects : uint32_t
{
	SpriteEffects_None = 0,
	SpriteEffects_FlipHorizontally = 1,
    SpriteEffects_FlipVertically = 2,
    SpriteEffects_FlipBoth = SpriteEffects_FlipHorizontally | SpriteEffects_FlipVertically,
    
};*/

class CD3DSpriteBatch
{
	/*struct SpriteInfo
	{
		RECT source;
		RECT destination; // as translation
		D3DXVECTOR2 scaling;
		D3DXVECTOR2 rotationCenter;
		float rotationAngle;
		D3DCOLOR color;
		//DWORD flags;
		LPDIRECT3DTEXTURE9 texture;
	};*/
public:
	CD3DSpriteBatch();
	~CD3DSpriteBatch();
	
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();

	HRESULT Begin();

	static void TranslateToMatrix(D3DXMATRIX* pMatrix, const D3DXVECTOR2* pScaling, const D3DXVECTOR2* pRotationCenter, float rotationAngle, const D3DXVECTOR2* pTranslation, FLOAT depth);
	
	/*
	 * D3DColor: The color and alpha channels are modulated by this value. A value of 0xFFFFFFFF maintains the original source color and alpha data. Use the D3DCOLOR_ARGB macro to help generate this color.
	 */
	HRESULT Draw(
		LPDIRECT3DTEXTURE9 pTexture,
		const RECT* pSrcRect,
		const D3DXVECTOR2* pScaling,
		const D3DXVECTOR2* pRotationCenter,
		float rotationAngle,
		const D3DXVECTOR2 *pTranslation,
		D3DCOLOR color = D3DCOLOR_FULL,
		FLOAT depth = 0);

	HRESULT Draw(LPDIRECT3DTEXTURE9 pTexture,
				const RECT* pSrcRect,
				const D3DXMATRIX* pMatrix,
				D3DCOLOR color = D3DCOLOR_FULL);
	
	HRESULT End();
	HRESULT Flush();
	int GetDrawCounts(){ return m_iDrawCounts; }
	void ResetDrawCounts(){ m_iDrawCounts = 0; }
	int GetNotBatchedDrawCounts(){ return m_iNotBatchedDrawCounts; }
	void ResetNotBatchedDrawCounts(){ m_iNotBatchedDrawCounts = 0; }
	
	
private:
	HRESULT BeginInternal() const;
	HRESULT EndInternal();
	
	/*vector<CD3DSpriteBatch>	m_vecLayers;
	vector<SpriteInfo>		m_vecQueue;*/
	LPD3DXSPRITE			m_pd3dxSprite;
	LPDIRECT3DDEVICE9		m_pd3dDevice;
	LPDIRECT3DSTATEBLOCK9	m_pd3dStateBlock;
	UINT					m_iDrawCounts;
	UINT					m_iNotBatchedDrawCounts;
	UINT					m_iDrawQueue;
	bool					m_bIsBatching;
};
