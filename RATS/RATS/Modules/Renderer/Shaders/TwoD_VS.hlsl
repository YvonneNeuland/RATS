#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float3 coordinate : POSITION;
	float2 uv : TEXCOORD;
};

struct OUTPUT_VERTEX
{
	float2 uv : TEXCOORD;
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

	//if (fromVertexBuffer.coordinate.x == 0 && fromVertexBuffer.coordinate.y == 0)
	//	sendToRasterizer.uv = float2(1, 1);
	//else
		sendToRasterizer.uv.xy = fromVertexBuffer.uv.xy;

	return sendToRasterizer;
}