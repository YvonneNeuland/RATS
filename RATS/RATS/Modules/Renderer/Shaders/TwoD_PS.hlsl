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

float4 main(float2 uv : TEXCOORD) : SV_TARGET
{

	float4 temp = baseTexture.Sample(filters[0], uv);

	//temp.a = alpha;

	//return float4(uv.x, uv.y, 0, 1);

	temp *= color;

	temp.xyz = pow(temp.xyz, gamma);

	return temp;
}