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

cbuffer THIS_IS_VRAM
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

	sendToRasterizer.projectedCoordinate.xyz *= fromVertexBuffer.scale;



	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, fromVertexBuffer.instanceWorldMatrix);

	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, viewMatrix);
	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, projMatrix);

	sendToRasterizer.uv.xy = fromVertexBuffer.uv.xy;
	sendToRasterizer.color = fromVertexBuffer.color;

	float3 worldNormal = mul(fromVertexBuffer.norm, (float3x3)worldMatrix);
	sendToRasterizer.norm = normalize(worldNormal);

	return sendToRasterizer;
}