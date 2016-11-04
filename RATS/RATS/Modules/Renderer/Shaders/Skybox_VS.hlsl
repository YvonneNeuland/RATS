#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float4 coordinate : POSITION;
};

struct OUTPUT_VERTEX
{
	float3 pPos : POSITION;
	float4 color : COLOR;
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

	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, worldMatrix);
	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, viewMatrix);
	sendToRasterizer.projectedCoordinate = mul(sendToRasterizer.projectedCoordinate, projMatrix);

	sendToRasterizer.color[0] = 1;
	sendToRasterizer.color[1] = 1;
	sendToRasterizer.color[2] = 1;
	sendToRasterizer.color[3] = 1;
	sendToRasterizer.pPos.xyz = fromVertexBuffer.coordinate.xyz;

	return sendToRasterizer;
}