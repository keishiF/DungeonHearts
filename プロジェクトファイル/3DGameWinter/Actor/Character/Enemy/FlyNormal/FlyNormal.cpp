#include "FlyNormal.h"

#include "State/FlyNormalStateIdle.h"
#include "State/FlyNormalStateHit.h"
#include "State/FlyNormalStateDead.h"

namespace
{
	constexpr float kHp = 2.0f;

	constexpr float kIdleAnimSpeed = 0.5f;

	const char* kIdleAnimName = "CharacterArmature|Flying_Idle";

	constexpr float kColRadius = 100.0f;
	constexpr float kColSize = 200.0f;

	constexpr float kModelScale = 0.5f;
}

FlyNormal::FlyNormal(int model) :
	EnemyBase(ObjectTag::Enemy, ColliderData::Kind::Capsule)
{
	m_score = 1000;
	m_model = MV1DuplicateModel(model);
}

FlyNormal::~FlyNormal()
{
}

void FlyNormal::Init(Vector3& pos, Vector3& rot, Vector3& scale)
{
	m_status.m_hp = kHp;
	m_status.m_maxHp = kHp;

	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);
	auto colData = std::static_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->SetStartPos(m_rigidbody.GetPos());
	colData->SetRadius(kColRadius);
	m_rigidbody.SetGravity(false);

	MV1SetPosition(m_model, m_rigidbody.GetPos().ToDxVECTOR());
	if (rot.y == -90)
	{
		MV1SetRotationXYZ(m_model, VGet(0.0f, -DX_PI_F * 0.5f, 0.0f));
	}
	VECTOR modelScale = { scale.x * kModelScale, scale.y * kModelScale, scale.z * kModelScale };
	MV1SetScale(m_model, modelScale);

	m_animator.Init(m_model);
	m_animator.AttachAnim(m_animator.GetPrevAnim(), kIdleAnimName, kIdleAnimSpeed, true);

	m_state = std::make_shared<StateMachine<FlyNormal>>();
	m_state->Init(std::static_pointer_cast<FlyNormal>(shared_from_this()), std::make_shared<FlyNormalStateIdle>());
}

void FlyNormal::Update()
{
	m_rigidbody.SetVelo(Vector3());

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

void FlyNormal::Draw()
{
	if (!IsVisible()) return;
	MV1SetPosition(m_model, m_rigidbody.GetPos().ToDxVECTOR());
	MV1DrawModel(m_model);
}

void FlyNormal::OnDamage(int atk)
{
	if (m_blinkFrame > 0 || m_isDead || m_status.m_hp <= 0) return;

	m_status.m_hp -= atk;
	StartBlinking();

	if (m_status.m_hp <= 0)
	{
		SetDead(true);
		m_actorController.lock()->AddKillScore(m_score);
		m_state->ChangeState(std::make_shared<FlyNormalStateDead>());
	}
	else
	{
		m_state->ChangeState(std::make_shared<FlyNormalStateHit>());
	}
}
