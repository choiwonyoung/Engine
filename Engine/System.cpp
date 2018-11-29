#include "stdafx.h"
#include "System.h"


System::System()
: m_pInput(NULL)
, m_pGraphics(NULL)
{
}

System::System( const System & sys )
: m_pInput(NULL)
, m_pGraphics(NULL)
{
}


System::~System()
{
}

bool System::Initialize()
{
	int screenWidth = 0, screenHeight = 0;
	bool bResult;

	// ������ API�� �̿��Ͽ� �ʰ�ȭ
	InitializeWindows( screenWidth, screenHeight );

	// Input ��ü�� ���� , Ű���� �Է� ó��
	m_pInput = new Input;
	if( NULL == m_pInput )
		return false;

	// Input ��ü �ʱ�ȭ
	m_pInput->Initialize();

	// Graphics ��ü ���� , �׷��� ��� ó��
	m_pGraphics = new Graphics;
	if( NULL == m_pGraphics )
		return false;

	// Graphics �ʱ�ȭ
	bResult = m_pGraphics->Initialize( screenWidth, screenHeight, m_hWnd );
	if( false == bResult )
		return false;
	
	return true;
}

void System::ShutDown()
{
	// Graphics ��ü ��ȯ
	if( m_pGraphics )
	{
		m_pGraphics->ShutDown();
		delete m_pGraphics;
		m_pGraphics = NULL;
	}

	// Input ��ü ��ȯ
	if( m_pInput )
	{
		delete m_pInput;
		m_pInput = NULL;
	}

	// ������ â�� ����
	ShutDownWindows();
}

void System::Run()
{
	MSG msg;
	bool bResult;

	// �޼��� ����ü �ʱ�ȭ
	ZeroMemory( &msg, sizeof( msg ) );

	bool bDone = false;
	while( !bDone )
	{
		// ������ �޼��� ó��
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		// �����쿡�� ���Ḧ ��û�ϴ� ��� ����������.
		if( msg.message == WM_QUIT )
			bDone = true;
		else
		{
			// �� �ܿ��� Frame �Լ��� ó��
			bResult = Frame();
			if( !bResult )
				bDone = true;
		}
	}
}

LRESULT CALLBACK System::MessageHandler( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			m_pInput->KeyDown( ( unsigned int )wParam );
		}
		return 0;
	case WM_KEYUP:
		{
			m_pInput->KeyUp( ( unsigned int )wParam );
		}
		return 0;	
	}

	return DefWindowProc( hWnd , uMsg , wParam , lParam );
}

bool System::Frame()
{
	// ESC Ű�� �������� Ȯ��
	if( m_pInput->IsKeyDown( VK_ESCAPE ) )
		return false;

	// Graphics �۾� ó��
	bool bResult = m_pGraphics->Frame();
	if( !bResult )
		return false;

	return true;
}

void System::InitializeWindows( int & screenWidth , int & screenHeight )
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX , posY;

	//
	ApplicationHandle = this;

	// �� ���ø����̼��� �ν��Ͻ��� �����ɴϴ�.
	m_hInstance = GetModuleHandle( NULL );

	// ���ø����̼� �̸� ����
	m_applicationName = L"Engine";

	// ������ Ŭ���� �⺻ ����
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= m_hInstance;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm			= wc.hIcon;
	wc.hCursor			= LoadCursor( NULL , IDC_ARROW );
	wc.hbrBackground	= ( HBRUSH )GetStockObject( BLACK_BRUSH );
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= m_applicationName;
	wc.cbSize			= sizeof( WNDCLASSEX );

	// ������ Ŭ���� ���
	RegisterClassEx( &wc );

	// ����� ȭ���� �ػ󵵸� �˾ƿɴϴ�.
	screenWidth		= GetSystemMetrics( SM_CXSCREEN );
	screenHeight	= GetSystemMetrics( SM_CYSCREEN );

	// Ǯ ��ũ�� ��� ������ ���� ���� ȭ�� ����
	if( FULL_SCREEN )
	{
		// ȭ�� ũ�⸦ ����ũž ũ�⿡ ���߰� ������ 32bit�� �Ѵ�.
		memset( &dmScreenSettings , 0 , sizeof( dmScreenSettings ) );
		dmScreenSettings.dmSize			= sizeof( dmScreenSettings );
		dmScreenSettings.dmPelsWidth	= ( unsigned long )screenWidth;
		dmScreenSettings.dmPelsHeight	= ( unsigned long )screenHeight;
		dmScreenSettings.dmBitsPerPel	= 32;
		dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Ǯ ��ũ���� �´� ���÷��� ����
		ChangeDisplaySettings( &dmScreenSettings , CDS_FULLSCREEN );

		// �������� ��ġ�� ȭ�� ���� ����
		posX = 0 , posY = 0;
	}
	else
	{
		// ������ ����� 800x600 ũ�⸦ ������ �Ѵ�.
		screenWidth		= 800;
		screenHeight	= 600;

		// â�� ������� �߾ӿ� ������ �մϴ�.
		posX = ( GetSystemMetrics( SM_CXSCREEN ) - screenWidth ) / 2;
		posY = ( GetSystemMetrics( SM_CYSCREEN ) - screenHeight ) / 2;
	}

	// ������ ���� ������ â�� ����� �� �ڵ��� ���� �ɴϴ�.
	m_hWnd = CreateWindowEx( WS_EX_APPWINDOW , m_applicationName , m_applicationName , 
							 WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP ,
							 posX , posY , screenWidth , screenHeight , NULL , NULL , m_hInstance , NULL );

	// �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� �ش�.
	ShowWindow( m_hWnd , SW_SHOW );
	SetForegroundWindow( m_hWnd );
	SetFocus( m_hWnd );

	// ���콺 Ŀ���� ǥ������ ����
	ShowCursor( false );
}

void System::ShutDownWindows()
{
	// ���콺 Ŀ���� ǥ���մϴ�.
	ShowCursor( true );

	// Ǯ ��ũ�� ��带 ���� ���ö� ���÷��� ������ �ٲߴϴ�.
	if( FULL_SCREEN )
	{
		ChangeDisplaySettings( NULL , 0 );
	}

	// â�� ���� �մϴ�.
	DestroyWindow( m_hWnd );
	m_hWnd = NULL;

	// ���ø����̼� �ν��Ͻ��� ���� �մϴ�.
	UnregisterClass( m_applicationName , m_hInstance );

	// �� Ŭ������ ���� �ܺ� ������ ���� ����
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc( HWND hWnd , UINT uMessage , WPARAM wParam , LPARAM lParam )
{
	switch( uMessage )
	{
	case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return 0;
		}

	case WM_CLOSE:
		{
			PostQuitMessage( 0 );
			return 0;
		}
	}

	return ApplicationHandle->MessageHandler( hWnd , uMessage , wParam , lParam );
}