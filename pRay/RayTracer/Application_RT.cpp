#include "Application_RT.h"

#include "../Global/SimpleInput.h"

Application_RT::Application_RT()
: m_appName("BTH 2014- RayTracer"), m_resolution(Resolution(800,600))
{

}

Application_RT::~Application_RT()
{

}

bool Application_RT::VClose(ErrorMsg& msg)
{
	if (m_pInput)
	{
		delete m_pInput;
	}
	m_pInput = nullptr;

	return true;
}

bool Application_RT::VInit(ErrorMsg& msg)
{
	m_pInput = new SimpleInput();
	if (!m_pInput || !m_pInput->VInit())
	{
		msg = ErrorMsg("Failed to initialize input handling");
		return false;
	}

	return true;
}

bool Application_RT::VUpdate(Time time)
{
	if (m_pInput->VIsKeyDown(VK_ESCAPE))
		return false;

	return true;
}

LRESULT Application_RT::ApplicationProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
	{
		m_pInput->VKeyDown((uint)wParam);
		break;
	}
	case WM_KEYUP:
	{
		m_pInput->VKeyUp((uint)wParam);
		break;
	}
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}