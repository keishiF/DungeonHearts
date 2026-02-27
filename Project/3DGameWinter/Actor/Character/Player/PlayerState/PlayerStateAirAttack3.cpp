#include "PlayerStateAirAttack3.h"

#include "Actor/Character/Enemy/EnemyBase.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/PlayerWeapon.h"
#include "Camera/Camera.h"
#include "MyLib/Input.h"
#include "PlayerStateAirAttack1.h"
#include "PlayerStateAirAttack4.h"
#include "PlayerStateFall.h"
#include "PlayerStateRun.h"

namespace
{
	// アニメーションの再生速度
	constexpr float kAirAtk3AnimSpeed = 1.75f;

	// アニメーション名
	const char* kAirAtk3AnimName = "Player|AirAttack3";

	// 回転補間速度
	constexpr float kLerpT = 0.2f;

	// 攻撃中の前進速度
	constexpr float kAttackForwardSpeed = 5.0f;
}

void PlayerStateAirAttack3::OnEntry()
{
	auto player = m_parent.lock();
	if (!player) return;
	printf("プレイヤー：空中攻撃3段目状態エントリー\n");
	m_atkSE = LoadSoundMem("Data/Sound/SE/PlayerAtkSE.mp3");
	assert(m_atkSE >= 0);
	player->GetRigidbody().SetGravity(false);
	player->GetRigidbody().SetVelo(Vector3());
	player->GetAnimator().ChangeAnim(kAirAtk3AnimName, kAirAtk3AnimSpeed, false);
}

void PlayerStateAirAttack3::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto player = m_parent.lock();
	if (!player) return;
	player->GetWeapon()->Update(true, 0, 10);

	auto& input = Input::GetInstance();
	if (input.IsTrigger("Attack"))
	{
		m_isCombo = true;
		return;
	}

	// ロックオン中の敵がいる場合、その方向を向く
	if (player->IsLockOn() && player->GetLockedOnEnemy())
	{
		Vector3 enemyPos = player->GetLockedOnEnemy()->GetRigidbody().GetPos();
		Vector3 playerPos = player->GetRigidbody().GetPos();
		Vector3 dir = enemyPos - playerPos;
		dir.y = 0.0f; // Y軸は無視して水平方向のみ
		dir.Normalize();

		float angle = std::atan2(dir.x, dir.z);
		Quaternion targetRot;
		Vector3 yVec = { 0.0f, 1.0f, 0.0f };
		targetRot.AngleAxis(angle, yVec);

		Quaternion currentRot = player->GetRotation();

		// 向きの補間
		Quaternion newRot = Quaternion::Slerp(currentRot, targetRot, kLerpT);
		player->SetRotQ(newRot);
		MV1SetRotationXYZ(player->GetModel(), VGet(0.0f, newRot.ToEulerY(), 0.0f));
	}

	// ロックオン中の敵がいる場合、その方向に少し前進する
	if (player->IsLockOn() && player->GetLockedOnEnemy())
	{
		Vector3 enemyPos = player->GetLockedOnEnemy()->GetRigidbody().GetPos();
		Vector3 playerPos = player->GetRigidbody().GetPos();
		Vector3 dir = enemyPos - playerPos;
		dir.y = 0.0f; // Y軸は無視して水平方向のみ
		dir.Normalize();
		player->GetRigidbody().SetVelo(dir * kAttackForwardSpeed);
	}
	else
	{
		player->GetRigidbody().SetVelo(Vector3());
	}

	// アニメーションの現在フレームを取得
	const float currentAnimFrame = player->GetAnimator().GetCurrentFrame();

	// 攻撃判定が有効なフレーム範囲
	constexpr float kAttackStartFrame = 32.0f;
	constexpr float kAttackEndFrame = 40.0f;

	// 武器の当たり判定を有効化/無効化
	bool isWeaponActive = (currentAnimFrame >= kAttackStartFrame && currentAnimFrame <= kAttackEndFrame);
	PlaySoundMem(m_atkSE, DX_PLAYTYPE_BACK);
	player->GetWeapon()->Update(isWeaponActive, 0, 10);


	const float currentFrame = m_parent.lock()->GetAnimator().GetCurrentFrame();
	const float totalTime = m_parent.lock()->GetAnimator().GetAnimTransitionFrame();
	if (currentFrame >= totalTime)
	{
		if (m_isCombo)
		{
			m_machine.lock()->ChangeState(std::make_shared<PlayerStateAirAttack1>());
		}
		else if (input.IsTrigger("Run"))
		{
			m_machine.lock()->ChangeState(std::make_shared<PlayerStateRun>());
		}
		else
		{
			m_machine.lock()->ChangeState(std::make_shared<PlayerStateFall>());
		}
		m_isCombo = false;
		return;
	}
}

void PlayerStateAirAttack3::OnLeave()
{
	auto player = m_parent.lock();
	if (!player) return;
	player->GetRigidbody().SetGravity(true);
	printf("プレイヤー：空中攻撃3段目状態リーブ\n");
}
