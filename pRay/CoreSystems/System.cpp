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
	ErrorMsg emsg("CoreSystem Close");

	if (m_pApplication)
	{
		if (!m_pApplication->VClose(emsg))
			Print(emsg);

		delete m_pApplication;
	}

	m_pApplication = nullptr;

	if (m_pRenderer)
	{
		if (!m_pRenderer->VClose(emsg))
			Print(emsg);

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

	ErrorMsg emsg("CoreSystem Init");

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
		Print("Failed to create window");
		return CommonMsg::CM_WIN_FAIL;
	}
	/* Intialize Renderer */
	if (!m_pRenderer->VInit(emsg, m_hWnd, Resolution(m_clientW, m_clientH)))
	{
		Print(emsg);
		return CommonMsg::CM_GFX_FAIL;
	}
	/* Initialize Application*/
	if (!m_pApplication->VInit(emsg))
	{
		Print(emsg);
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

	/* main loop of system */
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
			Time dt = m_pTimer->VGetTime();			/* get time delta */
			run		= m_pApplication->VUpdate(dt);	/* update application */

			m_pRenderer->VDraw(dt);					/* update renderer */
			//run = m_pRenderer->VDraw(time);

			// check for errors and if we can recover
		}	
		m_pTimer->VUpdate();
	}

	return CommonMsg::CM_OK;
}