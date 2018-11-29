#pragma once

#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <fstream>

using namespace std;

////////////////////////////////////////////////////////////////////////////////////
// GPU 상에 존재하는 3D 모델들을 그리는데 사용하는 HLSL 쉐이더를 호출하는 클래스
////////////////////////////////////////////////////////////////////////////////////
class ColorShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
public:
	ColorShader();
	ColorShader( const ColorShader& other );
	~ColorShader();

	bool Initialize( ID3D11Device* device, HWND hWnd);
	void ShutDown();
	bool Render( ID3D11DeviceContext* , int , D3DXMATRIX , D3DXMATRIX , D3DXMATRIX );

private:
	bool InitializeShader( ID3D11Device* , HWND , const WCHAR* , const WCHAR* );
	void ShutDownShader();
	void OutputShaderErrorMessage( ID3D10Blob* , HWND , const WCHAR* );

	bool SetShaderParameters( ID3D11DeviceContext* , D3DXMATRIX , D3DXMATRIX , D3DXMATRIX );
	void RenderShader( ID3D11DeviceContext* , int );

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*	m_pixelShader;
	ID3D11InputLayout*	m_layout;
	ID3D11Buffer*		m_matrixBuffer;

};

