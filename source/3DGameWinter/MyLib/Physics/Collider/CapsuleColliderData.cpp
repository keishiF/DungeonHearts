#include "CapsuleColliderData.h"

CapsuleColliderData::CapsuleColliderData() :
	ColliderData(ColliderData::Kind::Capsule),
	m_startPos(0.0f, 0.0f, 0.0f),
	m_radius(0.0f),
	m_isStartPos(true)
{
}

CapsuleColliderData::~CapsuleColliderData()
{
}
