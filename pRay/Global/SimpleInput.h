#pragma once

#include "../CoreSystems/IInput.h"

class SimpleInput
	: public IInput
{
private:
	bool	m_keys[256];	
	bool	m_mouseMoved;
protected:
public:
	Point	m_mousePoint;

private:
protected:
public:
	bool MouseMoved() { return m_mouseMoved; }

	virtual bool VInit() final
	{
		for (uint i = 0; i < 256; ++i)
			m_keys[i] = false;
		m_mouseMoved = false;

		RAWINPUTDEVICE inputDevices[2];

		//adds mouse and allow legacy messages
		inputDevices[0].usUsagePage = 0x01;
		inputDevices[0].usUsage = 0x02;
		inputDevices[0].dwFlags = 0;
		inputDevices[0].hwndTarget = 0;

		//adds keyboard and allow legacy messages
		inputDevices[1].usUsagePage = 0x01;
		inputDevices[1].usUsage = 0x06;
		inputDevices[1].dwFlags = 0;
		inputDevices[1].hwndTarget = 0;

		if (RegisterRawInputDevices(inputDevices, 2, sizeof(inputDevices[0])) == FALSE)
		{
			return false;
		}
		return true;
	}

	virtual bool VIsKeyDown(uint uKey)	final { return m_keys[uKey]; }
	virtual void VKeyDown(uint uKey)	final { m_keys[uKey] = true; }
	virtual void VKeyUp(uint uKey)		final { m_keys[uKey] = false; }
	
	void Update(HWND &hWnd, HRAWINPUT& lParam)
	{
		m_mouseMoved = false;
		//get raw input data buffer size
		UINT dbSize;
		GetRawInputData(lParam, RID_INPUT, NULL, &dbSize, sizeof(RAWINPUTHEADER));


		//allocate memory for raw input data and get data
		BYTE* buffer = new BYTE[dbSize];
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &dbSize, sizeof(RAWINPUTHEADER));
		RAWINPUT* raw = (RAWINPUT*)buffer;

		/* Keyboard */
		if (raw->header.dwType == RIM_TYPEKEYBOARD)
		{
			switch (raw->data.keyboard.Message)
			{
			case WM_KEYDOWN:
				VKeyDown(raw->data.keyboard.VKey);
				break;
			case WM_KEYUP:
				VKeyUp(raw->data.keyboard.VKey);
				break;
			default:
				break;
			}		
		}

		/* mouse */
		else if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			m_mousePoint.x	= static_cast<float>(raw->data.mouse.lLastX);
			m_mousePoint.y	= static_cast<float>(raw->data.mouse.lLastY);
			m_mouseMoved	= true;
		}

		//free allocated memory
		delete[] buffer;
	}
};