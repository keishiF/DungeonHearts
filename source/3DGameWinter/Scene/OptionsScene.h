#pragma once
#include "SceneBase.h"
#include "UI/Option/Options.h"
#include <memory>
#include <vector>

// タイトルから開かれたのかゲームから開かれたのか
enum class OptionMode
{
	FromTitle,
	FromGame
};

class OptionsScene final : public SceneBase
{
public:
	OptionsScene(SceneController& controller, OptionMode mode = OptionMode::FromTitle);
	virtual ~OptionsScene();
	virtual void Update() override;
	virtual void Draw() override;

private:
	// メインメニューの項目
	enum class MainType
	{
		Camera,
		Sound,
		Back,
		BackGame
	};

	// 各種更新処理
	void UpdateMainMenu();
	void UpdateSubMenu();
	void UpdateCamera();
	void UpdateSound();

	// 各種描画処理
	void DrawMainMenu();
	void DrawSubMenu();
	void DrawCameraMenu();
	void DrawSoundMenu();

	int GetImageFromType(MainType menu) const;
	void DrawCursorAtButton(int x, int y);

private:
	Options& m_options; // コンストラクタで初期化
	OptionMode m_mode;

	// メインメニュー管理
	std::vector<MainType> m_mainEntries;
	int  m_mainIndex = 0;
	bool m_inSubMenu = false;

	// サブメニュー選択状態
	int m_cameraSel = 0; // 0:左右 1:上下
	int m_soundSel = 0;  // 0:Master 1:BGM 2:SE

	// UI画像ハンドル
	int m_bg = -1;

	int m_imgCamera = -1;
	int m_imgSound = -1;
	int m_imgBack = -1;
	int m_imgBackTitle = -1;
	int m_imgBackGame = -1;
	int m_imgCursor = -1; 

	// カメラ画像
	int m_imgCamXNormal = -1;
	int m_imgCamXReverse = -1;
	int m_imgCamYNormal = -1;
	int m_imgCamYReverse = -1;

	// スライダー画像
	int m_imgSliderBack  = -1; // スライダー背景
	int m_imgSlider      = -1; // スライダー本体
	int m_imgSliderKnob  = -1; // スライダーハンドル
	int m_imgSliderPanel = -1; // 背景パネル

	// フォントハンドル
	int m_font = -1;
};
