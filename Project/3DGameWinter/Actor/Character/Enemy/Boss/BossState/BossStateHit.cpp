#include "BossStateHit.h"

#include "Actor/ActorController.h"
#include "Actor/Character/Enemy/Boss/Boss.h"
#include "Actor/Character/Player/Player.h"
#include "BossStateIdle.h"

namespace
{
	constexpr float kAnimSpeed = 1.0f;
	const char* kAnimName = "GetHit_F";

	constexpr float kFindRange = 2000.0f;
	constexpr float kAttackRange = 500.0f;
}

void BossStateHit::OnEntry()
{
	auto boss = m_parent.lock();
	if (!boss) return;
	printf("ボス：被弾状態エントリー\n");
	boss->GetRigidbody().SetVelo(Vector3());
	boss->GetAnimator().ChangeAnim(kAnimName, kAnimSpeed, false);
}

void BossStateHit::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto boss = m_parent.lock();
	if (!boss) return;

	// アニメーションが終了したら状態遷移
	if (boss->GetAnimator().GetNextAnim().isEnd)
	{
		m_machine.lock()->ChangeState(std::make_shared<BossStateIdle>());
	}
}

void BossStateHit::OnLeave()
{
	printf("ボス：被弾状態リーブ\n");
}
