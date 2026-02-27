#pragma once
#include <string>
#include <unordered_map>

class SoundManager
{
public:
    static SoundManager& GetInstance();

    void Init();
    void Update();
    void Release();

    // BGM
    void PlayBGM(const std::string& name, bool loop = true);
    void StopBGM();

    // SE
    void PlaySE(const std::string& name);

    // 音量反映
    void ApplyVolume();

private:
    SoundManager() = default;

    std::unordered_map<std::string, int> m_bgmHandles;
    std::unordered_map<std::string, int> m_seHandles;

    int m_currentBGM = -1;
};