#pragma once
#include "MyLib/Physics/Collider/Collidable.h"

/// <summary>
/// 魔法の基底クラス
/// </summary>
class Magic : public Collidable
{
public:
    Magic(ObjectTag tag, ColliderData::Kind kind);
    virtual ~Magic();

    virtual void Init(const Vector3& pos, const Vector3& targetPos) abstract;
    virtual void Update() abstract;
    virtual void Draw() abstract;

    virtual int GetAttackPower() const { return m_atkPower; }
    bool IsDead() const { return m_isDead; }

protected:
    int m_atkPower;
    float m_lifeTime;
    float m_timer;
    bool m_isDead;
};

