#include "stdafx.h"
#include "BehavLandPoint.h"
#include "../Object Manager/GameObject.h"
#include "../../Wwise files/Transform.h"
#include "../Collision/CollisionManager.h"
#include "AICompBase.h"
#include "../Upgrade System/GameData.h"
#include "../VFX/RibbonEffect/RibbonEffect.h"
#include "../Object Manager/ObjectManager.h"
#include "../Object Manager/ObjectFactory.h"
#include "../../XTime.h"
extern XTime* m_Xtime;


BehavLandPoint::BehavLandPoint()
{
	m_logicType = bLandPoint;
	scale = 0.5f;
	goingUp = true;
}


BehavLandPoint::~BehavLandPoint()
{
}

void BehavLandPoint::DoLogic(float _dt)
{
	// TODO MARK PUT STUFF HERE

	//float percentRemaining = m_currDist / m_StartDist;


	if (goingUp)
	{
		scale += _dt;
	}
	else
	{
		scale -= _dt;
	}

	if (scale < 0.5f)
	{
		goingUp = true;
	}
	else if (scale > 1.5f)
	{
		goingUp = false;
	}

	Transform* transform = &GetOwner()->GetOwner()->GetTransform();

	transform->SetScaleAll(scale);

	//transform->RotateBy(2, _dt);
}
