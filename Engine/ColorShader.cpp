#include "stdafx.h"
#include "ColorShader.h"


ColorShader::ColorShader()
: m_vertexShader(NULL)
, m_pixelShader(NULL)
, m_layout(NULL)
, m_matrixBuffer(NULL)
{
}

ColorShader::ColorShader( const ColorShader & other )
: m_vertexShader(NULL)
, m_pixelShader(NULL)
, m_layout(NULL)
, m_matrixBuffer(NULL)
{
}


ColorShader::~ColorShader()
{
}

bool ColorShader::Initialize( ID3D11Device *device , HWND hWnd )
{	
	return InitializeShader( device , hWnd , L"../Engine/Color.vs" , L"../Engine/Color.ps" );
}

void ColorShader::ShutDown()
{
	// 정점 쉐이더와 픽셀 쉐이더 및 구와 관련된 것들을 반환
	ShutDownShader();
}

bool ColorShader::Render( ID3D11DeviceContext *deviceContext , int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix )
{
	// 렌더링에 사용할 쉐이더의 인자 입력
	bool result = SetShaderParameters( deviceContext , worldMatrix , viewMatrix , projectionMatrix );
	if( !result )
		return false;

	// 쉐이더를 이용하여 준비된 버퍼를 그린다.
	RenderShader( deviceContext , indexCount );
	return true;
}

bool ColorShader::InitializeShader( ID3D11Device *device , HWND hWnd,  const WCHAR* vsFileName , const WCHAR* psFileName )
{
	HRESULT hResult;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	
	errorMessage = NULL;
	vertexShaderBuffer = NULL;
	pixelShaderBuffer = NULL;

	// 정점 쉐이더를 컴파일
	hResult = D3DX11CompileFromFile( vsFileName , NULL , NULL , "ColorVertexShader" , "vs_5_0" , D3D10_SHADER_ENABLE_STRICTNESS , 0 , NULL , &vertexShaderBuffer , &errorMessage , NULL );
	if( FAILED( hResult ) )
	{
		if( errorMessage )
			OutputShaderErrorMessage( errorMessage , hWnd , vsFileName );
		else
			MessageBox( hWnd , vsFileName , L"Missing Shader File" , MB_OK );

		return false;
	}

	// 픽셀 쉐이더 컴파일
	hResult = D3DX11CompileFromFile( psFileName , NULL , NULL , "ColorPixelShader" , "ps_5_0" , D3D10_SHADER_ENABLE_STRICTNESS , 0 , NULL , &pixelShaderBuffer , &errorMessage , NULL );
	if( FAILED( hResult ) )
	{
		if( errorMessage )
			OutputShaderErrorMessage( errorMessage , hWnd , psFileName );
		else
			MessageBox( hWnd , psFileName , L"Missing Shader File" , MB_OK );

		return false;
	}

	// 버퍼로 부터 정점 쉐이더를 생성
	hResult = device->CreateVertexShader( vertexShaderBuffer->GetBufferPointer() , vertexShaderBuffer->GetBufferSize() , NULL , &m_vertexShader );
	if( FAILED( hResult ) )
		return false;

	// 버퍼로 부터 픽셀 쉐이더를 생성
	hResult = device->CreatePixelShader( pixelShaderBuffer->GetBufferPointer() , pixelShaderBuffer->GetBufferSize() , NULL , &m_pixelShader );
	if( FAILED( hResult ) )
		return false;
		
	// 정점 입력 레이아웃 description을 작성합니다.
	// 이 설정은 Model와 쉐이더에 잇는 VertexType과 일치해야 한다.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName			= "POSITION";
	polygonLayout[0].SemanticIndex			= 0;
	polygonLayout[0].Format					= DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot				= 0;
	polygonLayout[0].AlignedByteOffset		= 0;
	polygonLayout[0].InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate	= 0;

	polygonLayout[1].SemanticName			= "COLOR";
	polygonLayout[1].SemanticIndex			= 0;
	polygonLayout[1].Format					= DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot				= 0;
	polygonLayout[1].AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate	= 0;

	// 레이아웃 요소 개수를 가져옵니다.
	numElements = sizeof( polygonLayout ) / sizeof( polygonLayout[0] );

	// 정점 입력 레이아웃을 생성
	hResult = device->CreateInputLayout( polygonLayout , numElements , vertexShaderBuffer->GetBufferPointer() , vertexShaderBuffer->GetBufferSize() , &m_layout );
	if( FAILED( hResult ) )
		return false;

	// 더 이상 사용되지 않는 정점 쉐이더 버퍼와 픽셀 쉐이더 버퍼를 해제
	vertexShaderBuffer->Release();
	vertexShaderBuffer = NULL;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = NULL;


	//정점 쉐이더에 있는 행렬 상수 버퍼의 description을 작성
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof( MatrixBufferType );
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 쉐이더 상수 버퍼에 접근할 수 있게 한다.
	hResult = device->CreateBuffer( &matrixBufferDesc , NULL , &m_matrixBuffer );
	if( FAILED( hResult ) )
		return false;

	return true;
}

