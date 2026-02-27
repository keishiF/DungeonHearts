#include "TitleScene.h"

#include "Actor/ActorController.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/Character/Player/PlayerState/PlayerStateTitle.h"
#include "Actor/SkyDome/SkyDome.h"
#include "Camera/Camera.h"
#include "Core/game.h"
#include "GameScene.h"
#include "MyLib/Input.h"
#include "MyLib/Physics/Physics.h"
#include "OptionsScene.h"
#include "SceneController.h"
#include <cassert>
#include <DxLib.h>
#include <EffekseerForDXLib.h>

namespace
{
	constexpr int kFadeInterval = 60;
}

TitleScene::TitleScene(SceneController& controller) :
	SceneBase(controller),
	m_bgmHandle(-1),
	m_titleHandle(-1),
	m_fontHandle(-1),
	m_startButtonImg(-1),
	m_fadeFrame(kFadeInterval),
	m_startFrame(0),
	m_blinkFrame(0),
	m_isStarting(false),
	m_isWalk(false),
	m_update(&TitleScene::FadeInUpdate),
	m_draw(&TitleScene::FadeDraw)
{
	m_optionButtonImg = LoadGraph("Data/UI/Title/Option.png");
	m_exitButtonImg = LoadGraph("Data/UI/Title/Exit.png");
	m_cursorImg = LoadGraph("Data/UI/Cursor.png");

	m_menuSel = MenuItem::Start;

	m_bgmHandle = LoadSoundMem("Data/Sound/BGM/TitleBGM.mp3");
	assert(m_bgmHandle > 0);
	ChangeVolumeSoundMem(128, m_bgmHandle);
	PlaySoundMem(m_bgmHandle, DX_PLAYTYPE_LOOP);

	m_titleHandle = LoadGraph("Data/UI/Title/Title.png");
	assert(m_titleHandle >= 0);

	m_fontHandle = CreateFontToHandle("Algerian", 48, -1, DX_FONTTYPE_ANTIALIASING_8X8);
	assert(m_fontHandle != -1);

	m_startButtonImg = LoadGraph("Data/UI/Title/Start0.png");
	assert(m_startButtonImg >= 0);

	const char* characterDataPath  = "Data/CSV/Title/CharacterDataTitle.csv";
	const char* stageDataPath	   = "Data/CSV/Title/StageObjectDataTitle.csv";
	const char* spawnPointDataPath = "Data/CSV/Title/SpawnPoint.csv";
	m_actorManager = std::make_shared<ActorController>();
	m_actorManager->Init(characterDataPath, stageDataPath, spawnPointDataPath);

	m_camera = std::make_shared<Camera>();
	m_camera->SetCamera(m_actorManager->GetPlayer());

	m_sky = std::make_shared<SkyDome>();
	m_sky->Init(m_camera);

	// プレイヤーをリザルト状態に遷移
	if (m_actorManager->GetPlayer())
	{
		m_actorManager->GetPlayer()->GetState()->ChangeState(std::make_shared<PlayerStateTitle>());
	}
}

TitleScene::~TitleScene()
{
	DeleteGraph(m_optionButtonImg);
	DeleteGraph(m_exitButtonImg);
	DeleteGraph(m_cursorImg);
	if (m_bgmHandle > 0)
	{
		DeleteSoundMem(m_bgmHandle);
	}
	DeleteGraph(m_titleHandle);
	DeleteGraph(m_startButtonImg);
	DeleteFontToHandle(m_fontHandle);
}

void TitleScene::Update()
{
	(this->*m_update)();
}

void TitleScene::Draw()
{
	(this->*m_draw)();
}

