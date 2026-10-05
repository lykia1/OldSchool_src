#include "stdafx.h"
#include "D3DSpriteBatch.h"
#include "DXUtil.h"

CD3DSpriteBatch::CD3DSpriteBatch()
{
	m_pd3dxSprite = nullptr;
	m_pd3dDevice = nullptr;
	m_iDrawCounts = 0;
	m_iDrawQueue = 0;
	m_bIsBatching = false;
}

CD3DSpriteBatch::~CD3DSpriteBatch() = default;

HRESULT CD3DSpriteBatch::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;
	return S_OK;
}

HRESULT CD3DSpriteBatch::RestoreDeviceObjects()
{
	HRESULT hr;
	
	hr = D3DXCreateSprite(m_pd3dDevice, &m_pd3dxSprite);
	if(FAILED(hr))
	{
#ifdef CLIENT_CONSOLE
		printf("CD3DSpriteBatch::RestoreDeviceObjects for D3DXCreateSprite failed 0x%08x\n", hr);
#endif
		return hr;
	}

		hr = m_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_pd3dStateBlock);
		if(FAILED(hr))
		{
	#ifdef CLIENT_CONSOLE
			printf("CD3DSpriteBatch::RestoreDeviceObjects for m_pd3dDevice->CreateStateBlock failed 0x%08x\n", hr);
	#endif
			return hr;
		}
	
	return S_OK;
}

HRESULT CD3DSpriteBatch::InvalidateDeviceObjects()
{
	SAFE_RELEASE(m_pd3dxSprite);
	SAFE_RELEASE(m_pd3dStateBlock);
	return S_OK;
}

HRESULT CD3DSpriteBatch::DeleteDeviceObjects()
{
	m_pd3dDevice = nullptr;
	return S_OK;
}

HRESULT CD3DSpriteBatch::Begin()
{
	//return S_OK;
	
	if(m_pd3dxSprite == nullptr)
	{
		return E_POINTER;
	}
	
	HRESULT hr;
	
	hr = BeginInternal();	
	if(FAILED(hr))
	{
#ifdef CLIENT_CONSOLE
		printf("CD3DSpriteBatch::Begin for BeginInternal failed 0x%08x\n", hr);
#endif
		return hr;
	}

	m_bIsBatching = true;
	
	return hr;
}

void CD3DSpriteBatch::TranslateToMatrix(D3DXMATRIX* pMatrix, const D3DXVECTOR2* pScaling, const D3DXVECTOR2* pRotationCenter, float rotationAngle, const D3DXVECTOR2* pTranslation, FLOAT depth)
{
	D3DXMatrixTransformation2D(pMatrix,
		nullptr,
		0,
		pScaling, 
		pRotationCenter,
		-rotationAngle, // negative as 9.0b to 9.0c rotation fix
		pTranslation);
	if(depth > 0)
	{
		D3DXMatrixTranslation(pMatrix, 0, 0, depth);
	}
}

HRESULT CD3DSpriteBatch::Draw(LPDIRECT3DTEXTURE9 pTexture, const RECT* pSrcRect, const D3DXVECTOR2* pScaling, const D3DXVECTOR2* pRotationCenter, float rotationAngle, const D3DXVECTOR2* pTranslation, D3DCOLOR color, FLOAT depth)
{
	D3DXMATRIX mat;
	TranslateToMatrix(&mat, pScaling, pRotationCenter, rotationAngle, pTranslation, depth);

	return Draw(pTexture, pSrcRect, &mat, color);
}

