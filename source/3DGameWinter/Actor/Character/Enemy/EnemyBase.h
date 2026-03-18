#pragma once
#include "Actor/Actor.h"
#include "Actor/Character/StateMachine.h"

class EnemyBase : public Actor
{
public:
	EnemyBase(ObjectTag tag, ColliderData::Kind colliderKind);
	virtual ~EnemyBase() {};

	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) override {}
	virtual void Draw() override {}
	virtual void OnDamage(int atk) override {}

	virtual void Update() abstract;

	virtual bool IsBoss() const { return false; }

protected:
	int m_score = 0;
};

