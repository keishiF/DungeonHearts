#include "PlayerStateHit.h"

#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/PlayerWeapon.h"
#include "PlayerStateIdle.h"

namespace
{
	constexpr float kHitAnimSpeed = 1.0f;
	const char* kHitAnimName = "Player|Hit";
}

void PlayerStateHit::OnEntry()
{
	auto player = m_parent.lock();
	if (!player) return;
	printf("プレイヤー：被弾状態エントリー\n");
	player->GetRigidbody().SetVelo(Vector3());
	player->GetAnimator().ChangeAnim(kHitAnimName, kHitAnimSpeed, false);
}

void PlayerStateHit::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto player = m_parent.lock();
	if (!player) return;

	player->GetWeapon()->Update(false, 0, 0);

	// アニメーションが終了したら状態遷移
	if (player->GetAnimator().GetNextAnim().isEnd)
	{
		m_machine.lock()->ChangeState(std::make_shared<PlayerStateIdle>());
	}
}

void PlayerStateHit::OnLeave()
{
	printf("プレイヤー：被弾状態リーブ\n");
}
