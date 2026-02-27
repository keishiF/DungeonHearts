#include "PlayerStateTitle.h"

#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/PlayerWeapon.h"
#include "Camera/Camera.h"
#include "MyLib/Input.h"

namespace
{
	constexpr float kWalkSpeed = 2.0f;
	const char* kIdleAnimName = "Player|Idle";
	constexpr float kAnimSpeed = 1.0f; // アニメーションの再生速度
	const char* kWalkAnimName = "Player|Walk"; // 歩き（走り）アニメーション
	constexpr float kMoveSpeed = 5.0f;    // 前進する速度
	constexpr int kDuration = 60;
}

void PlayerStateTitle::OnEntry()
{
	auto player = m_parent.lock();
	if (!player) return;
	printf("プレイヤー：リザルト状態エントリー (ダンス開始)\n");
	m_timer = 0;
	m_isWalk = false;
	player->GetRigidbody().SetVelo(Vector3());
	player->GetAnimator().ChangeAnim(kIdleAnimName, kAnimSpeed, true);
}

void PlayerStateTitle::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto player = m_parent.lock();
	if (!player) return;

	player->GetWeapon()->Update(false, 0, 0);

	if (m_isWalk)
	{
		Vector3 moveDir = { 0.0f, 0.0f, 1.0f };
		player->GetRigidbody().SetVelo(moveDir * kWalkSpeed);
	}
	else
	{
		// 歩行中でなければ速度をゼロにする
		player->GetRigidbody().SetVelo(Vector3());
	}
}

void PlayerStateTitle::OnLeave()
{
	auto player = m_parent.lock();
	if (!player) return;
	player->GetRigidbody().SetVelo(Vector3());
}

void PlayerStateTitle::StartWalking()
{
	auto player = m_parent.lock();
	if (!player || m_isWalk) return;

	player->GetAnimator().ChangeAnim(kWalkAnimName, kAnimSpeed, true);
	m_isWalk = true;
}
