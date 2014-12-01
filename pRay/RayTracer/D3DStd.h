#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#pragma warning( disable : 4005 )
#include <D3D11.h>
#pragma warning( disable : 4005 )
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <Windows.h>
#include <d3dcommon.h>



using namespace DirectX;

struct MATRIX4X4;
struct VECTOR4;
struct VECTOR3;
struct VECTOR2;

struct COLOR
{
	FLOAT d[4];
	COLOR(float r, float g, float b, float a)
	{
		d[0] = r;
		d[1] = g;
		d[2] = b;
		d[3] = a;
	}
	operator const FLOAT* () { return (FLOAT*)this; }
};

struct VECTOR2
{
	XMFLOAT2 d;
	VECTOR2() : d(XMFLOAT2(0, 0))
	{}
	VECTOR2(float x, float y) : d(XMFLOAT2(x, y))
	{}
	XMFLOAT2 operator=(const XMFLOAT2& d2) { this->d = d2; }

	operator const XMFLOAT2(){ return this->d; }
	operator const XMVECTOR(){ return XMLoadFloat2(&d); }
};

struct VECTOR3
{
	XMFLOAT3 d;

	VECTOR3() : d(XMFLOAT3(0, 0, 0))
	{}
	VECTOR3(float x, float y, float z) : d(XMFLOAT3(x, y, z))
	{}
	XMFLOAT3 operator=(const XMFLOAT3& d2) { this->d = d2; }

	operator const XMFLOAT3(){ return this->d; }
	operator const XMVECTOR(){ return XMLoadFloat3(&d); }
};

struct VECTOR4
{
	XMFLOAT4 d;

	VECTOR4() : d(XMFLOAT4(0, 0, 0, 0))
	{}

	VECTOR4(float x, float y, float z, float w) : d(XMFLOAT4(x, y, z, w))
	{}

	XMFLOAT4 operator=(const XMFLOAT4& d2) { this->d = d2; }

	operator const XMFLOAT4(){ return this->d; }
	operator const XMVECTOR(){ return XMLoadFloat4(&d); }
};

struct MATRIX4X4
{
	XMFLOAT4X4 d;

	MATRIX4X4()
	{
		XMStoreFloat4x4(&d, XMMatrixIdentity());
	}

	MATRIX4X4(const MATRIX4X4& m4x4)
		: d(m4x4.d)
	{}

	MATRIX4X4(const XMFLOAT4X4& m4x4)
		: d(m4x4)
	{}

	//XMFLOAT4X4 operator=(const XMFLOAT4X4& d2)	{ XMStoreFloat4x4(&this->d, XMLoadFloat4x4(&d2)); }
	operator const XMFLOAT4*()	{ return (XMFLOAT4*)this; }
	operator XMMATRIX()			{ return XMLoadFloat4x4(&d); }
};

struct Vertex
{
	VECTOR3 position;
	VECTOR3 normal;
	VECTOR2 uv;

	Vertex(float size = 1.0f, VECTOR3 _p = VECTOR3(0, 0, 0), VECTOR3 _n = VECTOR3(0, 0, 0), VECTOR2 _uv = VECTOR2(0, 0))
		: position(_p), normal(_n), uv(_uv)
	{
		position.d.x *= size;
		position.d.y *= size;
		position.d.z *= size;
	}
};