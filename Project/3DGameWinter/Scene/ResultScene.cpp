#include "ResultScene.h"

#include "Core/game.h"
#include "GameScene.h"
#include "MyLib/Input.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "UI/UIManager.h"
#include <cassert>
#include <cmath>
#include <cstring>
#include <DxLib.h>
#include <iostream>
#include <string>

namespace
{
	constexpr int kFadeInterval = 60;

	constexpr int kCountUpSpeed = 1000;
	constexpr int kWaitTime = 120;
}

ResultScene::ResultScene(SceneController& controller, const ScoreData& scoreData):
	SceneBase(controller),
	m_bgmHandle(-1),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_restartButtonImg(-1),
	m_titleButtonImg(-1),
	m_update(&ResultScene::FadeInUpdate),
	m_draw(&ResultScene::FadeDraw),
	m_selectedButton(SelectedButton::Restart),
	m_state(State::TimeScore),
	m_finalScore(scoreData),
	m_displayScore(),
	m_waitTimer(0)
{
	m_bgmHandle = LoadSoundMem("Data/Sound/BGM/ResultBGM.mp3");
	assert(m_bgmHandle > 0);
	ChangeVolumeSoundMem(128, m_bgmHandle);
	PlaySoundMem(m_bgmHandle, DX_PLAYTYPE_LOOP);

	m_restartButtonImg = LoadGraph("Data/UI/Result/ReStart0.png");
	assert(m_restartButtonImg >= 0);
	m_titleButtonImg = LoadGraph("Data/UI/Option/BackTitle.png");
	assert(m_titleButtonImg >= 0);

	m_bgHandle = LoadGraph("Data/UI/BackGround/BackGround2.png");
	assert(m_bgHandle >= 0);

	m_clearHandle = LoadGraph("Data/UI/Result/GameClear.png");
	assert(m_clearHandle >= 0);
	
	m_fontHandle = CreateFontToHandle("Impact", 60, 3, DX_FONTTYPE_ANTIALIASING_EDGE);
	assert(m_fontHandle >= 0);
}

ResultScene::~ResultScene()
{
	if (m_bgmHandle > 0)
	{
		DeleteSoundMem(m_bgmHandle);
	}
	DeleteGraph(m_bgHandle);
	DeleteFontToHandle(m_fontHandle);
	DeleteGraph(m_restartButtonImg);
	DeleteGraph(m_titleButtonImg);
}

void ResultScene::Update()
{
	(this->*m_update)();
}

void ResultScene::Draw()
{
	(this->*m_draw)();
}

void ResultScene::NormalUpdate()
{
	auto& input = Input::GetInstance();

	if (input.IsPress("Attack"))
	{
		m_update = &ResultScene::FadeOutUpdate;
		m_draw = &ResultScene::FadeDraw;
		m_fadeFrame = 0;
	}
}

void ResultScene::FadeInUpdate()
{
	if (m_fadeFrame-- <= 0)
	{
		m_update = &ResultScene::CountUpUpdate;
		m_draw = &ResultScene::NormalDraw;
	}
}

