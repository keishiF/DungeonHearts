#include "BossStateKnockEnd.h"

#include "Actor/Character/Enemy/Boss/Boss.h"

namespace
{
	constexpr float kAnimSpeed = 1.0f;
	const char* kAnimName = "KnockEnd";
}

void BossStateKnockEnd::OnEntry()
{
	auto boss = m_parent.lock();
	if (!boss) return;
	printf("ボス：ダウン終了状態エントリー\n");
	boss->GetAnimator().ChangeAnim(kAnimName, kAnimSpeed, false);
	boss->GetRigidbody().SetVelo(Vector3());
}

void BossStateKnockEnd::OnUpdate(std::shared_ptr<Camera> camera)
{
}

void BossStateKnockEnd::OnLeave()
{
	printf("ボス：ダウン終了状態リーブ\n");
}
