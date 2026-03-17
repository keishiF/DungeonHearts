#include "MeleeNormalStateDead.h"

#include "Actor/Character/Enemy/MeleeNormal/MeleeNormal.h"

namespace
{
	constexpr float kDeadAnimSpeed = 0.25f;
	const char* kDeadAnimName = "CharacterArmature|Death";
}

void MeleeNormalStateDead::OnEntry()
{
	auto meleeNormal = m_parent.lock();
	if (!meleeNormal) return;
	printf("近接エネミー：死亡状態エントリー\n");
	meleeNormal->GetRigidbody().SetVelo(Vector3());
	meleeNormal->GetAnimator().ChangeAnim(kDeadAnimName, kDeadAnimSpeed, false);
}

void MeleeNormalStateDead::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto meleeNormal = m_parent.lock();
	if (!meleeNormal) return;
	
	// アニメーションが終了したら死亡フラグを立てる
	if (meleeNormal->GetAnimator().GetNextAnim().isEnd)
	{
		meleeNormal->SetDead(true);
	}
}

void MeleeNormalStateDead::OnLeave()
{
	printf("近接エネミー：死亡状態リーブ\n");
}
