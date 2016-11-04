
texture2D tex : register( t0 );

SamplerState samplerstate : register( s0 );

struct RibbonInput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float4 color : COLOR;
};

cbuffer THIS_IS_PRAM// : register(b0)
{

	float fadeAmount;
	float playerHealth = 1;
	float currTime = 0;
	float warpAmount;
	float4 color;

	float screenWarp;
	float cutAlpha;
	float gamma;

};

float4 main( RibbonInput input ) : SV_TARGET
{
	float4 temp = tex.Sample(samplerstate, input.uv) * input.color;

	temp.xyz = pow(temp.xyz, gamma);

	return temp;
}