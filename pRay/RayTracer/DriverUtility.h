#pragma once

#include "../CoreSystems/CoreStd.h"
#include "Renderer_RT.h"
#include "D3DStd.h"
#include <D3Dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")

using namespace DirectX;

extern IRenderer* g_pRenderer;

static ID3D11Device* Device()
{
	return dynamic_cast<Renderer_RT*>(g_pRenderer)->GetDriver()->GetDevice();
}

static ID3D11DeviceContext* Context()
{
	return dynamic_cast<Renderer_RT*>(g_pRenderer)->GetDriver()->GetContext();
}

static ID3D11UnorderedAccessView* BackBuffer()
{
	return dynamic_cast<Renderer_RT*>(g_pRenderer)->m_pBackBuffer;
}

static IDXGISwapChain* SwapChain()
{
	return dynamic_cast<Renderer_RT*>(g_pRenderer)->m_driver.GetSwapChain();
}

template<typename RWStruc, uint nElements>
static bool CreateUAV(ErrorMsg& msg, ID3D11UnorderedAccessView* uav, ID3D11ShaderResourceView* srv, ID3D11Buffer* buffer)
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd,sizeof(D3D11_BUFFER_DESC));

	bd.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bd.Usage				= D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags		= 0;
	bd.StructureByteStride	= sizeof(RWStruct);
	bd.ByteWidth			= sizeof(RWStruct)* nElements;
	bd.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	//ID3D11Buffer* buffer = nullptr;

	hr = Device()->CreateBuffer(&bd, NULL, &buffer);

	if (FAILED(hr))
	{
		msg.SetMsg(hr);
		return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
	ZeroMemory(&uavd, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	uavd.Buffer.FirstElement= 0;
	uavd.Buffer.Flags		= 0;
	uavd.Buffer.NumElements	= nElements;
	uavd.Format				= DXGI_FORMAT_UNKNOWN;
	uavd.ViewDimension		= D3D11_UAV_DIMENSION_BUFFER;

	Device()->CreateUnorderedAccessView(buffer, &uavd, &uav);

	if (FAILED(hr))
	{
		msg.SetMsg(hr);
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	srvd.Buffer.FirstElement	= 0;
	srvd.Buffer.ElementOffset	= 0;
	srvd.Buffer.NumElements		= nElements;
	srvd.Format					= DXGI_FORMAT_UNKNOWN;
	srvd.ViewDimension			= D3D11_SRV_DIMENSION_BUFFER;

	Device()->CreateShaderResourceView(buffer, &srvd, &srv);

	return true;
}

static bool StageUAV(ID3D11UnorderedAccessView* uav)
{
	ID3D11UnorderedAccessView* uavs[] = { uav };

	Context()->CSSetUnorderedAccessViews(0, 1, uavs, NULL);

	return true;
}

static bool CreateComputeShader(ErrorMsg& msg, LPCWSTR file, char* entryPoint, ID3D11ComputeShader*& cs)
{
	HRESULT hr = S_OK;

	ID3DBlob* compiledShader= nullptr;
	ID3DBlob* error			= nullptr;
	FILE* shaderBlob		= nullptr;
	TCHAR fileName[300];

	

	DWORD dwsf = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_IEEE_STRICTNESS | D3DCOMPILE_PREFER_FLOW_CONTROL;
#ifdef _DEBUG
	dwsf |= D3DCOMPILE_DEBUG;
	dwsf |= D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	dwsf |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	hr = D3DCompileFromFile(file, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, "cs_5_0", dwsf, NULL, &compiledShader, &error);
	
	if (FAILED(hr))
	{
		size_t s = error->GetBufferSize();
		char* c = new char[s];
		memcpy(c, error->GetBufferPointer(), s);

		msg.SetMsg(c);
		return false;
	}

	if (error)
	{
		msg.SetMsg((LPCTSTR)error->GetBufferPointer());
		return false;
	}

	hr = Device()->CreateComputeShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), nullptr, &cs);

	if (FAILED(hr))
	{
		msg.SetMsg(hr);
		return false;
	}

	SAFE_RELEASE(error);
	SAFE_RELEASE(compiledShader);

	return true;
}

static bool CreateUABuffer(ErrorMsg& msg, void** bufferData, ID3D11Buffer*& buffer, uint bufferSize, uint bufferStride)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC uad;
	ZeroMemory(&uad, sizeof(D3D11_BUFFER_DESC));

	uad.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	uad.ByteWidth			= bufferSize;
	uad.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	uad.StructureByteStride = bufferStride;
	uad.Usage				= D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(D3D11_SUBRESOURCE_DATA));

	sd.pSysMem = bufferData;
	hr = Device()->CreateBuffer(&uad, &sd, &buffer);

	if (FAILED(hr))
	{
		msg.SetMsg(hr);
		return false;
	}

	return true;
}

static bool DispatchUAV(ID3D11UnorderedAccessView** uavs, ID3D11ComputeShader* shader, uint cx = 32, uint cy = 32, uint cz = 1)
{
	HRESULT hr = S_OK;

	Context()->CSSetShader(shader, nullptr, 0);

	Context()->Dispatch(cx, cy, cz);
	Context()->CSSetShader(nullptr, nullptr, 0);

	return true;
}

static HRESULT CreateTexture(D3D11_TEXTURE2D_DESC& desc, ID3D11Texture2D*& texture)
{
	return Device()->CreateTexture2D(&desc, NULL, &texture);
}

static HRESULT CreateSRV(D3D11_SHADER_RESOURCE_VIEW_DESC& desc, ID3D11Texture2D* texture, ID3D11ShaderResourceView*& srv)
{
	return Device()->CreateShaderResourceView(texture, &desc, &srv);
}

static HRESULT CreateBuffer(D3D11_BUFFER_DESC& desc, ID3D11Buffer*& buffer, D3D11_SUBRESOURCE_DATA* initData = nullptr)
{
	return Device()->CreateBuffer(&desc, initData, &buffer);
}

static HRESULT CreateUAV(D3D11_UNORDERED_ACCESS_VIEW_DESC& desc, ID3D11Resource* resource, ID3D11UnorderedAccessView*& uav)
{
	return Device()->CreateUnorderedAccessView(resource, &desc, &uav);
}

template<typename T>
static HRESULT UpdateSubResource(ID3D11Resource* resource, T* data)
{
	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE msr;

	hr = Context()->Map(resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	if (FAILED(hr))
		return hr;

	msr.pData = data;

	Context()->Unmap(resource, 0);

	return hr;
}