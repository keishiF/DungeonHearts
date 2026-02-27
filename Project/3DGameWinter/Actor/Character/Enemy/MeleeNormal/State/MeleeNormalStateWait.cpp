#include "MeleeNormalStateWait.h"

#include "Actor/Character/Enemy/MeleeNormal/MeleeNormal.h"
#include "Actor/Character/Player/Player.h"
#include "MeleeNormalStateAttack.h"
#include "MeleeNormalStateChase.h"
#include "MeleeNormalStateIdle.h"

namespace
{
    constexpr int kWaitTime = 180;
    constexpr float kIdealDistance = 300.0f;

    constexpr float kChaseSpeed = 5.0f;
    
    constexpr float kFindRange = 750.0f;
    constexpr float kAttackRange = 200.0f;
    
    constexpr float kIdleAnimSpeed = 0.5f;
    const char* kIdleAnimName = "CharacterArmature|Idle";
    constexpr float kChaseAnimSpeed = 0.5f;
    const char* kChaseAnimName = "CharacterArmature|Walk";

    constexpr float kLerpT = 0.2f;
}

void MeleeNormalStateWait::OnEntry()
{
    auto meleeNormal = m_parent.lock();
    if (!meleeNormal) return;
    printf("近接エネミー：待機後退状態エントリー\n");
    m_timer = 0;
    meleeNormal->GetRigidbody().SetVelo(Vector3());
    meleeNormal->GetAnimator().ChangeAnim(kIdleAnimName, kIdleAnimSpeed, true);
}

void MeleeNormalStateWait::OnUpdate(std::shared_ptr<Camera> camera)
{
    auto meleeNormal = m_parent.lock();
    if (!meleeNormal) return;
    auto actorController = meleeNormal->GetActorController().lock();
    if (!actorController) return;
    auto player = actorController->GetPlayer();
    if (!player) return;
    
    m_timer++;
    
    // プレイヤーとの距離を保つ
    const Vector3 & playerPos = player->GetRigidbody().GetPos();
    const Vector3 & myPos = meleeNormal->GetRigidbody().GetPos();
    Vector3 dirToPlayer = playerPos - myPos;
    float distance = dirToPlayer.Length();
    
    SubState requiredState = SubState::Idle;
    if (distance > kIdealDistance)
    {
        requiredState = SubState::Chase;
        // 遠すぎる場合、プレイヤーに近づく
        Vector3 moveDir = dirToPlayer;
        moveDir.y = 0;
        moveDir.Normalize();
        meleeNormal->GetRigidbody().SetVelo(moveDir * kChaseSpeed); // ゆっくり近づく

        // 進行方向にモデルを回転させる
        Vector3 velocity = meleeNormal->GetRigidbody().GetVelo();
        if (velocity.x != 0.0f || velocity.z != 0.0f)
        {
            float angle = std::atan2(velocity.x, velocity.z) + DX_PI_F;
            Quaternion targetRot;
            Vector3 yVec = { 0.0f, 1.0f, 0.0f };
            targetRot.AngleAxis(angle, yVec);

            Quaternion currentRot = meleeNormal->GetRotation();
            Quaternion newRot = Quaternion::Slerp(currentRot, targetRot, kLerpT);
            meleeNormal->SetRotQ(newRot);

            MV1SetRotationXYZ(meleeNormal->GetModel(), VGet(0.0f, newRot.ToEulerY(), 0.0f));
        }
    }
    else
    {
        requiredState = SubState::Idle;
        // 近い場合は停止
        meleeNormal->GetRigidbody().SetVelo(Vector3());
    }
    
    // サブステートが変更された場合のみアニメーションを切り替える
    if (requiredState != m_subState)
    {
        m_subState = requiredState;
        if (m_subState == SubState::Chase)
        {
            meleeNormal->GetAnimator().ChangeAnim(kChaseAnimName, kChaseAnimSpeed, true);
        }
        else
        {
            meleeNormal->GetAnimator().ChangeAnim(kIdleAnimName, kIdleAnimSpeed, true);
        }
    }

    if (m_timer >= kWaitTime)
    {
        if (distance < kAttackRange)
        {
            m_machine.lock()->ChangeState(std::make_shared<MeleeNormalStateAttack>());
        }
        else if (distance < kFindRange)
        {
            m_machine.lock()->ChangeState(std::make_shared<MeleeNormalStateChase>());
        }
        else
        {
            m_machine.lock()->ChangeState(std::make_shared<MeleeNormalStateIdle>());
        }
    }
}

void MeleeNormalStateWait::OnLeave()
{
    printf("近接エネミー：待機後退状態リーブ\n");
}
