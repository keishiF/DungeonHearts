#include "Thunder.h"

#include "MyLib/Physics/Collider/CapsuleColliderData.h"
#include <EffekseerForDXLib.h>

namespace
{
    // 当たり判定
    constexpr float kColRadius = 15.0f;
    constexpr float kColHeight = 150.0f;

    constexpr int kAtkPower = 3;
    constexpr float kLifeTime = 120.0f;
}

Thunder::Thunder() :
    m_thunderEffect(-1),
    m_playingEffect(-1),
    Magic(ObjectTag::PlayerMagic, ColliderData::Kind::Capsule)
{
    m_thunderEffect = LoadEffekseerEffect("Data/Effect/Thunder.efkefc", 25.0f);
    assert(m_thunderEffect >= 0);
}

Thunder::~Thunder()
{
	if (m_thunderEffect != -1)
	{
		DeleteEffekseerEffect(m_thunderEffect);
	}
}

void Thunder::Init(const Vector3& pos, const Vector3& targetPos)
{
    Collidable::Init();
    m_rigidbody.Init();
    m_rigidbody.SetPos(pos);

    auto colData = std::static_pointer_cast<CapsuleColliderData>(m_colliderData);
    Vector3 colPos = m_rigidbody.GetPos();
    colPos.y += kColHeight;
    colData->SetStartPos(colPos);
    colData->SetRadius(kColRadius);

    m_atkPower = kAtkPower;
    m_lifeTime = kLifeTime;
}

void Thunder::Update()
{
    if (m_playingEffect == -1)
    {
        m_playingEffect = PlayEffekseer3DEffect(m_thunderEffect);
    }
    SetPosPlayingEffekseer3DEffect(m_playingEffect,
        m_rigidbody.GetPos().x, m_rigidbody.GetPos().y, m_rigidbody.GetPos().z);

    m_timer += 1.0f / 60.0f;
    if (m_timer >= m_lifeTime)
    {
        m_isDead = true;
    }
}

void Thunder::Draw()
{
}

void Thunder::OnCollide(std::shared_ptr<Collidable> collider)
{
    m_isDead = true;
}
