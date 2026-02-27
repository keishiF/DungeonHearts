#pragma once
#include "MyLib/Vector3.h"
#include "MyLib/TransformDataLoader.h"
#include <list>
#include <memory>

class Camera;
class EnemyBase;
class Attack;
class Player;
class SkyDome;
class StageController;
class ActorController : public std::enable_shared_from_this<ActorController>
{
public:
    ActorController();
    virtual ~ActorController();

    void Init(const char* characterDataPath, 
        const char* stageDataPath,
        const char* spawnGroupDataPath);
    void Update(std::shared_ptr<Camera> camera);
    void Draw();

    std::shared_ptr<StageController> GetStageController() const { return m_stageController; }
    std::shared_ptr<Player> GetPlayer() const { return m_player; }
    std::list<std::shared_ptr<EnemyBase>> GetEnemies() const { return m_enemies; }
    bool IsBossDead() const;

    void AddKillScore(int score) { m_killScore += score; }
    int GetKillScore() const { return m_killScore; };

    std::shared_ptr<EnemyBase> FindNearestEnemy(const Vector3& playerPosition, float findRange);
    std::list<Vector3> RangeFindEnemiesPosition(const Vector3& center, float range, int maxTargets);
    void SpawnAttack(std::shared_ptr<Attack> attack);

private:
    void SpawnCharacter(const ObjectData& data);

    std::shared_ptr<SkyDome> m_sky;
    std::shared_ptr<StageController> m_stageController;
    std::shared_ptr<Player> m_player;
    std::list<std::shared_ptr<EnemyBase>> m_enemies;
    std::list<std::shared_ptr<Attack>> m_attacks;

    bool m_isBossDead;

    int m_killScore;

    int m_enemyMeleeModel;
    int m_enemyFlyModel;
    int m_enemyBossModel;

    // 待機中のキャラクターオブジェクトのデータ
    struct WaitingObject
    {
        ObjectData data;
    };
    
    std::list<WaitingObject> m_waitingObjects;
    // 出現トリガーの情報
    struct SpawnTrigger
    {
        int groupID = 0;
        Vector3 pos = Vector3();
        float radius = 0.0f;
        bool triggered = false;
    };
    std::list<SpawnTrigger> m_spawnTriggers;
};

