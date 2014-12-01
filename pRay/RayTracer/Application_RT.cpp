#include "Application_RT.h"

#include "../Global/SimpleInput.h"
#include "DriverUtility.h"
#include "Ray.h"

#include <vector>
#include "D3DStd.h"

#include <comdef.h>
#include "../include/WICTextureLoader.h"

using namespace DirectX;

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
		Vertex(10.0f, VECTOR3(-1, 1, 1),	VECTOR3(-0.5, 0.5, -0.5),	VECTOR2(0, 1)),
		Vertex(10.0f, VECTOR3(-1, -1, 1),	VECTOR3(-0.5, -0.5, -0.5),	VECTOR2(0, 0)),
		Vertex(10.0f, VECTOR3(1, -1, 1),	VECTOR3(0.5, -0.5, -0.5),	VECTOR2(1, 0)),
		Vertex(10.0f, VECTOR3(1, 1, 1),		VECTOR3(0.5, 0.5, -0.5),	VECTOR2(1, 1)),
		Vertex(10.0f, VECTOR3(-1, 1, -1),	VECTOR3(-0.5, 0.5, 0.5),	VECTOR2(0, 1)),
		Vertex(10.0f, VECTOR3(-1, -1, -1),	VECTOR3(-0.5, -0.5, 0.5),	VECTOR2(0, 0)),
		Vertex(10.0f, VECTOR3(1, -1, -1),	VECTOR3(0.5, -0.5, -0.5),	VECTOR2(1, 0)),
		Vertex(10.0f, VECTOR3(1, 1, -1),	VECTOR3(0.5, 0.5, -0.5),	VECTOR2(1, 1))
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
	if (!m_cbCamera.Init(msg, BT_STRUCTURED, BB_CONSTANT, 1, sizeof(XMFLOAT4X4)))
		return false;
	if (!m_intersections.Init(msg, BT_STRUCTURED, BB_UAV, m_resolution.height * m_resolution.width, sizeof(Intersection)))
		return false;
	if (!m_colorAccumulation.Init(msg, BT_STRUCTURED, BB_UAV, m_resolution.height * m_resolution.width, sizeof(VECTOR4)))
		return false;

	ID3D11Texture2D* smiley;
	D3D11_TEXTURE2D_DESC dstex;
	ZeroMemory(&dstex, sizeof(dstex));
	dstex.Width = 1024;
	dstex.Height = 1024;
	dstex.MipLevels = 1;
	dstex.ArraySize = 1;
	dstex.SampleDesc.Count = 1;
	dstex.SampleDesc.Quality = 0;
	dstex.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dstex.Usage = D3D11_USAGE_DEFAULT;
	dstex.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	dstex.CPUAccessFlags = 0;
	dstex.MiscFlags = 0;

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		return false;
	}

	ID3D11Resource* tres;
	ID3D11ShaderResourceView* tsrv;
	
	
	ID3D11ShaderResourceView* srv = nullptr;
	hr = CreateWICTextureFromFile(Device(), L"smiley.jpg", NULL, (ID3D11ShaderResourceView**)&srv);
	bool b = m_textureSmiley.Init(msg, BT_TEXTURE2D, BB_SRV, 1, 675 * 647 * sizeof(float)* 4, srv);

	ID3D11ShaderResourceView* srvs[] = { m_vertices.GetSRV(), m_indices.GetSRV(), m_textureSmiley.GetSRV() };
	Context()->CSSetShaderResources(0, 3, srvs);

	ID3D11UnorderedAccessView* uavs[] = { BackBuffer(), m_rays.GetUAV(), m_intersections.GetUAV(), m_colorAccumulation.GetUAV() };
	Context()->CSSetUnorderedAccessViews(0, 4, uavs, NULL);

	ID3D11Buffer* cbuffers[] = { (ID3D11Buffer*)m_cbCamera.GetBuffer() };
	Context()->CSSetConstantBuffers(0, 1, cbuffers);

	m_linearSampler = nullptr;
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = Device()->CreateSamplerState(&sampDesc, &m_linearSampler);

	ID3D11SamplerState* samplers[] = { m_linearSampler };
	Context()->CSSetSamplers(0,1,samplers);

	if (FAILED(hr))
		return false;

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

	hr = UpdateSubResource<XMFLOAT4X4, cbCamera>(m_cbCamera.GetBuffer(), m_xmCamera.getInverseMatrix());


	if (FAILED(hr))
		return false;

	// dispatch primary rays
	DispatchUAV(m_primRayShader);

	// intersect & color
	for (int i = 0; i < 1; i++)
	{
		DispatchUAV(m_intersectionShader);
		DispatchUAV(m_colorShader);
	}

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
	m_pInput->VIsKeyDown('D') ? m_xmCamera.setMovementToggle(3, 1)  : m_xmCamera.setMovementToggle(3, 0);

	/* Mouse Look */
	if (m_pInput->MouseMoved())
	{
		m_xmCamera.adjustHeadingPitch(mul * m_pInput->m_mousePoint.x, mul * m_pInput->m_mousePoint.y);
		m_pInput->m_mousePoint.x = 0;
		m_pInput->m_mousePoint.y = 0;
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