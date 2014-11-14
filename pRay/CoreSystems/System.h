#pragma once

#include "CoreStd.h"
#include "Window.h"
#include "ITimer.h"

class IApplication;
class IRenderer;

class CoreSystem
{
/* Members */
private:
	IApplication*	m_pApplication;
	IRenderer*		m_pRenderer;
	ITimer*			m_pTimer;
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

	Resolution	GetResolution() { return Resolution(m_clientW, m_clientH); }
	HWND&		GetHWND() { return m_hWnd; }

protected:
public:
	CoreSystem();
	~CoreSystem();

	int Init(IApplication* _application, IRenderer* _renderer, HINSTANCE _hInstance);
	int Close();
	int Run();
};