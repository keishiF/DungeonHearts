#pragma once
#include "ColliderData.h"

/// <summary>
/// 球の当たり判定に必要なデータをまとめたクラス
/// </summary>
class SphereColliderData : public ColliderData
{
public:
	SphereColliderData();
	virtual ~SphereColliderData();

	float GetRadius() const { return m_radius; }
	void SetRadius(float radius) { m_radius = radius; }

	// 球の半径
	float m_radius;
};

