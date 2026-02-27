#include "Input.h"

#include <DxLib.h>

Input::Input()
{
	m_inputActionMap["OK"]		  = { {InputType::keyboard, KEY_INPUT_RETURN}, 
									  {InputType::pad,		PAD_INPUT_1} };

	m_inputActionMap["Cancel"]	  = { {InputType::keyboard, KEY_INPUT_ESCAPE},
									  {InputType::pad,	    PAD_INPUT_2} };

	m_inputActionMap["Attack"]    = { {InputType::keyboard, KEY_INPUT_8}, 
								      {InputType::pad,      PAD_INPUT_1} };

	m_inputActionMap["Jump"]      = { {InputType::keyboard, KEY_INPUT_SPACE}, 
								      {InputType::pad,	    PAD_INPUT_2} };

	m_inputActionMap["Guard"]     = { {InputType::keyboard, KEY_INPUT_LSHIFT},
									  {InputType::pad,		PAD_INPUT_3} };

	m_inputActionMap["Interact"]  = { {InputType::keyboard, KEY_INPUT_E}, 
									  {InputType::pad,	    PAD_INPUT_4} };

	m_inputActionMap["LockOn"]	  = { {InputType::pad,		PAD_INPUT_5} };

	m_inputActionMap["Magic1"]    = { {InputType::keyboard, KEY_INPUT_Q},
									  {InputType::pad,      PAD_INPUT_6} };

	m_inputActionMap["Up"]		  = { {InputType::keyboard, KEY_INPUT_UP},
									  {InputType::pad,		PAD_INPUT_UP} };

	m_inputActionMap["Right"]	  = { {InputType::keyboard, KEY_INPUT_RIGHT},
									  {InputType::pad,		PAD_INPUT_RIGHT} };

	m_inputActionMap["Down"]      = { {InputType::keyboard, KEY_INPUT_DOWN},
									  {InputType::pad,		PAD_INPUT_DOWN} };

	m_inputActionMap["Left"]      = { {InputType::keyboard, KEY_INPUT_LEFT},
									  {InputType::pad,		PAD_INPUT_LEFT} };

	m_inputActionMap["Run"]		  = { {InputType::pad,		PAD_INPUT_UP}, 
									  {InputType::pad,		PAD_INPUT_RIGHT},
									  {InputType::pad,		PAD_INPUT_DOWN}, 
									  {InputType::pad,		PAD_INPUT_LEFT} };

	m_inputActionMap["Option"]	  = { {InputType::pad,		PAD_INPUT_8} };
}

void Input::Update()
{
	m_lastInput = m_currentInput;

	char keyboardInput[256] = {};
	int padInput = 0;
	GetHitKeyStateAll(keyboardInput);
	// パッドの入力状態取得
	padInput = GetJoypadInputState(DX_INPUT_PAD1);

	for (const auto& keyInfo : m_inputActionMap)
	{
		const auto& key = keyInfo.first;
		m_currentInput[key] = false;
		for (const auto& inputInfo : keyInfo.second)
		{
			if (inputInfo.type == InputType::pad)
			{
				m_currentInput[key] = inputInfo.buttonId & padInput;
			}
			else if (inputInfo.type == InputType::keyboard)
			{
				m_currentInput[key] = keyboardInput[inputInfo.buttonId];
			}

			if (m_currentInput[key])
			{
				break;
			}
		}
	}
}

bool Input::IsPress(const char* key) const
{
	if (!m_currentInput.contains(key))
	{
		return false;
	}

	return m_currentInput.at(key);
}

bool Input::IsTrigger(const char* key) const
{
	if (!m_currentInput.contains(key))
	{
		return false;
	}

	return (m_currentInput.at(key) && !m_lastInput.at(key));
}

Input& Input::GetInstance()
{
	// TODO: return ステートメントをここに挿入します
	static Input instance;
	return instance;
}

void Input::ClearTriggers()
{
	for (const auto& kv : m_inputActionMap)
	{
		const auto& key = kv.first;
		m_currentInput[key] = false;
		m_lastInput[key] = false;
	}
}
