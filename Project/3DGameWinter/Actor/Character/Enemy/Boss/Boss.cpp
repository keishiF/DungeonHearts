#include "Boss.h"

#include "BossState/BossStateDeath.h"
#include "BossState/BossStateDormant.h"
#include "BossState/BossStateLaser.h"
#include "BossState/BossStateHit.h"

namespace
{
	constexpr float kHp = 1.0f;

	constexpr float kColRadius = 350.0f;
	constexpr float kColSize = 500.0f;

	constexpr float kModelScale = 5.0f;

	constexpr float kAnimSpeed = 1.0f;

	const char* kAnimName = "Idle1_Toilet";
}

Boss::Boss(int model) :
	m_cooldownTimer(0.0f),
	EnemyBase(ObjectTag::Boss, ColliderData::Kind::Capsule),
	m_isDrawingLaserWarning(false),
	m_hasFiredHalfHpLaser(false)
{
	m_score = 7500;
	m_model = MV1DuplicateModel(model);
}

Boss::~Boss()
{
}

void Boss::Init(Vector3& pos, Vector3& rot, Vector3& scale)
{
	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);
	auto colData = std::static_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->SetStartPos(m_rigidbody.GetPos());
	colData->SetRadius(kColRadius);

	MV1SetPosition(m_model, m_rigidbody.GetPos().ToDxVECTOR());
	VECTOR modelScale = VGet(scale.x * kModelScale, scale.y * kModelScale, scale.z * kModelScale);
	MV1SetScale(m_model, modelScale);
	rot.y = DX_PI_F;
	MV1SetRotationXYZ(m_model, rot.ToDxVECTOR());

	m_animator.Init(m_model);
	m_animator.AttachAnim(m_animator.GetPrevAnim(), kAnimName, kAnimSpeed, true);

	m_state = std::make_shared<StateMachine<Boss>>();
	m_state->Init(std::static_pointer_cast<Boss>(shared_from_this()), std::make_shared<BossStateDormant>());
}

void Boss::Update()
{
	m_rigidbody.SetVelo(Vector3());

	// クールダウンタイマーの更新
	UpdateCooldown();

	UpdateBlinking();
	// アニメーションの更新
	m_animator.UpdateAnim(m_animator.GetPrevAnim());
	m_animator.UpdateAnim(m_animator.GetNextAnim());
	m_animator.UpdateAnimBlend();

	//当たり判定
	auto colData = std::static_pointer_cast<CapsuleColliderData>(m_colliderData);
	Vector3 colPos = m_rigidbody.GetPos();
	colPos.y += kColSize;
	colData->SetStartPos(colPos);

	m_state->Update(nullptr);
}

void Boss::Draw()
{
	// モデルに移動を適用
	MV1SetPosition(m_model, m_rigidbody.GetPos().ToDxVECTOR());
	MV1DrawModel(m_model);

	if (m_isDrawingLaserWarning)
	{
		// 予兆のレーザー（半透明の赤いシリンダー）を描画
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawCapsule3D(
			m_laserWarningStart.ToDxVECTOR(),
			m_laserWarningEnd.ToDxVECTOR(),
			10.0f, 32, GetColor(255, 0, 0), GetColor(255, 100, 100), TRUE
			);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	VECTOR modelPos = MV1GetPosition(m_model);
	Vector3 pos = { modelPos.x, modelPos.y, modelPos.z };
}

void Boss::OnDamage(int atk)
{
	if (m_blinkFrame > 0 || m_isDead || m_status.m_hp <= 0) return;

	m_status.m_hp -= atk;
	StartBlinking();

	//// HPが50%以下になり、まだレーザーを撃っていない場合
	//if (m_status.m_hp / m_status.m_maxHp <= 0.5f && !m_hasFiredHalfHpLaser)
	//{
	//	m_hasFiredHalfHpLaser = true;
	//	m_state->ChangeState(std::make_shared<BossStateLaser>());
	//	return; // 他の状態遷移は行わない
	//}

	if (m_status.m_hp <= 0)
	{
		m_actorController.lock()->AddKillScore(m_score);
		m_state->ChangeState(std::make_shared<BossStateDeath>());
	}
	else
	{
		m_state->ChangeState(std::make_shared<BossStateHit>());
	}
}

void Boss::UpdateCooldown()
{
	if (m_cooldownTimer > 0.0f)
	{
		m_cooldownTimer -= 1.0f / 60.0f;
	}
}

void Boss::SetLaserWarning(bool isDrawing, const Vector3& start, const Vector3& end)
{
	m_isDrawingLaserWarning = isDrawing;
	m_laserWarningStart = start;
	m_laserWarningEnd = end;
}
