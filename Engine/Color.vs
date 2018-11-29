/////////////////
// Global
/////////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// Typedefs
struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//vertex Shader
PixelInputType ColorVertexShader( VertexInputType input )
{
	PixelInputType output;

	// �ùٸ��� ��� ������ �ϱ� ���� position ���͸� w���� �ִ� 4������ �ִ� ���� ���
	input.position.w = 1.0f;

	// ������ ��ġ�� ���� , �� , �翵 ������ ���
	output.position = mul(input.position , worldMatrix);
	output.position = mul(output.position , viewMatrix);
	output.position = mul(output.position , projectionMatrix);

	// �ȼ� ���̴����� ����ϱ� ���� �Է� ���� ����
	output.color = input.color;

	return output;
}