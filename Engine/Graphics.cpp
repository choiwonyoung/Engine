#include "stdafx.h"
#include "Graphics.h"


Graphics::Graphics()
: m_D3D(NULL)
, m_Camera(NULL)
, m_Model(NULL)
, m_ColorShader(NULL)
{
}

Graphics::Graphics( const Graphics& other )
: m_D3D(NULL)
, m_Camera(NULL)
, m_Model(NULL)
, m_ColorShader(NULL)
{
}


Graphics::~Graphics()
{
}

bool Graphics::Initialize( int screenWidth, int screenHeight, HWND hWnd )
{
	// D3D ��ü ����
	m_D3D = new D3D;
	if( !m_D3D )
		return false;

	// D3D ��ü �ʱ�ȭ
	bool result = m_D3D->Initialize( screenWidth , screenHeight , VSYNC_ENABLE , hWnd , FULL_SCREEN , SCREEN_DEPTH , SCREEN_NEAR );
	if( !result )
	{
		MessageBox( hWnd , L"Could not initialize Direct3D" , L"Error" , MB_OK );
		return false;
	}

	// ī�޶� ��ü ����
	m_Camera = new Camera;
	if( !m_Camera )
		return false;

	// ī�޶� ��ġ �ʱ�ȭ
	m_Camera->SetPosition( 0.0f , 0.0f , -10.0f );


	// �� ��ü ����
	m_Model = new Model;
	if( !m_Model )
		return false;

	// �� ��ü �ʱ�ȭ
	result = m_Model->Initialize( m_D3D->GetDevice() );
	if( !result )
	{
		MessageBox( hWnd , L"Could not initialize the Model object" , L"Error" , MB_OK );
		return false;
	}

	// Į�� ���̴� ��ü ����
	m_ColorShader = new ColorShader;
	if( !m_ColorShader )
		return false;

	result = m_ColorShader->Initialize( m_D3D->GetDevice() , hWnd );
	if( !result )
	{
		MessageBox( hWnd , L"Could not initialize the color shader object." , L"Error" , MB_OK );
		return false;
	}
		

	return true;
}

void Graphics::ShutDown()
{
	if( m_ColorShader )
	{
		m_ColorShader->ShutDown();
		delete m_ColorShader;
		m_ColorShader = NULL;
	}

	if( m_Model )
	{
		m_Model->ShutDown();
		delete m_Model;
		m_Model = NULL;
	}

	if( m_Camera )
	{
		delete m_Camera;
		m_Camera = NULL;
	}

	// D3D ��ü�� ��ȯ
	if( m_D3D )
	{
		m_D3D->ShutDown();
		delete m_D3D;
		m_D3D = NULL;
	}

}

bool Graphics::Frame()
{
	// �׷��� �������� ����
	bool result = Render();
	return result;
}

bool Graphics::Render()
{	
	// �� �׸��⸦ �����ϱ� ���� ������ ������ �����.
	m_D3D->BeginScene( 0.5f , 0.5f , 0.5f , 1.0f );

	// ī�޶� ��ġ���� �� ����� �����.
	m_Camera->Render();

	D3DXMATRIX worldMatrix , viewMatrix , projectionMatrix;
	m_Camera->GetViewMatrix( viewMatrix );
	m_D3D->GetWorldMatrix( worldMatrix );
	m_D3D->GetProjectionMatrix( projectionMatrix );

	m_Model->Render( m_D3D->GetDeviceContext() );
	

	bool bResult = m_ColorShader->Render( m_D3D->GetDeviceContext() , m_Model->GetIndexCount() , worldMatrix , viewMatrix , projectionMatrix );
	if( !bResult )
		return false;
	// ���ۿ� �׷��� ���� ȭ�鿡 ǥ��
	m_D3D->EndScene();

	return true;
}
