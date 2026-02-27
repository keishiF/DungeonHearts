#include "PlayerStateJump.h"

#include "Camera/Camera.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/PlayerWeapon.h"
#include "MyLib/Input.h"
#include "PlayerStateAirAttack1.h"
#include "PlayerStateFall.h"
#include "PlayerStateFire.h"
#include "PlayerStateThunder.h"

namespace
{
    // 空中移動速度
    constexpr float kAirMoveSpeed = 8.5f;
    // ジャンプ力
    constexpr float kJumpForce = 20.0f;
    // ジャンプ力を加えるタイミング
    constexpr int kJumpApplyFrame = 24;
    // アニメーション再生速度
    constexpr float kJumpAnimSpeed = 1.5f;
    // アニメーション名
    const char* kJumpAnimName = "Player|JumpStart";
}

void PlayerStateJump::OnEntry()
{
    auto player = m_parent.lock();
    if (!player) return;
    printf("プレイヤー:ジャンプ状態エントリー\n");
    player->GetRigidbody().SetVelo(Vector3());
    player->GetAnimator().ChangeAnim(kJumpAnimName, kJumpAnimSpeed, false);
    m_frame = 0;
    // 地面から離れる
    player->GetRigidbody().SetOnGround(false);
}

void PlayerStateJump::OnUpdate(std::shared_ptr<Camera> camera)
{
    auto player = m_parent.lock();
    if (!player) return;
    auto& input = Input::GetInstance();
    auto& rb = player->GetRigidbody();

    player->GetWeapon()->Update(false, 0, 0);

    // 現在の速度を取得
    Vector3 finalVelo = rb.GetVelo();
    
    // 水平移動
    const Vector3 & camForward = camera->GetForward();
    const Vector3 & camRight = camera->GetRight();
    Vector3 moveDir = { 0.0f, 0.0f, 0.0f };
    
    if (input.IsPress("Up"))    moveDir += camForward;
    else if (input.IsPress("Down")) moveDir -= camForward;
    if (input.IsPress("Right")) moveDir += camRight;
    else if (input.IsPress("Left"))  moveDir -= camRight;
    
    if (input.IsPress("Attack"))
    {
        m_machine.lock()->ChangeState(std::make_shared<PlayerStateAirAttack1>());
        return;
    }

    if (moveDir.SqrLength() > 0.0f)
    {
        moveDir.Normalize();
        finalVelo.x = moveDir.x * kAirMoveSpeed;
        finalVelo.z = moveDir.z * kAirMoveSpeed;
    }
    else
    {
        finalVelo.x = 0.0f;
        finalVelo.z = 0.0f;
    }
    // ジャンプ
    m_frame++;
    if (m_frame == kJumpApplyFrame)
    {
        finalVelo.y = kJumpForce;
    }
    
    // 計算した最終的な速度をリジッドボディにセット
    rb.SetVelo(finalVelo);
    
    // 落下し始めたら落下状態へ
    if (rb.GetVelo().y < 0)
    {
        m_machine.lock()->ChangeState(std::make_shared<PlayerStateFall>());
        return;
    }
}

void PlayerStateJump::OnLeave()
{
    printf("プレイヤー:ジャンプ状態リーブ\n");
}
