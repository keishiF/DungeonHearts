#pragma once
#include "Attack.h"

class Fire : public Attack
{
public:
	Fire(ObjectTag tag);
	virtual ~Fire();

	virtual void Init(const Vector3& pos, const Vector3& targetPos, int durationTime, int atk) override;
	virtual void Update() override;
	virtual void Draw() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
	virtual int GetAttackPower() const override { return m_atk; }

private:
	int m_fireEffect;
	int m_playingEffect;
};