HRESULT CD3DSpriteBatch::Draw(LPDIRECT3DTEXTURE9 pTexture, const RECT* pSrcRect, const D3DXMATRIX* pMatrix, D3DCOLOR color)
{
	if(m_pd3dxSprite == nullptr)
	{
		return E_POINTER;
	}

	if(color == 0)
	{
#ifdef CLIENT_CONSOLE
		printf("CD3DSpriteBatch::Draw zero color received\n");
#endif
		return S_OK;
	}
	
	m_iDrawCounts++;
	
	HRESULT hr;

	if(m_bIsBatching == false)
	{
		hr = m_pd3dStateBlock->Capture();
		if(FAILED(hr))
		{
#ifdef CLIENT_CONSOLE
			printf("CD3DSpriteBatch::Draw for m_pd3dStateBlock->Capture failed 0x%08x\n", hr);
#endif
			return hr;
		}
		
		hr = BeginInternal();
		if(FAILED(hr))
		{
#ifdef CLIENT_CONSOLE
			printf("CD3DSpriteBatch::Draw for BeginInternal failed 0x%08x\n", hr);
#endif
			return hr;
		}
		m_iNotBatchedDrawCounts++;
	}
	
	
	m_pd3dxSprite->SetTransform(pMatrix);
	hr = m_pd3dxSprite->Draw(pTexture, pSrcRect, NULL, NULL, color);

	if(FAILED(hr))
	{
#ifdef CLIENT_CONSOLE
		printf("CD3DSpriteBatch::Draw for m_pd3dxSprite->Draw failed 0x%08x\n", hr);
#endif
		return hr;
	}

	m_iDrawQueue++;

	if(m_bIsBatching == false)
	{
		hr = EndInternal();
		if(FAILED(hr))
		{
#ifdef CLIENT_CONSOLE
			printf("CD3DSpriteBatch::Draw for EndInternal failed 0x%08x\n", hr);
#endif
			return hr;
		}
		
		hr = m_pd3dStateBlock->Apply();
		if(FAILED(hr))
		{
#ifdef CLIENT_CONSOLE
			printf("CD3DSpriteBatch::Draw for m_pd3dStateBlock->Apply failed 0x%08x\n", hr);
#endif
			return hr;
		}
	}
	
	return hr;
}

HRESULT CD3DSpriteBatch::End()
{
	//return S_OK;
	
	if(m_pd3dxSprite == nullptr)
	{
		return E_POINTER;
	}

	HRESULT hr;

	if(m_bIsBatching == false)
	{
		return S_OK;
	}
	
	m_bIsBatching = false;
	
	hr = EndInternal();
	if(FAILED(hr))
	{
#ifdef CLIENT_CONSOLE
		printf("CD3DSpriteBatch::End for EndInternal failed 0x%08x\n", hr);
#endif
		return hr;
	}
	
	return hr;
}

HRESULT CD3DSpriteBatch::Flush()
{
	if(m_iDrawQueue > 0)
	{
		HRESULT hr = m_pd3dxSprite->Flush();
		if(FAILED(hr))
		{
#ifdef CLIENT_CONSOLE
			printf("CD3DSpriteBatch::Flush for m_pd3dxSprite->Flush failed 0x%08x\n", hr);
#endif
			return hr;
		}
		m_iDrawQueue = 0;
	}

	return S_OK;
}

HRESULT CD3DSpriteBatch::BeginInternal() const
{
	HRESULT hr;
		hr = m_pd3dxSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);	
		if(FAILED(hr))
		{
	#ifdef CLIENT_CONSOLE
			printf("CD3DSpriteBatch::BeginInternal for m_pd3dxSprite->Begin failed 0x%08x\n", hr);
	#endif
			return hr;
		}

		/*m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTA_TEXTURE );*/
	
	return S_OK;
}

HRESULT CD3DSpriteBatch::EndInternal()
{
	HRESULT hr;
	if(m_pd3dxSprite){
		hr = m_pd3dxSprite->End();
		if(FAILED(hr))
		{
	#ifdef CLIENT_CONSOLE
			printf("CD3DSpriteBatch::EndInternal for m_pd3dxSprite->End failed 0x%08x\n", hr);
	#endif
			return hr;
		}
	}
	m_iDrawQueue = 0;
	
	return hr;
}

