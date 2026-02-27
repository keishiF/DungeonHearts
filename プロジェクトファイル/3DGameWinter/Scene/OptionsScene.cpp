#include "OptionsScene.h"

#include "Core/Game.h"
#include "MyLib/Input.h"
#include "MyLib/SoundManager.h"
#include "SceneController.h"
#include <DxLib.h>
#include <algorithm>
#include <memory>

#include "TitleScene.h"

namespace
{
    constexpr int kMainButtonWidth = 400;
    constexpr int kMainButtonHeight = 100;
    constexpr int kMainButtonGap = 30;

    constexpr int kInactiveAlpha = 150;

    constexpr int kCameraStartY = 260;
    constexpr int kCameraGapY = 160;

    constexpr int kSoundPanelGap = 40;

    constexpr int kSliderValueMax = 10;
}

OptionsScene::OptionsScene(SceneController& controller, OptionMode mode) : 
    SceneBase(controller), 
    m_options(Options::GetInstance()), 
    m_mode(mode)
{
    m_options.Load();

    m_bg = LoadGraph("Data/UI/BackGround/BackGround1.png");
    m_imgCamera = LoadGraph("Data/UI/Option/Camera.png");
    m_imgSound = LoadGraph("Data/UI/Option/Sound.png");
    m_imgBack = LoadGraph("Data/UI/Option/Back.png");
    m_imgBackTitle = LoadGraph("Data/UI/Option/BackTitle.png");
    m_imgBackGame = LoadGraph("Data/UI/Option/BackGame.png");
    m_imgCursor = LoadGraph("Data/UI/Cursor.png");

    m_imgCamXNormal = LoadGraph("Data/UI/Option/CameraBesideNormal.png");
    m_imgCamXReverse = LoadGraph("Data/UI/Option/CameraBesideReverse.png");
    m_imgCamYNormal = LoadGraph("Data/UI/Option/CameraVerticalNormal.png");
    m_imgCamYReverse = LoadGraph("Data/UI/Option/CameraVerticalReverse.png");

    m_imgSliderPanel = LoadGraph("Data/UI/Option/SubMenuBack.png");
    m_imgSliderBack = LoadGraph("Data/UI/Option/SoundSliderBack.png");
    m_imgSlider = LoadGraph("Data/UI/Option/SoundSlider.png");
    m_imgSliderKnob = LoadGraph("Data/UI/Option/SliderHandle.png");

    // メニュー構築
    m_mainEntries.push_back(MainType::Camera);
    m_mainEntries.push_back(MainType::Sound);

    if (m_mode == OptionMode::FromTitle)
    {
        m_mainEntries.push_back(MainType::Back);
    }
    else
    {
        m_mainEntries.push_back(MainType::Back);
        m_mainEntries.push_back(MainType::BackGame);
    }
}

OptionsScene::~OptionsScene()
{
    m_options.Save();

    DeleteGraph(m_bg);
    DeleteGraph(m_imgCamera);
    DeleteGraph(m_imgSound);
    DeleteGraph(m_imgBack);
    DeleteGraph(m_imgBackTitle);
    DeleteGraph(m_imgBackGame);
    DeleteGraph(m_imgCursor);

    DeleteGraph(m_imgCamXNormal);
    DeleteGraph(m_imgCamXReverse);
    DeleteGraph(m_imgCamYNormal);
    DeleteGraph(m_imgCamYReverse);

    DeleteGraph(m_imgSliderPanel);
    DeleteGraph(m_imgSliderBack);
    DeleteGraph(m_imgSlider);
    DeleteGraph(m_imgSliderKnob);
}

void OptionsScene::Update()
{
    if (!m_inSubMenu)
    {
        UpdateMainMenu();

    }
    else
    {
        UpdateSubMenu();
    }
}

void OptionsScene::UpdateMainMenu()
{
    auto& input = Input::GetInstance();
    auto& sound = SoundManager::GetInstance();

    if (input.IsTrigger("Up"))
    {
        m_mainIndex = (m_mainIndex - 1 + m_mainEntries.size()) % m_mainEntries.size();
        sound.PlaySE("Cursor");
    }

    if (input.IsTrigger("Down"))
    {
        m_mainIndex = (m_mainIndex + 1) % m_mainEntries.size();
        sound.PlaySE("Cursor");
    }

    if (input.IsTrigger("Attack"))
    {
        MainType type = m_mainEntries[m_mainIndex];

        if (type == MainType::Camera || type == MainType::Sound)
        {
            m_inSubMenu = true;
        }
        else
        {
            if (type == MainType::Back)
            {
                if (m_mode == OptionMode::FromTitle)
                {
                    m_controller.PopScene(); // タイトル画面からオプションを開いた場合はタイトルに戻る
                }
                else // m_mode == OptionMode::FromGame
                {
                    // ゲーム画面からオプションを開き、「タイトルに戻る」を選択した場合
                    m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
                }
            }
            else if (type == MainType::BackGame)
            {
                // ゲーム画面からオプションを開き、「ゲームに戻る」を選択した場合
                m_controller.PopScene();
            }
        }
    }
}

