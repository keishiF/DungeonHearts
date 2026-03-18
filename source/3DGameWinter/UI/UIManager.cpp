#include "UIManager.h"

#include "Actor/Character/Enemy/EnemyBase.h"
#include "Actor/Character/Player/Player.h"
#include "Gauge/BossHPGauge.h"
#include "Gauge/EnemyHPGauge.h"
#include "Gauge/PlayerHPGauge.h"
#include "LockOn/LockOnUI.h"
#include "UIBase.h"
#include <cassert>
#include <vector>

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}

void UIManager::Init(const std::shared_ptr<Player>& player, 
    const std::list<std::shared_ptr<EnemyBase>>& enemies)
{
    m_uiElements.clear();
    m_enemyGaugeMap.clear();

    auto lockOn = std::make_shared<LockOnUI>(std::weak_ptr<Player>(player));
    lockOn->Init();
    m_uiElements.push_back(lockOn);

    auto playerHP = std::make_shared<PlayerHPGauge>(player);
    playerHP->Init();
    m_uiElements.push_back(playerHP);

    for (auto& enemy : enemies)
    {
        /*if (enemy && !enemy->IsBoss())
        {
            auto bossHP = std::make_shared<BossHPGauge>(enemy);
            bossHP->Init();
            m_uiElements.push_back(bossHP);
        }
        else
        {
            auto enemyHP = std::make_shared<EnemyHPGauge>(enemy);
            enemyHP->Init();
            m_uiElements.push_back(enemyHP);
        }*/
        std::shared_ptr<UIBase> gauge;
        if (enemy->IsBoss())
        {
            gauge = std::make_shared<BossHPGauge>(enemy);
        }
        else
        {
            gauge = std::make_shared<EnemyHPGauge>(enemy);
        }
        gauge->Init();
        m_uiElements.push_back(gauge);
        m_enemyGaugeMap[enemy.get()] = gauge; // マップに登録
    }
}

void UIManager::Update(const std::list<std::shared_ptr<EnemyBase>>& enemies)
{
    /*for (auto& enemy : enemies)
    {
        if (enemy->IsBoss())
        {
            auto bossHP = std::make_shared<BossHPGauge>(enemy);
            bossHP->Init();
            m_uiElements.push_back(bossHP);
        }
        else
        {
            auto enemyHP = std::make_shared<EnemyHPGauge>(enemy);
            enemyHP->Init();
            m_uiElements.push_back(enemyHP);
        }
    }*/

    // 死んだ敵や無効なゲージをリストから削除
    // std::remove_if と std::list::erase を組み合わせるか、std::erase_ifを使用
    m_uiElements.remove_if([this](const std::shared_ptr<UIBase>& ui) 
        {
            // EnemyHPGaugeまたはBossHPGaugeの場合のみ処理
            if (auto enemyHpGauge = std::dynamic_pointer_cast<EnemyHPGauge>(ui)) 
            {
                // 敵が期限切れ（shared_ptrがもう存在しない）か、死んでいる場合は削除
                return enemyHpGauge->GetEnemy().expired() || 
                    enemyHpGauge->GetEnemy().lock()->IsDead();
            }
            if (auto bossHpGauge = std::dynamic_pointer_cast<BossHPGauge>(ui)) 
            {
                // 敵が期限切れ（shared_ptrがもう存在しない）か、死んでいる場合は削除
                return bossHpGauge->GetEnemy().expired() || 
                    bossHpGauge->GetEnemy().lock()->IsDead();
            }
            return false;
        });
    
    // m_enemyGaugeMapからも死んだ敵を削除
    // std::erase_if は C++20 から利用可能
    // C++17以前の場合は手動でループして削除する必要があります
    std::erase_if(m_enemyGaugeMap, [](const auto& pair) 
        {
            // EnemyBase* が指すオブジェクトが死んでいる場合
            return pair.first->IsDead();
        });
    // 新しく出現した敵のHPゲージを生成
    for (const auto& enemy : enemies)
    {
        // 敵が有効で、まだHPゲージが作成されていない場合
        if (enemy && !enemy->IsDead() && 
            m_enemyGaugeMap.find(enemy.get()) ==m_enemyGaugeMap.end())
        {
            std::shared_ptr<UIBase> gauge;
            if (enemy->IsBoss())
            {
                gauge = std::make_shared<BossHPGauge>(enemy);
            }
            else
            {
                gauge = std::make_shared<EnemyHPGauge>(enemy);
            }
            gauge->Init();
            m_uiElements.push_back(gauge);
            m_enemyGaugeMap[enemy.get()] = gauge; // マップに登録
        }
    }

    // 管理している全てのUI要素のUpdateを呼ぶ
    for (auto& ui : m_uiElements)
    {
        ui->Update();
    }
}

void UIManager::Draw()
{
    // 管理している全てのUI要素のDrawを呼ぶ
    for (auto& ui : m_uiElements)
    {
        ui->Draw();
    }
}
