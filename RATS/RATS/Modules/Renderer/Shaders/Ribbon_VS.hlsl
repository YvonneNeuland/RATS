#pragma pack_matrix(row_major)

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

RibbonPair main( RibbonPair currPair )
{
	return currPair;
}