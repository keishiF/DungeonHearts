#include "SphereColliderData.h"

SphereColliderData::SphereColliderData() :
	ColliderData(ColliderData::Kind::Sphere),
	m_radius(0.0f)
{
}

SphereColliderData::~SphereColliderData()
{
}
