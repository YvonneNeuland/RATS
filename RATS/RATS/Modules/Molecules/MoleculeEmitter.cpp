#include "stdafx.h"
#include "MoleculeEmitter.h"
#include "Molecule.h"
#include "../Object Manager/GameObject.h"
#include <DirectXMath.h>

using namespace DirectX;

// Helper Function

float3 ScaleVector3( const float3 &v, float scale );
float3 AddVector3( const float3 &v1, const float3 &v2 );
float3 SubVector3( const float3 &v1, const float3 &v3 );


MoleculeEmitter::MoleculeEmitter()
{
	Reset();
}


MoleculeEmitter::~MoleculeEmitter()
{
}

void MoleculeEmitter::InitEmitter()
{
	if ( m_attach == nullptr )
	{
		m_LocalMatrix.SetLocalMatrix( TransformPointToMatrix( m_position ) );
	}
}

// returns true if Emitter has died
bool MoleculeEmitter::Update( float dt )
{
	m_spawnTimer -= dt;
	if ( m_spawnTimer < 0.0f )
	{
		m_spawnTimer += m_spawnTime;
		//if (m_data.size - num_live_particles >= m_spawnCount)
		// This check is done inside SpawnMolecule, so shouldn't be necessary
		{
			// spawn new ones
			if ( !GetFlags( EmitterOff ) )
				SpawnMolecule();
		}
	}

	if ( m_attach != nullptr )
	{

		if ( m_attach->GetFlag( MostlyDead ) || m_attach->GetFlag( Waiting ) || m_attach->GetStatusFlags() == 0 )
		{
			// KillEmitter();
			SetFlags( EmitterOff );
			loop = false;
		}
	}

	for ( int i = 0; i < num_live_particles; i++ )
	{

		// update molecule
		float percent_life = (m_maxLife - m_data.mArray[i].m_fLifetime) / m_maxLife;

		SetVelocity( &m_data.mArray[i], percent_life, dt );
		m_data.mArray[i].Update( dt );


		if ( m_data.mArray[i].GetLifetime() < 0 )
		{
			num_live_particles--;
			m_data.mArray[i] = m_data.mArray[num_live_particles];
			m_data.mArray[num_live_particles].Reset();
		}
	}

	if ( !loop )
		m_data.timer -= dt;

	if ( m_data.timer < 0.0f )
	{
		SetFlags( EmitterOff );
		if ( num_live_particles == 0 )
			return true;
	}

	// Should be irrelevant now
	if ( num_live_particles > m_data.size )
	{
		std::cout << "Underflowed\n";
		num_live_particles = 0;
	}

	return false;
}

void MoleculeEmitter::SetMolecule( Molecule *mole )
{
	// Rotate
	float percent_life = (m_maxLife - mole->m_fLifetime) / m_maxLife;
	float cur_rot = percent_life * XMConvertToRadians( m_degrees_rotation );

	// Scale
	float scl = mole->m_Scale = m_startScale + (m_endScale - m_startScale) * percent_life;

	float cosResult = cosf( cur_rot ) * scl;
	float sinResult = sinf( cur_rot ) * scl;

	XMFLOAT4X4 nextLocal = XMFLOAT4X4(
		cosResult, -sinResult, 0.0f, 0.0f,
		sinResult, cosResult, 0.0f, 0.0f,
		0.0f, 0.0f, scl, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f );

	// Position
	(XMFLOAT3&)nextLocal._41 = mole->GetPosition();

	// Color
	if ( GetFlags( RandomColor ) )
	{
		float r,g,b,a;
		a = m_startColor.w;
		int temp; 

		25500;  
		15000;

		temp = rand()%125+1;
		temp = (int)(temp + 50.0f);
		r = (float)temp/255.0f;

		temp = rand()%125+1;
		temp = (int)(temp + 50.0f);
		g = (float)temp/255.0f;

		temp = rand()%125+1;
		temp = (int)(temp + 50.0f);
		b = (float)temp/255.0f;

		mole->m_Color = DirectX::XMFLOAT4( r, g, b, a );
	}

	else
	{
		float r, g, b, a;
		r = m_startColor.x + (m_endColor.x - m_startColor.x) * percent_life;
		g = m_startColor.y + (m_endColor.y - m_startColor.y) * percent_life;
		b = m_startColor.z + (m_endColor.z - m_startColor.z) * percent_life;
		a = m_startColor.w + (m_endColor.w - m_startColor.w) * percent_life;

		mole->m_Color = DirectX::XMFLOAT4( r, g, b, a );

	}

	// Matrix with rotation
	m_Matrix.SetLocalMatrix( nextLocal );

}

