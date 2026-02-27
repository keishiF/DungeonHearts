#pragma once
#include "Attack.h"

class BossBullet : public Attack
{
public:
	BossBullet();
	~BossBullet();

	virtual void Init(const Vector3& pos, const Vector3& targetPos, int durationTime, int atk) override;
	virtual void Update() override;
	virtual void Draw() override;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
	virtual int GetAttackPower() const override { return m_atk; }

private:
	int m_bulletEffect = -1;
	int m_playingEffect = -1;
};

