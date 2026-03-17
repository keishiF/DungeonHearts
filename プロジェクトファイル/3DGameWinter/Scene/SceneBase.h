#pragma once

class SceneController;

class SceneBase
{
public:
	SceneBase(SceneController& controller);

	// 更新、描画
	virtual void Update() abstract;
	virtual void Draw() = 0;

	// Called when options/settings changed (e.g. volumes)
	virtual void OnOptionsChanged() {}

protected:
	SceneController& m_controller;
};