void MoleculeEmitter::SpawnMolecule()
{

	for ( unsigned int i = 0; i < m_spawnCount; i++ )
	{
		// Check to see if our allocation of particles has room to use
		if ( num_live_particles < m_data.size )
		{
			Molecule* temp = &m_data.mArray[num_live_particles];

			// Lifetime
			// Time to set variable lifetimes (min + (rand max-min))
			float lifespan;
			if ( m_maxLife > m_minLife )
			{
				lifespan = (m_maxLife - m_minLife)*100.0f;
				lifespan = (float)(rand() % (int)lifespan);
				lifespan *= 0.01f;
				lifespan += m_minLife;
				temp->SetLifetime( lifespan );
			}
			else
				temp->SetLifetime( m_maxLife );

			// Set Spawn Position
			float3 startPos, startVel;

			// Set start values based on attach or not
			if ( m_attach != nullptr )
			{
				startPos = *m_attach->GetTransform().GetPosition();
				m_LocalMatrix = m_attach->GetTransform();
				m_targetPosition = startPos;
			}

			else
			{
				startPos = m_position;
				//m_LocalMatrix = TransformPointToMatrix( m_position ); (this happens at spawn)
			}


			startPos = SetSpawnPosition( startPos, m_LocalMatrix );
			startVel = SetSpawnVelocity( m_LocalMatrix, startPos );

			temp->SetPosition( startPos );
			temp->SetVelocity( startVel );

			num_live_particles++;
		}
	}
}

void MoleculeEmitter::KillEmitter()
{
	loop = false;
	m_data.timer = -0.1f;
	num_live_particles = 0;
	m_attach = nullptr;
}

void MoleculeEmitter::Reset()
{
	m_Matrix.ResetTransform();

	num_live_particles = 0;
	m_render.m_renderInfo = nullptr;
	m_attach = nullptr;
	m_degrees_rotation = 0.0f;
	m_maxLife = 1.0f;
	m_endScale = 1.0f;
	m_startSpeed = 1.0f;
	m_endSpeed = 1.0f;

	m_offset = float3( 0, 0, 0 );
	m_targetPosition = m_offset;
	m_constDirection = m_offset;
	m_randDirection = m_offset;

	m_startColor = m_endColor = float4( 1, 1, 1, 1 );

	m_data.timer = 0.0f;
	loop = true;

	m_spawnCount = 0;
	m_spawnNum = 0;
	m_spawnTimer = 0.0f;
	m_spawnTime = 0.0f;

	m_flags = 0;

}


//////////  OPERATOR OVERLOAD  //////////

MoleculeEmitter& MoleculeEmitter::operator=(const MoleculeEmitter &rhs)
{
	m_endColor = rhs.m_endColor;
	m_startColor = rhs.m_startColor;

	m_startScale = rhs.m_startScale;
	m_endScale = rhs.m_endScale;

	m_degrees_rotation = rhs.m_degrees_rotation;

	m_maxLife = rhs.m_maxLife;
	m_minLife = rhs.m_minLife;

	m_constDirection = rhs.m_constDirection;
	m_randDirection = rhs.m_randDirection;
	m_startSpeed = rhs.m_startSpeed;
	m_endSpeed = rhs.m_endSpeed;

	m_offset = rhs.m_offset;
	m_data.timer = rhs.m_data.timer;

	m_spawnCount = rhs.m_spawnCount;
	m_spawnTimer = rhs.m_spawnTimer;
	m_spawnTime = rhs.m_spawnTime;
	m_spawnNum = rhs.m_spawnNum;

	m_flags = rhs.m_flags;
	loop = rhs.loop;

	m_render.SetID( rhs.m_render.GetID() );

	return *this;
}


