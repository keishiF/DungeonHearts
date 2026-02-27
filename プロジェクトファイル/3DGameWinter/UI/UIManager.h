#pragma once
#include <map>
#include <memory>
#include <list>

class EnemyBase;
class Player;
class UIBase;
// 全てのUI要素を管理するクラス
class UIManager
{
public:
    // コンストラクタでPlayerのポインタを受け取り、
    // 生成するUI要素に渡す
    UIManager();
    virtual ~UIManager();

    void Init(const std::shared_ptr<Player>& player,
        const std::list<std::shared_ptr<EnemyBase>>& enemies);
    void Update(const std::list<std::shared_ptr<EnemyBase>>& enemies);
    void Draw();

private:
    // 管理するUI要素のリスト
    std::list<std::shared_ptr<UIBase>> m_uiElements;
    // 敵とHPゲージのマップ (敵のポインタをキーに、ゲージを管理)
    std::map<EnemyBase*, std::weak_ptr<UIBase>> m_enemyGaugeMap;
};

