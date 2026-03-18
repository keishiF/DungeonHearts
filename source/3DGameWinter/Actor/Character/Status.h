#pragma once

class Status
{
public:
	float m_hp    = 0.0f;
	float m_maxHp = 0.0f;
	float m_atk   = 0.0f;

	// 移動速度（デフォルトは既存コードに合わせる）
	float m_walkSpeed = 0.0f;
	float m_runSpeed  = 0.0f;
	float m_airSpeed  = 0.0f;
};

