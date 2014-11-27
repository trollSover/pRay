#include "Application_RT.h"

#include "../Global/SimpleInput.h"
#include "DriverUtility.h"
#include "Ray.h"

#include <vector>
#include "D3DStd.h"

struct Intersection
{
	int id;
	float u, v, t;

	Intersection()
		: id(-1), t(0), u(0), v(0)
	{}
};

Application_RT::Application_RT()
	: m_appName("BTH 2014- RayTracer"), m_resolution(Resolution(800,600))
{
	m_vertexBuffer		= nullptr;
	m_primRayShader		= nullptr;
	m_intersectionShader= nullptr;
	m_colorShader		= nullptr;
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

	if (!CreateComputeShader(msg, L"RayTracer/CSPrimaryRays.hlsl", "CSGeneratePrimaryRays", m_primRayShader))
	{
		Print(msg);
		return false;
	}
	if (!CreateComputeShader(msg, L"RayTracer/CSIntersections.hlsl", "CSComputeIntersections", m_intersectionShader))
	{
		Print(msg);
		return false;
	}
	if (!CreateComputeShader(msg, L"RayTracer/CSComputeColorRT.hlsl", "CSComputeColor", m_colorShader))
	{
		Print(msg);
		return false;
	}
	
	std::vector<Vertex> vertices;
	Vertex v[8] =
	{
		Vertex{ VECTOR4(-1, 1, 1, 1), {} },
		Vertex{ VECTOR4(-1, -1, 1, 1), {} },
		Vertex{ VECTOR4(1, -1, 1, 1), {} },
		Vertex{ VECTOR4(1, 1, 1, 1), {} },
		Vertex{ VECTOR4(-1, 1, -1, 1), {} },
		Vertex{ VECTOR4(-1, -1, -1, 1), {} },
		Vertex{ VECTOR4(1, -1, -1, 1), {} },
		Vertex{ VECTOR4(1, 1, -1, 1), {} },
	};

	uint i[36] =
	{	0, 1, 2,	0, 2, 3,	// front
		4, 5, 6,	4, 6, 7,	// back
		0, 1, 5,	0, 5, 4,	// left
		3, 2, 6,	3, 6, 7,	// right
		0, 3, 7,	0, 7, 4,	// top
		1, 2, 6,	1, 6, 5,	// bottom
	};

	/* SRVS */
	if (!m_vertices.Init(msg, BT_STRUCTURED, BB_SRV, 8, sizeof(Vertex), &v[0]))								
		return false;
	if (!m_indices.Init(msg, BT_STRUCTURED, BB_SRV, 36, sizeof(uint), &i[0]))									
		return false;

	/* UAVS */
	if (!m_rays.Init(msg, BT_STRUCTURED, BB_UAV, m_resolution.height * m_resolution.width, sizeof(Ray)))	
		return false;
	if (!m_cbCamera.Init(msg, BT_STRUCTURED, BB_CONSTANT, 1, sizeof(MATRIX4X4)))							
		return false;
	if (!m_intersections.Init(msg, BT_STRUCTURED, BB_UAV, m_resolution.height * m_resolution.width, sizeof(Intersection)))
		return false;
	if (!m_colorAccumulation.Init(msg, BT_STRUCTURED, BB_UAV, m_resolution.height * m_resolution.width, sizeof(VECTOR4)))
		return false;

	ID3D11ShaderResourceView* srvs[] = { m_vertices.GetSRV(), m_indices.GetSRV()};
	Context()->CSSetShaderResources(0, 2, srvs);

	ID3D11UnorderedAccessView* uavs[] = { BackBuffer(), m_rays.GetUAV(), m_intersections.GetUAV(), m_colorAccumulation.GetUAV() };
	Context()->CSSetUnorderedAccessViews(0, 4, uavs, NULL);

	return true;
}

bool Application_RT::VUpdate(Time time)
{
	if (m_pInput->VIsKeyDown(VK_ESCAPE))
		return false;

	UpdateCamera(time);

	HRESULT hr = S_OK;
	//ID3D11UnorderedAccessView* uav[] = { m_rays.GetUAV() };
	//Context()->CSSetUnorderedAccessViews(0, 1, uav, NULL);

	hr = UpdateSubResource<XMFLOAT4X4>(m_cbCamera.GetBuffer(), m_xmCamera.getInverseMatrix());

	if (FAILED(hr))
		return false;

	// dispatch primary rays
	DispatchUAV(m_primRayShader);

	// intersect & color
	for (int i = 0; i < 2; i++)
	{
		DispatchUAV(m_intersectionShader);
		DispatchUAV(m_colorShader);
	}

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