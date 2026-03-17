#include "PlayerStateDead.h"

#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/PlayerWeapon.h"

namespace
{
	constexpr float kDeadAnimSpeed = 1.25f;
	const char* kDeadAnimName = "Player|Dead";
}

void PlayerStateDead::OnEntry()
{
	auto player = m_parent.lock();
	if (!player) return;
	printf("プレイヤー：死亡状態エントリー\n");
	player->GetRigidbody().SetVelo(Vector3());
	player->GetAnimator().ChangeAnim(kDeadAnimName, kDeadAnimSpeed, false);
}

void PlayerStateDead::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto player = m_parent.lock();
	if (!player) return;

	player->GetWeapon()->Update(false, 0, 10);

	// アニメーションが終了したら死亡フラグを立てる
	if (player->GetAnimator().GetNextAnim().isEnd)
	{
		player->SetDead(true);
	}
}

void PlayerStateDead::OnLeave()
{
	printf("プレイヤー：死亡状態リーブ\n");
}
