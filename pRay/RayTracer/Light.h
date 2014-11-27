#pragma once

#include "D3DStd.h"

class ILight
{
private:
	ILight(const ILight&) = delete;
protected:
	ILight() {}
	virtual ~ILight() {}
public:
	virtual void VMove(VECTOR3 dtTranslate) {}
};

class PointLight
	: public ILight
{
private:
	VECTOR3 m_position;
	float	m_radius;
protected:
public:

private:
protected:
public:
	PointLight(VECTOR3 position, float radius)
		: m_position(position), m_radius(radius)
	{}

	void VMove(VECTOR3 dtTranslate) final { m_position = dtTranslate; }
};