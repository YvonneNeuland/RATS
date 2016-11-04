#pragma pack_matrix(row_major)

// A QUICK NOTE: THIS IS ONLY MEANT TO BE USED WITH A SINGLE THREAD. LITERALLY A GROUP OF SIZE 1,1,1
// IF YOU MAKE MORE THAN 1 THREAD, IT'S YOUR FAULT.

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

ConsumeStructuredBuffer<uint> deadList : register( u2 );

//StructuredBuffer<float3> spawnMeshVerts : register( u3 );
//StructuredBuffer<uint> spawnMeshInds : register( u4 );

cbuffer constbutter : register( b0 )
{
	// THE THINGS WE DO FOR PADLESS BUFFERS

	float4 startColor;
	float4 startColorError;

	float3 startRot;
	uint ejectionType;
	float3 startRotError;
	float coneAngle;

	float3 startScale;
	uint dummy;
	float3 startScaleError;
	float startLife;

	float3 ejectionDirection;
	float startLifeError;

	float3 localX;
	float independentErrorScale; // BOOL - are axes able to scale independently via error?
	float3 localZ;
	float startSpeed;

	float startSpeedError;

	float useRandom; // BOOL - use randomness at all in particle init?

	uint numToSpawn;

	uint numSpawnMeshInds; // how many indices are in the spawn mesh's index buffer?

	float4x4 worldMatrix;

	// PRECALC'D RANDS
	float spawnInternalRands0;
	float spawnInternalRands1;
	float spawnInternalRands2;

	float spawnShellPointsRand;

	float spawnShellLinesRands0;
	float spawnShellLinesRands1;

	float spawnShellTrisRands0;
	float spawnShellTrisRands1;

	float detSpawnPosRand;

	float ejectConicRands0;
	float ejectConicRands1;

	float ejectRadialRands0;
	float ejectRadialRands1;

	float ejectSphericalRands0;
	float ejectSphericalRands1;
	float ejectSphericalRands2;

	float detSpawnVelocRand;

	float InitParticleRands0;
	float InitParticleRands1;
	float InitParticleRands2;
	float InitParticleRands3;
	float InitParticleRands4;
	float InitParticleRands5;
	float InitParticleRands6;
	float InitParticleRands7;
	float InitParticleRands8;
	float InitParticleRands9;
	float InitParticleRands10;

	float3 padding;
	uint simulationSpace;
}


// BEGIN HELPER FUNCS

float4 mulQuats(float4 q1, float4 q2)
{
	float4 qResult;

	qResult.xyz = (q1.w * q2.xyz) + (q2.w * q1.xyz) + cross(q1.xyz, q2.xyz);
	qResult.w = (q1.w * q2.w) - dot(q1.xyz, q2.xyz);

	return qResult;
}

float4 QuatFromAngleAxis(float3 axis, float angle)
{
	float4 q;
	float halfang = angle*0.5f;

	q.xyz = axis.xyz * sin(halfang);
	q.w = cos(halfang);

	return q;
}

// Ripped from http://permalink.gmane.org/gmane.games.devel.algorithms/14055 for fast, easy vector rotation
float3 RotateVecByQuat(float3 v, float4 q)
{
	float3 vq = q.xyz;
	float3 vqcrossv = cross(vq, v);
	float3 wvplusvqcrossv = q.w * v + vqcrossv;
	float3 vqcrosswvplusvqcrossv = cross(vq, wvplusvqcrossv);
	float3 res = q.w * wvplusvqcrossv + vqcrosswvplusvqcrossv;
	return dot(vq, v) * vq + res;
}

void InitRands(inout float myRands[28], in uint myIndex)
{
	myRands[0] = spawnInternalRands0;
	myRands[1] = spawnInternalRands1;
	myRands[2] = spawnInternalRands2;

	myRands[3] = spawnShellPointsRand;

	myRands[4] = spawnShellLinesRands0;
	myRands[5] = spawnShellLinesRands1;

	myRands[6] = spawnShellTrisRands0;
	myRands[7] = spawnShellTrisRands1;

	myRands[8] = detSpawnPosRand;

	myRands[9] = ejectConicRands0;
	myRands[10] = ejectConicRands1;

	myRands[11] = ejectRadialRands0;
	myRands[12] = ejectRadialRands1;

	myRands[13] = ejectSphericalRands0;
	myRands[14] = ejectSphericalRands1;
	myRands[15] = ejectSphericalRands2;

	myRands[16] = detSpawnVelocRand;

	myRands[17] = InitParticleRands0;
	myRands[18] = InitParticleRands1;
	myRands[19] = InitParticleRands2;
	myRands[20] = InitParticleRands3;
	myRands[21] = InitParticleRands4;
	myRands[22] = InitParticleRands5;
	myRands[23] = InitParticleRands6;
	myRands[24] = InitParticleRands7;
	myRands[25] = InitParticleRands8;
	myRands[26] = InitParticleRands9;
	myRands[27] = InitParticleRands10;

	[unroll]
	for (int currRand = 0; currRand < 28; ++currRand)
		myRands[currRand] = fmod(myRands[currRand] + (myRands[myIndex % 28]), 1);

}

