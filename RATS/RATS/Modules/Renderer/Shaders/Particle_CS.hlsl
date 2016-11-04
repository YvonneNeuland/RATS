#pragma pack_matrix(row_major)

struct particle
{

	float4	color :					COLOR;			// 16 bytes
	float3	coordinate :			POSITION;		// 12 bytes
	float3	rot :					ROTATION;		// 12 bytes
	float3	veloc :					VELOCITY;		// 12 bytes
	float3	scale :					SCALE;			// 12 bytes
	float	lifetime :				LIFETIME;		//  4 bytes

	float4	startColor :			STARTCOLOR;	  	// 16 bytes
	float3	startCoordinate :		STARTPOSITION;	// 12 bytes
	float3	startRot :				STARTROTATION;	// 12 bytes
	float3	startVeloc :			STARTVELOCITY;	// 12 bytes
	float3	startScale :			STARTSCALE;	  	// 12 bytes
	float	startLifetime :			STARTLIFETIME;	//  4 bytes

};

struct transform
{
	float lifetime;		//  4 bytes
	float4 transColor;	// 16 bytes
	float4x4 localMat;	// 64 bytes
};

RWStructuredBuffer<particle> particles : register( u0 );
RWStructuredBuffer<transform> transforms : register( u1 );

AppendStructuredBuffer<uint> deadList : register( u2 );


cbuffer constbutter : register( b0 )
{
	float4 colorCurve[9];

	// SEND HELP THE PADDING IS STRANGLING ME
	float3 rotCurve0;
	float speedCurve0;
	float3 rotCurve1;
	float speedCurve1;
	float3 rotCurve2;
	float speedCurve2;
	float3 rotCurve3;
	float speedCurve3;
	float3 rotCurve4;
	float speedCurve4;
	float3 rotCurve5;
	float speedCurve5;
	float3 rotCurve6;
	float speedCurve6;
	float3 rotCurve7;
	float speedCurve7;
	float3 rotCurve8;
	float speedCurve8;

	float3 scaleCurve0;
	uint billboardType; // ENUM (0-2) - what type of billboarding is the system doing?
	float3 scaleCurve1;
	uint simulationSpace; // ENUM (0-1) - is the simulation in world space or local space?
	float3 scaleCurve2;
	uint rotationCurveIndices; // how many indices in the rotation curve?
	float3 scaleCurve3;
	uint scaleCurveIndices; // how many indices in the scale curve?
	float3 scaleCurve4;
	uint speedCurveIndices; // how many indices in the speed curve?
	float3 scaleCurve5;
	uint colorCurveIndices; // how many indices in the color curve?
	float3 scaleCurve6;
	float deltaTime;
	float3 scaleCurve7;
	uint numParticlesCubeRoot;
	float3 scaleCurve8;
	float padding0;

	float3 constantForce;
	float padding1;

	float3 focusPoint;
	float padding2;

	float4x4 worldMatrix;
};

// HELPER FUNCS

particle GetParticle(in uint index)
{
	particle p = particles[index];

	return p;
}

transform GetTransform(in uint index)
{
	transform t = transforms[index];

	return t;
}


void UpdateParticleRotation(inout particle p, in float lifeRatio, in float prevLifeRatio, in float curveRatio)
{
	if (rotationCurveIndices > 0)
	{
		float3 rotationCurve[9] =
		{
			rotCurve0,
			rotCurve1,
			rotCurve2,
			rotCurve3,
			rotCurve4,
			rotCurve5,
			rotCurve6,
			rotCurve7,
			rotCurve8
		};

		curveRatio = (rotationCurveIndices+1) - ((rotationCurveIndices+1) * lifeRatio);
		float prevCurveRatio = (rotationCurveIndices + 1) - ((rotationCurveIndices + 1) * prevLifeRatio);

		uint lowerIndex = floor(curveRatio), upperIndex = ceil(curveRatio);

		float3 from, to;
		if (curveRatio < 1)
		{
			from = p.startRot;
			to = rotationCurve[0];
		}
		else
		{
			from = rotationCurve[lowerIndex - 1];
			to = rotationCurve[upperIndex - 1];
		}

		p.rot = lerp(from, to, curveRatio - lowerIndex);

	}
}

void UpdateParticleScale(inout particle p, in float lifeRatio, in float prevLifeRatio, in float curveRatio)
{
	if (scaleCurveIndices > 0)
	{
		float3 scaleCurve[9] =
		{
			scaleCurve0,
			scaleCurve1,
			scaleCurve2,
			scaleCurve3,
			scaleCurve4,
			scaleCurve5,
			scaleCurve6,
			scaleCurve7,
			scaleCurve8
		};

		curveRatio = (scaleCurveIndices+1) - ((scaleCurveIndices+1) * lifeRatio);
		float prevCurveRatio = (scaleCurveIndices+1) - ((scaleCurveIndices+1) * prevLifeRatio);

		uint lowerIndex = floor(curveRatio), upperIndex = ceil(curveRatio);

		float3 from, to;
		if (curveRatio < 1)
		{
			from = p.startScale;
			to = scaleCurve[0];
		}
		else
		{
			from = scaleCurve[lowerIndex - 1];
			to = scaleCurve[upperIndex - 1];
		}

		p.scale = lerp(from, to, curveRatio - lowerIndex);
	}
}