void MoleculeEmitter::SetVelocity( Molecule *mol, float percent_life, float dt )
{
	float curSpeed = (m_endSpeed - m_startSpeed) * percent_life;
	curSpeed += m_startSpeed;
	float3 velocity = mol->m_Velocity;
	float3 temp = float3( 0, 0, 0 );

	//THIS IS A PERFORMANCE OPTIMIZATION

	if ( m_flags & MoveToPosition)
	{
		// Normal velocity
		velocity = SubVector3( m_targetPosition, mol->m_Position );
		//vel = ScaleVectorToLocal(vel, mat);
		XMVECTOR direction =
			XMLoadFloat3( &velocity );
		direction = XMVector3Normalize( direction );
		XMStoreFloat3( &velocity, direction );

		velocity.x *= m_constDirection.x;
		velocity.y *= m_constDirection.y;
		velocity.z *= m_constDirection.z;
	}

	if( m_flags & MoveAwayFromPosition)
	{
		velocity = ScaleVector3( velocity, -1.0f);
	}

	//THIS IS A PERFORMANCE OPTIMIZATION
	if ( m_flags & AddParentVelocity )
	{
		if ( m_attach != nullptr )
		{
			temp = m_attach->GetVelocity();
			temp = ScaleVectorToLocal( temp, m_attach->GetTransform() );

			mol->m_Position.x += temp.x * dt;
			mol->m_Position.y += temp.y * dt;
			mol->m_Position.z += temp.z * dt;

			//velocity = AddVector3( velocity, temp );
		}
	}

	mol->m_Position.x += velocity.x * dt * curSpeed;
	mol->m_Position.y += velocity.y * dt * curSpeed;
	mol->m_Position.z += velocity.z * dt * curSpeed;

}

// Pos passed is starting position, chosen by attached to object or not
float3 MoleculeEmitter::SetSpawnPosition( const float3 &pos, const Transform &mat )
{
	float3 finalPos;
	// If World, set position, add offset
	finalPos = m_offset;
	if ( GetFlags( ShapeEmitter ) )
		AdjustOffset( finalPos );


	// Scale to local if necessary
	if ( !GetFlags( WorldSpace ) )
	{
		finalPos = ScaleVectorToLocal( finalPos, mat );
	}

	finalPos = AddVector3( finalPos, pos );
	return finalPos;

}

void MoleculeEmitter::AdjustOffset( float3 &off )
{

	// Change how we handle the offset based on emitter types (currently circle or point)
	float x, y, z;
	x = z = y = 0;
	// If there is an offset value set, do math and use it
	if ( off.x != 0.0f )
	{
		x = 100.0f * off.x;
		x = (float)(rand() % (int)x + 1);
		if ( off.x < 0 )
			x *= -1.0f;

		if ( GetFlags( NegativeX ) )
		{
			x *= 2.0f;
			if ( off.x > 0.0f )
				x -= off.x * 100.0f;
			else
				x += off.x * 100.0f;
		}
		x *= 0.01f;
	}

	if ( off.y != 0.0f )
	{
		y = 100.0f * off.y;
		y = (float)(rand() % (int)y + 1);

		if ( off.y < 0 )
			y *= -1.0f;

		if ( GetFlags( NegativeY ) )
		{
			y *= 2.0f;
			if ( off.y > 0.0f )
				y -= off.y * 100.0f;
			else
				y += off.y * 100.0f;
		}

		y *= 0.01f;
	}

	if ( off.z != 0.0f )
	{
		z = 100.0f * off.z;
		z = (float)(rand() % (int)z + 1);
		if ( off.z < 0 )
			z *= -1.0f;

		if ( GetFlags( NegativeZ ) )
		{
			z *= 2.0f;
			if ( off.z > 0.0f )
				z -= off.z * 100.0f;
			else
			{
				z += off.z * 100.0f;
			}
		}
		z *= 0.01f;
	}

	XMVECTOR normVec;

	XMFLOAT3 normFloats = XMFLOAT3( x, y, z );

	normVec = XMVector3Normalize( XMLoadFloat3( &normFloats ) );

	XMStoreFloat3( &normFloats, normVec );

	float3 tempoff;

	if ( GetFlags( SquareEmission ) )
		// Don't need to normalize, we WANT square shapes
	{
		tempoff.x = x;
		tempoff.y = y;
		tempoff.z = z;
	}

	else
		// We DON'T want squares, so normalize and scale 
	{
		// Makes a quarter circle without using abs
		tempoff.x = x * abs( normFloats.x );
		tempoff.y = y * abs( normFloats.y );
		tempoff.z = z * abs( normFloats.z );
	}


	if ( GetFlags( ShellEmitter ) )
	{
		XMVECTOR length;
		float3 floatLength;
		normFloats = float3( tempoff.x, tempoff.y, tempoff.z );
		normVec = XMVector3Normalize( XMLoadFloat3( &normFloats ) );

		length = XMVector3Length( XMLoadFloat3( &off ) );

		normVec = XMVectorMultiply( length, normVec );
		XMStoreFloat3( &off, normVec );

	}

	else
	{
		off = tempoff;
	}
}

