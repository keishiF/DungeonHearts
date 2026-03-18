#pragma once
#include "Actor/Actor.h"
#include "Actor/Character/StateMachine.h"

class Camera;
class EnemyBase;
class PlayerWeapon;
class Thunder;
class Player : public Actor
{
public:
	Player();
	~Player();

	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
	virtual void Draw() override;
	virtual void OnDamage(int atk) override;

	void Update(std::shared_ptr<Camera> camera);

	void LockOn(const std::shared_ptr<EnemyBase>& enemy) { m_lockOnEnemy = enemy; m_isLockOn = true; }
	void LockOff() { m_lockOnEnemy = nullptr; m_isLockOn = false; }

	Vector3 GetPos() const { return m_rigidbody.GetPos(); }
	std::shared_ptr<PlayerWeapon> GetWeapon() { return m_weapon; }
	std::shared_ptr<StateMachine<Player>> GetState() const { return m_state; }
	bool IsLockOn() const { return m_isLockOn; }
	std::shared_ptr<EnemyBase> GetLockedOnEnemy() const { return m_lockOnEnemy; }
	void SetForward(Vector3 forward) { m_forward = forward; }

	void ChangeState(std::shared_ptr<StateNode<Player>> newState)
	{ m_state->ChangeState(newState); }

private:
	bool m_isLockOn;

	std::shared_ptr<StateMachine<Player>> m_state;
	std::shared_ptr<PlayerWeapon> m_weapon;

	std::shared_ptr<EnemyBase> m_lockOnEnemy;
};

