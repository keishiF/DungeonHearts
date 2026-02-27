#include "MeleeNormalStateAttack.h"

#include "Actor/Character/Enemy/MeleeNormal/MeleeNormal.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/EnemyMeleeWeapon.h"
#include "MeleeNormalStateChase.h"
#include "MeleeNormalStateDead.h"
#include "MeleeNormalStateIdle.h"
#include "MeleeNormalStateWait.h"

namespace
{
	constexpr float kAttackAnimSpeed = 0.3f;

	const char* kAttackAnimName = "CharacterArmature|Punch";

	constexpr float kFindRange = 750.0f;
	constexpr float kAttackRange = 200.0f;

	// 攻撃判定を出すアニメーションのフレーム範囲
	constexpr float kAttackStartFrame = 12.0f;
	constexpr float kAttackEndFrame = 18.0f;
	// 攻撃力
	constexpr int kAttackPower = 10;
}

void MeleeNormalStateAttack::OnEntry()
{
	auto meleeNormal = m_parent.lock();
	if (!meleeNormal) return;
	printf("近接エネミー：攻撃状態エントリー\n");
	meleeNormal->GetRigidbody().SetVelo(Vector3());
	meleeNormal->GetAnimator().ChangeAnim(kAttackAnimName, kAttackAnimSpeed, false);
}

void MeleeNormalStateAttack::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto meleeNormal = m_parent.lock();
	if (!meleeNormal) return;
	auto actorController = meleeNormal->GetActorController().lock();
	if (!actorController) return;

	// プレイヤーの方向を向く
	auto player = actorController->GetPlayer();
	if (player)
	{
		Vector3 playerPos = player->GetRigidbody().GetPos();
		Vector3 myPos = meleeNormal->GetRigidbody().GetPos();
		Vector3 dir = playerPos - myPos;
		dir.y = 0.0f; // y軸は無視
		dir.Normalize();
		
		float angle = std::atan2(dir.x, dir.z);
		Quaternion targetRot;
		Vector3 yVec = { 0.0f, 1.0f, 0.0f };
		targetRot.AngleAxis(angle, yVec);
		Quaternion currentRot = meleeNormal->GetRotation();
		
		// 向きの補間
		Quaternion newRot = Quaternion::Slerp(currentRot, targetRot, 0.1f);
		meleeNormal->SetRotQ(newRot);
		MV1SetRotationXYZ(meleeNormal->GetModel(), VGet(0.0f, newRot.ToEulerY() + DX_PI_F, 0.0f));
	}

	const float currentFrame = meleeNormal->GetAnimator().GetCurrentFrame();
	bool isAttacking = (currentFrame >= kAttackStartFrame && currentFrame <= kAttackEndFrame);
	meleeNormal->GetWeapon()->Update(isAttacking, kAttackPower);

	// アニメーションが終了したら状態遷移
	if (meleeNormal->GetAnimator().GetNextAnim().isEnd)
	{
		m_machine.lock()->ChangeState(std::make_shared<MeleeNormalStateWait>());
		return;
		//auto player = actorController->GetPlayer();
		//if (!player)
		//{
		//	m_machine.lock()->ChangeState(std::make_shared<EnemyMeleeNormalStateIdle>());
		//	return;
		//}
		//
		//const Vector3 & playerPos = player->GetRigidbody().GetPos();
		//const Vector3 & myPos = meleeNormal->GetRigidbody().GetPos();
		//float distance = (playerPos - myPos).Length();
		//
		//if (meleeNormal->GetStatusComp().m_hp <= 0)
		//{
		//	m_machine.lock()->ChangeState(std::make_shared<EnemyMeleeNormalStateDead>());
		//}
		//else if (distance < kAttackRange)
		//{
		//	// 攻撃範囲にまだいるなら、再度攻撃
		//	m_machine.lock()->ChangeState(std::make_shared<EnemyMeleeNormalStateAttack>());
		//}
		//else if (distance < kFindRange)
		//{
		//	m_machine.lock()->ChangeState(std::make_shared<EnemyMeleeNormalStateChase>());
		//}
		//else
		//{
		//	m_machine.lock()->ChangeState(std::make_shared<EnemyMeleeNormalStateIdle>());
		//}
	}
}

void MeleeNormalStateAttack::OnLeave()
{
	printf("近接エネミー：攻撃状態リーブ\n");
	auto meleeNormal = m_parent.lock();
	if (!meleeNormal) return;
	meleeNormal->GetWeapon()->Update(false, 0);
}
