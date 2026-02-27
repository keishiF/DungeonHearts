#pragma once
#include "ColliderData.h"

/// <summary>
/// カプセルの当たり判定に必要なデータをまとめたクラス
/// </summary>
class CapsuleColliderData : public ColliderData
{
public:
	CapsuleColliderData();
	virtual ~CapsuleColliderData();

	// ゲッター
	// 終点
	Vector3 GetStartPos() const { return m_startPos; }
	//次の座標
	Vector3 GetNextStartPos(Vector3 vec) const { return m_startPos + vec; };
	// 半径
	float GetRadius() const { return m_radius; }
	// セッター
	void SetStartPos(Vector3 pos) { m_startPos = pos; }
	void SetRadius(float radius) { m_radius = radius; }

	// 終点
	Vector3 m_startPos;
	// 半径
	float m_radius;

	bool m_isStartPos;
};

