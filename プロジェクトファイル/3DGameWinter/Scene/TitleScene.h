#pragma once
#include "SceneBase.h"
#include "MyLib/Vector3.h"
#include <memory>

class ActorController;
class Camera;
class SkyDome;
class UIManager;
class TitleScene final: public SceneBase
{
public:
	TitleScene(SceneController& controller);
	virtual ~TitleScene();

	virtual void Update() override;
	virtual void Draw() override;

private:
	enum class MenuItem
	{
		Option,
		Start,
		Exit,
		Max
	};

	MenuItem m_menuSel;
	int m_optionButtonImg;
	int m_exitButtonImg;
	int m_cursorImg;

	int m_bgmHandle;
	int m_titleHandle;
	int m_fontHandle;
	int m_startButtonImg;
	int m_startFrame;
	int m_fadeFrame;
	int m_blinkFrame;
	bool m_isStarting;
	bool m_isWalk;

	using UpdateFunc_t = void(TitleScene::*)();
	using DrawFunc_t = void(TitleScene::*)();

	UpdateFunc_t m_update;
	DrawFunc_t m_draw;

	std::shared_ptr<SkyDome> m_sky;
	std::shared_ptr<ActorController> m_actorManager;
	std::shared_ptr<Camera> m_camera;

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
};

