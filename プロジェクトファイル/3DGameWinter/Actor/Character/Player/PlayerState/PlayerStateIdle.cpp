#include "PlayerStateIdle.h"

#include "Camera/Camera.h"
#include "MyLib/Input.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/PlayerWeapon.h"
#include "PlayerStateFall.h"
#include "PlayerStateFire.h"
#include "PlayerStateGroundAttack1.h"
#include "PlayerStateJump.h"
#include "PlayerStateRun.h"
#include "PlayerStateThunder.h"

namespace
{
    // アニメーションの再生速度
    constexpr float kIdleAnimSpeed = 0.98f;

	// アニメーション名
	const char* kIdleAnimName = "Player|Idle";
}

void PlayerStateIdle::OnEntry()
{
	auto player = m_parent.lock();
	if (!player) return;
	printf("プレイヤー：待機状態エントリー\n");
	player->GetRigidbody().SetVelo(Vector3());
	player->GetAnimator().ChangeAnim(kIdleAnimName, kIdleAnimSpeed, true);
}

void PlayerStateIdle::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto player = m_parent.lock();
	if (!player) return;
	auto& input = Input::GetInstance();

	player->GetWeapon()->Update(false, 0, 0);

	// 地面にいなければ落下状態へ
	if (!player->GetRigidbody().IsOnGround())
	{
		m_machine.lock()->ChangeState(std::make_shared<PlayerStateFall>());
		return;
	}
	// 地面にいる間は滑らないように速度を0にする
	else
	{
		player->GetRigidbody().SetVelo(Vector3());
	}

	if (input.IsTrigger("Jump"))
	{
		m_machine.lock()->ChangeState(std::make_shared<PlayerStateJump>());
		return;
	}

	if (input.IsTrigger("Attack"))
	{
		m_machine.lock()->ChangeState(std::make_shared<PlayerStateGroundAttack1>());
	}

    if (input.IsTrigger("Magic1"))
    {
        m_machine.lock()->ChangeState(std::make_shared<PlayerStateFire>());
    }
    /*else if (input.IsTrigger("Magic2"))
    {
        m_machine.lock()->ChangeState(std::make_shared<PlayerStateThunder>());
    }*/

	if (input.IsPress("Up")   || input.IsPress("Right") || 
		input.IsPress("Down") || input.IsPress("Left"))
	{
		m_machine.lock()->ChangeState(std::make_shared<PlayerStateRun>());
		return;
	}
}

void PlayerStateIdle::OnLeave()
{
	printf("プレイヤー：待機状態リーブ\n");
}
