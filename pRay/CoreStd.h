#pragma once

/* Includes */
#include <Windows.h>

#include <memory>
#include <malloc.h>
#include <string>

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
	const float dt;
	const float total;

	Time(float _dt = 0, float _total = 0)
		: dt(_dt), total(_total)
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
};
