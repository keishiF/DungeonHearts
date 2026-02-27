#include "MeleeNormalStateIdle.h"

#include "Actor/Character/Enemy/MeleeNormal/MeleeNormal.h"
#include "Actor/Character/Player/Player.h"
#include "MeleeNormalStateAttack.h"
#include "MeleeNormalStateChase.h"
#include "MeleeNormalStateDead.h"

namespace
{
	constexpr float kIdleAnimSpeed = 0.5f;

	const char* kIdleAnimName = "CharacterArmature|Idle";

	constexpr float kFindRange = 750.0f;
	constexpr float kAttackRange = 200.0f;
}

void MeleeNormalStateIdle::OnEntry()
{
	auto meleeNormal = m_parent.lock();
	if (!meleeNormal) return;
	printf("近接エネミー：待機状態エントリー\n");
	meleeNormal->GetRigidbody().SetVelo(Vector3());
	meleeNormal->GetAnimator().ChangeAnim(kIdleAnimName, kIdleAnimSpeed, true);
}

void MeleeNormalStateIdle::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto meleeNormal = m_parent.lock();
	if (!meleeNormal) return;
	auto actorController = meleeNormal->GetActorController().lock();
	if (!actorController) return;
	auto player = actorController->GetPlayer();
	if (!player) return;

	const Vector3 & playerPos = player->GetRigidbody().GetPos();
	const Vector3 & myPos = meleeNormal->GetRigidbody().GetPos();
	
	float distance = (playerPos - myPos).Length();
	
	if (meleeNormal->GetStatusComp().m_hp <= 0)
	{
		m_machine.lock()->ChangeState(std::make_shared<MeleeNormalStateDead>());
	}
	else if (distance < kAttackRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<MeleeNormalStateAttack>());
	}
	else if (distance < kFindRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<MeleeNormalStateChase>());
	}
	else
	{
		meleeNormal->GetRigidbody().SetVelo(Vector3());
	}
}

void MeleeNormalStateIdle::OnLeave()
{
	printf("近接エネミー：待機状態リーブ\n");
}
