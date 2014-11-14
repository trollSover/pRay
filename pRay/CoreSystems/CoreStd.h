#pragma once

/* Includes */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include <Windows.h>
#include <comdef.h>

#include <memory>
#include <malloc.h>
#include <string>


/* Defines */
#define SAFE_RELEASE(x)			if( x ) { (x)->Release();	(x) = nullptr; }
#define SAFE_DELETE(x)			if( x ) { delete(x);		(x) = nullptr; }
#define SAFE_DELETE_ARRAY(x)	if( x ) { delete[](x);		(x) = nullptr; }

/* Typedefs */
typedef unsigned int uint;

/* Common Types */
struct ErrorMsg
{
private:
	LPCTSTR msg;
	LPCTSTR domain;

public:
	ErrorMsg(HRESULT _hr, const LPCTSTR _domain)
		: domain(_domain)
	{
		_com_error err(_hr);
		msg = err.ErrorMessage();
	}

	ErrorMsg(const LPCTSTR _domain)
		: msg(""), domain(_domain)
	{}

	~ErrorMsg()	{ }

	const LPCTSTR Msg()		{ return msg; }
	const LPCTSTR Domain()	{ return domain; }

	void SetMsg(const LPCTSTR _msg) { msg = _msg; }
	void SetMsg(const HRESULT _hr)
	{
		_com_error err(_hr);
		msg = err.ErrorMessage();
	}
};

struct Time
{
	const float dtMS;
	const float totalS;

	Time(float _dt = 0, float _total = 0)
		: dtMS(_dt), totalS(_total)
	{}

	Time(const Time& other)
		: dtMS(other.dtMS), totalS(other.totalS)
	{}
};

struct Resolution
{
	uint width, height;

	Resolution(uint w = 320, uint h = 240)
	{
		width = w;
		height = h;
	}
};

struct Point
{
	float x, y;
	Point()
		: x(0.0f), y(0.0f)
	{ }
};

/* Common Enums */
enum CommonMsg
{
	CM_OK,
	CM_WIN_FAIL,
	CM_APP_NULL,
	CM_APP_FAIL,
	CM_GFX_NULL,
	CM_GFX_FAIL,
	CM_TIME_FAIL,
};

#ifdef _DEBUG
static void Print(HRESULT hr, LPCTSTR errorDomain = "Undefined")
{
	_com_error err(hr);
	printf("%s: %s\n", errorDomain, err.ErrorMessage());
}

static void Print(ErrorMsg& emsg)
{
	printf("%s: %s\n", emsg.Domain(), emsg.Msg());
}

static void Print(LPCTSTR error)
{
	printf("%s\n", error);
}
#else
static void Print(HRESULT hr, LPCTSTR errorDomain = "Undefined")
{
	_com_error err(hr);
	MessageBox(NULL, err.ErrorMessage(), errorDomain, MB_OK);
}

static void Print(ErrorMsg& emsg)
{
	MessageBox(NULL, emsg.Msg(), emsg.Domain(), MB_OK);
}
static void Print(LPCTSTR error)
{
	MessageBox(NULL, error, "Undefined", MB_OK);
}
#endif