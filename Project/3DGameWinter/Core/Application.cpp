#include "Application.h"

#include "game.h"
#include "MyLib/Input.h"
#include "MyLib/SoundManager.h"
#include "MyLib/HitStopManager.h" // Add this include
#include "Scene/SceneController.h"
#include <cassert>
#include <DxLib.h>
#include <EffekseerForDXLib.h>

Application& Application::GetInstance()
{
	// TODO: return ステートメントをここに挿入します
	static Application app;
	return app;
}

bool Application::Init()
{
	// フルスクリーンではなく、ウインドウモードで開くようにする
	ChangeWindowMode(Game::kDefaultWindowMode);
	// ウィンドウ名設定
	SetMainWindowText(Game::kTitleText);
	// 画面のサイズを変更する
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);

	// バックグラウンド実行
	// 別のウィンドウを触っていても実行が止まらないようにする
	SetAlwaysRunFlag(true);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return false;			// エラーが起きたら直ちに終了
	}

	Effekseer_Init(20000);
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	Effekseer_InitDistortion();

	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);
	SetUseBackCulling(true);

	SoundManager::GetInstance().Init();

	// 描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

	return true;
}

void Application::Run()
{
	m_frame = 0;

	// メモリの確保
	SceneController sceneController;
	Input& input = Input::GetInstance();

	// ゲームループ
	while (ProcessMessage() == 0)	// Windowsが行う処理を待つ必要がある
	{
		// パッドの入力更新処理
		input.Update();

		UpdateEffekseer3D();

		HitStopManager::GetInstance().Update(); // Update hit stop manager every frame

		// 今回のループが始まった時間を覚えておく
		LONGLONG time = GetNowHiPerformanceCount();

		// 画面全体をクリアする
		ClearDrawScreen();

		// ここにゲームの処理を書く
		// If not hit stopping, update game logic
		if (!HitStopManager::GetInstance().IsHitStopping())
		{
			sceneController.Update();
			// Physics::GetInstance().Update() is called within sceneController.Update() -> GameScene::NormalUpdate()
			// So we don't need to call it here directly.
		}
		// However, Physics::Update() is called in GameScene::NormalUpdate().
		// If sceneController.Update() is skipped, Physics::Update() is also skipped. This is correct.

		sceneController.Draw(); // Drawing should still happen during hit stop

		Effekseer_Sync3DSetting();
		DrawEffekseer3D();

		// 画面の切り替わりを待つ必要がある
		ScreenFlip();	// 1/60秒経過するまで待つ
		++m_frame;

		// FPS(Frame Per Second)60に固定
		while (GetNowHiPerformanceCount() - time < 16667)
		{
		}

		// もしＥＳＣキーが押されていたらループから抜ける
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}
	}
}

void Application::Terminate()
{
	SoundManager::GetInstance().Release();
	Effkseer_End();
	DxLib_End();				// ＤＸライブラリ使用の終了処理
}
