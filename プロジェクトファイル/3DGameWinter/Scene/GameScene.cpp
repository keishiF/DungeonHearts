#include "GameScene.h"

#include "Actor/ActorController.h"
#include "Actor/Character/Player/Player.h"
#include "Actor/SkyDome/SkyDome.h"
#include "Actor/Stage/StageController.h"
#include "Camera/Camera.h"
#include "Core/game.h"
#include "GameOverScene.h"
#include "MyLib/Input.h"
#include "MyLib/Physics/Physics.h"
#include "MyLib/SoundManager.h"
#include "OptionsScene.h"
#include "ResultScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "UI/Option/Options.h"
#include "UI/UIManager.h"
#include <EffekseerForDXLib.h>

namespace
{
	constexpr int kFadeInterval = 60;
}

GameScene::GameScene(SceneController& controller) :
	SceneBase(controller),
	m_frame(0),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_elapsedTime(0.0f),
	m_scoreData(),
	m_update(&GameScene::FadeInUpdate),
	m_draw(&GameScene::FadeDraw)
{
	auto& soundManager = SoundManager::GetInstance();
	soundManager.PlayBGM("Game");

	const char* characterDataPath = "Data/CSV/Game/CharacterDataGame.csv";
	const char* stageDataPath = "Data/CSV/Game/StageObjectDataGame.csv";
	const char* spawnPointDataPath = "Data/CSV/Game/SpawnPoint.csv";
	m_actorManager = std::make_shared<ActorController>();
	m_actorManager->Init(characterDataPath, stageDataPath, spawnPointDataPath);

	m_camera = std::make_shared<Camera>();
	m_camera->SetCamera(m_actorManager->GetPlayer());

	m_sky = std::make_shared<SkyDome>();
	m_sky->Init(m_camera);

	m_uiManager = std::make_shared<UIManager>();
	m_uiManager->Init(m_actorManager->GetPlayer(), m_actorManager->GetEnemies());
}

GameScene::~GameScene()
{
	SoundManager::GetInstance().StopBGM();
}

void GameScene::Update()
{
	(this->*m_update)();
}

void GameScene::Draw()
{
	(this->*m_draw)();
}

void GameScene::NormalUpdate()
{
	auto& input = Input::GetInstance();
	++m_frame;
	++m_blinkFrame;
	m_elapsedTime += 1.0f / 60.0f;

	if (input.IsTrigger("Option"))
	{
		m_controller.PushScene(std::make_shared<OptionsScene>(m_controller, OptionMode::FromGame));
		return;
	}

	m_actorManager->Update(m_camera);
	m_camera->Update(m_actorManager->GetPlayer(), m_actorManager);
	auto& physics = Physics::GetInstance();
	physics.Update();
	m_sky->Update(m_camera);
	m_uiManager->Update(m_actorManager->GetEnemies());

	if (m_actorManager->GetPlayer() && m_actorManager->GetPlayer()->IsDead())
	{
		m_update = &GameScene::FedeOutGameOverUpdate;
		m_draw = &GameScene::FadeDraw;
		m_fadeFrame = 0;
	}

	if (m_actorManager->IsBossDead())
	{
		m_update = &GameScene::FadeOutUpdate;
		m_draw = &GameScene::FadeDraw;
		m_fadeFrame = 0;
	}
}

void GameScene::FadeInUpdate()
{
	if (--m_fadeFrame <= 0)
	{
		m_update = &GameScene::NormalUpdate;
		m_draw = &GameScene::NormalDraw;
	}
}

void GameScene::FadeOutUpdate()
{
	if (++m_fadeFrame >= kFadeInterval)
	{
		// タイムスコア計算 (例:3分(180秒)を基準に、早いほど高得点)
		constexpr int kBaseTimeScore = 50000;
		constexpr float kBaseTime = 180.0f;
		m_scoreData.timeScore = static_cast<int>(kBaseTimeScore *
			(kBaseTime / max(m_elapsedTime, 1.0f)));

		// 撃破スコア取得
		m_scoreData.killScore = m_actorManager->GetKillScore();
		// 合計スコア
		m_scoreData.totalScore = m_scoreData.timeScore + m_scoreData.killScore;

		SoundManager::GetInstance().StopBGM();
		m_controller.ChangeScene(std::make_shared<ResultScene>(m_controller, m_scoreData));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

void GameScene::FedeOutGameOverUpdate()
{
	if (++m_fadeFrame >= kFadeInterval)
	{
		SoundManager::GetInstance().StopBGM();
		m_controller.ChangeScene(std::make_shared<GameOverScene>(m_controller));
		return;
	}
}

void GameScene::NormalDraw()
{
#ifdef _DEBUG
	auto& physics = Physics::GetInstance();
	physics.DebugDraw();
	// 点滅効果のための条件
	if ((m_blinkFrame / 30) % 2 == 0)
	{
		DrawString(0, 0, "Game Scene", 0xffffff);
	}

#endif
	printf("frame %d\r", m_frame);

	m_sky->Draw();
	m_actorManager->Draw();
	m_uiManager->Draw();
}

void GameScene::FadeDraw()
{
	NormalDraw();

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameScene::OnOptionsChanged()
{
	// Apply updated master volume to BGM
	auto& opt = Options::GetInstance();
	int master = opt.masterVolume; //0..250
	int vol = (master * 255) / Options::VolumeMax; // scale to0..255
}
