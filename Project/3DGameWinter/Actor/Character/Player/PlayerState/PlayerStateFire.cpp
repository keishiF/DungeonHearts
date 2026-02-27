#include "PlayerStateFire.h"

#include "Actor/Character/Enemy/EnemyBase.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Attack/Fire.h"
#include "Actor/Attack/PlayerWeapon.h"
#include "Camera/Camera.h"
#include "MyLib/Input.h"
#include "PlayerStateGroundAttack1.h"
#include "PlayerStateIdle.h"
#include "PlayerStateRun.h"
#include "PlayerStateThunder.h"

namespace
{
	// アニメーションの再生速度
	constexpr float kFireAnimSpeed = 1.5f;
	const char* kFireAnimName = "Player|Fire";

	constexpr float kMoveSpeed = 9.0f;

	constexpr int kTiming = 24;

	constexpr int kFireMaxTargets = 1;
	constexpr float kFireRange = 500.0f;
	constexpr int kFireDamage = 20;

	constexpr int kAtk = 1;
	constexpr float kDurationFrame = 180.0f;

	// 回転補間速度
	constexpr float kLerpT = 0.2f;
}

PlayerStateFire::PlayerStateFire()
{
}

PlayerStateFire::~PlayerStateFire()
{
	if (m_fireSE != -1)
	{
		DeleteSoundMem(m_fireSE);
	}
}

void PlayerStateFire::OnEntry()
{
	auto player = m_parent.lock();
	if (!player) return;
	printf("プレイヤー：炎攻撃状態エントリー\n");
	m_frame = 0;
	m_isAttacked = false;
	m_fireSE = LoadSoundMem("Data/Sound/SE/FireSE.mp3");
	assert(m_fireSE >= 0);
	player->GetAnimator().ChangeAnim(kFireAnimName, kFireAnimSpeed, false);
}

void PlayerStateFire::OnUpdate(std::shared_ptr<Camera> camera)
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

	//auto& input = Input::GetInstance();
	//if (input.IsPress("Up"))
	//{
	//	moveDir += camForward;
	//}
	//else if (input.IsPress("Down"))
	//{
	//	moveDir -= camForward;
	//}

	//if (input.IsPress("Right"))
	//{
	//	moveDir += camRight;
	//}
	//else if (input.IsPress("Left"))
	//{
	//	moveDir -= camRight;
	//}

	//// 移動入力がある場合
	//if (moveDir.SqrLength() > 0.0f)
	//{
	//	moveDir.Normalize();
	//	player->GetRigidbody().SetVelo(moveDir * kMoveSpeed);
	//}

	// ロックオン中の敵がいる場合、その方向を向く
	if (player->IsLockOn() && player->GetLockedOnEnemy())
	{
		Vector3 enemyPos = player->GetLockedOnEnemy()->GetRigidbody().GetPos();
		Vector3 playerPos = player->GetRigidbody().GetPos();
		Vector3 dir = enemyPos - playerPos;
		dir.y = 0.0f; // Y軸は無視して水平方向のみ
		dir.Normalize();

		float angle = std::atan2(dir.x, dir.z);
		Quaternion targetRot;
		Vector3 yVec = { 0.0f, 1.0f, 0.0f };
		targetRot.AngleAxis(angle, yVec);

		Quaternion currentRot = player->GetRotation();

		// 向きの補間
		Quaternion newRot = Quaternion::Slerp(currentRot, targetRot, kLerpT);
		player->SetRotQ(newRot);
		MV1SetRotationXYZ(player->GetModel(), VGet(0.0f, newRot.ToEulerY(), 0.0f));
	}

	// 攻撃処理を一度だけ実行
	if (!m_isAttacked)
	{
		if (++m_frame >= kTiming)
		{
			m_isAttacked = true; // 発射処理を一度しか行わないようにする
			
			Vector3 targetPos;
			bool targetFound = false;
			
			// プレイヤーがロックオン状態か確認
			if (player->IsLockOn())
			{
				// ロックオン中の敵を取得
				if (auto lockedOnEnemy = player->GetLockedOnEnemy())
				{
					targetPos = lockedOnEnemy->GetRigidbody().GetPos();
					targetFound = true;

					// ロックオン対象がボスの場合は少し上を狙う
					if (lockedOnEnemy->IsBoss())
					{
						targetPos.y += 300.0f;
					}
					else
					{
						targetPos.y += 0.0f;
					}
				}
			}
			
			// ロックオンしていない場合、プレイヤーの正面を目標に設定
			if (!targetFound)
			{
				// 移動入力があればその方向、なければカメラの正面をプレイヤーの「正面」とする
				Vector3 playerForward = moveDir.SqrLength() > 0.0f ? moveDir : camForward;
				targetPos = player->GetPos() + playerForward * kFireRange;
			}
			
			// Fireを生成
			PlaySoundMem(m_fireSE, DX_PLAYTYPE_BACK);
			auto fire = std::make_shared<Fire>(ObjectTag::PlayerWeapon);
			Vector3 playerPos = player->GetPos();
			
			// 魔法の発生位置と目標の高さを少し上に調整（地面から浮かす）
			playerPos.y += 100.0f;
			targetPos.y += 100.0f;
			
			fire->Init(playerPos, targetPos, kDurationFrame, kAtk);
			actorController->SpawnAttack(fire);
		}
	}

	const float currentFrame = m_parent.lock()->GetAnimator().GetCurrentFrame();
	const float totalTime = m_parent.lock()->GetAnimator().GetAnimTransitionFrame();
	if (currentFrame >= totalTime)
	{
		m_machine.lock()->ChangeState(std::make_shared<PlayerStateIdle>());
		return;
	}
}

void PlayerStateFire::OnLeave()
{
	printf("プレイヤー：炎攻撃状態リーブ\n");
}