void OptionsScene::UpdateSubMenu()
{
    auto& input = Input::GetInstance();

    if (input.IsTrigger("Cancel"))
    {
        m_inSubMenu = false;
        return;
    }

    MainType type = m_mainEntries[m_mainIndex];

    if (type == MainType::Camera)
    {
        UpdateCamera();
    }
    else if (type == MainType::Sound)
    {
        UpdateSound();
    }
}

void OptionsScene::UpdateCamera()
{
    auto& input = Input::GetInstance();

    if (input.IsTrigger("Up"))
    {
        m_cameraSel = max(0, m_cameraSel - 1);
    }

    if (input.IsTrigger("Down"))
    {
        m_cameraSel = min(1, m_cameraSel + 1);
    }

    if (input.IsTrigger("Left") ||
        input.IsTrigger("Right") ||
        input.IsTrigger("Attack"))
    {
        if (m_cameraSel == 0)
        {
            m_options.invertX = !m_options.invertX;
        }
        else
        {
            m_options.invertY = !m_options.invertY;
        }
    }
}

void OptionsScene::UpdateSound()
{
    auto& input = Input::GetInstance();
    auto& sound = SoundManager::GetInstance();

    if (input.IsTrigger("Up"))
    {
        m_soundSel = max(0, m_soundSel - 1);
    }

    if (input.IsTrigger("Down"))
    {
        m_soundSel = min(2, m_soundSel + 1);
    }

    int delta = 0;
    if (input.IsTrigger("Left"))  delta = -1;
    if (input.IsTrigger("Right")) delta = 1;

    if (delta == 0) return;

    int* target = nullptr;

    if (m_soundSel == 0) target = &m_options.masterVolume;
    if (m_soundSel == 1) target = &m_options.bgmVolume;
    if (m_soundSel == 2) target = &m_options.seVolume;

    if (!target) return;

    int display = *target / Options::VolumeStep;
    display = std::clamp(display + delta, 0, kSliderValueMax);
    *target = display * Options::VolumeStep;

    sound.ApplyVolume();
}

void OptionsScene::Draw()
{
    DrawGraph(0, 0, m_bg, TRUE);

    if (!m_inSubMenu)
    {
        DrawMainMenu();
    }
    else
    {
        DrawSubMenu();
    }
}

