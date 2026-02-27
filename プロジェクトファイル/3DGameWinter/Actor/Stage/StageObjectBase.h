#pragma once
#include "Actor/Actor.h"

class StageObjectBase : public Actor
{
public:
	StageObjectBase(ObjectTag tag, ColliderData::Kind colliderKind);
	virtual ~StageObjectBase() {};

	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) override {}
	virtual void Draw() override {}
	virtual void OnDamage(int atk) override {};

	int GetModel() { return m_model; }

	Vector3 GetScale() const { return m_scale; }

	virtual void Update() abstract;

protected:
	Vector3 m_pos   = Vector3();
	Vector3 m_scale = Vector3();
	Vector3 m_rot   = Vector3();
};

