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

	// ���� ���ۿ� �ε��� ���۸� �ʱ�ȭ
	result = InitializeBuffers( device );
	return result;
}

void Model::ShutDown()
{
	// ���� ���ۿ� �ε��� ���۸� ����
	ShutDownBuffers();
}

void Model::Render( ID3D11DeviceContext * deviceContext )
{
	// ���� ���ۿ� �ε��� ���۸� �׷��Ƚ� ���������ο� �־� ȭ�鿡 �׸� �غ� �Ѵ�.
	RenderBuffers( deviceContext );
}

bool Model::InitializeBuffers( ID3D11Device *device )
{	
	// ���� �迭�� ���̸� ����
	m_vertexCount = 3;

	// �ε��� �迭�� ���̸� ����
	m_indexCount = 3;

	// ���� �迭�� ����
	VertexType* vertices = new VertexType[m_vertexCount];
	if( !vertices )
		return false;

	// �ε��� �迭�� ����
	unsigned long* indices = new unsigned long[m_indexCount];
	if( !indices )
		return false;

	// ���� �迭�� ���� �ֽ��ϴ�.
	vertices[0].position = D3DXVECTOR3( -1.0f , -1.0f , 0.0f );		// ���� �Ʒ�
	vertices[0].color = D3DXVECTOR4( 0.0f , 1.0f , 0.0f , 1.0f );

	vertices[1].position = D3DXVECTOR3( 0.0f , 1.0f , 0.0f );		// ��� ���
	vertices[1].color = D3DXVECTOR4( 0.0f , 1.0f , 0.0f , 1.0f );

	vertices[2].position = D3DXVECTOR3( 1.0f , -1.0f , 0.0f );		// ������ �Ʒ�
	vertices[2].color = D3DXVECTOR4( 0.0f , 1.0f , 0.0f , 1.0f );

	// �ε��� �迭�� ���� �ִ´�.
	indices[0] = 0;		// ���� �Ʒ� Bottom left
	indices[1] = 1;		// ��� ���
	indices[2] = 2;		// ������ �Ʒ�

	// ���� ������ description�� �ۼ�
	D3D11_BUFFER_DESC vertexBufferDesc , indexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( VertexType ) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData , indexData;
	// ���� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ�
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���۸� ����
	HRESULT hResult = device->CreateBuffer( &vertexBufferDesc , &vertexData , &m_vertexBuffer );
	if( FAILED( hResult ) )
		return false;

	// �ε��� ������ Description�� �ۼ�
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof( unsigned long )* m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ�
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� ����
	hResult = device->CreateBuffer( &indexBufferDesc , &indexData , &m_indexBuffer );
	if( FAILED( hResult ) )
		return false;

	// �����ǰ� ���� �Ҵ�� ���� ���ۿ� �ε��� ���� ����
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void Model::ShutDownBuffers()
{
	// �ε��� ���۸� ����
	if( m_indexBuffer )
	{
		m_indexBuffer->Release();
		m_indexBuffer = NULL;
	}

	// ���� ���۸� ����
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

	// ���� ������ ������ �������� ����
	stride = sizeof( VertexType );
	offset = 0;

	// input assembler�� ���� ���۸� Ȱ��ȭ�Ͽ� �׷��� �� �ְ� �Ѵ�.
	deviceContext->IASetVertexBuffers( 0 , 1 , &m_vertexBuffer , &stride , &offset );

	// input assembler�� �ε��� ���۸� Ȱ��ȭ�Ͽ� �׷��� �� �ְ� �Ѵ�.
	deviceContext->IASetIndexBuffer( m_indexBuffer , DXGI_FORMAT_R32_UINT , 0 );

	// ���� ���۷� �׸� �⺻���� ���� , ���⼭�� �ﰢ��
	deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}
