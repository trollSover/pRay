#pragma once

#include "D3DStd.h"

class Ray
{
private:
	VECTOR3 m_origin;
	VECTOR3 m_delta;
	VECTOR4	m_color;
	int		m_triangleId;

protected:
public:

private:
	Ray(const Ray&) = delete;
protected:
public:
	Ray()
		: m_triangleId(-1), m_origin(0,0,0), m_delta(0,0,0), m_color(0,0,0,0)
	{}

	~Ray() {}
};