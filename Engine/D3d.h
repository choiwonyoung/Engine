#pragma once

#pragma comment(lib , "dxgi.lib")
#pragma comment(lib , "d3d11.lib")
#pragma comment(lib , "d3dx11.lib")
#pragma comment(lib , "d3dx10.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <D3DX10math.h>

class D3D
{
public:
	D3D();
	D3D( const D3D& other );
	~D3D();

	bool Initialize( int screenWidth , int screenHeight , bool vsync , HWND hWnd , bool fullScreen , float screenDepth , float screenNear );	
	void ShutDown();

	void BeginScene(float , float , float , float);
	void EndScene();

	ID3D11Device*			GetDevice()							{ return m_device; }
	ID3D11DeviceContext*	GetDeviceContext()					{ return m_deviceContext; }

	void GetProjectionMatrix( D3DXMATRIX& projectionMatrix )	{ projectionMatrix = m_projectionMatrix; }
	void GetWorldMatrix( D3DXMATRIX& worldMatrix )				{ worldMatrix = m_worldMatrix; }
	void GetOrthoMatrix( D3DXMATRIX& orthoMatrix )				{ orthoMatrix = m_orthoMatrix; }

	void GetVideoCardInfo( char* , int& );

private:
	bool						m_vsync_enabled;
	int							m_videoCardMemory;
	char						m_videoCardDescription[128];
	IDXGISwapChain*				m_swapChain;
	ID3D11Device*				m_device;
	ID3D11DeviceContext*		m_deviceContext;
	ID3D11RenderTargetView*		m_renderTargetView;
	ID3D11Texture2D*			m_depthStencilBuffer;
	ID3D11DepthStencilState*	m_depthStencilState;
	ID3D11DepthStencilView*		m_depthStencilView;
	ID3D11RasterizerState*		m_rasterState;
	D3DXMATRIX					m_projectionMatrix;
	D3DXMATRIX					m_worldMatrix;
	D3DXMATRIX					m_orthoMatrix;


};

