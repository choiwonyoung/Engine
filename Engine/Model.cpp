#include "stdafx.h"
#include "Model.h"


Model::Model()
	: m_vertexBuffer(NULL)
	, m_indexBuffer(NULL)
{
}

Model::Model( const Model & other )
: m_vertexBuffer(NULL)
, m_indexBuffer(NULL)
{
}


Model::~Model()
{
}

bool Model::Initialize( ID3D11Device* device )
{
	bool result;

	// 정범 버퍼와 인덱스 버퍼를 초기화
	result = InitializeBuffers( device );
	return result;
}

void Model::ShutDown()
{
	// 정점 버퍼와 인덱스 버퍼를 해제
	ShutDownBuffers();
}

void Model::Render( ID3D11DeviceContext * deviceContext )
{
	// 정점 버퍼와 인덱스 버퍼를 그래픽스 파이프라인에 넣어 화면에 그릴 준비를 한다.
	RenderBuffers( deviceContext );
}

bool Model::InitializeBuffers( ID3D11Device *device )
{	
	// 정점 배열의 길이를 설정
	m_vertexCount = 3;

	// 인덱스 배열의 길이를 설정
	m_indexCount = 3;

	// 정점 배열을 생성
	VertexType* vertices = new VertexType[m_vertexCount];
	if( !vertices )
		return false;

	// 인덱스 배열을 생성
	unsigned long* indices = new unsigned long[m_indexCount];
	if( !indices )
		return false;

	// 정점 배열에 값을 넣습니다.
	vertices[0].position = D3DXVECTOR3( -1.0f , -1.0f , 0.0f );		// 왼쪽 아래
	vertices[0].color = D3DXVECTOR4( 0.0f , 1.0f , 0.0f , 1.0f );

	vertices[1].position = D3DXVECTOR3( 0.0f , 1.0f , 0.0f );		// 상단 가운데
	vertices[1].color = D3DXVECTOR4( 0.0f , 1.0f , 0.0f , 1.0f );

	vertices[2].position = D3DXVECTOR3( 1.0f , -1.0f , 0.0f );		// 오른쪽 아래
	vertices[2].color = D3DXVECTOR4( 0.0f , 1.0f , 0.0f , 1.0f );

	// 인덱스 배열에 값을 넣는다.
	indices[0] = 0;		// 왼쪽 아래 Bottom left
	indices[1] = 1;		// 상단 가운데
	indices[2] = 2;		// 오른쪽 아래

	// 정점 버퍼의 description을 작성
	D3D11_BUFFER_DESC vertexBufferDesc , indexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( VertexType ) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData , indexData;
	// 정점 데이터를 가르키는 보조 리소스 구조체를 작성
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼를 생성
	HRESULT hResult = device->CreateBuffer( &vertexBufferDesc , &vertexData , &m_vertexBuffer );
	if( FAILED( hResult ) )
		return false;

	// 인덱스 버퍼의 Description을 작성
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof( unsigned long )* m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가르키는 보조 리소스 구조체를 작성
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성
	hResult = device->CreateBuffer( &indexBufferDesc , &indexData , &m_indexBuffer );
	if( FAILED( hResult ) )
		return false;

	// 생성되고 값이 할당된 정점 버퍼와 인덱스 버퍼 삭제
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void Model::ShutDownBuffers()
{
	// 인덱스 버퍼를 해제
	if( m_indexBuffer )
	{
		m_indexBuffer->Release();
		m_indexBuffer = NULL;
	}

	// 정점 버퍼를 해제
	if( m_vertexBuffer )
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = NULL;
	}
}

void Model::RenderBuffers( ID3D11DeviceContext *deviceContext )
{
	unsigned int stride;
	unsigned int offset;

	// 정점 버퍼의 단위와 오프셋을 설정
	stride = sizeof( VertexType );
	offset = 0;

	// input assembler에 정점 버퍼를 활성화하여 그려질 수 있게 한다.
	deviceContext->IASetVertexBuffers( 0 , 1 , &m_vertexBuffer , &stride , &offset );

	// input assembler에 인덱스 버퍼를 활성화하여 그려질 수 있게 한다.
	deviceContext->IASetIndexBuffer( m_indexBuffer , DXGI_FORMAT_R32_UINT , 0 );

	// 정점 버퍼로 그릴 기본형을 설정 , 여기서는 삼각형
	deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}
