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

	// 올바르게 행렬 연산을 하기 위해 position 벡터를 w까지 있는 4성분이 있는 것을 사용
	input.position.w = 1.0f;

	// 정점의 위치를 월드 , 뷰 , 사영 순으로 계산
	output.position = mul(input.position , worldMatrix);
	output.position = mul(output.position , viewMatrix);
	output.position = mul(output.position , projectionMatrix);

	// 픽셀 쉐이더에서 사용하기 위해 입력 색상 저장
	output.color = input.color;

	return output;
}