#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float4 coordinate : POSITION;
	float2 uv : TEXCOORD0;
	float3 norm : NORMAL;

};

struct OUTPUT_VERTEX
{
	float2 uv : TEXCOORD;
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

	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, worldMatrix);
	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, viewMatrix);
	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, projMatrix);

	sendToRasterizer.uv.xy = fromVertexBuffer.uv.xy;

	//float3 worldNormal = mul(fromVertexBuffer.norm, (float3x3)worldMatrix);
	sendToRasterizer.norm = fromVertexBuffer.norm;// worldNormal;
	
	return sendToRasterizer;
}