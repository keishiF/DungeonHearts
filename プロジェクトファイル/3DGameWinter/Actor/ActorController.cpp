#include "ActorController.h"

#include "Camera/Camera.h"
#include "Character/Enemy/Boss/Boss.h"
#include "Character/Enemy/EnemyBase.h"
#include "Character/Enemy/FlyNormal/FlyNormal.h"
#include "Character/Enemy/MeleeNormal/MeleeNormal.h"
#include "Character/Player/Player.h"
#include "Attack/Attack.h"
#include "MyLib/Physics/Physics.h"
#include "MyLib/TransformDataLoader.h"
#include "Stage/StageController.h"
#include "Stage/StageObjectBase.h"
#include <fstream>
#include <sstream>

ActorController::ActorController() :
    m_isBossDead(false),
    m_killScore(0),
    m_enemyMeleeModel(-1),
    m_enemyFlyModel(-1),
    m_enemyBossModel(-1)
{
}

ActorController::~ActorController()
{
    // まず、登録されている全ての攻撃オブジェクトを物理エンジンから解除し破棄
    for (const auto& attack : m_attacks)
    {
        if (attack)
        {
            attack->Final(); // Physics から登録解除
        }
    }
    m_attacks.clear();

    // 管理している敵をすべて物理エンジンから解除し破棄
    for (const auto& enemy : m_enemies)
    {
        if (enemy)
        {
            enemy->Final();
        }
    }
    m_enemies.clear();

    // プレイヤーを物理エンジンから解除
    if (m_player)
    {
        m_player->Final();
        m_player.reset();
    }

    // ステージコントローラの解放（内部リソースは StageController 側で管理）
    if (m_stageController)
    {
        m_stageController.reset();
    }

    // モデル解放
    if (m_enemyMeleeModel  != -1) { MV1DeleteModel(m_enemyMeleeModel); m_enemyMeleeModel = -1; }
    if (m_enemyFlyModel    != -1) { MV1DeleteModel(m_enemyFlyModel);   m_enemyFlyModel = -1; }
    if (m_enemyBossModel   != -1) { MV1DeleteModel(m_enemyBossModel);  m_enemyBossModel = -1; }
}

void ActorController::Init(const char* characterDataPath, 
    const char* stageDataPath,
    const char* spawnGroupDataPath)
{
    TransformDataLoader loader;
    auto transformDataList = loader.LoadDataCSV(characterDataPath);

    // スポーン地点のデータを読み込む
    std::ifstream spawnFile(spawnGroupDataPath);
    if (spawnFile.is_open())
    {
        std::string line;
        std::getline(spawnFile, line); // ヘッダーをスキップ
        while (std::getline(spawnFile, line))
        {
            std::stringstream ss(line);
            std::string part;
            std::vector<std::string> values;
            while (std::getline(ss, part, ','))
            {
                values.emplace_back(part);
            }
            if (values.size() >= 5)
            {
                SpawnTrigger trigger;
                trigger.groupID = std::stoi(values[0]);
                trigger.pos = 
                { 
                    std::stof(values[1]) * 100.0f, 
                    std::stof(values[2]) * 100.0f,
                    std::stof(values[3]) * 100.0f
                };
                trigger.radius = std::stof(values[4]);
                trigger.triggered = false;
                m_spawnTriggers.emplace_back(trigger);
            }
        }
    }

    m_enemyMeleeModel = MV1LoadModel("Data/Model/Enemy/MeleeNormal/MeleeNormal.mv1");
    assert(m_enemyMeleeModel >= 0);
    m_enemyFlyModel = MV1LoadModel("Data/Model/Enemy/FlyNormal/FlyNormal.mv1");
    assert(m_enemyFlyModel >= 0);
    m_enemyBossModel = MV1LoadModel("Data/Model/Enemy/Boss/Boss.mv1");
    assert(m_enemyBossModel >= 0);

    for (const auto& data : transformDataList)
    {
        if (data.groupId == 0)
        {
            // GroupIDが0の場合は即時スポーン
            SpawnCharacter(data);
        }
        else
        {
            // それ以外は待機リストに追加
            m_waitingObjects.push_back({ data });
        }
    }

    m_stageController = std::make_shared<StageController>();
    m_stageController->Init(stageDataPath);
}