void ColorShader::ShutDownShader()
{
	// 상수 버퍼를 해제
	if( m_matrixBuffer )
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = NULL;
	}

	// 레이 아웃을 해제
	if( m_layout )
	{
		m_layout->Release();
		m_layout = NULL;
	}

	// 픽셀 쉐이더를 해제
	if( m_pixelShader )
	{
		m_pixelShader->Release();
		m_pixelShader = NULL;
	}

	// 정점 쉐이더를 해제
	if( m_vertexShader )
	{
		m_vertexShader->Release();
		m_vertexShader = NULL;
	}
}

void ColorShader::OutputShaderErrorMessage( ID3D10Blob *errorMessage , HWND hWnd ,const WCHAR *shaderFileName )
{
	char* compileErrors;
	unsigned long bufferSize , i;
	ofstream fout;

	// 에러 메세지를 담고 있는 문자열 버퍼의 포인터를 가져 온다.
	compileErrors = ( char* )( errorMessage->GetBufferPointer() );

	// 메세지의 길이를 가져온다.
	bufferSize = errorMessage->GetBufferSize();

	// 파일을 열고 안에 메세지를 기록
	fout.open( "shader-error.txt" );

	// 에러 메세지를 씁니다.
	for( i = 0; i < bufferSize; ++i )
	{
		fout << compileErrors[i];
	}

	// 파일을 닫습니다.
	fout.close();

	// 에러 메세지를 반환합니다.
	errorMessage->Release();
	errorMessage = NULL;

	// 컴파일 에러가 있음을 팝업 메세지로 알려줍니다.
	MessageBox( hWnd , L"Error compiling shader. Check shader-error.txt for message." , shaderFileName , MB_OK );
}

bool ColorShader::SetShaderParameters( ID3D11DeviceContext *deviceContext , D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix )
{
	HRESULT hResult;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// 행렬을 transpose 하여 쉐이더에서 사용할 수 있게 합니다.
	D3DXMatrixTranspose( &worldMatrix , &worldMatrix );
	D3DXMatrixTranspose( &viewMatrix , &viewMatrix );
	D3DXMatrixTranspose( &projectionMatrix , &projectionMatrix );

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	hResult = deviceContext->Map( m_matrixBuffer , 0 , D3D11_MAP_WRITE_DISCARD , 0 , &mappedResource );
	if( FAILED( hResult ) )
		return false;

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	dataPtr = ( MatrixBufferType* )mappedResource.pData;

	// 상수 버퍼에 행렬을 복사합니다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 상수 버퍼에 잠금을 풉니다.
	deviceContext->Unmap( m_matrixBuffer , 0 );

	// 정점 쉐이더에서의 상수 버퍼의 위치를 설정합니다.
	bufferNumber = 0;

	// 마지막으로 정점 쉐이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	deviceContext->VSSetConstantBuffers( bufferNumber , 1 , &m_matrixBuffer );

	return true;
}

void ColorShader::RenderShader( ID3D11DeviceContext *deviceContext , int indexCount )
{
	// 정점 입력 레이아웃을 설정
	deviceContext->IASetInputLayout( m_layout );

	// 삼각형을 그릴 정점 쉐이더와 픽셀 쉐이더를 설정
	deviceContext->VSSetShader( m_vertexShader , NULL , 0 );
	deviceContext->PSSetShader( m_pixelShader , NULL , 0 );

	// 삼각형을 그립니다.
	deviceContext->DrawIndexed( indexCount , 0 , 0 );
}
