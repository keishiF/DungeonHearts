#include "GameOverScene.h"

#include "Actor/ActorController.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Character/Player/PlayerState/PlayerStateGameOver.h"
#include "Actor/SkyDome/SkyDome.h"
#include "Camera/Camera.h"
#include "Core/game.h"
#include "GameScene.h"
#include "MyLib/Input.h"
#include "SceneController.h"
#include "TitleScene.h"
#include <cassert>
#include <DxLib.h>

namespace
{
    constexpr int kFadeInterval = 60;
}

GameOverScene::GameOverScene(SceneController& controller) :
    SceneBase(controller),
    m_bgmHandle(-1),
    m_bgHandle(-1),
    m_gameOverHandle(-1),
    m_fadeFrame(kFadeInterval),
    m_restartButtonImg(-1),
    m_titleButtonImg(-1),
    m_update(&GameOverScene::FadeInUpdate),
    m_draw(&GameOverScene::FadeDraw),
    m_selectedButton(SelectedButton::Restart)
{
    m_bgmHandle = LoadSoundMem("Data/Sound/BGM/GameOver.mp3");
    ChangeVolumeSoundMem(128, m_bgmHandle);
    PlaySoundMem(m_bgmHandle, DX_PLAYTYPE_LOOP);
    
    m_bgHandle = LoadGraph("Data/UI/Result/Result.png");
    assert(m_bgHandle >= 0);

    m_gameOverHandle = LoadGraph("Data/UI/Result/GameOver.png");
    assert(m_gameOverHandle >= 0);

    m_restartButtonImg = LoadGraph("Data/UI/Result/ReStart.png");
    assert(m_restartButtonImg >= 0);
    m_titleButtonImg = LoadGraph("Data/UI/Result/BackTitle.png");
    assert(m_titleButtonImg >= 0);
}

GameOverScene::~GameOverScene()
{
    DeleteGraph(m_bgHandle);
    DeleteGraph(m_restartButtonImg);
    DeleteGraph(m_titleButtonImg);
    if (m_bgmHandle > 0)
    {
        DeleteSoundMem(m_bgmHandle);
    }
}

void GameOverScene::Update()
{
    (this->*m_update)();
}

void GameOverScene::Draw()
{
    (this->*m_draw)();
}

void GameOverScene::NormalUpdate()
{
    auto& input = Input::GetInstance();

    if (input.IsTrigger("Right"))
    {
        m_selectedButton = SelectedButton::Title;
    }
    else if (input.IsTrigger("Left"))
    {
        m_selectedButton = SelectedButton::Restart;
    }

    if (input.IsTrigger("Attack"))
    {
        if (m_bgmHandle > 0) StopSoundMem(m_bgmHandle);

        if (m_selectedButton == SelectedButton::Restart)
        {
            m_nextScene = std::make_shared<GameScene>(m_controller);
        }
        else // Title
        {
            m_nextScene = std::make_shared<TitleScene>(m_controller);
        }
        m_update = &GameOverScene::FadeOutUpdate;
        m_draw = &GameOverScene::FadeDraw;
        m_fadeFrame = 0;
    }
}

void GameOverScene::FadeInUpdate()
{
    if (--m_fadeFrame <= 0)
    {
        m_update = &GameOverScene::NormalUpdate;
        m_draw = &GameOverScene::NormalDraw;
    }
}

void GameOverScene::FadeOutUpdate()
{
    if (++m_fadeFrame >= kFadeInterval)
    {
        StopSoundMem(m_bgmHandle);
        m_controller.ChangeScene(m_nextScene);
        return;
    }
}

void GameOverScene::NormalDraw()
{
    int width = 0, height = 0;
    GetGraphSize(m_bgHandle, &width, &height);
    int x = (Game::kScreenWidth - width) / 2;
    int y = Game::kScreenHeight / 6;
    //DrawRotaGraph(x, y, 2.5f, 0.0f, m_bgHandle, TRUE);

    width = 0, height = 0;
    GetGraphSize(m_gameOverHandle, &width, &height);
    x = (Game::kScreenWidth - width) / 2;
    y = Game::kScreenHeight / 15; // 画面の上部1/5の位置
    DrawGraph(x, y, m_gameOverHandle, TRUE);

    int restartW = 0, restartH = 0, titleW = 0, titleH = 0;
    GetGraphSize(m_restartButtonImg, &restartW, &restartH);
    GetGraphSize(m_titleButtonImg, &titleW, &titleH);

    int buttonY = 800;
    int spacing = 100;
    int totalWidth = restartW + titleW + spacing;
    int restartX = (Game::kScreenWidth - totalWidth) / 2;
    int titleX = restartX + restartW + spacing;

    SetDrawBright(
        m_selectedButton == SelectedButton::Restart ? 255 : 150,
        m_selectedButton == SelectedButton::Restart ? 255 : 150,
        m_selectedButton == SelectedButton::Restart ? 255 : 150
    );
    DrawGraph(restartX, buttonY, m_restartButtonImg, TRUE);

    SetDrawBright(
        m_selectedButton == SelectedButton::Title ? 255 : 150,
        m_selectedButton == SelectedButton::Title ? 255 : 150,
        m_selectedButton == SelectedButton::Title ? 255 : 150
    );
    DrawGraph(titleX, buttonY, m_titleButtonImg, TRUE);

    SetDrawBright(255, 255, 255);
}

void GameOverScene::FadeDraw()
{
    NormalDraw();
    float rate = static_cast<float>(m_fadeFrame) /
        static_cast<float>(kFadeInterval);
    SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255));
    DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}