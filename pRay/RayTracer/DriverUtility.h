#pragma once

#include "../CoreSystems/CoreStd.h"
#include "Renderer_RT.h"
#include "D3DStd.h"
#include <D3Dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")

using namespace DirectX;

extern IRenderer* g_pRenderer;

ID3D11Device* Device()
{
	return dynamic_cast<Renderer_RT*>(g_pRenderer)->GetDriver()->GetDevice();
}

ID3D11DeviceContext* Context()
{
	return dynamic_cast<Renderer_RT*>(g_pRenderer)->GetDriver()->GetContext();
}

ID3D11UnorderedAccessView* BackBuffer()
{
	return dynamic_cast<Renderer_RT*>(g_pRenderer)->m_pBackBuffer;
}

IDXGISwapChain* SwapChain()
{
	return dynamic_cast<Renderer_RT*>(g_pRenderer)->m_driver.GetSwapChain();
}

template<typename RWStruc, uint nElements>
bool CreateUAV(ErrorMsg& msg, ID3D11UnorderedAccessView* uav, ID3D11ShaderResourceView* srv, ID3D11Buffer* buffer)
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

bool StageUAV(ID3D11UnorderedAccessView* uav)
{
	ID3D11UnorderedAccessView* uavs[] = { uav };

	Context()->CSSetUnorderedAccessViews(0, 1, uavs, NULL);

	return true;
}

bool CreateComputeShader(ErrorMsg& msg, LPCWSTR file, char* entryPoint, ID3D11ComputeShader*& cs)
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

	hr = D3DCompileFromFile(file, NULL, NULL, entryPoint, "cs_5_0", dwsf, NULL, &compiledShader, &error);
	
	if (FAILED(hr))
	{
		msg.SetMsg(hr);
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

bool CreateUABuffer(ErrorMsg& msg, void** bufferData, ID3D11Buffer*& buffer, uint bufferSize, uint bufferStride)
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