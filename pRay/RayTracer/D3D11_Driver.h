#pragma once

#include "../CoreSystems/CoreStd.h"

#pragma warning( disable : 4005 )

#include <D3D11.h>

#pragma warning( disable : 4005 )

class D3D11_Driver
{
private:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
	IDXGISwapChain*			m_pSwapChain;

protected:
public:
	D3D11_Driver();
	~D3D11_Driver();

	ID3D11Device*			GetDevice()		const { return m_pDevice; }
	ID3D11DeviceContext*	GetContext()	const { return m_pContext; }
	IDXGISwapChain*			GetSwapChain()	const { return m_pSwapChain; }

	void	Close();

private:
protected:
public:
	HRESULT InitDevice(Resolution, HWND&, bool windowed);
	HRESULT CreateBackBuffer(ID3D11UnorderedAccessView* uav);
};