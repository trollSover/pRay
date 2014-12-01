#pragma once

#include "../CoreSystems/IApplication.h"
#include "XMCamera.h"
#include "../Global/SimpleInput.h"
#include "D3DBuffer.h"

#include "../include/WICTextureLoader/DirectXTex.h"
#pragma comment(lib, "DirectXTex.lib")

struct cbCamera
{
	XMFLOAT4X4 g_mfWorld;

	cbCamera()
	{
		XMStoreFloat4x4(&g_mfWorld, XMMatrixIdentity());
	}

	void Update(XMFLOAT4X4 matrix)
	{
		g_mfWorld = matrix;
	}
};

class Application_RT
	: public IApplication
{
private:
	Resolution		m_resolution;
	std::string		m_appName;
	SimpleInput*	m_pInput;
	XMCamera		m_xmCamera;

	ID3D11Buffer*			m_vertexBuffer;
	ID3D11ComputeShader*	m_primRayShader;
	ID3D11ComputeShader*	m_intersectionShader;
	ID3D11ComputeShader*	m_colorShader;
	D3DBuffer				m_rays;
	D3DBuffer				m_intersections;
	D3DBuffer				m_vertices;
	D3DBuffer				m_indices;
	D3DBuffer				m_cbCamera;
	D3DBuffer				m_colorAccumulation;

	D3DBuffer				m_textureSmiley;

	ID3D11SamplerState*		m_linearSampler;

protected:
public:

private:
	Application_RT(const Application_RT&) = delete;

	bool	VInit(ErrorMsg&)	final;
	bool	VUpdate(Time time)	final;
	bool	VClose(ErrorMsg&)	final;

	Resolution		VGetResolution()	const final { return m_resolution; }
	virtual const	LPCSTR VGetAppName()const final { return m_appName.c_str(); }

	LRESULT	ApplicationProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) final;

	void UpdateCamera(const Time& time);

protected:
public:
	Application_RT();
	~Application_RT();
};