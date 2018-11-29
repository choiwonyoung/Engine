#pragma once

#include <d3d11.h>
#include <D3DX10math.h>

class Model
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};
public:
	Model();
	Model( const Model& other );
	~Model();

	bool Initialize( ID3D11Device* );
	void ShutDown();
	void Render( ID3D11DeviceContext* );

	int GetIndexCount() { return m_indexCount; }

private:
	bool InitializeBuffers( ID3D11Device* );
	void ShutDownBuffers();
	void RenderBuffers( ID3D11DeviceContext* );

private:
	ID3D11Buffer *m_vertexBuffer , *m_indexBuffer;
	int m_vertexCount , m_indexCount;

};

