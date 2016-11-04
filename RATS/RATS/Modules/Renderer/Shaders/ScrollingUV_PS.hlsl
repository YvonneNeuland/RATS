texture2D baseTexture : register(t0);

SamplerState filters[2] : register(s0);


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
	float sphereScroll;

};

float4 main(float2 uv : TEXCOORD) : SV_TARGET
{

	float2 swag = uv;

	float intensity = 0.005f;
	float numFlaps = 8;

	if (sphereScroll > 1)
	{

		swag.x += warpAmount * 0.5f * 0.25f;
		swag.y += warpAmount * 0.25f * 0.5f;
	}
	else
	{
		swag.x += warpAmount * 0.5f;
		swag.y += warpAmount;
	}




	

	float4 temp = baseTexture.Sample(filters[1], swag);
		temp.xyz = pow(temp.xyz, gamma);

		return temp;
}