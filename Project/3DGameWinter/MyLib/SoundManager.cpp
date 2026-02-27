#include "SoundManager.h"
#include "UI/Option/Options.h"
#include <DxLib.h>

SoundManager& SoundManager::GetInstance()
{
    static SoundManager instance;
    return instance;
}

void SoundManager::Init()
{
    // BGM読み込み
    m_bgmHandles["Title"]    = LoadSoundMem("Data/Sound/BGM/Title.mp3");
    m_bgmHandles["Game"]     = LoadSoundMem("Data/Sound/BGM/GameBGM.mp3");
    m_bgmHandles["Result"]   = LoadSoundMem("Data/Sound/BGM/ResultBGM.mp3");
    m_bgmHandles["GameOver"] = LoadSoundMem("Data/Sound/BGM/GameOver.mp3");

    // SE読み込み
    m_seHandles["Attack"] = LoadSoundMem("Data/Sound/SE/PlayerAtkSE.mp3");
    m_seHandles["Hit"]    = LoadSoundMem("Data/Sound/SE/HitSE.mp3");
    m_seHandles["Fire"]   = LoadSoundMem("Data/Sound/SE/FireSE.mp3");
    m_seHandles["Cursor"] = LoadSoundMem("Data/Sound/SE/CursorSE.mp3");
    m_seHandles["Button"] = LoadSoundMem("Data/Sound/SE/ButtonSE.mp3");

    ApplyVolume();
}

void SoundManager::Release()
{
    for (auto& bgm : m_bgmHandles)
        DeleteSoundMem(bgm.second);

    for (auto& se : m_seHandles)
        DeleteSoundMem(se.second);
}

void SoundManager::PlayBGM(const std::string& name, bool loop)
{
    auto it = m_bgmHandles.find(name);
    if (it == m_bgmHandles.end()) return;

    if (m_currentBGM != -1)
        StopSoundMem(m_currentBGM);

    m_currentBGM = it->second;

    PlaySoundMem(m_currentBGM, loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK);
}

void SoundManager::StopBGM()
{
    if (m_currentBGM != -1)
        StopSoundMem(m_currentBGM);
}

void SoundManager::PlaySE(const std::string& name)
{
    auto it = m_seHandles.find(name);
    if (it == m_seHandles.end()) return;

    PlaySoundMem(it->second, DX_PLAYTYPE_BACK);
}

void SoundManager::ApplyVolume()
{
    auto& opt = Options::GetInstance();

    int master = opt.masterVolume;
    int bgm = opt.bgmVolume;
    int se = opt.seVolume;

    int bgmFinal = bgm * master / Options::VolumeMax;
    int seFinal = se * master / Options::VolumeMax;

    bgmFinal = bgmFinal * 255 / Options::VolumeMax;
    seFinal = seFinal * 255 / Options::VolumeMax;

    for (auto& bgm : m_bgmHandles)
        ChangeVolumeSoundMem(bgmFinal, bgm.second);

    for (auto& se : m_seHandles)
        ChangeVolumeSoundMem(seFinal, se.second);
}
