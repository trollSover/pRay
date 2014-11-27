#include "D3DBuffer.h"

#include "DriverUtility.h"
#include "D3DBufferUtility.h"

D3DBuffer::D3DBuffer()
: m_pBuffer(nullptr), m_pUAV(nullptr), m_pSRV(nullptr)
{ }

D3DBuffer::~D3DBuffer()
{
	SAFE_RELEASE(m_pBuffer);
	SAFE_RELEASE(m_pUAV);
	SAFE_RELEASE(m_pSRV);
}

HRESULT D3DBuffer::OnBufferType(ErrorMsg& msg, BufferBind bBind, uint nElements, uint sElements, void* initData)
{
	HRESULT hr = S_OK;
	switch (bBind)
	{
	case BB_CONSTANT:
		{
			D3D11_BUFFER_DESC cdesc = GetConstBufferDesc(sElements);
			hr = CreateBuffer(cdesc, (ID3D11Buffer*&)m_pBuffer);
		}
		break;
	default:
		{
			D3D11_BUFFER_DESC ddesc = GetDynamicBufferDesc(sElements, nElements, bBind);
			hr = CreateBuffer(ddesc, (ID3D11Buffer*&)m_pBuffer, (D3D11_SUBRESOURCE_DATA*)initData);

			if (FAILED(hr))
				break;

			if (bBind == BB_UAV || bBind == BB_SRV_UAV)
			{
				D3D11_UNORDERED_ACCESS_VIEW_DESC udesc = GetDynamicUAVDesc(m_pBuffer);
				hr = CreateUAV(udesc, m_pBuffer, m_pUAV);
			}
			if (bBind == BB_SRV || bBind == BB_SRV_UAV)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC sdesc = GetDynamicSRVDesc(m_pBuffer);
				hr = CreateSRV(sdesc, (ID3D11Texture2D*)m_pBuffer, m_pSRV);
			}
		}
		break;
	}

	return hr;
}

bool D3DBuffer::Init(ErrorMsg& msg, BufferType bType, BufferBind bBind, uint nElements, uint sElements, void* initData)
{
	HRESULT hr = S_OK;
	switch (bType)
	{
	case BT_TEXTURE2D:
	{
		D3D11_TEXTURE2D_DESC desc = GetTextureDesc(nElements);
		hr = CreateTexture(desc, (ID3D11Texture2D*&)m_pBuffer);

		if (FAILED(hr))
			break;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc = GetSRVDesc(nElements);
		hr = CreateSRV(srvdesc, (ID3D11Texture2D*&)m_pBuffer, m_pSRV);
	}
		break;
	default:
		hr = OnBufferType(msg, bBind, nElements, sElements, initData);
		break;
	}

	msg.SetMsg(hr);

	if (FAILED(hr))
		return false;

	return true; 
}