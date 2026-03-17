#include "BossStateDeath.h"

#include "Actor/ActorController.h"
#include "Actor/Character/Enemy/Boss/Boss.h"
#include <EffekseerForDxLib.h>

namespace
{
	constexpr float kAnimSpeed = 0.5f;
	const char* kIdleDeathAnimName = "Death1";
	const char* kKnockDeathAnimName = "KnockDeath";
}

BossStateDeath::~BossStateDeath()
{
	if (m_deadEffect != -1)
	{
		DeleteEffekseerEffect(m_deadEffect);
	}
}

void BossStateDeath::OnEntry()
{
	auto boss = m_parent.lock();
	if (!boss) return;
	printf("ボス：死亡状態エントリー\n");
	m_deadEffect = LoadEffekseerEffect("Data/Effect/Dead.efkefc", 75.0f);
	assert(m_deadEffect >= 0);
	boss->GetRigidbody().SetVelo(Vector3());
	boss->GetAnimator().ChangeAnim(kIdleDeathAnimName, kAnimSpeed, false);
}

void BossStateDeath::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto boss = m_parent.lock();
	if (!boss) return;

	if (m_playingEffect == -1)
	{
		m_playingEffect = PlayEffekseer3DEffect(m_deadEffect);
	}
	SetPosPlayingEffekseer3DEffect(m_playingEffect,
		boss->GetRigidbody().GetPos().ToDxVECTOR().x,
		boss->GetRigidbody().GetPos().ToDxVECTOR().y,
		boss->GetRigidbody().GetPos().ToDxVECTOR().z);

	if (IsEffekseer3DEffectPlaying(m_playingEffect))
	{
		boss->SetDead(true);
	}

	//// アニメーションが終了したら死亡フラグを立てる
	//if (boss->GetAnimator().GetNextAnim().isEnd)
	//{
	//	boss->SetDead(true);
	//}
}

void BossStateDeath::OnLeave()
{
	printf("ボス：死亡状態リーブ\n");
}
