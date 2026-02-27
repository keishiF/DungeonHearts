#pragma once
#include "SceneBase.h"
#include <memory>

class ActorController;
class Camera;
class SkyDome;
class UIManager;
class SceneBase;
class GameOverScene : public SceneBase
{
public:
	GameOverScene(SceneController & controller);
	virtual ~GameOverScene();
	
	virtual void Update() override;
	virtual void Draw() override;
	
private:
	int m_bgmHandle;
	int m_bgHandle;
	int m_gameOverHandle;
	int m_fadeFrame;
	int m_restartButtonImg;
	int m_titleButtonImg;
	
	std::shared_ptr<SceneBase> m_nextScene;
	
	using UpdateFunc_t = void(GameOverScene::*)();
	using DrawFunc_t = void(GameOverScene::*)();
	
	UpdateFunc_t m_update;
	DrawFunc_t m_draw;

	enum class SelectedButton
	{
		Restart,
		Title,
	};
	SelectedButton m_selectedButton;
	
	void NormalUpdate();
	void FadeInUpdate();
	void FadeOutUpdate();
	void NormalDraw();
	void FadeDraw();
};

