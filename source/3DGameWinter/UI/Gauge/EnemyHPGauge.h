#pragma once
#include "UI/UIBase.h"

class EnemyBase;
class EnemyHPGauge : public UIBase
{
public:
    EnemyHPGauge(const std::shared_ptr<EnemyBase>& enemy);
    virtual ~EnemyHPGauge();

    void Init() override;
    void Update() override {}
    void Draw() override;

    std::weak_ptr<EnemyBase> GetEnemy() const { return m_enemy; }

private:
    std::weak_ptr<EnemyBase> m_enemy;

    static int s_backImg;
    static int s_barImg;
    static int s_refCount;
    static int s_width;
    static int s_height;
};

