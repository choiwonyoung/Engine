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
	// ���� ���̴��� �ȼ� ���̴� �� ���� ���õ� �͵��� ��ȯ
	ShutDownShader();
}

bool ColorShader::Render( ID3D11DeviceContext *deviceContext , int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix )
{
	// �������� ����� ���̴��� ���� �Է�
	bool result = SetShaderParameters( deviceContext , worldMatrix , viewMatrix , projectionMatrix );
	if( !result )
		return false;

	// ���̴��� �̿��Ͽ� �غ�� ���۸� �׸���.
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

	// ���� ���̴��� ������
	hResult = D3DX11CompileFromFile( vsFileName , NULL , NULL , "ColorVertexShader" , "vs_5_0" , D3D10_SHADER_ENABLE_STRICTNESS , 0 , NULL , &vertexShaderBuffer , &errorMessage , NULL );
	if( FAILED( hResult ) )
	{
		if( errorMessage )
			OutputShaderErrorMessage( errorMessage , hWnd , vsFileName );
		else
			MessageBox( hWnd , vsFileName , L"Missing Shader File" , MB_OK );

		return false;
	}

	// �ȼ� ���̴� ������
	hResult = D3DX11CompileFromFile( psFileName , NULL , NULL , "ColorPixelShader" , "ps_5_0" , D3D10_SHADER_ENABLE_STRICTNESS , 0 , NULL , &pixelShaderBuffer , &errorMessage , NULL );
	if( FAILED( hResult ) )
	{
		if( errorMessage )
			OutputShaderErrorMessage( errorMessage , hWnd , psFileName );
		else
			MessageBox( hWnd , psFileName , L"Missing Shader File" , MB_OK );

		return false;
	}

	// ���۷� ���� ���� ���̴��� ����
	hResult = device->CreateVertexShader( vertexShaderBuffer->GetBufferPointer() , vertexShaderBuffer->GetBufferSize() , NULL , &m_vertexShader );
	if( FAILED( hResult ) )
		return false;

	// ���۷� ���� �ȼ� ���̴��� ����
	hResult = device->CreatePixelShader( pixelShaderBuffer->GetBufferPointer() , pixelShaderBuffer->GetBufferSize() , NULL , &m_pixelShader );
	if( FAILED( hResult ) )
		return false;
		
	// ���� �Է� ���̾ƿ� description�� �ۼ��մϴ�.
	// �� ������ Model�� ���̴��� �մ� VertexType�� ��ġ�ؾ� �Ѵ�.
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

	// ���̾ƿ� ��� ������ �����ɴϴ�.
	numElements = sizeof( polygonLayout ) / sizeof( polygonLayout[0] );

	// ���� �Է� ���̾ƿ��� ����
	hResult = device->CreateInputLayout( polygonLayout , numElements , vertexShaderBuffer->GetBufferPointer() , vertexShaderBuffer->GetBufferSize() , &m_layout );
	if( FAILED( hResult ) )
		return false;

	// �� �̻� ������ �ʴ� ���� ���̴� ���ۿ� �ȼ� ���̴� ���۸� ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = NULL;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = NULL;


	//���� ���̴��� �ִ� ��� ��� ������ description�� �ۼ�
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof( MatrixBufferType );
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �Ѵ�.
	hResult = device->CreateBuffer( &matrixBufferDesc , NULL , &m_matrixBuffer );
	if( FAILED( hResult ) )
		return false;

	return true;
}

void ColorShader::ShutDownShader()
{
	// ��� ���۸� ����
	if( m_matrixBuffer )
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = NULL;
	}

	// ���� �ƿ��� ����
	if( m_layout )
	{
		m_layout->Release();
		m_layout = NULL;
	}

	// �ȼ� ���̴��� ����
	if( m_pixelShader )
	{
		m_pixelShader->Release();
		m_pixelShader = NULL;
	}

	// ���� ���̴��� ����
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

	// ���� �޼����� ��� �ִ� ���ڿ� ������ �����͸� ���� �´�.
	compileErrors = ( char* )( errorMessage->GetBufferPointer() );

	// �޼����� ���̸� �����´�.
	bufferSize = errorMessage->GetBufferSize();

	// ������ ���� �ȿ� �޼����� ���
	fout.open( "shader-error.txt" );

	// ���� �޼����� ���ϴ�.
	for( i = 0; i < bufferSize; ++i )
	{
		fout << compileErrors[i];
	}

	// ������ �ݽ��ϴ�.
	fout.close();

	// ���� �޼����� ��ȯ�մϴ�.
	errorMessage->Release();
	errorMessage = NULL;

	// ������ ������ ������ �˾� �޼����� �˷��ݴϴ�.
	MessageBox( hWnd , L"Error compiling shader. Check shader-error.txt for message." , shaderFileName , MB_OK );
}

bool ColorShader::SetShaderParameters( ID3D11DeviceContext *deviceContext , D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix )
{
	HRESULT hResult;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// ����� transpose �Ͽ� ���̴����� ����� �� �ְ� �մϴ�.
	D3DXMatrixTranspose( &worldMatrix , &worldMatrix );
	D3DXMatrixTranspose( &viewMatrix , &viewMatrix );
	D3DXMatrixTranspose( &projectionMatrix , &projectionMatrix );

	// ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
	hResult = deviceContext->Map( m_matrixBuffer , 0 , D3D11_MAP_WRITE_DISCARD , 0 , &mappedResource );
	if( FAILED( hResult ) )
		return false;

	// ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
	dataPtr = ( MatrixBufferType* )mappedResource.pData;

	// ��� ���ۿ� ����� �����մϴ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ���ۿ� ����� Ǳ�ϴ�.
	deviceContext->Unmap( m_matrixBuffer , 0 );

	// ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
	bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲߴϴ�.
	deviceContext->VSSetConstantBuffers( bufferNumber , 1 , &m_matrixBuffer );

	return true;
}

void ColorShader::RenderShader( ID3D11DeviceContext *deviceContext , int indexCount )
{
	// ���� �Է� ���̾ƿ��� ����
	deviceContext->IASetInputLayout( m_layout );

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� ����
	deviceContext->VSSetShader( m_vertexShader , NULL , 0 );
	deviceContext->PSSetShader( m_pixelShader , NULL , 0 );

	// �ﰢ���� �׸��ϴ�.
	deviceContext->DrawIndexed( indexCount , 0 , 0 );
}
