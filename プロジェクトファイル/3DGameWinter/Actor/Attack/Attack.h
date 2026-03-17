#pragma once
#include "MyLib/Physics/Collider/Collidable.h"
#include "MyLib/Vector3.h"
#include <cassert>
#include <EffekseerForDXLib.h>

class Attack : public Collidable
{
public:
	Attack(ObjectTag tag, ColliderData::Kind kind);
	~Attack();

	virtual void Init(const Vector3& pos, const Vector3& targetPos, int durationTime, int atk) abstract;
	virtual void Update() abstract;
	virtual void Draw() abstract;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
	virtual int GetAttackPower() const override { return m_atk; }
	bool IsDead() const { return m_isDead; }

protected:
	int m_timer;
	int m_durationTime;
	int m_atk;
	Vector3 m_dir;
	bool m_isDead;
};

