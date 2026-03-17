#include "MeleeNormalStateChase.h"

#include "Actor/Character/Enemy/MeleeNormal/MeleeNormal.h"
#include "Actor/Character/Player/Player.h"
#include "MeleeNormalStateAttack.h"
#include "MeleeNormalStateDead.h"
#include "MeleeNormalStateIdle.h"

namespace
{
	constexpr float kChaseAnimSpeed = 0.5f;

	const char* kChaseAnimName = "CharacterArmature|Walk";

	constexpr float kChaseSpeed = 5.0f;

	constexpr float kFindRange = 750.0f;
	constexpr float kAttackRange = 200.0f;
	constexpr float kLerpT = 0.2f;
}

void MeleeNormalStateChase::OnEntry()
{
	auto meleeNormal = m_parent.lock();
	if (!meleeNormal) return;
	printf("近接エネミー：追跡状態エントリー\n");
	meleeNormal->GetRigidbody().SetVelo(Vector3());
	meleeNormal->GetAnimator().ChangeAnim(kChaseAnimName, kChaseAnimSpeed, true);
}

void MeleeNormalStateChase::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto meleeNormal = m_parent.lock();
	if (!meleeNormal) return;
	auto actorController = meleeNormal->GetActorController().lock();
	if (!actorController) return;
	auto player = actorController->GetPlayer();
	if (!player) return;

	const Vector3 & playerPos = player->GetRigidbody().GetPos();
	const Vector3 & myPos = meleeNormal->GetRigidbody().GetPos();
	
	float distance = (playerPos - myPos).Length();
	
	if (meleeNormal->GetStatusComp().m_hp <= 0)
	{
		m_machine.lock()->ChangeState(std::make_shared<MeleeNormalStateDead>());
	}
	else if (distance > kFindRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<MeleeNormalStateIdle>());
		return;
	}
	else if (distance < kAttackRange)
	{
		m_machine.lock()->ChangeState(std::make_shared<MeleeNormalStateAttack>());
		return;
	}
	
	// プレイヤーの方向を向く
	Vector3 dir = playerPos - myPos;
	dir.y = 0.0f; // 上下方向は無視
	dir.Normalize();
	
	// プレイヤーに向かって移動
	meleeNormal->GetRigidbody().SetVelo(dir * kChaseSpeed);
	
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

void MeleeNormalStateChase::OnLeave()
{
	printf("近接エネミー：追跡状態リーブ\n");
}