void ActorController::Update(std::shared_ptr<Camera> camera)
{
    // スポーン処理
    if (m_player)
    {
        for (auto& trigger : m_spawnTriggers)
        {
            if (!trigger.triggered)
            {
                float distanceSq = (m_player->GetPos() - trigger.pos).SqrLength();
                if (distanceSq < trigger.radius * trigger.radius)
                {
                    trigger.triggered = true;
                    // 該当するGroupIDの敵をスポーンさせる
                    std::erase_if(m_waitingObjects, 
                        [this, &trigger](const WaitingObject& pending) 
                        {
                            if (pending.data.groupId == trigger.groupID)
                            {
                                SpawnCharacter(pending.data);
                                return true; // リストから削除
                            }
                            return false;
                        });
                }
            }
        }
    }

    if (m_stageController) m_stageController->Update();

    // Playerを更新
    if (m_player) m_player->Update(camera);

    // 敵を更新
    for (const auto& enemy : m_enemies)
    {
        enemy->Update();
    }

    // 魔法を更新
    for (const auto& attack : m_attacks)
    {
        attack->Update();
    }
    
    // 使用済みの魔法をリストから削除
    std::erase_if(m_attacks, [](const auto& attack) 
        {
            if (attack->IsDead())
            {
                attack->Final(); // Physicsから登録解除
                return true;
            }
            return false;
        });

    // 死亡した敵をリストから削除
    std::erase_if(m_enemies, [this](const auto& enemy)
        {
            if (enemy->IsDead())
            {
                if (enemy->GetTag() == ObjectTag::Boss)
                {
                    m_isBossDead = true;
                }
                enemy->Final(); // Physicsから登録解除
                return true;
            }
            return false;
        });
}

void ActorController::Draw()
{
    // Playerを描画
    if (m_player) m_player->Draw();

    // 敵を描画
    for (const auto& enemy : m_enemies) enemy->Draw();

    // 魔法を描画
    for (const auto& attack : m_attacks) attack->Draw();

    m_stageController->Draw();
}

bool ActorController::IsBossDead() const
{
    return m_isBossDead;
}

std::shared_ptr<EnemyBase> ActorController::FindNearestEnemy(const Vector3& playerPosition, float findRange)
{
    std::shared_ptr<EnemyBase> nearestEnemy = nullptr;
    float minDistanceSq = FLT_MAX; 

    for (const auto& enemy : m_enemies)
    {
        // 死亡している敵は対象外
        if (enemy->IsDead())
        {
            continue;
        }
        
        // 範囲外の敵は対象外
        float currentDistanceSq = (playerPosition -
        enemy->GetRigidbody().GetPos()).SqrLength();
        if (currentDistanceSq > findRange * findRange)
        {
            continue;
        }

        float distanceSq = (playerPosition - enemy->GetRigidbody().GetPos()).SqrLength();
        if (distanceSq < minDistanceSq)
        {
            minDistanceSq = distanceSq;
            nearestEnemy = enemy;
        }
    }
    
    return nearestEnemy;
}

std::list<Vector3> ActorController::RangeFindEnemiesPosition(const Vector3& center, float range, int maxTargets)
{
    // 距離と敵の位置のペアを格納する一時的なリスト
    std::list<std::pair<float, Vector3>> enemiesInRange;
    float rangeSq = range * range;

    for (const auto& enemy : m_enemies)
    {
        if (enemy->IsDead()) continue;

        Vector3 enemyPos = enemy->GetRigidbody().GetPos();
        float distanceSq = (center - enemyPos).SqrLength();
        if (distanceSq <= rangeSq)
        {
            enemiesInRange.push_back({ distanceSq, enemyPos });
        }
    }

    // 距離が近い順にソート (std::list のメンバ関数 sort を使用)
    enemiesInRange.sort([](const auto& a, const auto& b) 
        {
            return a.first < b.first;
        });
    
    // 返却用のリストを準備
    std::list<Vector3> result;

    int numToTake = min(static_cast<int>(enemiesInRange.size()), maxTargets);
    auto it = enemiesInRange.begin();
    for (int i = 0; i < numToTake; ++i)
    {
        result.emplace_back(it->second); // .second は Vector3
        ++it;
    }
    return result;
}

void ActorController::SpawnAttack(std::shared_ptr<Attack> attack)
{
    m_attacks.push_back(attack);
}

void ActorController::SpawnCharacter(const ObjectData& data)
{
    Vector3 pos = { data.pos.x, data.pos.y, data.pos.z };
    Vector3 rot = { data.rot.x, data.rot.y, data.rot.z };
    Vector3 scale = { data.scale.x, data.scale.y, data.scale.z };
    
    if (data.name == "Player")
    {
        // Playerに配置データを渡して初期化
        m_player = std::make_shared<Player>();
        m_player->Init(pos, rot, scale);
        m_player->SetActorController(shared_from_this());
    }
    else if (data.name == "EnemyMelee")
    {
        auto melee = std::make_shared<MeleeNormal>(m_enemyMeleeModel);
        melee->Init(pos, rot, scale);
        melee->SetActorController(shared_from_this());
        m_enemies.emplace_back(melee);
    }
    else if (data.name == "EnemyFly")
    {
        auto fly = std::make_shared<FlyNormal>(m_enemyFlyModel);
        fly->Init(pos, rot, scale);
        fly->SetActorController(shared_from_this());
        m_enemies.emplace_back(fly);
    }
    else if (data.name == "Boss")
    {
        auto boss = std::make_shared<Boss>(m_enemyBossModel);
        boss->Init(pos, rot, scale);
        boss->SetActorController(shared_from_this());
        m_enemies.emplace_back(boss);
    }
}
