#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{

	float4 coordinate : POSITION;
	float2 uv : TEXCOORD0;
	float3 norm : NORMAL;
	float4 color : COLOR;
	float scale : SCALE;
	matrix <float, 4, 4> instanceWorldMatrix : MATRIX;

};

struct OUTPUT_VERTEX
{

	float2 uv : TEXCOORD;
	float4 color : COLOR;
	float3 norm : NORMAL;
	float4 projectedCoordinate : SV_POSITION;

};

cbuffer THIS_IS_VRAM// : register(b0)
{
	matrix <float, 4, 4> worldMatrix;
	matrix <float, 4, 4> viewMatrix;
	matrix <float, 4, 4> projMatrix;

};

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;
	sendToRasterizer.projectedCoordinate.xyz = fromVertexBuffer.coordinate.xyz;
	sendToRasterizer.projectedCoordinate.w = 1;


	matrix <float, 4, 4> modelView = mul(fromVertexBuffer.instanceWorldMatrix, viewMatrix);

	float angle = 0;

	if (fromVertexBuffer.instanceWorldMatrix._21 != 0 || fromVertexBuffer.instanceWorldMatrix._11 != 0)
	{
		angle = atan2(fromVertexBuffer.instanceWorldMatrix._21, fromVertexBuffer.instanceWorldMatrix._11);
	}

	modelView._11 = cos(angle);
	modelView._12 = -sin(angle);
	modelView._13 = 0.0f;

	modelView._21 = sin(angle);
	modelView._22 = cos(angle);
	modelView._23 = 0.0f;

	modelView._31 = 0.0f;
	modelView._32 = 0.0f;
	modelView._33 = 1.0f;


	//matrix <float, 4, 4> scaleMatrix;
	//scaleMatrix._11 = scale;
	//scaleMatrix._12 = 0;
	//scaleMatrix._13 = 0;
	//scaleMatrix._14 = 0;

	//scaleMatrix._21 = 0;
	//scaleMatrix._22 = scale;
	//scaleMatrix._23 = 0;
	//scaleMatrix._24 = 0;

	//scaleMatrix._31 = 0;
	//scaleMatrix._32 = 0;
	//scaleMatrix._33 = scale;
	//scaleMatrix._34 = 0;

	//scaleMatrix._41 = 0;
	//scaleMatrix._42 = 0;
	//scaleMatrix._43 = 0;
	//scaleMatrix._44 = 1;

	sendToRasterizer.projectedCoordinate.xyz *= fromVertexBuffer.scale;



	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, modelView);

	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, projMatrix);

	sendToRasterizer.uv.xy = fromVertexBuffer.uv.xy;
	sendToRasterizer.color = fromVertexBuffer.color;

	float3 worldNormal = mul(fromVertexBuffer.norm, (float3x3)worldMatrix);
		sendToRasterizer.norm = normalize(worldNormal);

	return sendToRasterizer;
}