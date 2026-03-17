#include "BossStateMove.h"

#include "Actor/ActorController.h"
#include "Actor/Character/Enemy/Boss/Boss.h"
#include "Actor/Character/Player/Player.h"
#include "BossStateAttack.h"
#include "BossStateDeath.h"
#include "BossStateLaser.h"
#include "BossStateIdle.h"
#include "BossStateShot.h"

namespace
{
	// 探知範囲
	constexpr float kFindRange = 2000.0f;
	// 射撃を開始する距離
	constexpr float kShotRange = 1000.0f;
	// 近接攻撃を開始する距離
	constexpr float kAttackRange = 600.0f;

	constexpr float kMoveSpeed = 5.0f;
	constexpr float kLerpT = 0.2f;

	constexpr float kAnimSpeed = 1.0f;

	const char* kAnimName = "Walk_F";
}

void BossStateMove::OnEntry()
{
	auto boss = m_parent.lock();
	if (!boss) return;
	printf("ボス：移動状態エントリー\n");
	boss->GetAnimator().ChangeAnim(kAnimName, kAnimSpeed, true);
	boss->GetRigidbody().SetVelo(Vector3());
}

void BossStateMove::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto boss = m_parent.lock();
	if (!boss) return;

	auto actorController = boss->GetActorController().lock();
	if (!actorController) return;
	auto player = actorController->GetPlayer();
	if (!player) return;

	const Vector3& playerPos = player->GetRigidbody().GetPos();
	const Vector3& myPos = boss->GetRigidbody().GetPos();

	float distance = (playerPos - myPos).Length();

	if (boss->GetStatusComp().m_hp <= 0)
	{
		m_machine.lock()->ChangeState(std::make_shared<BossStateDeath>());
		return;
	}
	else if (distance < kShotRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<BossStateShot>());
		return;
	}
	/*else if (distance < kAttackRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<BossStateAttack>());
		return;
	}
	else if (distance < kShotRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<BossStateShot>());
		return;
	}
	else if (distance > kFindRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<BossStateIdle>());
		return;
	}*/

	// プレイヤーの方向を向く
	Vector3 dir = playerPos - myPos;
	dir.y = 0.0f; // 上下方向は無視
	dir.Normalize();

	// プレイヤーに向かって移動
	boss->GetRigidbody().SetVelo(dir * kMoveSpeed);

	// 進行方向にモデルを回転させる
	Vector3 velocity = boss->GetRigidbody().GetVelo();
	if (velocity.x != 0.0f || velocity.z != 0.0f)
	{
		float angle = std::atan2(velocity.x, velocity.z) + DX_PI_F;
		Quaternion targetRot;
		Vector3 yVec = { 0.0f, 1.0f, 0.0f };
		targetRot.AngleAxis(angle, yVec);

		Quaternion currentRot = boss->GetRotation();
		Quaternion newRot = Quaternion::Slerp(currentRot, targetRot, kLerpT);
		boss->SetRotQ(newRot);

		MV1SetRotationXYZ(boss->GetModel(), VGet(0.0f, newRot.ToEulerY() + DX_PI_F, 0.0f));
	}
}

void BossStateMove::OnLeave()
{
	printf("ボス：移動状態リーブ\n");
}
