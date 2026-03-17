#pragma once
#include "MyLib/Physics/Collider/Collidable.h"

class EnemyMeleeWeapon : public Collidable
{
public:
	EnemyMeleeWeapon();
	virtual ~EnemyMeleeWeapon();

	// 初期化
	void Init(int parentModel);
	// 更新
	void Update(bool isAttacking, int attackPower);
	// 描画
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
	virtual int GetAttackPower() const override;

private:
	int m_parentModel;
	int m_currentAttackPower;
};

