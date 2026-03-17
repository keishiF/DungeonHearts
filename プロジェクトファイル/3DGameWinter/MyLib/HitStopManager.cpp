#include "HitStopManager.h"

HitStopManager& HitStopManager::GetInstance()
{
    static HitStopManager instance;
    return instance;
}

void HitStopManager::Update()
{
    if (m_hitStopTimer > 0)
    {
        m_hitStopTimer--;
    }
}

void HitStopManager::RequestHitStop(int frames)
{
    m_hitStopTimer = frames;
}
