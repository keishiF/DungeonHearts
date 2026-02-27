#include "LockOnUI.h"

#include "Actor/Character/Player/Player.h"
#include "Actor/Character/Enemy/EnemyBase.h"
#include <DxLib.h>

namespace
{
    // 回転速度
    constexpr float kRotationSpeed = 0.05f;
}

LockOnUI::LockOnUI(std::weak_ptr<Player> player) :
    m_player(player),
    m_screenPos(),
    m_image(-1),
    m_refCount(0),
    m_isDrawing(false),
    m_angle(0.0f)
{
    m_refCount++;
}

LockOnUI::~LockOnUI()
{
    m_refCount--;
    if (m_refCount == 0)
    {
        DeleteGraph(m_image);
        m_image = -1;
    }
}

void LockOnUI::Init()
{
    // 画像の読み込み
    m_image = LoadGraph("Data/UI/LockOn.png");
}

void LockOnUI::Update()
{
    m_isDrawing = false;
    // 角度を更新
    m_angle += kRotationSpeed;
    
    auto player = m_player.lock();
    if (!player) return;
    
    // プレイヤーがロックオン中か確認
    if (player->IsLockOn())
    {
        // ロックオン対象を取得
        auto target = player->GetLockedOnEnemy();
        if (target)
        {
            // 対象の3D座標を2Dスクリーン座標に変換
            Vector3 targetPos = target->GetRigidbody().GetPos();
            // ボスかどうかでY座標のオフセットを変更
            if (target->IsBoss())
            {
                targetPos.y += 500.0f; // ボス用の高い位置
            }
            else
            {
                targetPos.y += 100.0f; // 通常の敵の位置
            }
            VECTOR m_screenPosDx = ConvWorldPosToScreenPos(targetPos.ToDxVECTOR());
            m_screenPos = { m_screenPosDx.x, m_screenPosDx.y, m_screenPosDx.z };
            m_isDrawing = true;
        }
    }
}

void LockOnUI::Draw()
{
    if (m_isDrawing)
    {
        // 画像を回転させながら描画
        DrawRotaGraph(static_cast<int>(m_screenPos.x), 
            static_cast<int>(m_screenPos.y), 
            0.1f, m_angle, m_image, TRUE);
    }
}
