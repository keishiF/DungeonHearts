#include "FlyNormalStateDead.h"

#include "Actor/Character/Enemy/FlyNormal/FlyNormal.h"

namespace
{
	constexpr float kDeadAnimSpeed = 0.25f;
	const char* kDeadAnimName = "CharacterArmature|Death";
}

void FlyNormalStateDead::OnEntry()
{
	auto flyNormal = m_parent.lock();
	if (!flyNormal) return;
	printf("飛行エネミー：死亡状態エントリー\n");
	flyNormal->GetRigidbody().SetVelo(Vector3());
	flyNormal->GetAnimator().ChangeAnim(kDeadAnimName, kDeadAnimSpeed, false);
}

void FlyNormalStateDead::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto flyNormal = m_parent.lock();
	if (!flyNormal) return;

	// アニメーションが終了したら死亡フラグを立てる
	if (flyNormal->GetAnimator().GetNextAnim().isEnd)
	{
		flyNormal->SetDead(true);
	}
}

void FlyNormalStateDead::OnLeave()
{
	printf("飛行エネミー：死亡状態リーブ\n");
}
