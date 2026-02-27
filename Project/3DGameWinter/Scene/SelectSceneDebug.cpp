#include "SelectSceneDebug.h"

#include "Core/game.h"
#include "GameOverScene.h"
#include "GameScene.h"
#include "MyLib/Input.h"
#include "ResultScene.h"
#include "SceneController.h"
#include "Score.h"
#include "TitleScene.h"
#include "OptionsScene.h"
#include <algorithm>
#include <DxLib.h>
#include <string>

namespace
{
	constexpr int kFadeInterval =60;
}

SelectSceneDebug::SelectSceneDebug(SceneController& controller) :
	SceneBase(controller)
{
}

SelectSceneDebug::~SelectSceneDebug()
{
}

void SelectSceneDebug::Init()
{
	m_currentMenu = Menu::Title;
}

void SelectSceneDebug::Update()
{
	auto& input = Input::GetInstance();
	if (input.IsTrigger("OK"))
	{
		ScoreData scoreData;
		scoreData.totalScore =100000;
		switch (m_currentMenu)
		{
		case Menu::Title:
			m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));
			break;
		case Menu::Game:
			m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));
			break;
		case Menu::Result:
			m_controller.ChangeScene(std::make_shared<ResultScene>(m_controller, scoreData));
			break;
		case Menu::GameOver:
			m_controller.ChangeScene(std::make_shared<GameOverScene>(m_controller));
			break;
		case Menu::Options:
			// Push options so we can return to this menu
			m_controller.PushScene(std::make_shared<OptionsScene>(m_controller));
			break;
		default:
			break;
		}
		return;
	}
	int menu = static_cast<int>(m_currentMenu);
	if (input.IsTrigger("Up"))menu--;
	if (input.IsTrigger("Down"))menu++;
	menu = std::clamp(menu,0, static_cast<int>(Menu::Options));
	m_currentMenu = static_cast<Menu>(menu);
}

void SelectSceneDebug::Draw()
{
	for (int i =0; i <= static_cast<int>(Menu::Options); i++)
	{
		int color =0xffffff;
		if (m_currentMenu == static_cast<Menu>(i))
		{
			color =0xffff00;
		}
		std::string text = "Title";
		switch (static_cast<Menu>(i))
		{
		case Menu::Title:
			text = "Title";
			break;
		case Menu::Game:
			text = "Game";
			break;
		case Menu::Result:
			text = "Result";
			break;
		case Menu::GameOver:
			text = "GameOver";
			break;
		case Menu::Options:
			text = "Options";
			break;
		default:
			break;
		}
		DxLib::DrawString(100,100 + i *24, text.c_str(), color);
	}
	DxLib::DrawString(100,50, "[A]キーで決定",0xffffff);
}
