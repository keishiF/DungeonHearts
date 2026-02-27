#pragma once
#include "SceneBase.h"
#include "Score.h"
#include <memory>

class ActorController;
class Camera;
class SceneBase;
class SkyDome;
class UIManager;
class ResultScene final: public SceneBase
{
public:
    ResultScene(SceneController& controller, const ScoreData& scoreData);
    virtual ~ResultScene();

    virtual void Update() override;
    virtual void Draw() override;

private:
	int m_bgmHandle;
	int m_fadeFrame;
	int m_blinkFrame;
	int m_restartButtonImg;
	int m_titleButtonImg;

	std::shared_ptr<SceneBase> m_nextScene;

	using UpdateFunc_t = void(ResultScene::*)();
	using DrawFunc_t = void(ResultScene::*)();

	UpdateFunc_t m_update;
	DrawFunc_t m_draw;

	// 通常時更新処理
	void NormalUpdate();
	// フェードイン
	void FadeInUpdate();
	// フェードアウト
	void FadeOutUpdate();

	// 通常時描画
	void NormalDraw();
	// フェード時の描画
	void FadeDraw();

	enum class SelectedButton
	{
		Restart,
		Title,
	};
	SelectedButton m_selectedButton;

private:
	// カウントアップの状態
	enum class State
	{
		TimeScore,
		KillScore,
		TotalScore,
		Wait,
		FadeOut,
	};

	State m_state;
	ScoreData m_finalScore;
	ScoreData m_displayScore;
	int m_waitTimer;
	
	int m_bgHandle;
	int m_clearHandle;
	int m_fontHandle;
	
	// 更新処理
	void CountUpUpdate();
	
	// 描画処理
	void UpdateScore();
};

