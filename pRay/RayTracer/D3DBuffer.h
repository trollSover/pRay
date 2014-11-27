#pragma once

#include "D3DStd.h"
#include "../CoreSystems/CoreStd.h"

enum BufferType{ BT_RAW, BT_STRUCTURED, BT_TEXTURE2D };

enum BufferBind{ BB_SRV, BB_UAV, BB_SRV_UAV, BB_CONSTANT };

class D3DBuffer
{
private:
	ID3D11Resource*				m_pBuffer;
	ID3D11UnorderedAccessView*	m_pUAV;
	ID3D11ShaderResourceView*	m_pSRV;
protected:
public:

private:
	D3DBuffer(const D3DBuffer&) = delete;

	HRESULT OnBufferType(ErrorMsg& msg, BufferBind bBind, uint nElements, uint sElements, void* initData = nullptr);

protected:
public:
	D3DBuffer();
	~D3DBuffer();

	bool Init(ErrorMsg& msg, BufferType bType, BufferBind bBind, uint nElements, uint sElements, void* initData = nullptr);
	
	ID3D11Resource*				GetBuffer() { return m_pBuffer; }
	ID3D11UnorderedAccessView*	GetUAV()	{ return m_pUAV; }
	ID3D11ShaderResourceView*	GetSRV()	{ return m_pSRV; }
};