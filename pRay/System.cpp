#include "System.h"
#include "IApplication.h"

CoreSystem::CoreSystem()
	: m_pApplication(nullptr)
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

	return CommonMsg::CM_OK;
}

int CoreSystem::Init(IApplication* _application, HINSTANCE _hInstance)
{
	m_hInstance = _hInstance;

	if (_application == nullptr)
		return CommonMsg::CM_APP_NULL;

	ErrorMsg emsg;
	m_pApplication = _application;
	if (!m_pApplication->VInit(emsg))
	{
		printf("System Init: %s\n", emsg.Msg().c_str());
		return CommonMsg::CM_APP_FAIL;
	}

	if (!m_window.Init(m_hInstance, m_hWnd, m_pApplication->VGetResolution(), m_pApplication->VGetAppName()))
	{
		printf("System Init: Failed to open window");
		return CommonMsg::CM_WIN_FAIL;
	}

	return CommonMsg::CM_OK;
}

int CoreSystem::Run()
{
	bool run = true;
	Time time;
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
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
			run = m_pApplication->VUpdate(time);		
		}		
	}

	return CommonMsg::CM_OK;
}