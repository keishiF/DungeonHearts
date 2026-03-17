#include "Fire.h"

#include "MyLib/Physics/Collider/SphereColliderData.h"

namespace
{
    // 速度
    constexpr float kSpeed = 12.5f;
    // 当たり判定
	constexpr float kColRadius = 25.0f;
    constexpr float kHeight = 100.0f;
}

Fire::Fire(ObjectTag tag) :
    m_fireEffect(-1),
    m_playingEffect(-1),
	Attack(tag, ColliderData::Kind::Sphere)
{
}

Fire::~Fire()
{
    DeleteEffekseerEffect(m_fireEffect);
}

void Fire::Init(const Vector3& pos, const Vector3& targetPos, int durationTime, int atk)
{
    if (m_fireEffect == -1)
    {
        m_fireEffect = LoadEffekseerEffect("Data/Effect/Fire.efkefc", 50.0f);
        assert(m_fireEffect >= 0);
    }

    Collidable::Init();
    m_rigidbody.Init();
    m_rigidbody.SetPos(pos);

    m_dir = targetPos - m_rigidbody.GetPos();
    m_dir.Normalize();

    auto colData = std::static_pointer_cast<SphereColliderData>(m_colliderData);
    Vector3 colPos = m_rigidbody.GetPos();
    colPos.y += kHeight;
    colData->SetRadius(kColRadius);

    m_atk = atk;
    m_durationTime = durationTime;
}

void Fire::Update()
{
    if (m_isDead) return;

    m_timer += 1.0f / 60.0f;

    m_rigidbody.SetVelo(m_dir * kSpeed);

    if (m_playingEffect == -1)
    {
        m_playingEffect = PlayEffekseer3DEffect(m_fireEffect);
    }
    SetPosPlayingEffekseer3DEffect(m_playingEffect,
        m_rigidbody.GetPos().x,
        m_rigidbody.GetPos().y,
        m_rigidbody.GetPos().z);
    float angleY = std::atan2(m_dir.x, m_dir.z);
    float angleX = -DX_PI_F / 2.0f;
    // エフェクトの向きを進行方向に合わせる
    SetRotationPlayingEffekseer3DEffect(m_playingEffect, angleX, angleY, 0.0f);

    if (m_timer >= m_durationTime)
    {
        m_isDead = true;
    }
}

void Fire::Draw()
{
}

void Fire::OnCollide(std::shared_ptr<Collidable> collider)
{
    if (m_isDead) return;
    m_isDead = true;
    StopEffekseer3DEffect(m_playingEffect);
    m_playingEffect = -1;
}
