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
#define SAFE_RELEASE(x)			if( x ) { (x)->Release();	(x) = NULL; }
#define SAFE_DELETE(x)			if( x ) { delete(x);		(x) = NULL; }
#define SAFE_DELETE_ARRAY(x)	if( x ) { delete[](x);		(x) = NULL; }

/* Typedefs */
typedef unsigned int uint;

/* Common Types */
struct ErrorMsg
{
private:
	std::string msg;

public:
	ErrorMsg() {}

	ErrorMsg(const std::string _msg) { msg = _msg; }

	~ErrorMsg()	{ msg.clear(); }

	const std::string Msg() { return msg; }
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
	LPCTSTR emsg = err.ErrorMessage();
	printf("%s: %s\n", errorDomain, err);
}

static void Print(ErrorMsg& emsg, LPCTSTR errorDomain = "Undefined")
{
	printf("%s: %s\n", errorDomain, emsg.Msg().c_str());
}

static void Print(LPCTSTR error)
{
	printf("%s\n", error);
}
#else
static void Print(HRESULT hr, LPCTSTR errorDomain = "Undefined")
{
	_com_error err(hr);
	LPCTSTR emsg = err.ErrorMessage();
	MessageBox(NULL, emsg, errorDomain, MB_OK);
}

static void Print(ErrorMsg& emsg, LPCTSTR errorDomain = "Undefined")
{
	MessageBox(NULL, emsg.Msg().c_str(), errorDomain, MB_OK);
}
static void Print(LPCTSTR error)
{
	MessageBox(NULL, error, "Undefined", MB_OK);
}
#endif