#include "BossStateGetUp.h"

#include "Actor/ActorController.h"
#include "Actor/Character/Enemy/Boss/Boss.h"
#include "Actor/Character/Player/Player.h"
#include "BossStateIdle.h"
#include <EffekseerForDXLib.h>

namespace
{
	constexpr float kHp = 15.0f;

	const char* kGetUpAnimName = "TransitionIdle1ToIdle3";
	const char* kRoarAnimName = "Roar";

	constexpr float kGetUpAnimSpeed = 0.4f;
	constexpr float kRoarAnimSpeed = 0.5f;
}

BossStateGetUp::~BossStateGetUp()
{
	if (m_roarEffect != -1)
	{
		DeleteEffekseerEffect(m_roarEffect);
	}
}

void BossStateGetUp::OnEntry()
{
	auto boss = m_parent.lock();
	if (!boss) return;
	printf("ボス：起き上がり状態エントリー\n");
	m_roarEffect = LoadEffekseerEffect("Data/Effect/Roar.efkefc", 50.0f);
	boss->GetAnimator().ChangeAnim(kGetUpAnimName, kGetUpAnimSpeed, false);
}

void BossStateGetUp::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto boss = m_parent.lock();
	if (!boss) return;

	if (m_isRoar)
	{
		boss->SetHp(kHp);
		boss->SetMaxHp(kHp);
		if (boss->GetAnimator().GetNextAnim().isEnd)
		{
			m_machine.lock()->ChangeState(std::make_shared<BossStateIdle>());
		}
	}

	if (boss->GetAnimator().GetNextAnim().isEnd)
	{
		boss->GetAnimator().ChangeAnim(kRoarAnimName, kRoarAnimSpeed, false);
		m_isRoar = true;
	}
}

void BossStateGetUp::OnLeave()
{
	printf("ボス：起き上がり状態リーブ\n");
}
