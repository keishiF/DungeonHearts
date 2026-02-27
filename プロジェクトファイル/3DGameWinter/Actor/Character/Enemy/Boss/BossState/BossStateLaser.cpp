#include "BossStateLaser.h"

#include "Actor/ActorController.h"
#include "Actor/Attack/Laser.h"
#include "Actor/Character/Enemy/Boss/Boss.h"
#include "Actor/Character/Player/Player.h"
#include "BossStateIdle.h"
#include "MyLib/Quaternion.h"
#include <EffekseerForDXLib.h>

namespace
{
	constexpr float kAnimSpeed = 1.0f;
	const char* kAnimName = "Attack6_Shoot";

	constexpr int kAtk = 1;

	// チャージに要するフレーム数（60FPS想定）
	constexpr int kChargeFrame = 140;
	// 発射フレーム（チャージ完了直後に発射する）
	constexpr int kFireFrame = 141;
	// Laser::Init の durationTime に渡す値（秒）
	constexpr float kDurationTime = 3.0f;
	// レーザーエフェクトを取り付けるモデルのフレームインデックス（既存コードの値を維持）
	constexpr int kLaserAttachFrameIndex = 86;
	constexpr float kTargetLockTime = 80.0f;
}

BossStateLaser::~BossStateLaser()
{
	if (m_chargeEffect != -1)
	{
		DeleteEffekseerEffect(m_chargeEffect);
	}
}

void BossStateLaser::OnEntry()
{
	auto boss = m_parent.lock();
	if (!boss) return;
	
	printf("ボス：レーザー状態エントリー\n");
	m_frame = 0;
	m_lockedTargetPos = Vector3();
	if (m_chargeEffect == -1)
	{
		m_chargeEffect = LoadEffekseerEffect("Data/Effect/LaserCharge.efkefc", 100.0f);
		assert(m_chargeEffect >= 0);
	}

	m_lockedTargetPos = Vector3();
	
	boss->GetAnimator().ChangeAnim(kAnimName, kAnimSpeed, true);
}

void BossStateLaser::OnUpdate(std::shared_ptr<Camera> camera)
{
	auto boss = m_parent.lock();
	if (!boss) return;
	auto ac = boss->GetActorController().lock();
	if (!ac) return;
	auto player = ac->GetPlayer();
	if (!player) return;
	// 毎フレーム進める（60FPS 想定）
	++m_frame;

	// チャージ中はチャージエフェクトを再生・位置更新する
	if (m_frame <= kChargeFrame)
	{
		if (m_playingEffect == -1)
		{
			m_playingEffect = PlayEffekseer3DEffect(m_chargeEffect);
		}

		// エフェクトの位置をボスの該当フレーム位置に追従させる
		VECTOR framePos = MV1GetFramePosition(boss->GetModel(), kLaserAttachFrameIndex);
		SetPosPlayingEffekseer3DEffect(m_playingEffect, framePos.x, framePos.y, framePos.z);

		// エフェクトの向きをプレイヤーの方向に向ける（チャージ見た目）
		Vector3 bossPos = boss->GetRigidbody().GetPos();
		Vector3 playerPos = player->GetRigidbody().GetPos();
		Vector3 dir = playerPos - bossPos;
		dir.Normalize();
		/*float angleX = asinf(-dir.y);
		float angleY = atan2f(dir.x, dir.z);
		SetRotationPlayingEffekseer3DEffect(m_playingEffect, angleX, -angleY, 0.0f);*/

		// クォータニオンからオイラー角を計算する方式
		Vector3 defaultForward(0.0f, 0.0f, 1.0f);
		Quaternion rot;
		float dot = defaultForward.Dot(dir);
		
		if (fabs(dot) > 0.9999f)
		{
			if (dot < 0.0f)
			{
				float angle = DX_PI_F;
				Vector3 axis = { 0.0f, 1.0f, 0.0f };
				rot.AngleAxis(angle, axis);
			}
		}
		else
		{
			float rotAngle = acosf(dot);
			Vector3 rotAxis = defaultForward.Cross(dir).GetNormalize();
			rot.AngleAxis(rotAngle, rotAxis);
		}
		float angleX, angleY, angleZ;
		float sinX = -2.0f * (rot.y * rot.z - rot.w * rot.x);
		
		angleX = asinf(sinX);
		angleY = atan2f(2.0f * (rot.x * rot.z + rot.w * rot.y), 
			1.0f - 2.0f * (rot.x * rot.x + rot.y * rot.y));
		angleZ = atan2f(2.0f * (rot.x * rot.y + rot.w * rot.z), 
			1.0f - 2.0f * (rot.x * rot.x + rot.z * rot.z));
		
		SetRotationPlayingEffekseer3DEffect(m_playingEffect, angleX, angleY, angleZ);

		return;
	}

	// 80フレーム目にターゲット位置を記録
	if (m_frame == 80)
	{
		m_lockedTargetPos = player->GetRigidbody().GetPos();
	}

	// 発射タイミング
	if (m_frame == kFireFrame)
	{
		// 発射位置をモデルフレームから取得
		VECTOR framePos = MV1GetFramePosition(boss->GetModel(), kLaserAttachFrameIndex);
		Vector3 spawnPos = { framePos.x, framePos.y, framePos.z };

		// 目標はプレイヤーの当たり判定位置
		Vector3 targetPos = m_lockedTargetPos;

		// レーザー生成と初期化
		auto laser = std::make_shared<Laser>(ObjectTag::EnemyWeapon);
		laser->Init(spawnPos, targetPos, static_cast<int>(kDurationTime), kAtk);
		ac->SpawnAttack(laser);

		// チャージエフェクトを止める（存在するなら）
		if (m_playingEffect != -1)
		{
			StopEffekseer3DEffect(m_playingEffect);
			m_playingEffect = -1;
		}

		m_machine.lock()->ChangeState(std::make_shared<BossStateIdle>());
	}

}

void BossStateLaser::OnLeave()
{
	auto boss = m_parent.lock();
	if (!boss) return;
	
	// 再生中のチャージエフェクトがあれば停止
	if (m_playingEffect != -1)
	{
		StopEffekseer3DEffect(m_playingEffect);
		m_playingEffect = -1;
	}

	// ボスの速度をゼロにしておく
	boss->GetRigidbody().SetVelo(Vector3());
}
