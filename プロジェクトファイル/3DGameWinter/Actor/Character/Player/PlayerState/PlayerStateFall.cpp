#include "PlayerStateFall.h"

#include "Camera/Camera.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/PlayerWeapon.h"
#include "MyLib/Input.h"
#include "PlayerStateAirAttack1.h"
#include "PlayerStateFire.h"
#include "PlayerStateLand.h"
#include "PlayerStateThunder.h"

namespace
{
    // 空中での移動速度
    constexpr float kAirMoveSpeed = 10.0f;
    // アニメーション再生速度
    constexpr float kFallAnimSpeed = 0.5f;
    // アニメーション名
    const char* kFallAnimName = "Player|Jump";
}

void PlayerStateFall::OnEntry()
{
    auto player = m_parent.lock();
    if (!player) return;
    printf("プレイヤー:落下状態エントリー\n");
    player->GetRigidbody().SetVelo(Vector3());
    player->GetAnimator().ChangeAnim(kFallAnimName, kFallAnimSpeed, true);
}

void PlayerStateFall::OnUpdate(std::shared_ptr<Camera> camera)
{
    auto player = m_parent.lock();
    if (!player) return;
    auto& input = Input::GetInstance();
    auto& rb = player->GetRigidbody();

    player->GetWeapon()->Update(false, 0, 0);

    // 着地したら着地状態へ
    if (player->GetRigidbody().IsOnGround())
    {
        m_machine.lock()->ChangeState(std::make_shared<PlayerStateLand>());
        return;
    }

    // 現在の速度を取得
    Vector3 finalVelo = rb.GetVelo();

    // 水平移動
    const Vector3& camForward = camera->GetForward();
    const Vector3& camRight = camera->GetRight();
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
    rb.SetVelo(finalVelo);
}

void PlayerStateFall::OnLeave()
{
    printf("プレイヤー:落下状態リーブ\n");
}