void TitleScene::NormalUpdate()
{
	auto& input = Input::GetInstance();
	++m_blinkFrame;

	m_actorManager->Update(m_camera);
	m_camera->Update(m_actorManager->GetPlayer(), m_actorManager);
	auto& physics = Physics::GetInstance();
	physics.Update();

	if (m_isWalk)
	{
		if (++m_startFrame > 60)
		{
			m_update = &TitleScene::FadeOutUpdate;
			m_draw = &TitleScene::FadeDraw;
			m_fadeFrame = 0;
		}
		return; // 演出中はメニュー操作させない
	}

	if (input.IsTrigger("Left"))
	{
		int v = (static_cast<int>(m_menuSel) - 1 +
			static_cast<int>(MenuItem::Max))
			% static_cast<int>(MenuItem::Max);

		m_menuSel = static_cast<MenuItem>(v);
	}

	if (input.IsTrigger("Right"))
	{
		int v = (static_cast<int>(m_menuSel) + 1)
			% static_cast<int>(MenuItem::Max);

		m_menuSel = static_cast<MenuItem>(v);
	}

	if (input.IsTrigger("Attack"))
	{
		switch (m_menuSel)
		{
		case MenuItem::Option:
			Input::GetInstance().ClearTriggers();
			m_controller.PushScene(
				std::make_shared<OptionsScene>(m_controller, OptionMode::FromTitle));
			return;

		case MenuItem::Start:

			// 既存の歩行演出を開始
			if (!m_isStarting)
			{
				m_isStarting = true;
				m_isWalk = true;
				m_startFrame = 0;

				if (auto player = m_actorManager->GetPlayer())
				{
					auto state = std::dynamic_pointer_cast<PlayerStateTitle>(player->GetState()->GetCurrentState());
					if (state)
					{
						state->StartWalking();
					}
				}
			}
			break;

		case MenuItem::Exit:
			DxLib_End();
			return;
		}
	}
}

void TitleScene::FadeInUpdate()
{
	if (m_fadeFrame-- <= 0)
	{
		m_update = &TitleScene::NormalUpdate;
		m_draw = &TitleScene::NormalDraw;
	}
}

void TitleScene::FadeOutUpdate()
{
	if (m_fadeFrame++ >= kFadeInterval)
	{
		StopSoundMem(m_bgmHandle);
		m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

void TitleScene::NormalDraw()
{
	m_actorManager->Draw();

	DrawGraph(0, -100, m_titleHandle, TRUE);

	if (m_isWalk)
	{
		return;
	}

	const int bottomMargin = 40;
	const int sideMargin = 60;

	int optionW, optionH;
	int startW, startH;
	int exitW, exitH;

	GetGraphSize(m_optionButtonImg, &optionW, &optionH);
	GetGraphSize(m_startButtonImg, &startW, &startH);
	GetGraphSize(m_exitButtonImg, &exitW, &exitH);

	int optionX = sideMargin;
	int optionY = Game::kScreenHeight - optionH - bottomMargin;

	int startX = Game::kScreenWidth / 2 - startW / 2;
	int startY = Game::kScreenHeight - startH - bottomMargin;

	int exitX = Game::kScreenWidth - exitW - sideMargin;
	int exitY = Game::kScreenHeight - exitH - bottomMargin;

	auto DrawButton = [&](int img, int x, int y, bool selected)
		{
			if (selected)
			{
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				DrawGraph(x, y, img, TRUE);
			}
			else
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120); // ← 暗くする
				DrawGraph(x, y, img, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		};

	DrawButton(m_optionButtonImg, optionX, optionY, m_menuSel == MenuItem::Option);
	DrawButton(m_startButtonImg, startX, startY, m_menuSel == MenuItem::Start);
	DrawButton(m_exitButtonImg, exitX, exitY, m_menuSel == MenuItem::Exit);

	int cursorW, cursorH;
	GetGraphSize(m_cursorImg, &cursorW, &cursorH);

	auto DrawCursorRight = [&](int btnX, int btnY, int btnW, int btnH)
		{
			int cursorX = btnX + btnW - 50;
			int cursorY = btnY + (btnH - cursorH) / 2; 

			DrawGraph(cursorX, cursorY, m_cursorImg, TRUE);
		};

	switch (m_menuSel)
	{
	case MenuItem::Option:
		DrawCursorRight(optionX, optionY, optionW, optionH);
		break;

	case MenuItem::Start:
		DrawCursorRight(startX, startY, startW, startH);
		break;

	case MenuItem::Exit:
		DrawCursorRight(exitX, exitY, exitW, exitH);
		break;
	}
}

void TitleScene::FadeDraw()
{
	m_actorManager->Draw();

	DrawGraph(0, -100, m_titleHandle, true);

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
