#include "Laser.h"

#include "MyLib/Physics/Collider/CapsuleColliderData.h"
#include "MyLib/Quaternion.h"
#include <cassert>
#include <EffekseerForDXLib.h>

namespace
{
	constexpr float kSpeed = 15.0f;
	constexpr float kColRadius = 300.0f;
	constexpr float kLaserLength = 5000.0f;
	constexpr int kAtkPower = 25;
}

Laser::Laser(ObjectTag tag) :
	Attack(tag, ColliderData::Kind::Capsule),
	m_laserEffect(-1),
	m_playingEffect(-1)
{
}

Laser::~Laser()
{
	// 再生中のエフェクトがあれば停止してからエフェクトを解放
	if (m_playingEffect != -1)
	{
		StopEffekseer3DEffect(m_playingEffect);
		m_playingEffect = -1;
	}
	if (m_laserEffect >= 0)
	{
		DeleteEffekseerEffect(m_laserEffect);
	}
}

void Laser::Init(const Vector3& pos, const Vector3& targetPos, int durationTime, int atk)
{
	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);
	
	m_dir = targetPos - pos;
	m_dir.Normalize();

	m_atk = atk;

	m_durationTime = durationTime;

	auto colData = std::static_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->SetRadius(kColRadius);
	Vector3 endPos = m_rigidbody.GetPos() + m_dir * kLaserLength;
	colData->SetStartPos(endPos);

	m_laserEffect = LoadEffekseerEffect("Data/Effect/Laser.efkefc", 50.0f);
	assert(m_laserEffect >= 0);

	// 発射時点でエフェクトを即座に再生し、向きをプレイヤー方向に合わせる
	if (m_playingEffect == -1)
	{
		m_playingEffect = PlayEffekseer3DEffect(m_laserEffect);
		SetPosPlayingEffekseer3DEffect(m_playingEffect,
			m_rigidbody.GetPos().x,
			m_rigidbody.GetPos().y,
			m_rigidbody.GetPos().z);

		//// レーザーの向きを進行方向に合わせる
		//float angleX = asinf(-m_dir.y);
		//float angleY = atan2f(m_dir.x, m_dir.z);
		//SetRotationPlayingEffekseer3DEffect(m_playingEffect, angleX, -angleY, 0.0f);

		// レーザーの向きを進行方向に合わせる（クォータニオンからオイラー角を計算する方式）
		Vector3 defaultForward(0.0f, 0.0f, 1.0f);
		Vector3 targetDir = m_dir; // m_dirはInitで正規化済み
		
		Quaternion rot;
		float dot = defaultForward.Dot(targetDir);
		
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
			Vector3 rotAxis = defaultForward.Cross(targetDir).GetNormalize();
			rot.AngleAxis(rotAngle, rotAxis);
		}
		
		// クォータニオンからオイラー角を抽出 (YXZ回転順序を想定)
		float angleX, angleY, angleZ;
		float sinX = -2.0f * (rot.y * rot.z - rot.w * rot.x);
		
		// ジンバルロック対策
		if (fabs(sinX) > 0.9999f) 
		{
			angleX = DX_PI_F / 2.0f * sinX;
			angleY = atan2f(-2.0f * (rot.x * rot.z - rot.w * rot.y), 
				1.0f - 2.0f * (rot.y * rot.y + rot.z * rot.z));
			angleZ = 0.0f;	
		}
		else 
		{
			angleX = asinf(sinX);
			angleY = atan2f(2.0f * (rot.x * rot.z + rot.w * rot.y), 
				1.0f - 2.0f * (rot.x * rot.x + rot.y * rot.y));
			angleZ = atan2f(2.0f * (rot.x * rot.y + rot.w * rot.z), 
				1.0f - 2.0f * (rot.x * rot.x + rot.z * rot.z));
		}
		SetRotationPlayingEffekseer3DEffect(m_playingEffect, angleX, angleY, angleZ);
	}
}

void Laser::Update()
{
	if (m_isDead) return;

	m_timer += 1.0f / 60.0f;

	// エフェクトの再生
	if (m_playingEffect == -1)
	{
		m_playingEffect = PlayEffekseer3DEffect(m_laserEffect);
		SetPosPlayingEffekseer3DEffect(m_playingEffect,
			m_rigidbody.GetPos().x,
			m_rigidbody.GetPos().y,
			m_rigidbody.GetPos().z);
		
		// レーザーの向きを進行方向に合わせる
		float angleX = asinf(-m_dir.y);
		float angleY = atan2f(m_dir.x, m_dir.z);
		SetRotationPlayingEffekseer3DEffect(m_playingEffect, angleX, angleY, 0.0f);
	}

	if (m_timer >= m_durationTime)
	{
		StopEffekseer3DEffect(m_playingEffect);
		m_playingEffect = -1;
		m_isDead = true;
	}
}

void Laser::Draw()
{
}

void Laser::OnCollide(std::shared_ptr<Collidable> collider)
{
	if (m_isDead) return;
	
	// 自分自身や他の敵の武器とは衝突しない
	if (collider->GetTag() == GetTag() || 
		collider->GetTag() == ObjectTag::EnemyWeapon) return;
}
