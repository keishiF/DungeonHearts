#include "BossStateAttack.h"

#include "Actor/ActorController.h"
#include "Actor/Character/Enemy/Boss/Boss.h"
#include "Actor/Character/Player/Player.h"
#include "BossStateIdle.h"

namespace
{
	constexpr float kAnimSpeed = 1.0f;
	const char* kAnimName = "Attack5";

	constexpr float kAttackCooldown = 3.0f;
}

void BossStateAttack::OnEntry()
{
	auto boss = m_parent.lock();
	if (!boss) return;
	printf("ボス：近接攻撃状態エントリー\n");
	boss->GetAnimator().ChangeAnim(kAnimName, kAnimSpeed, false);
	boss->GetRigidbody().SetVelo(Vector3());
}

void BossStateAttack::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto boss = m_parent.lock();
	if (!boss) return;
	
	if (boss->GetAnimator().GetNextAnim().isEnd)
	{
		boss->StartCooldown(kAttackCooldown);
		m_machine.lock()->ChangeState(std::make_shared<BossStateIdle>());
	}
}

void BossStateAttack::OnLeave()
{
	printf("ボス：近接攻撃状態リーブn");
}
