#include "Application_RT.h"

#include "../Global/SimpleInput.h"
#include "DriverUtility.h"
#include "Ray.h"

#include <vector>
#include "D3DStd.h"

Application_RT::Application_RT()
	: m_appName("BTH 2014- RayTracer"), m_resolution(Resolution(800,600))
{
	m_vertexBuffer  = nullptr;
	m_primRays		= nullptr;
}

Application_RT::~Application_RT()
{

}

bool Application_RT::VClose(ErrorMsg& msg)
{
	SAFE_DELETE(m_pInput);

	return true;
}

bool Application_RT::VInit(ErrorMsg& msg)
{
	m_pInput = new SimpleInput();
	if (!m_pInput || !m_pInput->VInit())
	{
		msg.SetMsg("Failed to initialize input handling");
		return false;
	}

	if (!m_xmCamera.init(m_resolution.width, m_resolution.height))
	{
		msg.SetMsg("Failed to initialize camera");
		return false;
	}

	if (!CreateComputeShader(msg, L"RayTracer/CSPrimaryRays.hlsl", "CSGeneratePrimaryRays", m_primRays))
	{
		Print(msg);
		return false;
	}
	
	std::vector<Vertex> vertices;
	Vertex v;
	v.position  = VECTOR4(0, 0, 0, 1);
	v.normal	= VECTOR3(0, 1, 0);
	v.uv		= VECTOR2(0, 0);
	vertices.push_back(v);

	//if (!CreateUABuffer(msg, (void**)&vertices[0], m_vertexBuffer, vertices.size() * sizeof(Vertex), sizeof(Vertex)))
	//{
	//	Print(msg);
	//	return false;
	//}

	if (!m_rays.Init(msg, BT_STRUCTURED, BB_UAV, m_resolution.height * m_resolution.width, sizeof(Ray)))	return false;
	if (!m_cbCamera.Init(msg, BT_STRUCTURED, BB_CONSTANT, 1, sizeof(MATRIX4X4)))							return false;

	return true;
}

bool Application_RT::VUpdate(Time time)
{
	if (m_pInput->VIsKeyDown(VK_ESCAPE))
		return false;

	UpdateCamera(time);

	HRESULT hr = S_OK;
	ID3D11UnorderedAccessView* uav[] = { m_rays.GetUAV() };
	Context()->CSSetUnorderedAccessViews(0, 1, uav, NULL);

	hr = UpdateSubResource<MATRIX4X4>(m_cbCamera.GetBuffer(), &m_xmCamera.getInverseMatrix());

	if (FAILED(hr))
		return false;

	// dispatch primary rays
	DispatchUAV(uav, m_primRays);
	//Context()->CSSetShader(m_testShader, nullptr, 0);
	//Context()->Dispatch(32, 32, 1);
	//Context()->CSSetShader(nullptr, nullptr, 0);
	hr = SwapChain()->Present(0, 0);

	if (FAILED(hr))
		return false;

	return true;
}

void Application_RT::UpdateCamera(const Time& time)
{
	const float mul = 0.0025f;

	/* WASD Move */
	m_pInput->VIsKeyDown('W') ? m_xmCamera.setMovementToggle(0, 1)  : m_xmCamera.setMovementToggle(0, 0);
	m_pInput->VIsKeyDown('A') ? m_xmCamera.setMovementToggle(2, -1) : m_xmCamera.setMovementToggle(2, 0);
	m_pInput->VIsKeyDown('S') ? m_xmCamera.setMovementToggle(1, -1) : m_xmCamera.setMovementToggle(1, 0);
	m_pInput->VIsKeyDown('D') ? m_xmCamera.setMovementToggle(3, 1)  : m_xmCamera.setMovementToggle(0, 0);

	/* Mouse Look */
	if (m_pInput->MouseMoved())
	{
		m_xmCamera.adjustHeadingPitch(mul * m_pInput->m_mousePoint.x, mul * m_pInput->m_mousePoint.y);
	}

	/* Update Camera */
	m_xmCamera.update(time);
}


LRESULT Application_RT::ApplicationProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INPUT:
		m_pInput->Update(hWnd, (HRAWINPUT&)lParam);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}