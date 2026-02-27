#include "PlayerStateThunder.h"

#include "Actor/ActorController.h"
#include "Actor/Character/Enemy/EnemyBase.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/PlayerWeapon.h"
#include "Actor/Attack/Thunder.h"
#include "Camera/Camera.h"
#include "MyLib/Input.h"
#include "PlayerStateAirAttack1.h"
#include "PlayerStateFire.h"
#include "PlayerStateGroundAttack1.h"
#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include <algorithm>

namespace
{
	// アニメーションの再生速度
	constexpr float kThunderAnimSpeed = 1.5f;

	constexpr float kMoveSpeed = 9.0f;

	constexpr float kTiming = 32.0f;

	// アニメーション名
	const char* kThunderAnimName = "Player|GroundAttack1";

	constexpr int kThunderMaxTargets = 3;
	constexpr float kThunderRange = 1000.0f;
	constexpr int kThunderDamage = 20;
}

void PlayerStateThunder::OnEntry()
{
	auto player = m_parent.lock();
	if (!player) return;
	printf("プレイヤー：雷攻撃状態エントリー\n");
	m_frame = 0.0f;
	m_isAttacked = false;
	player->GetAnimator().ChangeAnim(kThunderAnimName, kThunderAnimSpeed, false);
}

void PlayerStateThunder::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto player = m_parent.lock();
	if (!player) return;
	auto actorController = player->GetActorController().lock();
	if (!actorController) return;

	player->GetWeapon()->Update(false, 0, 0);

	// カメラの向きを基準に移動方向を決定
	const Vector3& camForward = camera->GetForward();
	const Vector3& camRight = camera->GetRight();

	Vector3 moveDir = { 0.0f, 0.0f, 0.0f };

	auto& input = Input::GetInstance();
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
		player->GetRigidbody().SetVelo(moveDir * kMoveSpeed);
	}
	else
	{
		player->GetRigidbody().SetVelo(Vector3());
	}

	//// 攻撃処理を一度だけ実行
	//if (!m_isAttacked)
	//{
	//	if (++m_frame >= kTiming)
	//	{
	//		auto targetPositions = actorController->RangeFindEnemiesPosition(player->GetPos(), kThunderRange, kThunderMaxTargets);
	//		for (const auto& targetPos : targetPositions)
	//		{
	//			auto thunder = std::make_shared<Thunder>();
	//			thunder->Init(player->GetPos(), targetPos);
	//			actorController->SpawnMagic(thunder);
	//		}
	//		m_isAttacked = true;
	//	}
	//}

	const float currentFrame = m_parent.lock()->GetAnimator().GetCurrentFrame();
	const float totalTime = m_parent.lock()->GetAnimator().GetAnimTransitionFrame();
	if (currentFrame >= totalTime)
	{
		m_machine.lock()->ChangeState(std::make_shared<PlayerStateIdle>());
		return;
	}
}

void PlayerStateThunder::OnLeave()
{
	printf("プレイヤー：雷攻撃状態リーブ\n");
}
