#include "EnemyHPGauge.h"

#include "Actor/Character/Enemy/EnemyBase.h"
#include "Core/game.h"

namespace
{
    constexpr float kHeightOffset = 200.0f; // 頭上
}

int EnemyHPGauge::s_backImg = -1;
int EnemyHPGauge::s_barImg = -1;
int EnemyHPGauge::s_refCount = 0;
int EnemyHPGauge::s_width = 0;
int EnemyHPGauge::s_height = 0;

EnemyHPGauge::EnemyHPGauge(const std::shared_ptr<EnemyBase>& enemy) :
    m_enemy(enemy)
{
    if (s_refCount == 0)
    {
        s_backImg = LoadGraph("Data/UI/HPBack.png");
        s_barImg = LoadGraph("Data/UI/HP2.png");
        GetGraphSize(s_backImg, &s_width, &s_height);
    }
    s_refCount++;
}

EnemyHPGauge::~EnemyHPGauge()
{
    s_refCount--;
    if (s_refCount == 0)
    {
        DeleteGraph(s_backImg);
        DeleteGraph(s_barImg);
        s_backImg = -1;
        s_barImg = -1;
        s_width = 0;
        s_height = 0;
    }
}

void EnemyHPGauge::Init()
{
    // コンストラクタで初期化するため、この関数は空になります
}

void EnemyHPGauge::Draw()
{
    auto enemy = m_enemy.lock();
    if (!enemy || enemy->IsDead()) return;

    float hp = enemy->GetStatusComp().m_hp;
    float maxHp = enemy->GetStatusComp().m_maxHp;
    if (maxHp <= 0.0f) return;

    float ratio = std::clamp(hp / maxHp, 0.0f, 1.0f);

    // ===== ワールド → スクリーン =====
    Vector3 world = enemy->GetRigidbody().GetPos();
    world.y += kHeightOffset;

    VECTOR screen = ConvWorldPosToScreenPos(world.ToDxVECTOR());

    // HPゲージのスケール
    float scale = 0.3f;
    int scaled_width = static_cast<int>(s_width * scale);
    int scaled_height = static_cast<int>(s_height * scale);
    
    // HPゲージの描画開始位置 (左上)
    int draw_x = static_cast<int>(screen.x - scaled_width * 0.5f);
    int draw_y = static_cast<int>(screen.y - scaled_height * 0.5f);
    
    // 画面外なら描画しない
    if (screen.z < 0.0f || screen.z > 1.0f || draw_x + scaled_width < 0 ||
        draw_x > Game::kScreenWidth || draw_y + scaled_height < 0 ||
        draw_y > Game::kScreenHeight)
    {
        return;
    }

    // 背景
    DrawRotaGraph(draw_x + scaled_width * 0.5f, draw_y + scaled_height * 0.5f,
        scale, 0.0f, s_backImg, TRUE);

    // HPバー
    int drawW = static_cast<int>(s_width * ratio);
    if (drawW <= 0) return;

    // HPバーの描画中心X座標を調整し、左端を固定する
    int bar_center_x = draw_x + (static_cast<int>(drawW * scale) / 2);

    DrawRectRotaGraph(
        bar_center_x, draw_y + scaled_height * 0.5f,
        0, 0,
        drawW, s_height,
        scale, 0.0f,
        s_barImg,
        TRUE
    );
}