float3 MoleculeEmitter::SetSpawnVelocity( const Transform &mat, float3 pos )
{
	float3 randVel, vel;

	// Specific velocity setup for moving to a point
	if ( GetFlags( MoveToPosition ) )
	{
		// Normal velocity
		vel = SubVector3( m_targetPosition, pos );
		//vel = ScaleVectorToLocal(vel, mat);
		XMVECTOR direction =
			XMLoadFloat3( &vel );
		direction = XMVector3Normalize( direction );
		XMStoreFloat3( &vel, direction );

		vel.x *= m_constDirection.x;
		vel.y *= m_constDirection.y;
		vel.z *= m_constDirection.z;

		return vel;
	}

	else
	{
		vel = m_constDirection;
	}

	AddRandomDirection( randVel );

	// if localspace
	if ( !GetFlags( WorldSpace ) )
	{
		randVel = ScaleVectorToLocal( randVel, mat );

		vel = ScaleVectorToLocal( m_constDirection, mat );
	}

	return AddVector3( randVel, vel );
}

void MoleculeEmitter::SetFlags( unsigned int flags )
{
	m_flags |= flags;
}


// Naturally you can pass a single flag 
bool MoleculeEmitter::GetFlags( unsigned int flags )
{
	if ( m_flags & flags )
		return true;

	return false;
}

void MoleculeEmitter::ToggleFlag( unsigned int flag )
{
	m_flags ^= flag;
}

void MoleculeEmitter::ClearFlags()
{
	m_flags = 0;
}

void MoleculeEmitter::AddRandomDirection( float3 &vel )
{
	float x, y, z;
	x = z = y = 0;
	// If there is a random value set, do math and use it
	if ( m_randDirection.x != 0.0f )
	{
		x = 100.0f * m_randDirection.x;
		x = (float)((rand() % (int)x) + 1);
		if ( m_randDirection.x < 0 )
			x *= -1.0f;

		if ( GetFlags( NegativeX ) )
		{
			x *= 2.0f;
			if ( m_randDirection.x > 0.0f )
				x -= m_randDirection.x * 100.0f;
			else
				x += m_randDirection.x * 100.0f;
		}
		x *= 0.01f;
	}

	if ( m_randDirection.y != 0.0f )
	{
		y = 100.0f * m_randDirection.y;
		y = (float)((rand() % (int)y) + 1);

		if ( m_randDirection.y < 0 )
			y *= -1.0f;

		if ( GetFlags( NegativeY ) )
		{
			y *= 2.0f;
			if ( m_randDirection.y > 0.0f )
				y -= m_randDirection.y * 100.0f;
			else
				y += m_randDirection.y * 100.0f;
		}

		y *= 0.01f;
	}

	if ( m_randDirection.z != 0.0f )
	{
		z = 100.0f * m_randDirection.z;
		z = (float)((rand() % (int)z) + 1);
		if ( m_randDirection.z < 0 )
			z *= -1.0f;

		if ( GetFlags( NegativeZ ) )
		{
			z *= 2.0f;
			if ( m_randDirection.z > 0.0f )
				z -= m_randDirection.z * 100.0f;
			else
			{
				z += m_randDirection.z * 100.0f;
			}
		}
		z *= 0.01f;
	}


	if ( GetFlags( NoNormalize ) )
	{
		vel.x = x;
		vel.y = y;
		vel.z = z;
		return;
	}


	// Else we normalize.
	XMVECTOR normVec;

	XMFLOAT3 normFloats = XMFLOAT3( x, y, z );

	normVec = XMVector3Normalize( XMLoadFloat3( &normFloats ) );

	XMStoreFloat3( &normFloats, normVec );

	vel.x = normFloats.x;
	vel.y = normFloats.y;
	vel.z = normFloats.z;
}

