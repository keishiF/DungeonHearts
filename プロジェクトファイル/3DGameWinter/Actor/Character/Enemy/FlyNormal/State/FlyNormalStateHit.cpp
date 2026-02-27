#include "FlyNormalStateHit.h"

#include "Actor/Character/Enemy/FlyNormal/FlyNormal.h"
#include "FlyNormalStateIdle.h"

namespace
{
	constexpr float kHitAnimSpeed = 0.5f;
	const char* kHitAnimName = "CharacterArmature|HitReact";

	constexpr float kFindRange = 1000.0f;
	constexpr float kAttackRange = 750.0f;
}

void FlyNormalStateHit::OnEntry()
{
	auto flyNormal = m_parent.lock();
	if (!flyNormal) return;
	printf("飛行エネミー：被弾状態エントリー\n");
	flyNormal->GetRigidbody().SetVelo(Vector3());
	flyNormal->GetAnimator().ChangeAnim(kHitAnimName, kHitAnimSpeed, false);
}

void FlyNormalStateHit::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto flyNormal = m_parent.lock();
	if (!flyNormal) return;

	// アニメーションが終了したら状態遷移
	if (flyNormal->GetAnimator().GetNextAnim().isEnd)
	{
		m_machine.lock()->ChangeState(std::make_shared<FlyNormalStateIdle>());
	}
}

void FlyNormalStateHit::OnLeave()
{
	printf("飛行エネミー：被弾状態リーブ\n");
}
