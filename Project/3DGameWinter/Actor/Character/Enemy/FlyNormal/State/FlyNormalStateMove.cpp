#include "FlyNormalStateMove.h"

#include "Actor/Character/Enemy/FlyNormal/FlyNormal.h"
#include "Actor/Character/Player/Player.h"
#include "FlyNormalStateAttack.h"
#include "FlyNormalStateDead.h"
#include "FlyNormalStateIdle.h"

namespace
{
	constexpr float kMoveAnimSpeed = 0.5f;

	const char* kMoveAnimName = "CharacterArmature|Fast_Flying";

    // プレイヤーに近づく速度
    constexpr float kMoveSpeed = 4.0f;
    // プレイヤーとの適切な距離
    constexpr float kIdealHeight = 150.0f;
    
    // 索敵範囲（この範囲外に出たらIdleへ）
    constexpr float kFindRange = 1500.0f;
    // 攻撃範囲（この範囲内に入ったらAttackへ）
    constexpr float kAttackRange = 750.0f;
    
    // 向きの補間速度
    constexpr float kLerpT = 0.1f;
}

void FlyNormalStateMove::OnEntry()
{
	auto flyNormal = m_parent.lock();
	if (!flyNormal) return;
	printf("飛行エネミー：追跡状態エントリー\n");
	flyNormal->GetRigidbody().SetVelo(Vector3());
	flyNormal->GetAnimator().ChangeAnim(kMoveAnimName, kMoveAnimSpeed, true);
}

void FlyNormalStateMove::OnUpdate(std::shared_ptr<Camera> camera)
{
    auto flyNormal = m_parent.lock();
    if (!flyNormal) return;

    auto actorController = flyNormal->GetActorController().lock();
    if (!actorController) return;

    auto player = actorController->GetPlayer();
    if (!player) return;

    const Vector3& playerPos = player->GetRigidbody().GetPos();
    const Vector3& myPos = flyNormal->GetRigidbody().GetPos();

    // プレイヤーとの距離を計算
    float distance = (playerPos - myPos).Length();

    // 状態遷移のチェック
    if (flyNormal->GetStatusComp().m_hp <= 0)
    {
        m_machine.lock()->ChangeState(std::make_shared<FlyNormalStateDead>());
        return;
    }
    else if (distance < kAttackRange)
    {
        m_machine.lock()->ChangeState(std::make_shared<FlyNormalStateAttack>());
    }
    else if (distance < kFindRange)
    {
        m_machine.lock()->ChangeState(std::make_shared<FlyNormalStateMove>());
    }
    else if (distance > kFindRange)
    {
        m_machine.lock()->ChangeState(std::make_shared<FlyNormalStateIdle>());
    }

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

void FlyNormalStateMove::OnLeave()
{
	printf("飛行エネミー：追跡状態リーブ\n");
}