void ResultScene::FadeOutUpdate()
{
	if (m_fadeFrame++ >= kFadeInterval)
	{
		StopSoundMem(m_bgmHandle);
		m_controller.ChangeScene(m_nextScene);

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

void ResultScene::NormalDraw()
{
	DrawGraph(0, 0, m_bgHandle, true);

	int width = 0, height = 0;
	GetGraphSize(m_clearHandle, &width, &height);
	int x = (Game::kScreenWidth - width) / 2;
	int y = Game::kScreenHeight / 20; // 画面の上部1/5の位置
	DrawGraph(x, y, m_clearHandle, TRUE);

	// スコア表示の座標をスクリーンサイズから計算
	const int labelX = Game::kScreenWidth / 3;
	const int scoreX = Game::kScreenWidth * 2 / 3;
	const int baseY = Game::kScreenHeight / 2 - 100;
	const int lineHeight = 80;
	const int totalScoreYOffset = 40;

	int color = GetColor(255, 255, 255);
	/*DrawStringToHandle(350, 200, "TIME  SCORE", color, m_fontHandle);
	DrawFormatStringToHandle(750, 200, color, m_fontHandle, "%7d", m_displayScore.timeScore);*/
	DrawStringToHandle(labelX, baseY, 
		"TIME  SCORE", color, m_fontHandle);
	DrawFormatStringToHandle(scoreX, baseY, 
		color, m_fontHandle, 
		"%7d", m_displayScore.timeScore);

	/*DrawStringToHandle(350, 300, "KILL  SCORE", color, m_fontHandle);
	DrawFormatStringToHandle(750, 300, color, m_fontHandle, "%7d", m_displayScore.killScore);*/
	DrawStringToHandle(labelX, baseY + lineHeight, 
		"KILL  SCORE", color, m_fontHandle);
	DrawFormatStringToHandle(scoreX, baseY + lineHeight, 
		color, m_fontHandle,
		"%7d", m_displayScore.killScore);

	/*DrawStringToHandle(350, 450, "TOTAL SCORE", color, m_fontHandle);
	DrawFormatStringToHandle(750, 450, color, m_fontHandle, "%7d", m_displayScore.totalScore);*/
	DrawStringToHandle(labelX, baseY + lineHeight * 2, 
		"TOTAL SCORE", color, m_fontHandle);
	DrawFormatStringToHandle(scoreX, baseY + lineHeight * 2,
		color, m_fontHandle, 
		"%7d", m_displayScore.totalScore);

	if (m_state == State::Wait)
	{
		int restartW = 0, restartH = 0, titleW = 0, titleH = 0;
		GetGraphSize(m_restartButtonImg, &restartW, &restartH);
		GetGraphSize(m_titleButtonImg, &titleW, &titleH);

		int buttonY = baseY + lineHeight * 3 + totalScoreYOffset + 40;
		int spacing = 100;
		int totalWidth = restartW + titleW + spacing;
		int restartX = (Game::kScreenWidth - totalWidth) / 2;
		int titleX = restartX + restartW + spacing;

		// 選択されているボタンのスケールを少し大きくする
		double restartScale = (m_selectedButton == SelectedButton::Restart) ? 1.05 : 1.0;
		double titleScale = (m_selectedButton == SelectedButton::Title) ? 1.05 : 1.0;

		// リスタートボタン描画
		if (m_selectedButton != SelectedButton::Restart)
		{
			// 選択されていないボタンは少し半透明にする
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
		}
		DrawRotaGraph(restartX + restartW / 2, buttonY + restartH / 2, restartScale, 0.0, m_restartButtonImg, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// タイトルボタン描画
		if (m_selectedButton != SelectedButton::Title)
		{
			// 選択されていないボタンは少し半透明にする
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
		}
		DrawRotaGraph(titleX + titleW / 2, buttonY + titleH / 2, titleScale, 0.0, m_titleButtonImg, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void ResultScene::FadeDraw()
{
	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void ResultScene::CountUpUpdate()
{
	auto& input = Input::GetInstance();
	
	UpdateScore();

	if (m_state == State::Wait)
	{
		// 左右入力でボタン選択
		if (input.IsTrigger("Right"))
		{
			m_selectedButton = SelectedButton::Title;
		}
		else if (input.IsTrigger("Left"))
		{
			m_selectedButton = SelectedButton::Restart;
		}
		// 決定ボタンでシーン遷移
		if (input.IsTrigger("Attack"))
		{
			StopSoundMem(m_bgmHandle);
			
			if (m_selectedButton == SelectedButton::Restart)
			{
				m_nextScene = std::make_shared<GameScene>(m_controller);
			}
			else // Title
			{
				m_nextScene = std::make_shared<TitleScene>(m_controller);
			}
			StopSoundMem(m_bgmHandle);
			m_update = &ResultScene::FadeOutUpdate;
			m_draw = &ResultScene::FadeDraw;
			m_fadeFrame = 0;
		}
	}
}

void ResultScene::UpdateScore()
{
	switch (m_state)
	{
	case State::TimeScore:
		m_displayScore.timeScore += kCountUpSpeed;
		if (m_displayScore.timeScore >= m_finalScore.timeScore)
		{
			m_displayScore.timeScore = m_finalScore.timeScore;
			m_state = State::KillScore;
		}
		break;
	case State::KillScore:
		m_displayScore.killScore += kCountUpSpeed;
		if (m_displayScore.killScore >= m_finalScore.killScore)
		{
			m_displayScore.killScore = m_finalScore.killScore;
			m_state = State::TotalScore;
		}
		break;
	case State::TotalScore:
		m_displayScore.totalScore += kCountUpSpeed * 2; // 合計は倍速でカウント
		if (m_displayScore.totalScore >= m_finalScore.totalScore)
		{
			m_displayScore.totalScore = m_finalScore.totalScore;
			m_state = State::Wait;
			}
		break;
	case State::Wait:
		// 入力待ち
		break;
	}
}
