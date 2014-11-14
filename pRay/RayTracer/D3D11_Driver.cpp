#include "D3D11_Driver.h"

D3D11_Driver::D3D11_Driver()
	: m_pDevice(nullptr), m_pContext(nullptr), m_pSwapChain(nullptr)
{ }

D3D11_Driver::~D3D11_Driver()
{
	Close();
}

void D3D11_Driver::Close()
{
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);
}

HRESULT D3D11_Driver::InitDevice(Resolution resolution, HWND& hWnd, bool windowed)
{
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount			= 1;
	scd.BufferDesc.Width	= resolution.width;
	scd.BufferDesc.Height	= resolution.height;
	scd.BufferDesc.Format	= DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	scd.OutputWindow		= hWnd;
	scd.SampleDesc.Count	= 1;
	scd.SampleDesc.Quality	= 0;
	scd.Windowed			= windowed;

	scd.BufferDesc.RefreshRate.Numerator	= 60;
	scd.BufferDesc.RefreshRate.Denominator	= 1;

	D3D_DRIVER_TYPE DT;
	D3D_DRIVER_TYPE DTS[]	= { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_SOFTWARE };
	UINT NDT				= sizeof(DTS) / sizeof(D3D_DRIVER_TYPE);

	D3D_FEATURE_LEVEL FLS[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	D3D_FEATURE_LEVEL FL;

	for (uint i = 0; i < NDT; ++i)
	{
		DT = DTS[i];
		hr = D3D11CreateDeviceAndSwapChain(NULL, DT, NULL, createDeviceFlags, FLS, ARRAYSIZE(FLS), D3D11_SDK_VERSION, &scd, &m_pSwapChain, &m_pDevice, &FL, &m_pContext);

		if (SUCCEEDED(hr))
			break;
	}
	return hr;
}

HRESULT D3D11_Driver::CreateBackBuffer(ID3D11UnorderedAccessView* uav)
{
	HRESULT hr					= S_OK;
	ID3D11Texture2D* backBuffer = nullptr;

	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

	if (FAILED(hr))
		return hr;

	hr = m_pDevice->CreateUnorderedAccessView(backBuffer, NULL, &uav);

	return hr;
}