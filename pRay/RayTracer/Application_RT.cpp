#include "Application_RT.h"

#include "../Global/SimpleInput.h"
#include "Renderer_RT.h"

extern Renderer_RT* g_pRenderer;

Application_RT::Application_RT()
: m_appName("BTH 2014- RayTracer"), m_resolution(Resolution(800,600))
{

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

	return true;
}

bool Application_RT::VUpdate(Time time)
{
	if (m_pInput->VIsKeyDown(VK_ESCAPE))
		return false;

	UpdateCamera(time);

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