#include "FlyNormalStateAttack.h"

#include "Actor/Character/Enemy/FlyNormal/FlyNormal.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/Fire.h"
#include "FlyNormalStateIdle.h"
#include "FlyNormalStateMove.h"
#include "FlyNormalStateWait.h"

namespace
{
	constexpr float kAttackAnimSpeed = 0.3f;

	const char* kAttackAnimName = "CharacterArmature|Punch";

	// 攻撃（弾発射）のタイミング
	constexpr int kAttackTimingFrame = 30;
	
	// 攻撃をやめて移動を開始する距離
	constexpr float kMoveRange = 750.0f;

    constexpr int kAtk = 1;
    constexpr float kDurationTime = 180.0f;
}

void FlyNormalStateAttack::OnEntry()
{
	auto flyNormal = m_parent.lock();
	if (!flyNormal) return;
	printf("飛行エネミー：攻撃状態エントリー\n");
	m_frame = 0;
	m_isAttacked = false;
    m_fireSE = LoadSoundMem("Data/Sound/SE/FireSE.mp3");
    assert(m_fireSE >= 0);
	flyNormal->GetRigidbody().SetVelo(Vector3());
	flyNormal->GetAnimator().ChangeAnim(kAttackAnimName, kAttackAnimSpeed, false);
}

void FlyNormalStateAttack::OnUpdate(std::shared_ptr<Camera> camera)
{
    auto flyNormal = m_parent.lock();
    if (!flyNormal) return;

    auto actorController = flyNormal->GetActorController().lock();
    if (!actorController) return;

    auto player = actorController->GetPlayer();
    if (!player)
    {
        m_machine.lock()->ChangeState(std::make_shared<FlyNormalStateIdle>());
        return;
    }

    m_frame++;

    // プレイヤーの方向を向く
    const Vector3& playerPos = player->GetRigidbody().GetPos();
    const Vector3& myPos = flyNormal->GetRigidbody().GetPos();
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
        Quaternion newRot = Quaternion::Slerp(currentRot, targetRot, 0.1f); // 向きの補間
        flyNormal->SetRotQ(newRot);

        MV1SetRotationXYZ(flyNormal->GetModel(),
            VGet(0.0f, newRot.ToEulerY() + DX_PI_F, 0.0f));
    }

    // 攻撃タイミングで魔法を生成
    if (!m_isAttacked && m_frame >= kAttackTimingFrame)
    {
        PlaySoundMem(m_fireSE, DX_PLAYTYPE_BACK);
        auto fire = std::make_shared<Fire>(ObjectTag::EnemyWeapon);

        VECTOR myPosVECTOR = MV1GetFramePosition(flyNormal->GetModel(), 4);
        Vector3 magicStartPos = { myPosVECTOR.x, myPosVECTOR.y, myPosVECTOR.z };

        Vector3 targetPos = playerPos;
        targetPos.y += 100.0f; // プレイヤーの中心を狙う

        fire->Init(magicStartPos, targetPos, kDurationTime, kAtk);
        actorController->SpawnAttack(fire);
        m_isAttacked = true;
    }

    // アニメーションが終了したら状態遷移
    if (flyNormal->GetAnimator().GetNextAnim().isEnd)
    {
        m_machine.lock()->ChangeState(std::make_shared<FlyNormalStateWait>());
        return;
    }
}

void FlyNormalStateAttack::OnLeave()
{
	printf("飛行エネミー：攻撃状態リーブ\n");
}
