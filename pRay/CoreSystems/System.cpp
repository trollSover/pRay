#include "System.h"
#include "IApplication.h"
#include "IRenderer.h"
#include "../Global/TimerQPC.h"

CoreSystem::CoreSystem()
: m_pApplication(nullptr), m_pRenderer(nullptr), m_pTimer(nullptr)
{

}

CoreSystem::~CoreSystem()
{

}

int CoreSystem::Close()
{
	ErrorMsg emsg;

	if (m_pApplication)
	{
		if (!m_pApplication->VClose(emsg))
			printf("System Close: %s\n", emsg.Msg().c_str());

		delete m_pApplication;
	}

	m_pApplication = nullptr;

	if (m_pRenderer)
	{
		if (!m_pRenderer->VClose(emsg))
			printf("System Close: %s\n", emsg.Msg().c_str());

		delete m_pRenderer;
	}

	m_pRenderer = nullptr;

	return CommonMsg::CM_OK;
}

int CoreSystem::Init(IApplication* _application, IRenderer* _renderer, HINSTANCE _hInstance)
{
	if (_application == nullptr)
		return CommonMsg::CM_APP_NULL;
	if (_renderer == nullptr)
		return CommonMsg::CM_GFX_NULL;

	ErrorMsg emsg;
	m_hInstance		= _hInstance;
	m_pApplication	= _application;
	m_pRenderer		= _renderer;

	/* Initialize Timer */
	m_pTimer = new TimerQPC();
	if (m_pTimer == nullptr || !m_pTimer->VInit())
		return CommonMsg::CM_TIME_FAIL;

	/* Initialize Window Properties */
	if (!m_window.Init(m_hInstance, m_hWnd, m_pApplication->VGetResolution(), m_pApplication->VGetAppName()))
	{
		Print("System Init: Failed to open window");
		return CommonMsg::CM_WIN_FAIL;
	}
	/* Intialize Renderer */
	if (!m_pRenderer->VInit(emsg, m_hWnd, Resolution(m_clientW, m_clientH)))
	{
		Print(emsg, "System Init");
		return CommonMsg::CM_GFX_FAIL;
	}
	/* Initialize Application*/
	if (!m_pApplication->VInit(emsg))
	{
		Print(emsg, "System Init");
		return CommonMsg::CM_APP_FAIL;
	}

	return CommonMsg::CM_OK;
}

int CoreSystem::Run()
{
	bool run = true;
	
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	m_pTimer->VUpdate();	/* reset the timer */

	while (run)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			run = false;
		}
		else
		{
			Time dt = m_pTimer->VGetTime();

			run = m_pApplication->VUpdate(dt);
			//run = m_pRenderer->VDraw(time);
			m_pRenderer->VDraw(dt);
			// check if we can recover
		}	
		m_pTimer->VUpdate();
	}

	return CommonMsg::CM_OK;
}