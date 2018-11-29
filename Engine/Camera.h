#pragma once

#include <D3DX10math.h>


class Camera
{
public:
	Camera();
	Camera( const Camera& other );
	~Camera();

	void SetPosition( float , float , float );
	void SetRotation( float , float , float );

	D3DXVECTOR3 GetPosition() { return D3DXVECTOR3( m_positionX , m_positionY , m_positionZ ); }
	D3DXVECTOR3 GetRotation() { return D3DXVECTOR3( m_rotationX , m_rotationY , m_rotationZ ); }

	void Render();
	void GetViewMatrix( D3DXMATRIX& viewMatrix ) { viewMatrix = m_viewMatrix; }

private:
	float m_positionX , m_positionY , m_positionZ;
	float m_rotationX , m_rotationY , m_rotationZ;
	D3DXMATRIX m_viewMatrix;
};

