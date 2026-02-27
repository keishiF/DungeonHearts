#include "BossStateKnock.h"

#include "Actor/Character/Enemy/Boss/Boss.h"
#include "BossStateIdle.h"

namespace
{
	constexpr float kAnimSpeed = 1.0f;
	const char* kKnockAnimName = "KnockLoop";
	const char* kKnockStartAnimName = "KnockStart";

	constexpr int kKnockEndTime = 300;
}

void BossStateKnock::OnEntry()
{
	auto boss = m_parent.lock();
	if (!boss) return;
	printf("ボス：ダウン状態エントリー\n");
	m_timer = 0;
	m_isKnock = false;
	boss->GetAnimator().ChangeAnim(kKnockStartAnimName, kAnimSpeed, false);
	boss->GetRigidbody().SetVelo(Vector3());
}

void BossStateKnock::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto boss = m_parent.lock();
	if (!boss) return;

	if (m_isKnock)
	{
		if (++m_timer >= kKnockEndTime)
		{
			m_machine.lock()->ChangeState(std::make_shared<BossStateIdle>());
		}
	}

	if (boss->GetAnimator().GetNextAnim().isEnd)
	{
		boss->GetAnimator().ChangeAnim(kKnockAnimName, kAnimSpeed, true);
		m_isKnock = true;
	}
}

void BossStateKnock::OnLeave()
{
	printf("ボス：ダウン状態リーブ\n");
}
