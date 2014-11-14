#pragma once

#include "../CoreSystems/IRenderer.h"

#include "D3D11_Driver.h"

class Renderer_RT
	: public IRenderer
{
private:
	Resolution		m_resolution;
	D3D11_Driver	m_driver;

	ID3D11UnorderedAccessView*	m_pBackBuffer;

protected:
public:

private:
	bool	VInit(ErrorMsg&, HWND&, Resolution)	final;
	bool	VDraw(Time time)					final;
	bool	VClose(ErrorMsg&)					final;
protected:

public:
	Renderer_RT();
	~Renderer_RT();
};