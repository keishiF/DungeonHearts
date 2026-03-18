#pragma once
#include <list>
#include <memory>

class SceneBase;
class SceneController
{
public:
	SceneController();
	virtual ~SceneController() {}

	void Update();
	void Draw();

	void ChangeScene(std::shared_ptr<SceneBase> scene);
	void PushScene(std::shared_ptr<SceneBase> scene);
	void PopScene();

private:
	using SceneStack_t = std::list<std::shared_ptr<SceneBase>>;
	SceneStack_t m_scenes;
};

