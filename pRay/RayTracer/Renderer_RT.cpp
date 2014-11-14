#include "Renderer_RT.h"

Renderer_RT::Renderer_RT()
	: m_pBackBuffer(nullptr)
{ }

Renderer_RT::~Renderer_RT()
{
	SAFE_RELEASE(m_pBackBuffer);

	m_driver.Close();
}

bool Renderer_RT::VInit(ErrorMsg& msg, HWND& hWnd, Resolution resolution)
{
	HRESULT hr		= S_OK;
	m_resolution	= resolution;

	hr = m_driver.InitDevice(resolution, hWnd, true);

	if (FAILED(hr))
	{
		Print(hr, "D3D11 Driver Init Failure");
		return false;
	}
	
	hr = m_driver.CreateBackBuffer(m_pBackBuffer);

	if (FAILED(hr))
	{
		Print(hr, "BackBuffer Init Failure");
		return false;
	}

	return true;
}

bool Renderer_RT::VClose(ErrorMsg& msg)
{

	return true;
}

bool Renderer_RT::VDraw(Time time)
{
	HRESULT hr = S_OK;
	ID3D11UnorderedAccessView* uav[] = { m_pBackBuffer };
	m_driver.GetContext()->CSSetUnorderedAccessViews(0, 1, uav, NULL);

	hr = m_driver.GetSwapChain()->Present(0, 0);

	if (FAILED(hr))
		return false;

	return true;
}