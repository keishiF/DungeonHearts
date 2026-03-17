#include "FlyNormalStateWait.h"

#include "Actor/Character/Enemy/FlyNormal/FlyNormal.h"
#include "Actor/Character/Player/Player.h"
#include "FlyNormalStateAttack.h"
#include "FlyNormalStateMove.h"
#include "FlyNormalStateIdle.h"

namespace
{
    constexpr int kWaitTime = 180;
    constexpr float kIdealDistance = 300.0f;

    constexpr float kMoveSpeed = 4.0f;
    // プレイヤーとの適切な距離
    constexpr float kIdealHeight = 150.0f;

    constexpr float kFindRange = 1500.0f;
    constexpr float kAttackRange = 750.0f;

    constexpr float kIdleAnimSpeed = 0.5f;
    const char* kIdleAnimName = "CharacterArmature|Idle";
    constexpr float kChaseAnimSpeed = 0.5f;
    const char* kChaseAnimName = "CharacterArmature|Fast_Flying";

    constexpr float kLerpT = 0.2f;
}

void FlyNormalStateWait::OnEntry()
{
    auto flyNormal = m_parent.lock();
    if (!flyNormal) return;
    printf("飛行エネミー：待機後退状態エントリー\n");
    m_timer = 0;
    SubState m_subState = SubState::Idle;
    flyNormal->GetRigidbody().SetVelo(Vector3());
    flyNormal->GetAnimator().ChangeAnim(kIdleAnimName, kIdleAnimSpeed, true);
}

void FlyNormalStateWait::OnUpdate(std::shared_ptr<Camera> camera)
{
    auto flyNormal = m_parent.lock();
    if (!flyNormal) return;
    auto actorController = flyNormal->GetActorController().lock();
    if (!actorController) return;
    auto player = actorController->GetPlayer();
    if (!player) return;

    m_timer++;

    // プレイヤーとの距離を保つ
    const Vector3& playerPos = player->GetRigidbody().GetPos();
    const Vector3& myPos = flyNormal->GetRigidbody().GetPos();
    Vector3 dirToPlayer = playerPos - myPos;
    float distance = dirToPlayer.Length();

    SubState requiredState = SubState::Idle;
    if (distance > kIdealDistance)
    {
        requiredState = SubState::Chase;
        // 移動処理
        // プレイヤーの少し上を目標地点とする
        Vector3 targetPos = playerPos;
        targetPos.y += kIdealHeight;

        // 目標への方向ベクトル
        Vector3 dir = targetPos - myPos;
        dir.Normalize();

        // 移動
        flyNormal->GetRigidbody().SetVelo(dir * kMoveSpeed);

        // 向きの制御
        // プレイヤーの方向を向く（Y軸は無視）
        Vector3 lookDir = playerPos - myPos;
        lookDir.y = 0.0f;
        lookDir.Normalize();

        if (lookDir.SqrLength() > 0.001f)
        {
            float angle = std::atan2(lookDir.x, lookDir.z);
            Quaternion targetRot;
            Vector3 yVec = { 0.0f, 1.0f, 0.0f };
            targetRot.AngleAxis(angle, yVec);

            Quaternion currentRot = flyNormal->GetRotation();
            Quaternion newRot = Quaternion::Slerp(currentRot, targetRot, kLerpT);
            flyNormal->SetRotQ(newRot);

            MV1SetRotationXYZ(flyNormal->GetModel(),
                VGet(0.0f, newRot.ToEulerY() + DX_PI_F, 0.0f));
        }
    }
    else
    {
        requiredState = SubState::Idle;
        // 近い場合は停止
        flyNormal->GetRigidbody().SetVelo(Vector3());
    }

    // サブステートが変更された場合のみアニメーションを切り替える
    if (requiredState != m_subState)
    {
        m_subState = requiredState;
        if (m_subState == SubState::Chase)
        {
            flyNormal->GetAnimator().ChangeAnim(kChaseAnimName, kChaseAnimSpeed, true);
        }
        else
        {
            flyNormal->GetAnimator().ChangeAnim(kIdleAnimName, kIdleAnimSpeed, true);
        }
    }

    if (m_timer >= kWaitTime)
    {
        if (distance < kAttackRange)
        {
            m_machine.lock()->ChangeState(std::make_shared<FlyNormalStateAttack>());
        }
        else if (distance < kFindRange)
        {
            m_machine.lock()->ChangeState(std::make_shared<FlyNormalStateMove>());
        }
        else
        {
            m_machine.lock()->ChangeState(std::make_shared<FlyNormalStateIdle>());
        }
    }
}

void FlyNormalStateWait::OnLeave()
{
}
