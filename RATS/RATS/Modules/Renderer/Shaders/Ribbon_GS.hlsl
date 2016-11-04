#pragma pack_matrix(row_major)

cbuffer constbutter : register( b0 )
{
	float4x4 viewMatrix;
	float4x4 projMatrix;

	float3 rotAxis;
	float halfQuadWidth;
	float4 color;
}

struct RibbonPair
{
	float3 p1 : POSITION0;
	float3 n1 : NORMAL0;
	float3 r1 : ROTATIONAXIS0;
	float w1 : WIDTH0;

	float3 p2 : POSITION1;
	float3 n2 : NORMAL1;
	float3 r2 : ROTATIONAXIS1;
	float w2 : WIDTH1;

	float age : LIFETIME;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float4 color : COLOR;
};

void CreateQuad(RibbonPair input, inout TriangleStream< GSOutput > output, float portion)
{
	GSOutput p1_1, p1_2, p2_1, p2_2;

	float3 perp1 = normalize(cross(input.r1, input.n1)) * input.w1;
	float3 perp2 = normalize(cross(input.r2, input.n2)) * input.w2;

	float4x4 MVP = mul(viewMatrix, projMatrix);

	p1_1.pos = mul(float4(input.p1 - perp1, 1), MVP);
	p1_2.pos = mul(float4(input.p1 + perp1, 1), MVP);

	p2_1.pos = mul(float4(input.p2 - perp2, 1), MVP);
	p2_2.pos = mul(float4(input.p2 + perp2, 1), MVP);


	p1_1.uv = float2(0, 1);
	p1_2.uv = float2(1, 1);

	p2_1.uv = float2(0, lerp(1, 0, portion));
	p2_2.uv = float2(1, lerp(1, 0, portion));


	p1_1.color = p1_2.color = p2_1.color = p2_2.color = float4(color.x, color.y, color.z, input.age);

	output.Append(p2_1);
	output.Append(p2_2);
	output.Append(p1_1);
	output.RestartStrip();
	
	output.Append(p1_1);
	output.Append(p2_2);
	output.Append(p1_2);
	output.RestartStrip();
}

//void ConstructBezier(RibbonPair input, inout TriangleStream< GSOutput > output)
//{
//	RibbonPair currRibPair;
//
//	float3 bezierAnchor =
//		lerp(input.p1 + (input.n1 * dot(input.n1, input.p2 - input.p1)), input.p2 + (-input.n2 * dot(-input.n2, input.p1 - input.p2)), 0.5f);
//
//	float3 bezierNormal = lerp(input.n1, input.n2, 0.5f);
//
//	float3 bezierRotAxis = lerp(input.r1, input.r2, 0.5f);
//
//	currRibPair.p1 = input.p1;
//	currRibPair.n1 = input.n1;
//	currRibPair.r1 = input.r1;
//
//	currRibPair.age = input.age;
//
//	GSOutput p1, bp, p2;
//
//	p1.uv = bp.uv = p2.uv = float2(0, 0);
//	p1.color = bp.color = p2.color = float4(0, 1, 1, 1);
//
//	float4x4 MVP = mul(viewMatrix, projMatrix);
//	float fineness = 0.99f;
//	for (float currSegment = fineness; currSegment < 1; currSegment += fineness)
//	{
//
//		currRibPair.p2 = lerp(lerp(input.p1, bezierAnchor, currSegment), lerp(bezierAnchor, input.p2, currSegment), currSegment);
//		currRibPair.n2 = lerp(lerp(input.n1, bezierNormal, currSegment), lerp(bezierNormal, input.n2, currSegment), currSegment);
//		currRibPair.r2 = lerp(lerp(input.r1, bezierRotAxis, currSegment), lerp(bezierRotAxis, input.r2, currSegment), currSegment);
//
//
//		//p1.pos = mul(float4(currRibPair.p1,1), MVP);
//		//bp.pos = mul(float4(bezierAnchor,1), MVP);
//		//p2.pos = mul(float4(currRibPair.p2,1), MVP);
//
//		//output.Append(p1);
//		//output.Append(bp);
//		//output.Append(p2);
//		//output.RestartStrip();
//
//		CreateQuad(currRibPair, output, currSegment);
//		
//		currRibPair.p1 = currRibPair.p2;
//		currRibPair.n1 = currRibPair.n2;
//		currRibPair.r1 = currRibPair.r2;
//
//	}
//}

[maxvertexcount(6)]
void main(
	point RibbonPair input[1] : POSITION, 
	inout TriangleStream< GSOutput > output
)
{
	if (input[0].age < 0.5f)
	{
		input[0].p1 = lerp(input[0].p1, input[0].p2, (0.5f - input[0].age) / 0.5f);
	}

	//ConstructBezier(input, output);

	CreateQuad(input[0], output, 1);
}