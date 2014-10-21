#pragma once

#include "CoreStd.h"
#include "Window.h"

class IApplication;

class CoreSystem
{
/* Members */
private:
	IApplication*	m_pApplication;
	HINSTANCE		m_hInstance;
	HWND			m_hWnd;

	uint	m_clientW;
	uint	m_clientH;

protected:
public:

/* Functions */
private:
	CoreSystem(const CoreSystem&) = delete;
	Window m_window;

protected:
public:
	CoreSystem();
	~CoreSystem();

	int Init(IApplication* _application, HINSTANCE _hInstance);
	int Close();
	int Run();
};