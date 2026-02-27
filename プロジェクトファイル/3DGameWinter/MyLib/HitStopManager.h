#pragma once

class HitStopManager
{
public:
    static HitStopManager& GetInstance();

    void Update();
    void RequestHitStop(int frames);
    bool IsHitStopping() const { return m_hitStopTimer > 0; }

private:
    HitStopManager() = default;
    HitStopManager(const HitStopManager&) = delete;
    HitStopManager& operator=(const HitStopManager&) = delete;

    int m_hitStopTimer = 0;
};
