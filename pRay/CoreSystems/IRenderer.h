#pragma once

#include "CoreStd.h"

class IRenderer
{
private:
	IRenderer(const IRenderer&) = delete;
protected:
	IRenderer() {}
public:
	virtual ~IRenderer() {}

	virtual bool	VInit(ErrorMsg&, HWND&, Resolution)	= 0;
	virtual bool	VDraw(Time time)					= 0;
	virtual bool	VClose(ErrorMsg&)					= 0;
};