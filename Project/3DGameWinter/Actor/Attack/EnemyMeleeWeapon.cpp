#include "EnemyMeleeWeapon.h"

#include "MyLib/Physics/Collider/SphereColliderData.h"

namespace
{
	// 当たり判定の半径
	constexpr float kColRadius = 90.0f;
	// 攻撃判定を追従させるモデルのフレーム
	constexpr int kParentModelHandFrame = 36;
}

EnemyMeleeWeapon::EnemyMeleeWeapon() :
	m_parentModel(-1),
	m_currentAttackPower(0),
	Collidable(ObjectTag::EnemyWeapon, ColliderData::Kind::Sphere)
{
}

EnemyMeleeWeapon::~EnemyMeleeWeapon()
{
}

void EnemyMeleeWeapon::Init(int parentModel)
{
	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetGravity(false); // 武器自体は重力の影響を受けない
	
	auto colData = std::static_pointer_cast<SphereColliderData>(m_colliderData);
	colData->SetRadius(kColRadius);
	
	m_parentModel = parentModel;
	// 最初は当たり判定を無効にしておく
	SetActive(false);
}

void EnemyMeleeWeapon::Update(bool isAttacking, int attackPower)
{
	SetActive(isAttacking);
	m_currentAttackPower = isAttacking ? attackPower : 0;
	
	if (isAttacking)
	{
		// 親モデルの特定フレームの位置を取得して、そこに追従させる
		VECTOR handPos = MV1GetFramePosition(m_parentModel, kParentModelHandFrame);
		m_rigidbody.SetPos({ handPos.x, handPos.y, handPos.z });
	}
}

void EnemyMeleeWeapon::Draw()
{
	// 攻撃中のみ描画
	if (IsActive())
	{
		// 半透明の赤い球を描画
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
		DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), kColRadius, 32, GetColor(255,
			0, 0), GetColor(255, 100, 100), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void EnemyMeleeWeapon::OnCollide(std::shared_ptr<Collidable> collider)
{
}

int EnemyMeleeWeapon::GetAttackPower() const
{
	return m_currentAttackPower;
}
