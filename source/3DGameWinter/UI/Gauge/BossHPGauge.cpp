#include "BossHPGauge.h"

#include "Actor/Character/Enemy/Boss/Boss.h"
#include "Actor/Character/Enemy/EnemyBase.h"
#include "Core/game.h"

BossHPGauge::BossHPGauge(const std::shared_ptr<EnemyBase> boss) :
	m_boss(boss),
    m_width(0),
    m_backImg(-1),
    m_barImg(-1),
    m_refCount(0),
    m_height(0)
{
    m_refCount++;
}

BossHPGauge::~BossHPGauge()
{
    m_refCount--;
    if (m_refCount == 0)
    {
        DeleteGraph(m_backImg);
        DeleteGraph(m_barImg);
        m_backImg = -1;
        m_barImg = -1;
    }
}

void BossHPGauge::Init()
{
    if (m_backImg == -1)
    {
        m_backImg = LoadGraph("Data/UI/HPBossBack.png");
        m_barImg = LoadGraph("Data/UI/HPBoss.png");
    }
    GetGraphSize(m_backImg, &m_width, &m_height);
}

void BossHPGauge::Draw()
{
    float hp = m_boss->GetStatusComp().m_hp;
    float maxHp = m_boss->GetStatusComp().m_maxHp;
    if (maxHp <= 0.0f) return;

    float ratio = std::clamp(hp / maxHp, 0.0f, 1.0f);
    int drawW = static_cast<int>(m_width * ratio);

    const int x = (Game::kScreenWidth * 0.5f) - (m_width * 0.5f);
    const int y = (Game::kScreenHeight * 0.5f) - (m_height * 0.5f);

    // 背景
    DrawGraph(x, y + 200, m_backImg, TRUE);

    // HPバー
    DrawRectGraph(
        x, y + 200,
        0, 0,
        drawW, m_height,
        m_barImg,
        TRUE
    );
}
