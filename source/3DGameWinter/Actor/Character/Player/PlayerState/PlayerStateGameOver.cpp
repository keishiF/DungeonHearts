#include "PlayerStateGameOver.h"

#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/PlayerWeapon.h"

namespace
{
	constexpr float kAnimSpeed = 300.0f;
	const char* kAnimName = "Player|Dead";
}

void PlayerStateGameOver::OnEntry()
{
	auto player = m_parent.lock();
	if (!player) return;
	printf("プレイヤー：ゲームオーバー状態エントリー\n");
	player->GetRigidbody().SetVelo(Vector3());
	player->GetAnimator().ChangeAnim(kAnimName, kAnimSpeed, false);
}

void PlayerStateGameOver::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto player = m_parent.lock();
	if (!player) return;
	player->GetWeapon()->Update(false, 0, 0);
}

void PlayerStateGameOver::OnLeave()
{
}
