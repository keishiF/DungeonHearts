#include "BossStateIdle.h"

#include "Actor/Character/Enemy/Boss/Boss.h"
#include "Actor/Character/Player/Player.h"
#include "BossStateAttack.h"
#include "BossStateMove.h"
#include "BossStateShot.h"
#include "BossStateLaser.h"

namespace
{
	// 探知範囲
	constexpr float kFindRange = 2000.0f;
	// レーザーを開始する距離
	constexpr float kLaserRange = 1200.0f;
	// 射撃を開始する距離
	constexpr float kShotRange = 1000.0f;
	// 近接攻撃を開始する距離
	constexpr float kAttackRange = 600.0f;

	// アニメーション名
	const char* kAnimName = "Idle2";

	// アニメーション再生速度
	constexpr float kAnimSpeed = 1.0f;
}

void BossStateIdle::OnEntry()
{
	auto boss = m_parent.lock();
	if (!boss) return;
	printf("ボス：待機状態エントリー\n");
	boss->GetAnimator().ChangeAnim(kAnimName, kAnimSpeed, false);
	boss->GetRigidbody().SetVelo(Vector3());
}

void BossStateIdle::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto boss = m_parent.lock();
	if (!boss) return;

	auto actorController = boss->GetActorController().lock();
	if (!actorController) return;
	auto player = actorController->GetPlayer();
	if (!player) return;

	// プレイヤーの方向を向く
	Vector3 playerPos = player->GetRigidbody().GetPos();
	Vector3 myPos = boss->GetRigidbody().GetPos();
	Vector3 dir = playerPos - myPos;
	dir.y = 0.0f; // y軸は無視
	float distance = dir.Length();
	dir.Normalize();

	float angle = std::atan2(dir.x, dir.z);
	Quaternion targetRot;
	Vector3 yVec = { 0.0f, 1.0f, 0.0f };
	targetRot.AngleAxis(angle, yVec);
	Quaternion currentRot = boss->GetRotation();

	// 向きの補間
	Quaternion newRot = Quaternion::Slerp(currentRot, targetRot, 0.1f);
	boss->SetRotQ(newRot);
	MV1SetRotationXYZ(boss->GetModel(), VGet(0.0f, newRot.ToEulerY(), 0.0f));

	// クールダウン中は行動しない
	if (boss->IsOnCooldown())
	{
		return;
	}

	// プレイヤーとの距離に応じて状態遷移
	if (distance < kAttackRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<BossStateAttack>());
	}
	/*else if (distance < kLaserRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<BossStateLaser>());
	}*/
	else if (distance < kShotRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<BossStateShot>());
	}
	//else if (distance < kLaserRange)
	//{
	//	// ショットとレーザーをランダムに選択
	//	int randomAttack = rand() % 2;
	//	if (randomAttack == 0)
	//	{
	//		m_machine.lock()->ChangeState(std::make_shared<BossStateShot>());
	//	}
	//	else
	//	{
	//		m_machine.lock()->ChangeState(std::make_shared<BossStateLaser>());
	//	}
	//}
	else if (distance < kFindRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<BossStateMove>());
	}
}

void BossStateIdle::OnLeave()
{
	printf("ボス：待機状態リーブ\n");
}
