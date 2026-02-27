#pragma once
#include "Actor/Character/Enemy/EnemyBase.h"

class Boss : public EnemyBase
{
public:
    Boss(int model);
    virtual ~Boss();

    void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
    void Update() override;
    void Draw() override;

    void OnDamage(int atk) override;

    void StartCooldown(float time) { m_cooldownTimer = time; }
    bool IsOnCooldown() const { return m_cooldownTimer > 0.0f; }
    void UpdateCooldown();

    void ChangeState(std::shared_ptr<StateNode<Boss>> newState) 
    { m_state->ChangeState(newState); }

    bool IsBoss() const override { return true; }

    void SetLaserWarning(bool isDrawing, 
        const Vector3& start = Vector3(), 
        const Vector3& end = Vector3());

private:
    std::shared_ptr<StateMachine<Boss>> m_state;
    float m_cooldownTimer;

    bool m_isDrawingLaserWarning;
    Vector3 m_laserWarningStart;
    Vector3 m_laserWarningEnd;
    bool m_hasFiredHalfHpLaser;
};

