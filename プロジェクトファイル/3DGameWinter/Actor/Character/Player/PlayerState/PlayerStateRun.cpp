#include "PlayerStateRun.h"

#include "Camera/Camera.h"
#include "MyLib/Input.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/PlayerWeapon.h"
#include "PlayerStateFall.h"
#include "PlayerStateFire.h"
#include "PlayerStateGroundAttack1.h"
#include "PlayerStateIdle.h"
#include "PlayerStateJump.h"
#include "PlayerStateThunder.h"

namespace
{
    // 走り速度
    constexpr float kRunSpeed = 15.0f;

    // アニメーションの再生速度
    constexpr float kRunAnimSpeed = 1.0f;

    // アニメーション名
    const char* kRunAnimName = "Player|Run";
}

void PlayerStateRun::OnEntry()
{
	auto player = m_parent.lock();
	if (!player) return;
	printf("プレイヤー：走行状態エントリー\n");
    player->GetRigidbody().SetVelo(Vector3());
	player->GetAnimator().ChangeAnim(kRunAnimName, kRunAnimSpeed, true);
}

void PlayerStateRun::OnUpdate(std::shared_ptr<Camera> camera)
{
    auto& input = Input::GetInstance();
	auto player = m_parent.lock();
	if (!player) return;

    player->GetWeapon()->Update(false, 0, 0);

    // 地面にいなければ落下状態へ
    if (!player->GetRigidbody().IsOnGround())
    {
        m_machine.lock()->ChangeState(std::make_shared<PlayerStateFall>());
        return;
    }

    // カメラの向きを基準に移動方向を決定
    const Vector3& camForward = camera->GetForward();
    const Vector3& camRight = camera->GetRight();
    
    Vector3 moveDir = { 0.0f, 0.0f, 0.0f };
    
    if (input.IsPress("Up"))
    {
        moveDir += camForward;
    }
    else if (input.IsPress("Down"))
    {
        moveDir -= camForward;
    }
    
    if (input.IsPress("Right"))
    {
        moveDir += camRight;
    }
    else if (input.IsPress("Left"))
    {
        moveDir -= camRight;
    }
    
    // 移動入力がある場合
    if (moveDir.SqrLength() > 0.0f)
    {
        moveDir.Normalize();
        player->GetRigidbody().SetVelo(moveDir * kRunSpeed);
    }
    else
    {
        // 入力がない場合は待機状態に移行
        m_machine.lock()->ChangeState(std::make_shared<PlayerStateIdle>());
        return;
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
        m_machine.lock()->ChangeState(std::make_shared<PlayerStateFire>());
    }*/
}

void PlayerStateRun::OnLeave()
{
	printf("プレイヤー：走行状態リーブ\n");
    auto player = m_parent.lock();
    player->GetRigidbody().SetVelo(Vector3());
}
