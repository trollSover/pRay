#pragma once

#include "CoreStd.h"
#include "IInput.h"

class IApplication
{
private:
	IApplication(const IApplication&) = delete;
protected:
	IApplication() {}	
public:
	virtual ~IApplication() {}

	virtual bool	VInit(ErrorMsg&)	= 0;
	virtual bool	VUpdate(Time time) = 0;
	virtual bool	VClose(ErrorMsg&)	= 0;

	virtual			Resolution	VGetResolution()const = 0;
	virtual const	LPCSTR		VGetAppName()	const = 0;

	virtual LRESULT	ApplicationProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

	
};