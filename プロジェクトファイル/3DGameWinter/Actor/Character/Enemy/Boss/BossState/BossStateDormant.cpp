#include "BossStateDormant.h"

#include "Actor/Character/Enemy/Boss/Boss.h"
#include "Actor/Character/Player/Player.h"
#include "BossStateGetUp.h"

namespace
{
	// 探知範囲
	constexpr float kFindRange = 2000.0f;

	// アニメーション名
	const char* kAnimName = "Idle1_Toilet";

	// アニメーション再生速度
	constexpr float kAnimSpeed = 1.0f;
}

void BossStateDormant::OnEntry()
{
	auto boss = m_parent.lock();
	if (!boss) return;
	printf("ボス：睡眠状態エントリー\n");
	boss->GetAnimator().ChangeAnim(kAnimName, kAnimSpeed, false);
}

void BossStateDormant::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto boss = m_parent.lock();
	if (!boss) return;
	auto actorController = boss->GetActorController().lock();
	if (!actorController) return;
	auto player = actorController->GetPlayer();
	if (!player) return;
	
	float distanceSq = (player->GetPos() - boss->GetRigidbody().GetPos()).SqrLength();
	if (distanceSq < kFindRange * kFindRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<BossStateGetUp>());
	}
}

void BossStateDormant::OnLeave()
{
	printf("ボス：睡眠状態リーブn");
}
