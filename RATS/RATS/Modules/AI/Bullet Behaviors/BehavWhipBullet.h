#pragma once

#include "../BaseBehavior.h"

#include <list>

class AIManager;

class Whip
{

	friend AIManager;

	std::list<AICompBase*> m_cBullets;
	std::list<AICompBase*>::iterator m_currBullet, m_prevBullet;


	float m_fStiffness = 0.1f;
	float m_fCohesion = 0.7f;

	float m_timeSinceLastUpdate = 0;

	bool on = true;
public:
	void Update(float _dt);
	void Start() { on = true; }
	void Stop() { on = false; }
};

class BehavWhipBullet : public BaseBehavior
{


public:
	BehavWhipBullet();

	void DoLogic(float _dt){}
};