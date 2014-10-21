#pragma once

#include "IInput.h"

class SimpleInput
	: public IInput
{
private:
	bool m_keys[256];

protected:
public:

private:
	virtual bool VInit() final	
	{
		for (uint i = 0; i < 256; ++i)
			m_keys[i] = false;
		return true;
	}

	virtual bool VIsKeyDown(uint uKey)	final { return m_keys[uKey]; }
	virtual void VKeyDown(uint uKey)	final { m_keys[uKey] = true; }
	virtual void VKeyUp(uint uKey)		final { m_keys[uKey] = false; }

protected:
public:
};