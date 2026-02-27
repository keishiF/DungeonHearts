#pragma once
#include "UI/UIBase.h"

class EnemyBase;
class BossHPGauge : public UIBase
{
public:
    BossHPGauge(const std::shared_ptr<EnemyBase> boss);
    ~BossHPGauge();

    void Init() override;
    void Update() override {}
    void Draw() override;

    std::weak_ptr<EnemyBase> GetEnemy() const { return m_boss; }

private:
    const std::shared_ptr<EnemyBase> m_boss;

    int m_backImg;
    int m_barImg;
    int m_refCount;
    int m_width;
    int m_height;
};

