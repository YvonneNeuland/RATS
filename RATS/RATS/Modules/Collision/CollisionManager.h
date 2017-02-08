#pragma once
#include <list>
#include <vector>
#include <DirectXMath.h>
// FWRD DECL
class GameObject;
class ColliderComp;
class ObjectManager;
//using namespace DirectX;
// ----------------

class CollisionManager
{
	std::list<GameObject*>::iterator m_Iter;
	ObjectManager* m_pObjManager;
	float fLastDT;
public:
	CollisionManager();
	~CollisionManager();

	void SetOBJMNGR(ObjectManager* _ref) { m_pObjManager = _ref; }

	// reference to array of list of gameobj*
	void CheckCollisions(std::vector<std::list<GameObject*>>* _masterList, float _dt);

	bool Sphere2Sphere(ColliderComp& _lhs, ColliderComp& _rhs);
	bool Sphere2Capsule(ColliderComp& _lhs, ColliderComp& _rhs);
	bool Sphere2AABB(ColliderComp& _lhs, ColliderComp& _rhs);
	bool Capsule2AABB(ColliderComp& _lhs, ColliderComp& _rhs);
	static bool IntersectLine2Triangle(const DirectX::XMFLOAT3& _start, const DirectX::XMFLOAT3& _end, int& _triIndex, DirectX::XMFLOAT3* _out);

	void HandleReaction(ColliderComp& _lhs, ColliderComp& _rhs);
	void HandleReactionAggre(ColliderComp& _lhs, ColliderComp& _rhs);
	void PushBack(ColliderComp& _lhs, ColliderComp& _rhs, bool _extra = false);
	void PushBackStatic(ColliderComp& _static, ColliderComp& _dynamic);

	void HandleAchHazards();
	void HandleAchSpread();
	void HandleAchWhip();
	void HandleAchMissile();

};

