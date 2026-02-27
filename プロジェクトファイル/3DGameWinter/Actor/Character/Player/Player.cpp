#include "Player.h"

#include "Actor/ActorController.h"
#include "Actor/Character/Enemy/EnemyBase.h"
#include "Actor/Attack/PlayerWeapon.h"
#include "Camera/Camera.h"
#include "MyLib/Input.h"
#include "PlayerState/PlayerStateDead.h"
#include "PlayerState/PlayerStateHit.h"
#include "PlayerState/PlayerStateIdle.h"

namespace
{
	// ステータス
	constexpr float kHp = 25.0f;

	// 当たり判定の大きさ
	// カプセルの半径
	constexpr float kColRadius = 35.0f;
	// カプセルの長さ
	constexpr float kColSize = 100.0f;

	// アニメーションの再生速度
	constexpr float kIdleAnimSpeed = 1.0f;
	// 待機アニメーション
	const char* kIdleAnimName = "Player|Idle";

	constexpr float kLerpT = 0.2f;

	constexpr int kLockOnTargetNum = 1;
	constexpr float kLockOnRange = 1000.0f;
}

Player::Player() :
	m_isLockOn(false),
	Actor(ObjectTag::Player, ColliderData::Kind::Capsule)
{
}

Player::~Player()
{
}

void Player::Init(Vector3& pos, Vector3& rot, Vector3& scale)
{
	m_status.m_hp = kHp;
	m_status.m_maxHp = kHp;

	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);
	auto colData = std::static_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->SetStartPos(m_rigidbody.GetPos());
	colData->SetRadius(kColRadius);

	m_model = MV1LoadModel("Data/Model/Player/Player.mv1");
	assert(m_model >= 0);
	MV1SetPosition(m_model, m_rigidbody.GetPos().ToDxVECTOR());

	m_rotQ = Quaternion();

	m_weapon = std::make_shared<PlayerWeapon>();
	m_weapon->Init(m_model);

	m_animator.Init(m_model);
	m_animator.AttachAnim(m_animator.GetPrevAnim(), kIdleAnimName, kIdleAnimSpeed, true);

	m_state = std::make_shared<StateMachine<Player>>();
	m_state->Init(std::static_pointer_cast<Player>(shared_from_this()), std::make_shared<PlayerStateIdle>());
}

void Player::Update(std::shared_ptr<Camera> camera)
{
	auto& input = Input::GetInstance();

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

	m_state->Update(camera);

	// ロックオン中の敵が死んだ、または無効になった場合の処理
	if (m_isLockOn)
	{
		// 現在ロックオンしている敵が死んでいるか、または無効になっているかチェック
		if (!m_lockOnEnemy || m_lockOnEnemy->IsDead())
		{
			// ロックオン対象が死んだ、または無効になった場合、新しい敵を探す
			if (auto ac = m_actorController.lock())
			{
				auto newNearestEnemy = ac->FindNearestEnemy(GetPos(), kLockOnRange);
				if (newNearestEnemy)
				{
					LockOn(newNearestEnemy); // 新しい敵にロックオン
				}
				else
				{
					LockOff(); // 近くに敵がいないのでロックオン解除
				}
			}
		}
	}

	if (input.IsTrigger("LockOn"))
	{
		if (!m_isLockOn)
		{
			// ActorController を通じて最も近い敵を探す
			if (auto ac = m_actorController.lock())
			{
				auto nearestEnemy = ac->FindNearestEnemy(GetPos(), kLockOnRange);
				if (nearestEnemy)
				{
					LockOn(nearestEnemy);
				}
			}
		}
		else
		{
			// ロックオンを解除
			LockOff();
		}
	}

	if (CheckHitKey(KEY_INPUT_9))
	{
		m_rigidbody.SetPos(Vector3(-8100.0f, 10.0f, 18100.0f));
	}
}

void Player::Draw()
{
    // 進行方向にモデルを回転させる
    Vector3 velocity = m_rigidbody.GetVelo();
    if (velocity.x != 0.0f || velocity.z != 0.0f)
    {
        Vector3 axis(0.0f, 1.0f, 0.0f);
        float angle = std::atan2(velocity.x, velocity.z);

        Quaternion targetRot;
        targetRot.AngleAxis(angle, axis);

        m_rotQ = Quaternion::Slerp(m_rotQ, targetRot, kLerpT);

        float angleY = m_rotQ.ToEulerY();
        MV1SetRotationXYZ(m_model, VGet(0.0f, angleY, 0.0f));
    }

    // モデルに移動を適用
    MV1SetPosition(m_model, m_rigidbody.GetPos().ToDxVECTOR());
	MV1DrawModel(m_model);

    m_weapon->Draw();
}

void Player::OnDamage(int atk)
{
	if (m_blinkFrame > 0 || m_isDead || m_status.m_hp <= 0) return;

	m_status.m_hp -= 1;
	StartBlinking();

	if (m_status.m_hp <= 0)
	{
		m_state->ChangeState(std::make_shared<PlayerStateDead>());
	}
	else
	{
		m_state->ChangeState(std::make_shared<PlayerStateHit>());
	}
}

