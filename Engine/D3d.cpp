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
	- vsync(������� ���ΰ�ħ ������ ���� ������ ����)
*/

bool D3D::Initialize( int screenWidth , int screenHeight , bool vsync , HWND hWnd , bool fullScreen , float screenDepth , float screenNear )
{
	HRESULT hResult;

	//vsync(���� ����ȭ) ���� ����
	m_vsync_enabled = vsync;

	// DirectX �׷��� �������̽� ���丮�� ����ϴ�.
	IDXGIFactory* factory;
	hResult = CreateDXGIFactory( __uuidof( IDXGIFactory ) , ( void** )&factory );
	if( FAILED( hResult ) )
		return false;

	// ���丮 ��ü�� ����Ͽ� ù��° �׷��� ī�� �������̽��� ���� ����͸� ����ϴ�.
	IDXGIAdapter* adapter;
	hResult = factory->EnumAdapters( 0 , &adapter );
	if( FAILED( hResult ) )
		return false;

	// ���(�����)�� ���� ù��° ����͸� �����մϴ�.
	IDXGIOutput* adapterOutput;
	hResult = adapter->EnumOutputs( 0 , &adapterOutput );
	if( FAILED( hResult ) )
		return false;

	// DXGI_FORMAT_R8G8B8A8_UNORM ����� ��� ���÷��� ���信 �´� ����� ������ ���մϴ�.
	unsigned int numModes;
	hResult = adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM , DXGI_ENUM_MODES_INTERLACED , &numModes , NULL );
	if( FAILED( hResult ) )
		return false;

	// ������ ��� ����Ϳ� �׷��� ī�� ������ ������ ����Ʈ�� �����մϴ�.
	DXGI_MODE_DESC* displayModeList;
	displayModeList = new DXGI_MODE_DESC[numModes];
	if( !displayModeList )
		return false;

	// ���÷��� ��忡 ���� ����Ʈ ������ ä���ֽ��ϴ�.
	hResult = adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM , DXGI_ENUM_MODES_INTERLACED , &numModes , displayModeList );
	if( FAILED( hResult ) )
		return false;

	// ���� ��� ���÷��� ��忡 ���� ȭ�� �ʺ�/���̿� �´� ���÷��� ��带 ã���ϴ�.
	// ������ ���� ã���� ������� ���ΰ�ħ ������ �и�� ���� ���� �����մϴ�.
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
	// �����(�׷��� ī��)�� description�� �����ɴϴ�.
	DXGI_ADAPTER_DESC adapterDesc;	
	hResult = adapter->GetDesc( &adapterDesc );
	if( FAILED( hResult ) )
		return false;

	// ���� �׷���ī���� �޸� �뷮�� �ް�����Ʈ ������ �����մϴ�.
	m_videoCardMemory = ( int )( adapterDesc.DedicatedVideoMemory / ( 1024 * 1024 ) );

	// �׷��� ī���� �̸��� char�� ���ڿ� �迭�� �ٲ� �� �����մϴ�.
	unsigned int stringLength;
	int error = wcstombs_s( &stringLength , m_videoCardDescription , 128 , adapterDesc.Description , 128 );
	if( error != 0 )
		return false;

	// ���÷��� ��� ����Ʈ�� �Ҵ� ����
	delete[] displayModeList;
	displayModeList = NULL;

	// ��� ������� �Ҵ� ����
	adapterOutput->Release();
	adapterOutput = NULL;

	// ����� �Ҵ� ����
	adapter->Release();
	adapter = NULL;

	// ���丮 ��ü �Ҵ� ����
	factory->Release();
	factory = NULL;

	//-------------------------------------------------------------------------------------------------------------------
	// ���� ü�� description�� �ʱ�ȭ �մϴ�.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory( &swapChainDesc , sizeof( swapChainDesc ) );

	// �ϳ��� ����۸��� ����ϵ��� �մϴ�.
	swapChainDesc.BufferCount = 1;

	// ������� ���̸� �����մϴ�.
	swapChainDesc.BufferDesc.Width	= screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// ����۷� �Ϲ����� 32bit�� �����̽��� �����մϴ�.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//-------------------------------------------------------------------------------------------------------------------
	// ������� ���� ��ħ ������ ����
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

	// ������� �뵵�� �����մϴ�.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �������� �̷���� ������ �ڵ��� �����մϴ�.
	swapChainDesc.OutputWindow = hWnd;

	// ��Ƽ ���ø��� ���ϴ�.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// ������ ��� �Ǵ� Ǯ��ũ�� ��带 ����
	if( fullScreen )
	{
		swapChainDesc.Windowed = false;
	}
	else
		swapChainDesc.Windowed = true;

	// ��ĵ������ ���İ� ��ĵ���̴��� �������� ��������(unspecified) ����
	swapChainDesc.BufferDesc.ScanlineOrdering	= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling			= DXGI_MODE_SCALING_UNSPECIFIED;

	// ��µ� ������ ������� ������ �������� �մϴ�.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// �߰� �ɼ� �÷��׸� ������� �ʽ��ϴ�.
	swapChainDesc.Flags = 0;

	//-------------------------------------------------------------------------------------------------------------------
	// ��ó ������ DirectX 11�� ����
	D3D_FEATURE_LEVEL featureLevel;
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//-------------------------------------------------------------------------------------------------------------------
	// ���� ü�� , Direct3D ����̽� , Direct3D ����̽� ���ؽ�Ʈ�� ����
	hResult = D3D11CreateDeviceAndSwapChain( NULL , D3D_DRIVER_TYPE_HARDWARE , NULL , 0 , 
											 &featureLevel , 1 , D3D11_SDK_VERSION , &swapChainDesc , &m_swapChain , &m_device , NULL , &m_deviceContext );
	if( FAILED( hResult ) )
		return false;


	//-------------------------------------------------------------------------------------------------------------------
	// ����� �����͸� �����ɴϴ�.
	ID3D11Texture2D* backBufferPtr;
	hResult = m_swapChain->GetBuffer( 0 , __uuidof( ID3D11Texture2D ) , ( LPVOID* )&backBufferPtr );
	if( FAILED( hResult ) )
		return false;

	// ������� �����ͷ� ����Ÿ�� �並 �����մϴ�.
	hResult = m_device->CreateRenderTargetView( backBufferPtr , NULL , &m_renderTargetView );
	if( FAILED( hResult ) )
		return false;

	// ����� �����͸� �� �̻� ������� �����Ƿ� �Ҵ� ���� �մϴ�.
	backBufferPtr->Release();
	backBufferPtr = NULL;


	//-------------------------------------------------------------------------------------------------------------------
	// ���� ������ description�� �ʱ�ȭ �մϴ�.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory( &depthBufferDesc , sizeof( depthBufferDesc ) );

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;								// �ؽ��� �迭�� ����
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;		// ���� ���� 25bit , ���ٽ� 8bit
	depthBufferDesc.SampleDesc.Count = 1;						// ��Ƽ ���ø� ��
	depthBufferDesc.SampleDesc.Quality = 0;						// ��Ƽ ���ø� ����Ƽ
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;				// ����Ʈ ����
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;		// ���� ���ٽ� ���۷� ���
	depthBufferDesc.CPUAccessFlags = 0;							// CPU�κ��� �������� ����
	depthBufferDesc.MiscFlags = 0;

	// description�� ����Ͽ� ���� ������ �ؽ��ĸ� ����
	hResult = m_device->CreateTexture2D( &depthBufferDesc , NULL , &m_depthStencilBuffer );
	if( FAILED( hResult ) )
		return false;

	//-------------------------------------------------------------------------------------------------------------------
	// ���ٽ� ������ description�� �ʱ�ȭ
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

	// ����-���ٽ� ���¸� ����
	hResult = m_device->CreateDepthStencilState( &depthStencilDesc , &m_depthStencilState );
	if( FAILED( hResult ) )
		return false;

	// ����-���ٽ� ���¸� ����
	m_deviceContext->OMSetDepthStencilState( m_depthStencilState , 1 );

	// ����-���ٽ� ���� description �� �ʱ�ȭ
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory( &depthStencilViewDesc , sizeof( depthStencilViewDesc ) );

	// ����-���ٽ� ���� description�� �ۼ�
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// ����-���ٽ� �並 ����
	hResult = m_device->CreateDepthStencilView( m_depthStencilBuffer , &depthStencilViewDesc , &m_depthStencilView );
	if( FAILED( hResult ) )
		return false;

	// ����Ÿ�� ��� ����-���ٽ� ���۸� ���� ��� ���������ο� ���ε��մϴ�.
	m_deviceContext->OMSetRenderTargets( 1 , &m_renderTargetView , m_depthStencilView );


	//-------------------------------------------------------------------------------------------------------------------
	// � ������ ��� �׸� ������ �����ϴ� ������ȭ�� description �ۼ�
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

	// ������ȭ ���¸� ����
	hResult = m_device->CreateRasterizerState( &rasterDesc , &m_rasterState );
	if( FAILED( hResult ) )
		return false;

	// ������ȭ ���¸� ����
	m_deviceContext->RSSetState( m_rasterState );

	//-------------------------------------------------------------------------------------------------------------------
	// �������� ���� ����Ʈ�� ����
	D3D11_VIEWPORT viewPort;
	viewPort.Width	= ( float )screenWidth;
	viewPort.Height = ( float )screenHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;

	// ����Ʈ�� ����
	m_deviceContext->RSSetViewports( 1 , &viewPort );

	//-------------------------------------------------------------------------------------------------------------------
	// ���� ��� ����
	
	float fieldOfView = ( float )D3DX_PI / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D �������� ���� ���� ����� ����
	D3DXMatrixPerspectiveFovLH( &m_projectionMatrix , fieldOfView , screenAspect , screenNear , screenDepth );

	//-------------------------------------------------------------------------------------------------------------------
	// ���� ����� ���� ��ķ� �ʱ�ȭ
	D3DXMatrixIdentity( &m_worldMatrix );

	//-------------------------------------------------------------------------------------------------------------------
	// 2D �������� ���� ���翵 ����� �����մϴ�.
	D3DXMatrixOrthoLH( &m_orthoMatrix , ( float )screenWidth , ( float )screenHeight , screenNear , screenDepth );

	return true;
}

void D3D::ShutDown()
{
	// �����ϱ� ���� �̷��� ������ ���� �ٲ��� ������ ����ü���� �Ҵ� ������ �� ���ܰ� �߻�
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
	// ���۸� � �������� ���� ������ ����
	float color[4] = { red , blue , green , alpha };

	// ������� ������ �����.
	m_deviceContext->ClearRenderTargetView( m_renderTargetView , color );

	// ���� ���۸� �����.
	m_deviceContext->ClearDepthStencilView( m_depthStencilView , D3D11_CLEAR_DEPTH , 1.0f , 0 );
}

void D3D::EndScene()
{
	// �������� �Ϸ� �Ǿ����Ƿ� ������� ������ ȭ�鿡 ǥ��
	if( m_vsync_enabled )
	{
		// ���� ��ħ ����� ����
		m_swapChain->Present( 1 , 0 );
	}
	else
	{
		// ������ ������ ǥ��
		m_swapChain->Present( 0 , 0 );
	}
}

void D3D::GetVideoCardInfo( char* chardName, int& memory )
{
	strcpy_s( chardName , 128 , m_videoCardDescription );
	memory = m_videoCardMemory;
}

