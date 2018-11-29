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

	// 윈도우 API를 이용하여 초가화
	InitializeWindows( screenWidth, screenHeight );

	// Input 객체를 생성 , 키보드 입력 처리
	m_pInput = new Input;
	if( NULL == m_pInput )
		return false;

	// Input 객체 초기화
	m_pInput->Initialize();

	// Graphics 객체 생성 , 그래픽 요소 처리
	m_pGraphics = new Graphics;
	if( NULL == m_pGraphics )
		return false;

	// Graphics 초기화
	bResult = m_pGraphics->Initialize( screenWidth, screenHeight, m_hWnd );
	if( false == bResult )
		return false;
	
	return true;
}

void System::ShutDown()
{
	// Graphics 객체 반환
	if( m_pGraphics )
	{
		m_pGraphics->ShutDown();
		delete m_pGraphics;
		m_pGraphics = NULL;
	}

	// Input 객체 반환
	if( m_pInput )
	{
		delete m_pInput;
		m_pInput = NULL;
	}

	// 윈도우 창을 종료
	ShutDownWindows();
}

void System::Run()
{
	MSG msg;
	bool bResult;

	// 메세지 구조체 초기화
	ZeroMemory( &msg, sizeof( msg ) );

	bool bDone = false;
	while( !bDone )
	{
		// 윈도우 메세지 처리
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		// 윈도우에서 종료를 요청하는 경우 빠져나간다.
		if( msg.message == WM_QUIT )
			bDone = true;
		else
		{
			// 그 외에는 Frame 함수를 처리
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
	// ESC 키를 눌렀는지 확인
	if( m_pInput->IsKeyDown( VK_ESCAPE ) )
		return false;

	// Graphics 작업 처리
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

	// 이 어플리케이션의 인스턴스를 가져옵니다.
	m_hInstance = GetModuleHandle( NULL );

	// 어플리케이션 이름 설정
	m_applicationName = L"Engine";

	// 윈도우 클래스 기본 설정
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

	// 윈도우 클래스 등록
	RegisterClassEx( &wc );

	// 모니터 화면의 해상도를 알아옵니다.
	screenWidth		= GetSystemMetrics( SM_CXSCREEN );
	screenHeight	= GetSystemMetrics( SM_CYSCREEN );

	// 풀 스크린 모드 변수의 값에 따라 화면 설정
	if( FULL_SCREEN )
	{
		// 화면 크기를 데스크탑 크기에 맞추고 색상을 32bit로 한다.
		memset( &dmScreenSettings , 0 , sizeof( dmScreenSettings ) );
		dmScreenSettings.dmSize			= sizeof( dmScreenSettings );
		dmScreenSettings.dmPelsWidth	= ( unsigned long )screenWidth;
		dmScreenSettings.dmPelsHeight	= ( unsigned long )screenHeight;
		dmScreenSettings.dmBitsPerPel	= 32;
		dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 풀 스크린에 맞는 디스플레이 설정
		ChangeDisplaySettings( &dmScreenSettings , CDS_FULLSCREEN );

		// 윈도우의 위치를 화면 왼쪽 위로
		posX = 0 , posY = 0;
	}
	else
	{
		// 윈도우 모드라면 800x600 크기를 가지게 한다.
		screenWidth		= 800;
		screenHeight	= 600;

		// 창을 모니터의 중앙에 오도록 합니다.
		posX = ( GetSystemMetrics( SM_CXSCREEN ) - screenWidth ) / 2;
		posY = ( GetSystemMetrics( SM_CYSCREEN ) - screenHeight ) / 2;
	}

	// 설정한 것을 가지고 창을 만들고 그 핸들을 가져 옵니다.
	m_hWnd = CreateWindowEx( WS_EX_APPWINDOW , m_applicationName , m_applicationName , 
							 WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP ,
							 posX , posY , screenWidth , screenHeight , NULL , NULL , m_hInstance , NULL );

	// 원도우를 화면에 표시하고 포커스를 준다.
	ShowWindow( m_hWnd , SW_SHOW );
	SetForegroundWindow( m_hWnd );
	SetFocus( m_hWnd );

	// 마우스 커서를 표시하지 않음
	ShowCursor( false );
}

void System::ShutDownWindows()
{
	// 마우스 커서를 표시합니다.
	ShowCursor( true );

	// 풀 스크린 모드를 빠져 나올때 디스플레이 설정을 바꿉니다.
	if( FULL_SCREEN )
	{
		ChangeDisplaySettings( NULL , 0 );
	}

	// 창을 제거 합니다.
	DestroyWindow( m_hWnd );
	m_hWnd = NULL;

	// 어플리케이션 인스턴스를 제거 합니다.
	UnregisterClass( m_applicationName , m_hInstance );

	// 이 클래스의 대한 외부 포인터 참조 제거
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