texture2D baseTexture : register(t0);

SamplerState filters[1] : register(s0);

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

float4 main() : SV_TARGET
{

	float4 temp = color;
	//float4 temp = float4(1, 0, 0, 1);

	temp.xyz = pow(temp.xyz, gamma);

		return temp;
}