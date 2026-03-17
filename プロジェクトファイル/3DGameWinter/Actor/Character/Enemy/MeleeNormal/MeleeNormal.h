#pragma once
#include "Actor/Character/Enemy/EnemyBase.h"

class EnemyMeleeWeapon;
class MeleeNormal : public EnemyBase
{
public:
    MeleeNormal(int model);
    virtual ~MeleeNormal();

    void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
    void Update() override;
    void Draw() override;
    void OnDamage(int atk) override;

    std::shared_ptr<EnemyMeleeWeapon> GetWeapon() { return m_weapon; }

private:
    std::shared_ptr<StateMachine<MeleeNormal>> m_state;
    std::shared_ptr<EnemyMeleeWeapon> m_weapon;
};

