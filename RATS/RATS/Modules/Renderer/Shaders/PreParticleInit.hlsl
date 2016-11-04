
AppendStructuredBuffer<uint> deadList : register( u0 );

[numthreads(1, 1, 1)]
void main( uint3 index : SV_DispatchThreadID )
{
	deadList.Append(index.x + 1);
}