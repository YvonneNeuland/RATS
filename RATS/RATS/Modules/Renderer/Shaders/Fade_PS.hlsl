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

};

float4 main(float2 uv : TEXCOORD) : SV_TARGET
{

	float2 swag = uv;

	float intensity = 0.005f;
	float numFlaps = 8;

	if (screenWarp > 0)
	{
		swag.x += sin(3.14159265f * fmod(uv.y + warpAmount, 1) * numFlaps) * intensity;
		swag.y += sin(3.14159265f * fmod(uv.x + warpAmount, 1) * numFlaps) * intensity;

	}



	float4 temp = baseTexture.Sample(filters[0], swag);

	//if (temp.a < 0.1f)
	//{
	//	discard;
	//}

	temp.xyz *= fadeAmount;

	saturate(temp);

	//**************************************************************************************************************************//
	//NEVER EVER EVER EVER EVER EVER EVER RECOMPILE THIS SHADER OR THE CSH WILL MESS UP THE FADE EFFECT
	//**************************************************************************************************************************//


	//temp.a = 1.0f;
	//float4 temp = float4(1, 0, 0, 1);

		return temp;
}