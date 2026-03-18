#include "Actor.h"

Actor::Actor(ObjectTag tag, ColliderData::Kind colliderKind) :
    m_model(-1),
    m_radius(0.0f),
    m_isDead(false),
    m_frame(0),
    m_atkFrame(0),
    m_blinkFrame(0),
    m_forward({0.0f, 0.0f, 1.0f}),
    m_rotQ(Quaternion()),
    Collidable(tag, colliderKind)
{
}

Actor::~Actor()
{
    if (m_model != -1)
    {
        MV1DeleteModel(m_model);
    }
}

void Actor::OnCollide(std::shared_ptr<Collidable> collider)
{
    OnDamage(1);
    //// 衝突相手の攻撃力を取得する
    //int damage = collider->GetAttackPower();

    //// ダメージが0より大きい場合のみ処理する
    //if (damage > 0)
    //{
    //    OnDamage(damage);
    //}
}

void Actor::UpdateBlinking()
{
    if (m_blinkFrame > 0)
    {
        --m_blinkFrame;
    }
}

bool Actor::IsVisible() const
{
    // 点滅中の場合
    if (m_blinkFrame > 0)
    {
        // 4フレームごとに表示/非表示を切り替える
        if ((m_blinkFrame / 4) % 2 == 0)
        {
            // 非表示
            return false; 
        }
    }
    // 表示
    return true; 
}

void Actor::StartBlinking(int duration)
{
    // すでに点滅中でなければ、新たに点滅を開始する
    if (m_blinkFrame <= 0)
    {
        m_blinkFrame = duration;
    }
}