void UpdateParticleSpeed(inout particle p, in float lifeRatio, in float prevLifeRatio, in float curveRatio)
{
	if (speedCurveIndices > 0)
	{
		float speedCurve[9] =
		{
			speedCurve0,
			speedCurve1,
			speedCurve2,
			speedCurve3,
			speedCurve4,
			speedCurve5,
			speedCurve6,
			speedCurve7,
			speedCurve8
		};

		curveRatio = (speedCurveIndices + 1) - ((speedCurveIndices + 1) * lifeRatio);
		float prevCurveRatio = (speedCurveIndices + 1) - ((speedCurveIndices + 1) * prevLifeRatio);

		uint lowerIndex = floor(curveRatio), upperIndex = ceil(curveRatio);

		float from, to;
		if (curveRatio < 1)
		{
			from = length(p.startVeloc);
			to = speedCurve[0];
		}
		else
		{
			from = speedCurve[lowerIndex - 1];
			to = speedCurve[upperIndex - 1];
		}

		p.veloc = normalize(p.startVeloc) * lerp(from, to, max(0, curveRatio - lowerIndex));
	}
}

void UpdateParticleColor(inout particle p, in float lifeRatio, in float prevLifeRatio, in float curveRatio)
{
	if (colorCurveIndices > 0)
	{
		curveRatio = (colorCurveIndices+1) - ((colorCurveIndices+1) * lifeRatio);
		float prevCurveRatio = (colorCurveIndices + 1) - ((colorCurveIndices + 1) * prevLifeRatio);

		uint lowerIndex = floor(curveRatio), upperIndex = ceil(curveRatio);

		float4 from, to;
		if (curveRatio < 1)
		{
			from = p.startColor;
			to = colorCurve[0];
		}
		else
		{
			from = colorCurve[lowerIndex - 1];
			to = colorCurve[upperIndex - 1];
		}

		p.color = lerp(from, to, curveRatio - lowerIndex);

	}
}

void UpdateParticleWithCurves(inout particle p)
{
	float lifeRatio = max(0,(p.lifetime - deltaTime) / p.startLifetime);
	float prevLifeRatio = p.lifetime / p.startLifetime;

	float curveRatio;

	UpdateParticleRotation(p, lifeRatio, prevLifeRatio, curveRatio);
	UpdateParticleScale   (p, lifeRatio, prevLifeRatio, curveRatio);
	UpdateParticleSpeed   (p, lifeRatio, prevLifeRatio, curveRatio);
	UpdateParticleColor   (p, lifeRatio, prevLifeRatio, curveRatio);
}

void ConstructTransform(in particle p, inout transform t)
{
	float4x4 xRot, yRot, zRot, finalRot;
	float4x4 myPos, finalPos;
	float4x4 finalScale;
	float3 xAxis, yAxis, zAxis;

	t.transColor = p.color;

	zRot = float4x4( cos(p.rot.z), -sin(p.rot.z), 0, 0,
		sin(p.rot.z), cos(p.rot.z), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 );
	
	yRot = float4x4( cos(p.rot.y), 0, sin(p.rot.y), 0,
		0, 1, 0, 0,
		-sin(p.rot.y), 0, cos(p.rot.y), 0,
		0, 0, 0, 1 );
	
	xRot = float4x4( 1, 0, 0, 0,
		0, cos(p.rot.x), -sin(p.rot.x), 0,
		0, sin(p.rot.x), cos(p.rot.x), 0,
		0, 0, 0, 1 );

	finalScale = float4x4( p.scale.x, 0, 0, 0,
							0, p.scale.y, 0, 0,
							0, 0, p.scale.z, 0,
							0, 0, 0, 1 );

	myPos = float4x4( 1,0,0, 0,
						  0,1,0, 0,
						  0,0,1, 0,
				   p.coordinate, 1 );;

	if (simulationSpace == 0)
		finalPos = myPos;
	else
		finalPos = mul(myPos, worldMatrix);

	if (billboardType == 0)
	{
		finalRot = mul(xRot, mul(yRot, zRot));
	}
	else
	{
		zAxis = normalize(focusPoint - finalPos._41_42_43);
		xAxis = normalize(cross(t.localMat._21_22_23, zAxis));
		yAxis = normalize(cross(zAxis, xAxis));

		finalRot = float4x4(xAxis,0,
							yAxis,0,
							zAxis,0,
							0,0,0,1);

		finalRot = mul(zRot, finalRot);

		finalPos._11_12_13_14 = float4(1, 0, 0, 0);
		finalPos._21_22_23_24 = float4(0, 1, 0, 0);
		finalPos._31_32_33_34 = float4(0, 0, 1, 0);
	}

	t.localMat = mul(finalScale, mul(finalRot, finalPos));
}


void UpdateParticlePosition(inout particle p)
{
	p.coordinate += (p.veloc + constantForce) * deltaTime;
}

void StoreParticle(uint index, in particle p)
{
	particles[index] = p;
}

void StoreTransform(uint index, in transform t)
{
	transforms[index] = t;
}

//END HELPER FUNCS


[numthreads(1, 1, 1)]
void main(uint3 index : SV_DispatchThreadID)
{

	uint threeDIndex =
		((index.y * numParticlesCubeRoot + index.x)
			+ index.z*(numParticlesCubeRoot * numParticlesCubeRoot));

	particle p = GetParticle(threeDIndex);

	transform t = GetTransform(threeDIndex);

	if (p.lifetime > 0)
	{

		UpdateParticleWithCurves(p);

		UpdateParticlePosition(p);

		p.lifetime -= deltaTime;
		t.lifetime = p.lifetime;

		ConstructTransform(p, t);

		StoreParticle(threeDIndex, p);
		StoreTransform(threeDIndex, t);

		if (p.lifetime <= 0)
			deadList.Append(threeDIndex + 1);
	}
}