void OptionsScene::DrawMainMenu()
{
    int count = static_cast<int>(m_mainEntries.size());
    int totalH = kMainButtonHeight * count +
        kMainButtonGap * (count - 1);

    int baseX = (Game::kScreenWidth - kMainButtonWidth) / 2;
    int baseY = (Game::kScreenHeight - totalH) / 2;

    for (int i = 0; i < count; ++i)
    {
        int x = baseX;
        int y = baseY + i * (kMainButtonHeight + kMainButtonGap);

        int img = GetImageFromType(m_mainEntries[i]);

        DrawExtendGraph(x, y,
            x + kMainButtonWidth,
            y + kMainButtonHeight,
            img, TRUE);

        if (i == m_mainIndex)
        {
            DrawCursorAtButton(x, y);
        }
        else
        {
            SetDrawBlendMode(DX_BLENDMODE_MULA, kInactiveAlpha);
            DrawBox(x, y,
                x + kMainButtonWidth,
                y + kMainButtonHeight,
                0x000000, TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
    }
}

void OptionsScene::DrawSubMenu()
{
    MainType type = m_mainEntries[m_mainIndex];

    if (type == MainType::Camera)
    {
        DrawCameraMenu();
    }
    else if (type == MainType::Sound)
    {
        DrawSoundMenu();
    }
}

void OptionsScene::DrawCameraMenu()
{
    int centerX = Game::kScreenWidth / 2;

    int images[2] =
    {
        m_options.invertX ? m_imgCamXReverse : m_imgCamXNormal,
        m_options.invertY ? m_imgCamYReverse : m_imgCamYNormal
    };

    for (int i = 0; i < 2; ++i)
    {
        int w, h;
        GetGraphSize(images[i], &w, &h);

        int x = centerX - w / 2;
        int y = kCameraStartY + i * kCameraGapY;

        DrawGraph(x, y, images[i], TRUE);

        if (m_cameraSel == i)
        {
            int cw, ch;
            GetGraphSize(m_imgCursor, &cw, &ch);
            DrawGraph(x + w - cw / 2,
                y + (h - ch) / 2,
                m_imgCursor, TRUE);
        }
        else
        {
            SetDrawBlendMode(DX_BLENDMODE_MULA, kInactiveAlpha);
            DrawBox(x, y, x + w, y + h, 0x000000, TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
    }
}

void OptionsScene::DrawSoundMenu()
{
    int centerX = Game::kScreenWidth / 2;

    int panelW, panelH;
    GetGraphSize(m_imgSliderPanel, &panelW, &panelH);

    const int panelCount = 3;
    const int totalH =
        panelH * panelCount +
        kSoundPanelGap * (panelCount - 1);

    const int baseY =
        (Game::kScreenHeight - totalH) / 2;

    int sliderW, sliderH;
    GetGraphSize(m_imgSliderBack, &sliderW, &sliderH);

    int knobW, knobH;
    GetGraphSize(m_imgSliderKnob, &knobW, &knobH);

    int cursorW, cursorH;
    GetGraphSize(m_imgCursor, &cursorW, &cursorH);

    const char* labels[3] = { "マスター", "BGM", "SE" };
    int values[3] =
    {
        m_options.masterVolume,
        m_options.bgmVolume,
        m_options.seVolume
    };

    for (int i = 0; i < panelCount; ++i)
    {
        int panelX = centerX - panelW / 2;
        int panelY = baseY + i * (panelH + kSoundPanelGap);

        DrawGraph(panelX, panelY, m_imgSliderPanel, TRUE);

        int display = values[i] / Options::VolumeStep;

        int labelW =
            GetDrawStringWidth(labels[i],
                static_cast<int>(strlen(labels[i])));

        int labelX = panelX + (panelW - labelW) / 2;
        int labelY = panelY + 20;

        DrawString(labelX, labelY,
            labels[i],
            0xffffff);

        int sliderX =
            panelX + (panelW - sliderW) / 2;

        int sliderY =
            panelY + panelH / 2;

        char valueStr[8];
        sprintf_s(valueStr, "%d", display);

        int valueW =
            GetDrawStringWidth(valueStr,
                static_cast<int>(strlen(valueStr)));

        DrawString(
            sliderX - valueW - 20,
            sliderY + sliderH / 2 - 10,
            valueStr,
            0xffffff);

        DrawGraph(sliderX, sliderY,
            m_imgSliderBack, TRUE);

        float rate =
            static_cast<float>(display) / kSliderValueMax;

        int fillW =
            static_cast<int>(sliderW * rate);

        if (fillW > 0)
        {
            DrawRectGraph(
                sliderX,
                sliderY,
                0,
                0,
                fillW,
                sliderH,
                m_imgSlider,
                TRUE);
        }

        int knobX =
            sliderX + static_cast<int>((sliderW - knobW) * rate);

        int knobY =
            sliderY + (sliderH - knobH) / 2;

        DrawGraph(knobX, knobY,
            m_imgSliderKnob, TRUE);

        if (m_soundSel != i)
        {
            SetDrawBlendMode(DX_BLENDMODE_MULA, kInactiveAlpha);
            DrawBox(panelX,
                panelY,
                panelX + panelW,
                panelY + panelH,
                0x000000,
                TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
        else
        {
            int cx =
                panelX + panelW - cursorW / 2;

            int cy =
                panelY + (panelH - cursorH) / 2;

            DrawGraph(cx, cy,
                m_imgCursor,
                TRUE);
        }
    }
}

int OptionsScene::GetImageFromType(MainType type) const
{
    switch (type)
    {
    case MainType::Camera:
        return m_imgCamera;
    case MainType::Sound:
        return m_imgSound;
    case MainType::Back:
        return (m_mode == OptionMode::FromTitle) ? m_imgBack : m_imgBackTitle;
    case MainType::BackGame:
        return m_imgBackGame;
    }
    return -1;
}

void OptionsScene::DrawCursorAtButton(int x, int y)
{
    int cw, ch;
    GetGraphSize(m_imgCursor, &cw, &ch);

    DrawGraph(x + kMainButtonWidth - cw / 2,
        y + (kMainButtonHeight - ch) / 2,
        m_imgCursor, TRUE);
}