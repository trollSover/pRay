#pragma once

#include "D3DStd.h"

class Sphere
{
private:
	VECTOR3 m_position;
	float	m_radius;

protected:
public:

private:
protected:
public:
	Sphere(VECTOR3 position)
		: m_position(position)
	{}

	~Sphere() {}
};

class Box
{
private:
	VECTOR3 m_position;
	VECTOR3 m_volume;
protected:
public:

private:
protected:
public:
	Box(VECTOR3 position, VECTOR3 volume)
		: m_position(position), m_volume(volume)
	{}

	~Box() {}
};