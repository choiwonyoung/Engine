#pragma once

#include <Windows.h>

#include "D3d.h"
#include "Camera.h"
#include "Model.h"
#include "ColorShader.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLE = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	Graphics();
	Graphics( const Graphics& );
	~Graphics();

	bool Initialize( int , int , HWND );
	void ShutDown();
	bool Frame();

private:
	bool Render();


private:
	D3D*			m_D3D;
	Camera*			m_Camera;
	Model*			m_Model;
	ColorShader*	m_ColorShader;
};

