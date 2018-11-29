#include "stdafx.h"
#include "Camera.h"


Camera::Camera()
: m_positionX(0.0f)
, m_positionY(0.0f)
, m_positionZ(0.0f)
, m_rotationX(0.0f)
, m_rotationY(0.0f)
, m_rotationZ(0.0f)
{
}

Camera::Camera( const Camera & other )
: m_positionX(0.0f)
, m_positionY(0.0f)
, m_positionZ(0.0f)
, m_rotationX(0.0f)
, m_rotationY(0.0f)
, m_rotationZ(0.0f)
{
}


Camera::~Camera()
{
}

void Camera::SetPosition( float x, float y, float z)
{
	m_positionX = x , m_positionY = y , m_positionZ = z;
}

void Camera::SetRotation( float x, float y, float z)
{
	m_rotationX = x , m_rotationY = y , m_rotationZ = z;
}

void Camera::Render()
{
	D3DXVECTOR3 up , position , lookAt;
	float yaw , pitch , roll;
	

	// Setup the vector that points upward
	up.x = 0.0f , up.y = 1.0f , up.z = 0.0f;

	// Setup the position of the camera in the world
	position.x = m_positionX , position.y = m_positionY , position.z = m_positionZ;

	// Setup where the camera is looking by default
	lookAt.x = 0.0f , lookAt.y = 0.0f , lookAt.z = 1.0f;

	// Set the yaw( Y axis ) , picth( X axis ) , and roll( Z axis ) rotation in radian
	pitch	= m_rotationX * 0.0174532925f;
	yaw		= m_rotationY * 0.0174532925f;
	roll	= m_rotationZ * 0.0174532925f;

	// create the rotation matrix from the yaw , pitch , roll values.
	D3DXMATRIX rotationMatrix;
	D3DXMatrixRotationYawPitchRoll( &rotationMatrix , yaw , pitch , roll );

	// Transform the lookAt and Up vector by the rotation matrix so the view is correctly rotated at the origine.
	D3DXVec3TransformCoord( &lookAt , &lookAt , &rotationMatrix );
	D3DXVec3TransformCoord( &up , &up , &rotationMatrix );


	// Translate the rotated camera position to the location of the viewer
	lookAt = position + lookAt;

	// Finally Create the view matrix from the thrre updated vector.
	D3DXMatrixLookAtLH( &m_viewMatrix , &position , &lookAt , &up );
}
