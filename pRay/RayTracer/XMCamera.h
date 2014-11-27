#pragma once
#include "D3DStd.h"
#include "../CoreSystems/CoreStd.h"
class XMCamera
{
private:
	XMFLOAT3	m_position;
	XMFLOAT3	m_lookAt;
	XMFLOAT3	m_up;
	XMFLOAT3	m_forward;
	XMFLOAT3	m_right;

	XMFLOAT4X4	m_viewMatrix;
	XMFLOAT4X4	m_projectionMatrix;
	XMFLOAT4X4	m_rotationMatrix;
	XMFLOAT4X4  m_inverseMatrix;

	float m_roll;
	float m_pitch;
	float m_yaw;

	int movementToggles[4];	//forward:back:left:right
	float movementSpeed;

protected:
public:

private:
	void updateView();
	void setPerspectiveProjectionLH(float fov, float width, float height, float zNear, float zFar);

protected:
public:
	XMCamera();
	~XMCamera();

	void update(const Time&);

	bool init(unsigned int clientWidth, unsigned int clientHeight);
	void setPositionAndView(float x, float y, float z, float hDeg, float pDeg);
	void transposeMatrix(XMFLOAT4X4& _mat4x4);

	void setMovementToggle(int i, int v);
	void adjustHeadingPitch(float hRad, float pRad);

	MATRIX4X4	getViewMatrix()			const { return m_viewMatrix; }
	MATRIX4X4	getProjectionMatrix()	const { return m_projectionMatrix; }
	MATRIX4X4	getInverseMatrix()		const { return m_inverseMatrix; }
};