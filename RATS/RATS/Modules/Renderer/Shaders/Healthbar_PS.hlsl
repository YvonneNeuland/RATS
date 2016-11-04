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

	float4 barColor;

	if (playerHealth <= 1.0/3.0)
		barColor = float4(1, 0, 0, 1);
	else if (playerHealth <= 2.0/3.0)
		barColor = float4(1, 1, 0, 1);
	else
		barColor = float4(0, 1, 0, 1);

	temp *= barColor;

	temp.xyz = pow(temp.xyz, gamma);

	return temp;
}