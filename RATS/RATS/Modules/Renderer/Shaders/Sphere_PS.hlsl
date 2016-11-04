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

float4 main(float2 uv : TEXCOORD, float3 norm : NORMAL) : SV_TARGET
{

	float2 swag = uv;
	swag[0] *= 32;
	swag[1] *= 16;


	if (swag[0] > 1.0f)
	{
		swag[0] = swag[0] % 1;
	}
	if (swag[1] > 1.0f)
	{
		swag[1] = swag[1] % 1;
	}




	float4 temp = baseTexture.Sample(filters[0], swag);

	bool edited = false;
	if (abs(norm.x) > 1.0f)
	{
		temp.x += (abs(norm.x) - 1.0f);
		edited = true;
	}
	if (abs(norm.y) > 1.0f)
	{
		temp.y += (abs(norm.y) - 1.0f);
		edited = true;

	}
	if (abs(norm.z) > 1.0f)
	{
		temp.z += (abs(norm.z) - 1.0f);
		edited = true;
	}
	
	if (edited) temp.a = 1;

	//float4 temp = float4(1, 0, 0, 1);
	temp.xyz = pow(temp.xyz, gamma);
		return temp;
}