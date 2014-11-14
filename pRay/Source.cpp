#include "CoreSystems/System.h"
#include "RayTracer/Application_RT.h"
#include "RayTracer/Renderer_RT.h"

CoreSystem		core;
IApplication*	g_pApplication;
IRenderer*		g_pRenderer;

#ifdef _DEBUG

#include <ios>
#include <crtdbg.h>

int main(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
		printf("%s\n", argv[i]);


	g_pApplication	= new Application_RT();
	g_pRenderer		= new Renderer_RT();

	int msg = 0;

	if (msg = core.Init(g_pApplication, g_pRenderer, GetModuleHandle(NULL)) == CommonMsg::CM_OK)
	{
		core.Run();
	}

	if( msg = core.Close() != 0)
		return msg;

	_CrtDumpMemoryLeaks();

	return msg;
}

#else

#include <Windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_pApplication	= new Application_RT();
	g_pRenderer		= new Renderer_RT();
	int msg = 0;

	if (msg = core.Init(g_pApplication, g_pRenderer, GetModuleHandle(NULL)) == CommonMsg::CM_OK)
	{
		core.Run();
	}

	if (msg = core.Close() != 0)
		return msg;

	return msg;
}

#endif