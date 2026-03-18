#pragma once
#include "SceneBase.h"
#include "Score.h"
#include "MyLib/Vector3.h"
#include <DxLib.h>
#include <memory>

class ActorController;
class Camera;
class SkyDome;
class UIManager;
class GameScene final: public SceneBase
{
public:
	GameScene(SceneController& controller);
	virtual ~GameScene();

	virtual void Update() override;
	virtual void Draw() override;

	// Respond to options changes
	virtual void OnOptionsChanged() override;

private:
	int m_frame;
	int m_fadeFrame;
	int m_blinkFrame;

	float m_elapsedTime;
	ScoreData m_scoreData;

	using UpdateFunc_t = void(GameScene::*)();
	using DrawFunc_t = void(GameScene::*)();
	UpdateFunc_t m_update;
	DrawFunc_t m_draw;

	std::shared_ptr<SkyDome> m_sky;
	std::shared_ptr<ActorController> m_actorManager;
	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<UIManager> m_uiManager;

	// 通常時更新処理
	void NormalUpdate();
	// フェードイン
	void FadeInUpdate();
	// フェードアウト
	void FadeOutUpdate();
	void FedeOutGameOverUpdate();

	// 通常時描画
	void NormalDraw();
	// フェード時の描画
	void FadeDraw();
};

