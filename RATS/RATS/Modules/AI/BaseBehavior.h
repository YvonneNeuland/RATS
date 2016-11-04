#pragma once

enum eBEHAVIORS { bSpawn, bFollow, bFlock, bShoot, bRandMove, bWhipBullet, bEnvMove, bProxMine, bProminenceFire,
	bProminenceBullet, bDodge, bGrow, bLandPoint, bBOSS, bPICKUP	 /*, more decls*/ };

class AICompBase;
class BaseBehavior
{
protected:
	friend AICompBase; 

	eBEHAVIORS m_logicType;
	AICompBase* m_Owner;
public:
	BaseBehavior();
	virtual ~BaseBehavior();

	virtual void DoLogic(float _dt) = 0;
	void SetOwner(AICompBase* _in) { m_Owner = _in; }
	AICompBase* GetOwner() { return m_Owner; }
	eBEHAVIORS GetLogicType() { return m_logicType; }
};

