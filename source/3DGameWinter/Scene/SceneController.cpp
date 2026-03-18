#include "SceneController.h"

#include "GameScene.h"
#include "SceneBase.h"
#include "SelectSceneDebug.h"
#include "TitleScene.h"
#include "MyLib/Input.h"

SceneController::SceneController()
{
	ChangeScene(std::make_shared<TitleScene>(*this));
#if _DEBUG
	ChangeScene(std::make_shared<SelectSceneDebug>(*this));
#endif
}

void SceneController::Update()
{
	m_scenes.back()->Update();
}

void SceneController::Draw()
{
	for (auto& scene : m_scenes)
	{
		scene->Draw();
	}
}

void SceneController::ChangeScene(std::shared_ptr<SceneBase> scene)
{
	// 既存のシーンをすべて破棄する
	while (!m_scenes.empty())
	{
		m_scenes.pop_back();
	}
	// 新しいシーンを追加する
	m_scenes.push_back(scene);
	Input::GetInstance().Update();
}

void SceneController::PushScene(std::shared_ptr<SceneBase> scene)
{
	m_scenes.push_back(scene);
	Input::GetInstance().Update();
}

void SceneController::PopScene()
{
	if (m_scenes.size() == 1)
	{
		// 実行すべきシーンがなくなるため許可しない
		return;
	}
	// 末尾を取り除く
	m_scenes.pop_back();
	Input::GetInstance().Update();
	// Notify new top scene about potential options changes
	if (!m_scenes.empty())
	{
		m_scenes.back()->OnOptionsChanged();
	}
}
