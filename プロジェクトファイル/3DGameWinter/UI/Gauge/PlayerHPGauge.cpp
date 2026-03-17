#include "PlayerHPGauge.h"

#include "Actor/Character/Player/Player.h"
#include <algorithm>

PlayerHPGauge::PlayerHPGauge(const std::shared_ptr<Player> player) :
    m_player(player),
    m_width(0),
    m_backImg(-1),
    m_barImg(-1),
    m_refCount(0),
    m_height(0)
{
    m_refCount++;
}

PlayerHPGauge::~PlayerHPGauge()
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

void PlayerHPGauge::Init()
{
    if (m_backImg == -1)
    {
        m_backImg = LoadGraph("Data/UI/HPBack.png");
        m_barImg = LoadGraph("Data/UI/HP.png");
    }
    GetGraphSize(m_backImg, &m_width, &m_height);
}

void PlayerHPGauge::Draw()
{
    if (!m_player) return;

    float hp = m_player->GetStatusComp().m_hp;
    float maxHp = m_player->GetStatusComp().m_maxHp;
    if (maxHp <= 0.0f) return;

    float ratio = std::clamp(hp / maxHp, 0.0f, 1.0f);
    int drawW = static_cast<int>(m_width * ratio);

    const int x = 30;
    const int y = 30;

    // 背景
    DrawGraph(x, y, m_backImg, TRUE);

    // HPバー
    DrawRectGraph(
        x, y,
        0, 0,
        drawW, m_height,
        m_barImg,
        TRUE
    );
}
