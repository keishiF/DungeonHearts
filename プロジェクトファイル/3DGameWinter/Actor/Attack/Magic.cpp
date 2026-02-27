#include "Magic.h"

Magic::Magic(ObjectTag tag, ColliderData::Kind kind) :
	m_atkPower(0),
	m_lifeTime(0.0f),
	m_timer(0.0f),
	m_isDead(false),
	Collidable(tag, kind)
{
}

Magic::~Magic()
{
}