particle GetParticle(in uint index)
{
	particle p = particles.Load(index);

	return p;
}

void EjectDirectional(inout particle p)
{
	p.veloc = ejectionDirection;
}

void EjectConic(in float myRands[28], inout particle p)
{
	float maxComponent = max(ejectionDirection.x, max(ejectionDirection.y, ejectionDirection.z));


	float3 axis1, axis2;
	if (maxComponent == ejectionDirection.x)
		axis1 = cross(float3(0, 1, 0), ejectionDirection);
	else if (maxComponent == ejectionDirection.y)
		axis1 = cross(float3(0, 0, 1), ejectionDirection);
	else
		axis1 = cross(float3(1, 0, 0), ejectionDirection);
	axis2 = cross(axis1, ejectionDirection);

	float4 rot = mulQuats(QuatFromAngleAxis(axis1, ((myRands[9] * 2) - 1)*coneAngle), QuatFromAngleAxis(axis2, ((myRands[10] * 2) - 1)*coneAngle));

	p.veloc = RotateVecByQuat(ejectionDirection,rot);
}

void EjectRadial(in float myRands[28], inout particle p)
{
	p.veloc =
		normalize((localX * ((myRands[11] * 2) - 1))
		+ (localZ * ((myRands[12] * 2) - 1)));
}

void EjectSpherical(in float myRands[28], inout particle p)
{
	p.veloc =
		normalize((float3(1, 0, 0) * ((myRands[13] * 2) - 1))
		+ (float3(0, 1, 0) * ((myRands[14] * 2) - 1))
		+ (float3(0, 0, 1) * ((myRands[15] * 2) - 1)));
}

void DetermineSpawnVelocity(in float myRands[28], inout particle p)
{
	switch (ejectionType)
	{
	case 0:
		EjectDirectional(p);
		break;
	case 1:
		EjectConic(myRands, p);
		break;
	case 2:
		EjectRadial(myRands, p);
		break;
	case 3:
		EjectSpherical(myRands, p);
		break;
	default:
		break;
	}

	if (!useRandom)
		p.veloc = p.startVeloc = normalize(p.veloc) * startSpeed;
	else
		p.veloc = p.startVeloc = normalize(p.veloc) * startSpeed * (1 - (myRands[16] * startSpeedError));
}

void InitParticle(in float myRands[28], inout particle p)
{
	//DetermineSpawnPosition(p);
	if (simulationSpace == 0)
		p.coordinate = worldMatrix._41_42_43;
	else
		p.coordinate = float3(0, 0, 0);

	if (!useRandom)
	{
		p.lifetime = p.startLifetime = startLife;
		p.rot = p.startRot = startRot;
		p.scale = p.startScale = startScale;
		p.color = p.startColor = startColor;
	}
	else
	{
		p.lifetime = p.startLifetime =
			startLife * (1 - (startLifeError * myRands[17]));

		p.rot = startRot;
		p.rot.x *= 1 - (startRotError.x * myRands[18]);
		p.rot.y *= 1 - (startRotError.y * myRands[19]);
		p.rot.z *= 1 - (startRotError.z * myRands[20]);
		p.startRot = p.rot;

		p.scale = startScale;
		if (!independentErrorScale)
		{
			p.scale.x *= 1 - (startScaleError.x * myRands[21]);
			p.scale.y *= 1 - (startScaleError.x * myRands[21]);
			p.scale.z *= 1 - (startScaleError.x * myRands[21]);
		}
		else
		{
			p.scale.x *= 1 - (startScaleError.x * myRands[21]);
			p.scale.y *= 1 - (startScaleError.y * myRands[22]);
			p.scale.z *= 1 - (startScaleError.z * myRands[23]);
		}
		p.startScale = p.scale;

		p.color = startColor;
		p.color.x *= 1 - (startColorError.x * myRands[24]);
		p.color.y *= 1 - (startColorError.y * myRands[25]);
		p.color.z *= 1 - (startColorError.z * myRands[26]);
		p.color.w *= 1 - (startColorError.w * myRands[27]);
		p.startColor = p.color;
	}

	DetermineSpawnVelocity(myRands, p);
}

void StoreParticle(in uint index, in particle p)
{
	particles[index] = p;
}

void InitTransform(out transform t, in particle p)
{
	t.lifetime = p.lifetime;
	t.transColor = float4(1, 0, 0, 1);
	t.localMat = float4x4(1, 0, 0, 0,
						  0, 1, 0, 0,
						  0, 0, 1, 0,
		p.coordinate.x, p.coordinate.y, p.coordinate.z, 1);
}

void StoreTransform(in uint index, in transform t)
{
	transforms[index] = t;
}

// END HELPER FUNCS



[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{

	uint myIndex = deadList.Consume();

	if (myIndex != 0)
	{
		float myRands[28];
		InitRands(myRands, myIndex);

		--myIndex;

		particle p = GetParticle(myIndex);
		transform t;

		InitParticle(myRands, p);

		InitTransform(t, p);

		StoreParticle(myIndex, p);
		StoreTransform(myIndex, t);
	}
}