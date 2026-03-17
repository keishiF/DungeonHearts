#include "FlyNormalStateIdle.h"

#include "Actor/Character/Enemy/FlyNormal/FlyNormal.h"
#include "Actor/Character/Player/Player.h"
#include "FlyNormalStateAttack.h"
#include "FlyNormalStateMove.h"
#include "FlyNormalStateDead.h"

namespace
{
	constexpr float kIdleAnimSpeed = 0.5f;

	const char* kIdleAnimName = "CharacterArmature|Flying_Idle";

	constexpr float kFindRange = 1500.0f;
	constexpr float kAttackRange = 750.0f;
}

void FlyNormalStateIdle::OnEntry()
{
	auto flyNormal = m_parent.lock();
	if (!flyNormal) return;
	printf("飛行エネミー：待機状態エントリー\n");
	flyNormal->GetRigidbody().SetVelo(Vector3());
	flyNormal->GetAnimator().ChangeAnim(kIdleAnimName, kIdleAnimSpeed, true);
}

void FlyNormalStateIdle::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto flyNormal = m_parent.lock();
	if (!flyNormal) return;
	auto actorController = flyNormal->GetActorController().lock();
	if (!actorController) return;
	auto player = actorController->GetPlayer();
	if (!player) return;

	const Vector3& playerPos = player->GetRigidbody().GetPos();
	const Vector3& myPos = flyNormal->GetRigidbody().GetPos();

	float distance = (playerPos - myPos).Length();

	if (flyNormal->GetStatusComp().m_hp <= 0)
	{
		m_machine.lock()->ChangeState(std::make_shared<FlyNormalStateDead>());
	}
	else if (distance < kAttackRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<FlyNormalStateAttack>());
	}
	else if (distance < kFindRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<FlyNormalStateMove>());
	}
	else
	{
		flyNormal->GetRigidbody().SetVelo(Vector3());
	}
}

void FlyNormalStateIdle::OnLeave()
{
	printf("飛行エネミー：待機状態リーブ\n");
}
