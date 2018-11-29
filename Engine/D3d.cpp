#include "stdafx.h"
#include "D3d.h"

D3D::D3D()
	: m_swapChain(NULL)
	, m_device(NULL)
	, m_deviceContext(NULL)
	, m_renderTargetView(NULL)
	, m_depthStencilBuffer(NULL)
	, m_depthStencilState(NULL)
	, m_depthStencilView(NULL)
	, m_rasterState(NULL)
{
}

D3D::D3D( const D3D & other )
: m_swapChain(NULL)
, m_device(NULL)
, m_deviceContext(NULL)
, m_renderTargetView(NULL)
, m_depthStencilBuffer(NULL)
, m_depthStencilState(NULL)
, m_depthStencilView(NULL)
, m_rasterState(NULL)
{
}


D3D::~D3D()
{
}
/*
	@param
	- vsync(모니터의 새로고침 비율에 맞출 것인지 여부)
*/

bool D3D::Initialize( int screenWidth , int screenHeight , bool vsync , HWND hWnd , bool fullScreen , float screenDepth , float screenNear )
{
	HRESULT hResult;

	//vsync(수직 동기화) 설정 저장
	m_vsync_enabled = vsync;

	// DirectX 그래픽 인터페이스 팩토리를 만듭니다.
	IDXGIFactory* factory;
	hResult = CreateDXGIFactory( __uuidof( IDXGIFactory ) , ( void** )&factory );
	if( FAILED( hResult ) )
		return false;

	// 팩토리 객체를 사용하여 첫번째 그래픽 카드 인터페이스에 대한 어댑터를 만듭니다.
	IDXGIAdapter* adapter;
	hResult = factory->EnumAdapters( 0 , &adapter );
	if( FAILED( hResult ) )
		return false;

	// 출력(모니터)에 대한 첫번째 어댑터를 나열합니다.
	IDXGIOutput* adapterOutput;
	hResult = adapter->EnumOutputs( 0 , &adapterOutput );
	if( FAILED( hResult ) )
		return false;

	// DXGI_FORMAT_R8G8B8A8_UNORM 모니터 출력 디스플레이 포멧에 맞는 모드의 개수를 구합니다.
	unsigned int numModes;
	hResult = adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM , DXGI_ENUM_MODES_INTERLACED , &numModes , NULL );
	if( FAILED( hResult ) )
		return false;

	// 가능한 모든 모니터와 그래픽 카드 조합을 저장할 리스트를 생성합니다.
	DXGI_MODE_DESC* displayModeList;
	displayModeList = new DXGI_MODE_DESC[numModes];
	if( !displayModeList )
		return false;

	// 디스플레이 모드에 대한 리스트 구조를 채워넣습니다.
	hResult = adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM , DXGI_ENUM_MODES_INTERLACED , &numModes , displayModeList );
	if( FAILED( hResult ) )
		return false;

	// 이제 모든 디스플레이 모드에 대한 화면 너비/높이에 맞는 디스플레이 모드를 찾습니다.
	// 적합한 것을 찾으면 모니터의 새로고침 비율의 분모와 분자 값을 저장합니다.
	unsigned int numerator , denominator;
	for( unsigned int i = 0; i < numModes; ++i )
	{
		if( displayModeList[i].Width == ( unsigned int )screenWidth )
		{
			if( displayModeList[i].Height == ( unsigned int )screenHeight )
			{
				numerator	= displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//-------------------------------------------------------------------------------------------------------------------
	// 어댑터(그래픽 카드)의 description을 가져옵니다.
	DXGI_ADAPTER_DESC adapterDesc;	
	hResult = adapter->GetDesc( &adapterDesc );
	if( FAILED( hResult ) )
		return false;

	// 현재 그래픽카드의 메모리 용량을 메가바이트 단위로 저장합니다.
	m_videoCardMemory = ( int )( adapterDesc.DedicatedVideoMemory / ( 1024 * 1024 ) );

	// 그래픽 카드의 이름을 char형 문자열 배열로 바꾼 뒤 저장합니다.
	unsigned int stringLength;
	int error = wcstombs_s( &stringLength , m_videoCardDescription , 128 , adapterDesc.Description , 128 );
	if( error != 0 )
		return false;

	// 디스플레이 모드 리스트의 할당 해제
	delete[] displayModeList;
	displayModeList = NULL;

	// 출력 어댑터의 할당 해제
	adapterOutput->Release();
	adapterOutput = NULL;

	// 어댑터 할당 해제
	adapter->Release();
	adapter = NULL;

	// 팩토리 객체 할당 해제
	factory->Release();
	factory = NULL;

	//-------------------------------------------------------------------------------------------------------------------
	// 스왑 체인 description을 초기화 합니다.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory( &swapChainDesc , sizeof( swapChainDesc ) );

	// 하나의 백버퍼만을 사용하도록 합니다.
	swapChainDesc.BufferCount = 1;

	// 백버퍼의 높이를 설정합니다.
	swapChainDesc.BufferDesc.Width	= screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 백버퍼로 일반적인 32bit의 서페이스를 저장합니다.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//-------------------------------------------------------------------------------------------------------------------
	// 백버퍼의 새로 고침 비율을 설정
	if( m_vsync_enabled )
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator		= numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator	= denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator		= 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
	}

	// 백버퍼의 용도를 설정합니다.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 렌더링이 이루어질 윈도우 핸들을 설정합니다.
	swapChainDesc.OutputWindow = hWnd;

	// 멀티 샘플링을 끕니다.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 윈도우 모드 또는 풀스크린 모드를 설정
	if( fullScreen )
	{
		swapChainDesc.Windowed = false;
	}
	else
		swapChainDesc.Windowed = true;

	// 스캔라인의 정렬과 스캔라이닝을 지정하지 않음으로(unspecified) 설정
	swapChainDesc.BufferDesc.ScanlineOrdering	= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling			= DXGI_MODE_SCALING_UNSPECIFIED;

	// 출력된 이후의 백버퍼의 내용을 버리도록 합니다.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 추가 옵션 플래그를 사용하지 않습니다.
	swapChainDesc.Flags = 0;

	//-------------------------------------------------------------------------------------------------------------------
	// 피처 레벨을 DirectX 11로 설정
	D3D_FEATURE_LEVEL featureLevel;
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//-------------------------------------------------------------------------------------------------------------------
	// 스왑 체인 , Direct3D 디바이스 , Direct3D 디바이스 컨텍스트를 생성
	hResult = D3D11CreateDeviceAndSwapChain( NULL , D3D_DRIVER_TYPE_HARDWARE , NULL , 0 , 
											 &featureLevel , 1 , D3D11_SDK_VERSION , &swapChainDesc , &m_swapChain , &m_device , NULL , &m_deviceContext );
	if( FAILED( hResult ) )
		return false;


	//-------------------------------------------------------------------------------------------------------------------
	// 백버퍼 포인터를 가져옵니다.
	ID3D11Texture2D* backBufferPtr;
	hResult = m_swapChain->GetBuffer( 0 , __uuidof( ID3D11Texture2D ) , ( LPVOID* )&backBufferPtr );
	if( FAILED( hResult ) )
		return false;

	// 백버퍼의 포인터로 렌더타겟 뷰를 생성합니다.
	hResult = m_device->CreateRenderTargetView( backBufferPtr , NULL , &m_renderTargetView );
	if( FAILED( hResult ) )
		return false;

	// 백버퍼 포인터를 더 이상 사용하지 않으므로 할당 해제 합니다.
	backBufferPtr->Release();
	backBufferPtr = NULL;


	//-------------------------------------------------------------------------------------------------------------------
	// 깊이 버퍼의 description을 초기화 합니다.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory( &depthBufferDesc , sizeof( depthBufferDesc ) );

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;								// 텍스쳐 배열의 개수
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;		// 깊이 버퍼 25bit , 스텐실 8bit
	depthBufferDesc.SampleDesc.Count = 1;						// 멀티 샘플링 수
	depthBufferDesc.SampleDesc.Quality = 0;						// 멀티 샘플링 퀄리티
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;				// 디폴트 사용법
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;		// 깊이 스텐실 버퍼로 사용
	depthBufferDesc.CPUAccessFlags = 0;							// CPU로부터 접근하지 않음
	depthBufferDesc.MiscFlags = 0;

	// description을 사용하여 깊이 버퍼의 텍스쳐를 생성
	hResult = m_device->CreateTexture2D( &depthBufferDesc , NULL , &m_depthStencilBuffer );
	if( FAILED( hResult ) )
		return false;

	//-------------------------------------------------------------------------------------------------------------------
	// 스텐실 상태의 description을 초기화
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory( &depthStencilDesc , sizeof( depthStencilDesc ) );

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operation if pixel is front-facing
	depthStencilDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

	// Stencil operation if pixel is back-facing
	depthStencilDesc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

	// 깊이-스텐실 상태를 생성
	hResult = m_device->CreateDepthStencilState( &depthStencilDesc , &m_depthStencilState );
	if( FAILED( hResult ) )
		return false;

	// 깊이-스텐실 상태를 설정
	m_deviceContext->OMSetDepthStencilState( m_depthStencilState , 1 );

	// 깊이-스텐실 뷰의 description 을 초기화
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory( &depthStencilViewDesc , sizeof( depthStencilViewDesc ) );

	// 깊이-스텐실 뷰의 description을 작성
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이-스텐실 뷰를 생성
	hResult = m_device->CreateDepthStencilView( m_depthStencilBuffer , &depthStencilViewDesc , &m_depthStencilView );
	if( FAILED( hResult ) )
		return false;

	// 렌더타겟 뷰와 깊이-스텐실 버퍼를 각각 출력 파이프라인에 바인딩합니다.
	m_deviceContext->OMSetRenderTargets( 1 , &m_renderTargetView , m_depthStencilView );


	//-------------------------------------------------------------------------------------------------------------------
	// 어떤 도형을 어떻게 그릴 것인지 결정하는 래스터화기 description 작성
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 래스터화 상태를 생성
	hResult = m_device->CreateRasterizerState( &rasterDesc , &m_rasterState );
	if( FAILED( hResult ) )
		return false;

	// 래스터화 상태를 설정
	m_deviceContext->RSSetState( m_rasterState );

	//-------------------------------------------------------------------------------------------------------------------
	// 렌더링을 위한 뷰포트를 설정
	D3D11_VIEWPORT viewPort;
	viewPort.Width	= ( float )screenWidth;
	viewPort.Height = ( float )screenHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;

	// 뷰포트를 생성
	m_deviceContext->RSSetViewports( 1 , &viewPort );

	//-------------------------------------------------------------------------------------------------------------------
	// 투영 행렬 설정
	
	float fieldOfView = ( float )D3DX_PI / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D 렌더링을 위한 투영 행렬을 생성
	D3DXMatrixPerspectiveFovLH( &m_projectionMatrix , fieldOfView , screenAspect , screenNear , screenDepth );

	//-------------------------------------------------------------------------------------------------------------------
	// 월드 행렬을 단위 행렬로 초기화
	D3DXMatrixIdentity( &m_worldMatrix );

	//-------------------------------------------------------------------------------------------------------------------
	// 2D 렌더링에 사용될 정사영 행렬을 생성합니다.
	D3DXMatrixOrthoLH( &m_orthoMatrix , ( float )screenWidth , ( float )screenHeight , screenNear , screenDepth );

	return true;
}

void D3D::ShutDown()
{
	// 종료하기 전에 이렇게 윈도우 모드로 바꾸지 않으면 스왑체인을 할당 해제할 때 예외가 발생
	if( m_swapChain )
		m_swapChain->SetFullscreenState( false , NULL );

	SAFE_RELEASE( m_rasterState );
	SAFE_RELEASE( m_depthStencilView );
	SAFE_RELEASE( m_depthStencilState );
	SAFE_RELEASE( m_depthStencilBuffer );
	SAFE_RELEASE( m_renderTargetView );
	SAFE_RELEASE( m_deviceContext );
	SAFE_RELEASE( m_device );
	SAFE_RELEASE( m_swapChain );
}

void D3D::BeginScene( float red , float green, float blue, float alpha )
{
	// 버퍼를 어떤 색상으로 지울 것인지 설정
	float color[4] = { red , blue , green , alpha };

	// 백버퍼의 내용을 지운다.
	m_deviceContext->ClearRenderTargetView( m_renderTargetView , color );

	// 깊이 버퍼를 지운다.
	m_deviceContext->ClearDepthStencilView( m_depthStencilView , D3D11_CLEAR_DEPTH , 1.0f , 0 );
}

void D3D::EndScene()
{
	// 렌더링이 완료 되었으므로 백버퍼의 내용을 화면에 표시
	if( m_vsync_enabled )
	{
		// 새로 고침 비용을 고정
		m_swapChain->Present( 1 , 0 );
	}
	else
	{
		// 가능한 빠르게 표시
		m_swapChain->Present( 0 , 0 );
	}
}

void D3D::GetVideoCardInfo( char* chardName, int& memory )
{
	strcpy_s( chardName , 128 , m_videoCardDescription );
	memory = m_videoCardMemory;
}

