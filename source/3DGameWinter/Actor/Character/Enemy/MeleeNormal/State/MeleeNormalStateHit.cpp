#include "MeleeNormalStateHit.h"

#include "Actor/Character/Enemy/MeleeNormal/MeleeNormal.h"
#include "MeleeNormalStateIdle.h"

namespace
{
	constexpr float kHitAnimSpeed = 0.25f;
	const char* kHitAnimName = "CharacterArmature|HitReact";

	constexpr float kFindRange = 750.0f;
	constexpr float kAttackRange = 200.0f;
}

void MeleeNormalStateHit::OnEntry()
{
	auto meleeNormal = m_parent.lock();
	if (!meleeNormal) return;
	printf("近接エネミー：被弾状態エントリー\n");
	meleeNormal->GetRigidbody().SetVelo(Vector3());
	meleeNormal->GetAnimator().ChangeAnim(kHitAnimName, kHitAnimSpeed, false);
}

void MeleeNormalStateHit::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto meleeNormal = m_parent.lock();
	if (!meleeNormal) return;

	// アニメーションが終了したら状態遷移
	if (meleeNormal->GetAnimator().GetNextAnim().isEnd)
	{
		m_machine.lock()->ChangeState(std::make_shared<MeleeNormalStateIdle>());
	}
}

void MeleeNormalStateHit::OnLeave()
{
	printf("近接エネミー：被弾状態リーブ\n");
}
