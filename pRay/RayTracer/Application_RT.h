#pragma once

#include "../CoreSystems/IApplication.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

class Application_RT
	: public IApplication
{
private:
	Resolution	m_resolution;
	std::string m_appName;
	IInput*		m_pInput;

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

protected:
public:
	Application_RT();
	~Application_RT();
};