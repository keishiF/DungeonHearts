#include "BossStateShot.h"

#include "Actor/ActorController.h"
#include "Actor/Character/Enemy/Boss/Boss.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/BossBullet.h"
#include "BossStateIdle.h"

namespace
{
	constexpr float kAnimSpeed = 0.25f;
	
	constexpr float kShotCooldown = 2.0f;
	constexpr int kShotIntervalFrames = 10;
	constexpr float kTotalShotDuration = 1.0f;

	constexpr int kAtk = 1;
	constexpr float kDurationTime = 180.0f;

	const char* kAnimName = "Attack6_Shoot";
}

void BossStateShot::OnEntry()
{
	auto boss = m_parent.lock();
	if (!boss) return;
	printf("ボス：遠隔攻撃状態エントリー\n");
	boss->GetAnimator().ChangeAnim(kAnimName, kAnimSpeed, false);
	boss->GetRigidbody().SetVelo(Vector3());
}

void BossStateShot::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto boss = m_parent.lock();
	if (!boss) return;

	auto actorController = boss->GetActorController().lock();
	if (!actorController) return;
	auto player = actorController->GetPlayer();
	if (!player) return;

	m_stateDuration += 1.0f / 60.0f; 
	m_shotFrameCounter++;
	
	if (m_shotFrameCounter >= kShotIntervalFrames)
	{
		auto actorController = boss->GetActorController().lock();
		if (!actorController) return;
		
		auto bullet = std::make_shared<BossBullet>();
		VECTOR framePosDx = MV1GetFramePosition(boss->GetModel(), 76);
		Vector3 framePos = { framePosDx.x, framePosDx.y, framePosDx.z };
		Vector3 playerPos = player->GetRigidbody().GetPos();
		playerPos.y += 150.0f;
		bullet->Init(framePos, playerPos, kDurationTime, kAtk);
		actorController->SpawnAttack(bullet);
		m_shotFrameCounter = 0;
	}
	
	if (m_stateDuration >= kTotalShotDuration)
	{
		boss->StartCooldown(kShotCooldown);
		m_machine.lock()->ChangeState(std::make_shared<BossStateIdle>());
	}
}

void BossStateShot::OnLeave()
{
	printf("ボス：遠隔状態リーブn");
}