float3 MoleculeEmitter::ScaleVectorToLocal( const float3 &vel, const Transform &mat )
{

	float3 tempZ, tempY, tempX, vNorm, vSum;
	// Yes I know these are backwards, deal with it

	// Z Axis
	//vNorm = *m_attach->GetTransform().GetZAxis();
	vNorm = *mat.GetZAxis();
	tempZ = ScaleVector3( vNorm, vel.z );

	// Y Axis
	//vNorm = *m_attach->GetTransform().GetYAxis();
	vNorm = *mat.GetYAxis();
	tempY = ScaleVector3( vNorm, vel.y );

	// X Axis
	//vNorm = *m_attach->GetTransform().GetXAxis();
	vNorm = *mat.GetXAxis();
	tempX = ScaleVector3( vNorm, vel.x );

	// Sum up the 3 vectors now
	vSum.x = tempX.x + tempY.x + tempZ.x;
	vSum.y = tempX.y + tempY.y + tempZ.y;
	vSum.z = tempX.z + tempY.z + tempZ.z;

	return vSum;
}


DirectX::XMFLOAT4X4 MoleculeEmitter::TransformPointToMatrix( float3 pos )
{
	// Fake a full matrix based on a position
	using namespace DirectX;
	float3 x, y, z;
	z = float3( 0, 0, 1 );
	XMVECTOR vx, vy, vz;

	// Y axis is "up" from center of world to position (on sphere)
	y = pos; // so 0,0,0 - pos which is just negative position (no, wrong wrong wrong)
	vy = XMLoadFloat3( &y );
	vy = XMVector3Normalize( vy );		// Normalizes are good here
	vz = XMLoadFloat3( &z );
	vx = XMVector3Cross( vy, vz );
	vx = XMVector3Normalize( vx );		// X and Y now correct
	vz = XMVector3Cross( vx, vy );
	vz = XMVector3Normalize( vz );		// All three normalized

	XMStoreFloat3( &x, vx );
	XMStoreFloat3( &y, vy );
	XMStoreFloat3( &z, vz );

	XMFLOAT4X4 mat;
	ZeroMemory( (void*)&mat, sizeof(XMFLOAT4X4) );

	memcpy( (void*)&mat._11, (void*)&x, sizeof(float3) );
	memcpy( (void*)&mat._21, (void*)&y, sizeof(float3) );
	memcpy( (void*)&mat._31, (void*)&z, sizeof(float3) );
	memcpy( (void*)&mat._41, (void*)&pos, sizeof(float3) );
	mat._44 = 1.0f;

	return mat;
}


//////////// Some float3 MATHEMATICS ////////////

// Takes in a float 3, returns a vector with each component multiplied by scale
float3 ScaleVector3( const float3 &v, float scale )
{
	return float3( v.x * scale, v.y * scale, v.z * scale );
}

float3 AddVector3( const float3 &v1, const float3 &v2 )
{
	float3 temp;
	temp.x = v1.x + v2.x;
	temp.y = v1.y + v2.y;
	temp.z = v1.z + v2.z;

	return temp;
}

float3 SubVector3( const float3 &v1, const float3 &v2 )
{
	float3 temp;
	temp.x = v1.x - v2.x;
	temp.y = v1.y - v2.y;
	temp.z = v1.z - v2.z;

	return temp;
}
