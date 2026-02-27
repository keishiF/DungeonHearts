#pragma once
#include "Actor/Character/Enemy/EnemyBase.h"

class FlyNormal : public EnemyBase
{
public:
    FlyNormal(int model);
    virtual ~FlyNormal();

    void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
    void Update() override;
    void Draw() override;
    void OnDamage(int atk) override;

private:
    std::shared_ptr<StateMachine<FlyNormal>> m_state;
};

