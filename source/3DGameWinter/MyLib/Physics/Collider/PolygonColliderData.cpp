#include "PolygonColliderData.h"

PolygonColliderData::PolygonColliderData() :
	ColliderData(ColliderData::Kind::Polygon),
	m_modelHandle(-1),
	m_nearWallHitPos(0.0f, 0.0f, 0.0f),
	m_nearFloorHitPos(0.0f, 0.0f, 0.0f),
	m_hitDim()
{
}

PolygonColliderData::~PolygonColliderData()
{
	if (m_hitDim.HitNum > 0)
	{
		MV1CollResultPolyDimTerminate(m_hitDim);
	}
}

void PolygonColliderData::TerminateHitDim()
{
	if (m_hitDim.HitNum > 0)
	{
		MV1CollResultPolyDimTerminate(m_hitDim);
		m_hitDim.HitNum = 0; // ヒット数をリセット
	}
}
