#pragma once
#include "SceneBase.h"

class SelectSceneDebug final : public SceneBase
{
public:
	SelectSceneDebug(SceneController& controller);
	virtual ~SelectSceneDebug();

	void Init();
	virtual void Update() override;
	virtual void Draw() override;

private:
	enum class Menu : int
	{
		Title =0,
		Game =1,
		Result =2,
		GameOver =3,
		Options =4
	};
	Menu m_currentMenu = Menu::Title;
};

