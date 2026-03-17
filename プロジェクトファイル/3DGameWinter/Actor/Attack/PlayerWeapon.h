#pragma once
#include "MyLib/Physics/Collider/Collidable.h"

class PlayerWeapon : public Collidable
{
public:
	PlayerWeapon();
	virtual ~PlayerWeapon();

	// 初期化
	void Init(int parentModel);
	// 更新
	void Update(bool isAttacking, int currentFrame, int attackPower);
	// 描画
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
	virtual int GetAttackPower() const override;

private:
	int m_swordModel;
	int m_parentModel;
	int m_currentAttackPower;
};

